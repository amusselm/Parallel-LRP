//Based on 
//http://www.drdobbs.com/parallel/a-gentle-introduction-to-opencl/231002854
#define PROGRAM_FILE "test.cl"
#define KERNEL_FUNC "add_numbers"
#define ARRAY_SIZE 64

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "lrtest.h"

#ifdef MAC
#include <OpenCL/cl.h>
#else
#include <CL/cl.h>
#endif

/* Find a GPU or CPU associated with the first available platform */
cl_device_id create_device() {

   cl_platform_id platform;
   cl_device_id dev;
   int err;

   /* Identify a platform */
   err = clGetPlatformIDs(1, &platform, NULL);
   if(err < 0) {
      perror("Couldn't identify a platform");
      exit(1);
   } 

   /* Access a device */
   err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &dev, NULL);
   if(err == CL_DEVICE_NOT_FOUND) {
      err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_CPU, 1, &dev, NULL);
   }
   if(err < 0) {
      perror("Couldn't access any devices");
      exit(1);   
   }

   return dev;
}

/* Create program from a file and compile it */
cl_program build_program(cl_context ctx, cl_device_id dev, const char* filename) {

   cl_program program;
   FILE *program_handle;
   char *program_buffer, *program_log;
   size_t program_size, log_size;
   int err;

   /* Read program file and place content into buffer */
   program_handle = fopen(filename, "r");
   if(program_handle == NULL) {
      perror("Couldn't find the program file");
      exit(1);
   }
   fseek(program_handle, 0, SEEK_END);
   program_size = ftell(program_handle);
   rewind(program_handle);
   program_buffer = (char*)malloc(program_size + 1);
   program_buffer[program_size] = '\0';
   fread(program_buffer, sizeof(char), program_size, program_handle);
   fclose(program_handle);

   /* Create program from file */
   program = clCreateProgramWithSource(ctx, 1, 
      (const char**)&program_buffer, &program_size, &err);
   if(err < 0) {
      perror("Couldn't create the program");
      exit(1);
   }
   free(program_buffer);

   /* Build program */
   err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
   if(err < 0) {

      /* Find size of log and print to std output */
      clGetProgramBuildInfo(program, dev, CL_PROGRAM_BUILD_LOG, 
            0, NULL, &log_size);
      program_log = (char*) malloc(log_size + 1);
      program_log[log_size] = '\0';
      clGetProgramBuildInfo(program, dev, CL_PROGRAM_BUILD_LOG, 
            log_size + 1, program_log, NULL);
      printf("%s\n", program_log);
      free(program_log);
      exit(1);
   }

   return program;
}

int main() {

   /* OpenCL structures */
   cl_device_id device;
   cl_context context;
   cl_program program;
   cl_kernel kernel;
   cl_command_queue queue;
   cl_int i, j, err;
   size_t local_size, global_size;

   /* Data and buffers */
   double output;
   struct LR_min LR;
   LR.eps_dielect=15.0;
   LR.sgm_conductivity=0.005;
   LR.eno_ns_surfref=301.0;
   LR.frq_mhz=300.0;
   LR.radio_climate=5;
   LR.pol=0;
   LR.conf=0.50;
   LR.rel=0.50;
   LR.erp=0.0;
   

   cl_mem  output_buffer;
   cl_int num_groups;

   /* Create device and context */
   device = create_device();
   context = clCreateContext(NULL, 1, &device, NULL, NULL, &err);
   if(err < 0) {
      perror("Couldn't create a context");
      exit(1);   
   }

   /* Build program */
   program = build_program(context, device, PROGRAM_FILE);

   /* Create data buffer */
   //This effectively means having only a single work-item, which means no
   //paraellizm. That's okay, this is only a test. 
   global_size = 1;

   output_buffer = clCreateBuffer(context, CL_MEM_READ_WRITE |
         CL_MEM_COPY_HOST_PTR, sizeof(output), &output, &err);
   if(err < 0) {
      perror("Couldn't create a buffer");
      exit(1);   
   };

   cl_mem LRBuffer = clCreateBuffer(context, 
      CL_MEM_READ_ONLY |CL_MEM_COPY_HOST_PTR, 
      sizeof(struct LR_min),
      &LR,
      &err);
   if(err < 0) {
      fprintf(stderr,"Couldn't create a buffer (LR): error code: %d\n",err);
      exit(1);   
   };



   /* Create a command queue */
   queue = clCreateCommandQueue(context, device, 0, &err);
   if(err < 0) {
      perror("Couldn't create a command queue");
      exit(1);   
   };

   /* Create a kernel */
   //kernel = clCreateKernel(program, KERNEL_FUNC, &err);
   kernel = clCreateKernel(program, "test", &err);
   if(err < 0) {
      perror("Couldn't create a kernel");
      exit(1);
   };

   err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &LRBuffer);
   if(err < 0) {
      fprintf(stderr,"Couldn't create a kernel argument:LRBuffer Code:%d",err);
      exit(1);
   }

   err = clSetKernelArg(kernel, 1, sizeof(cl_mem), &output_buffer);
   if(err < 0) {
      fprintf(stderr,"Error setting kernel arguments, code: %d \n",err);
   }
 
   /* Enqueue kernel */
   err = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &global_size, 
         NULL, 0, NULL, NULL); 
   if(err < 0) {
      fprintf(stderr,"Couldn't enqueue the kernel, error code %d\n",err);
      exit(1);
   }

   /* Read the kernel's output */
   err = clEnqueueReadBuffer(queue, output_buffer, CL_TRUE, 0, 
         sizeof(output), &output, 0, NULL, NULL);
   if(err < 0) {
      perror("Couldn't read the buffer");
      exit(1);
   }

   printf("Output is: %lf\n",output);

   /* Deallocate resources */
   clReleaseKernel(kernel);
   clReleaseMemObject(output_buffer);
   clReleaseCommandQueue(queue);
   clReleaseProgram(program);
   clReleaseContext(context);
   return 0;
}
