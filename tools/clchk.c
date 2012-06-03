/**
 * A program to attempt to compile open cl code
 *
 * Code borrowed from http://www.drdobbs.com/parallel/231002854
 * TODO: Extend this so that I can compile on multiple devices
 */
#include"clchk.h"

#define ARRAY_SIZE 64

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
int build_program(cl_context ctx, cl_device_id dev, const char* filename) {
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
      if(program_log == NULL){
         perror("Couldn't allocate memoery, malloc failure");
         exit(1);
      }
      program_log[log_size] = '\0';
      clGetProgramBuildInfo(program, dev, CL_PROGRAM_BUILD_LOG, 
            log_size + 1, program_log, NULL);
      printf("Error Compiling: %s\n", program_log);
      free(program_log);
      return(err);
   }

   return 0;
}

int main(int argc, char* argv[]) {

   /* OpenCL structures */
   cl_device_id device;
   cl_context context;
   cl_program program;
   cl_int i, err;

   /* Data and buffers */
   float data[ARRAY_SIZE];
   float sum[2], total, actual_sum;
   cl_mem input_buffer, sum_buffer;

   /*Check input */
   if(argc < 2 || argc > 2){
      printf("Useage: clchk openClProgram.cl]\n");
      exit(1);
   }

   /* Create device and context */
   device = create_device();
   context = clCreateContext(NULL, 1, &device, NULL, NULL, &err);
   if(err < 0) {
      perror("Couldn't create a context");
      exit(1);   
   }

   /* Build program */
   err = build_program(context, device, argv[1]);
   if(err < 0){
      fprintf(stdout,"Compile failed\n");
      exit(1);   
   }
   
   printf("CLCHK - Compile successful!\n");

   return 0;
}
