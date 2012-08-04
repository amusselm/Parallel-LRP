/**
 * A program to instincate cl_devices for devices instaled on a system.
 * Draws heavily from http://www.drdobbs.com/parallel/231002854
 *
 * @author Andrew J. Musselman
 */

#include "clutil.h"


/**
 * getDevices - Gets all aviable OpenCL devices and puts them into the 
 *  provided devices array. This also respects the maxPlatform and 
 *  maxDevice attributes so that it doesn't overwrite the array. 
 *  This function also has it's own limitations on the maximum number of devices.
 */

int getDevices(cl_device_id devices[MAX_PLATFORM][MAX_DEVICE], size_t maxPlatform, 
   size_t maxDevice, cl_uint numDev[MAX_PLATFORM], cl_uint *numPlatforms){
   cl_platform_id platforms[MAX_PLATFORM];
   int err = 0;
   int ii,jj;

   //Truncate to alocated memory
   if(MAX_PLATFORM < maxPlatform){
      maxPlatform = MAX_PLATFORM;
   } 
   //Truncate to alocated memory
   if(MAX_DEVICE < maxDevice){
      maxDevice = MAX_DEVICE;
   } 

   /* Identify a platform */
   err = clGetPlatformIDs(maxPlatform, platforms, numPlatforms);
   if(err < 0) {
      perror("Couldn't identify a platform");
      return 1;
   }
   
   if(*numPlatforms >= maxPlatform){
      fprintf(stderr,"Truncating number of devices to %ld\n",maxPlatform);
      
      *numPlatforms = maxPlatform;
   }

   for(ii = 0; ii < *numPlatforms; ii++){
      err = clGetDeviceIDs(platforms[ii], CL_DEVICE_TYPE_ALL, maxDevice, 
         devices[ii],&numDev[ii]);
      if(err < 0) {
         perror("Couldn't access any devices");
         return 1;
      }
      if(numDev[ii] >= maxDevice){
         fprintf(stderr,"Truncating number of devices to %ld for platform %d \n",
            maxPlatform,ii);
         numDev[ii] = maxDevice;
      }
   }

   return err;

}

/**
 * Compile Kernel - Compiles the open CL Kernel function for use elsewhere in 
 * in the program.
 * Code borrowed from http://www.drdobbs.com/parallel/231002854
 */
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



