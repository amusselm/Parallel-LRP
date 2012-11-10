/**
 * A program to dump information about the OpenCL devices instaled on a system.
 * Draws heavily from http://www.drdobbs.com/parallel/231002854
 *
 * @author Andrew J. Musselman
 */

#include "devicelister.h"

#define MAX_PLATFORM 64
#define MAX_DEVICE 1024 
#define INFO_BUFFER_SIZE 2048

int main(int argc, char* argv[]){
   cl_platform_id platforms[MAX_PLATFORM];
   cl_device_id devices[MAX_PLATFORM][MAX_DEVICE]; 
   int err;
   cl_uint numDev[MAX_PLATFORM];
   cl_uint numPlatform;
   int ii,jj;

   /* Identify a platform */
   err = clGetPlatformIDs(MAX_PLATFORM, platforms, &numPlatform);
   if(err < 0) {
      perror("Couldn't identify a platform");
      exit(1);
   }
   
   if(numPlatform >= MAX_PLATFORM){
      fprintf(stderr,"Truncating number of devices to %d\n",MAX_PLATFORM);
      
      numPlatform = MAX_PLATFORM;
   }

   for(ii = 0; ii < numPlatform; ii++){
      err = clGetDeviceIDs(platforms[ii], CL_DEVICE_TYPE_ALL, MAX_DEVICE, 
         devices[ii],&numDev[ii]);
      if(err < 0) {
         perror("Couldn't access any devices");
         exit(1);   
      }
      if(numDev[ii] >= MAX_DEVICE){
         fprintf(stderr,"Truncating number of devices to %d for platform %d \n",
            MAX_PLATFORM,ii);
         numDev[ii] = MAX_DEVICE;
      }
   }
   
   printf("Platforms:\n");
   for(ii = 0; ii < numPlatform; ii++){
      printf("Platform %d\n",ii);
      printPlatformInfo(platforms[ii]);
      printf("Devices:\n");
      for(jj=0; jj < numDev[ii]; jj++){
         printf("Device %d\n",jj);
         printDeviceInfo(devices[ii][jj]);
      }
   }


   return 0;

}

void printPlatformInfo(cl_platform_id platform){
   char stringBuf[INFO_BUFFER_SIZE];

   clGetPlatformInfo(platform,CL_PLATFORM_PROFILE,INFO_BUFFER_SIZE*sizeof(char),
      stringBuf,NULL);
   fprintf(stdout,"CL PLATFORM INFO: %s \n",stringBuf);

   clGetPlatformInfo(platform,CL_PLATFORM_VERSION,INFO_BUFFER_SIZE*sizeof(char),
      stringBuf,NULL);
   fprintf(stdout,"CL_PLATFORM_VERSION : %s \n",stringBuf);

   clGetPlatformInfo(platform,CL_PLATFORM_NAME,INFO_BUFFER_SIZE*sizeof(char),
      stringBuf,NULL);
   fprintf(stdout,"CL_PLATFORM_NAME : %s \n",stringBuf);

   clGetPlatformInfo(platform,CL_PLATFORM_VENDOR,INFO_BUFFER_SIZE*sizeof(char),
      stringBuf,NULL);
   fprintf(stdout,"CL_PLATFORM_VENDOR : %s \n",stringBuf);

   clGetPlatformInfo(platform,CL_PLATFORM_EXTENSIONS,INFO_BUFFER_SIZE*sizeof(char),
      stringBuf,NULL);
   fprintf(stdout,"CL_PLATFORM_EXTENSIONS : %s \n",stringBuf);
}

void printDeviceInfo(cl_device_id device){
   char stringBuf[INFO_BUFFER_SIZE];
   size_t maxParamSize;
   cl_ulong memAllocSize;
   cl_ulong localMemAllocSize;

   clGetDeviceInfo(device,CL_DEVICE_NAME,INFO_BUFFER_SIZE*sizeof(char),
      stringBuf,NULL);
   fprintf(stdout,"CL_DEVICE_NAME:%s\n",stringBuf);

   clGetDeviceInfo(device,CL_DEVICE_MAX_PARAMETER_SIZE,sizeof(maxParamSize),
      &maxParamSize,NULL);
   fprintf(stdout,"CL_DEVICE_MAX_PARAMETER_SIZE: %ld\n",maxParamSize);

   clGetDeviceInfo(device,CL_DEVICE_MAX_MEM_ALLOC_SIZE,sizeof(memAllocSize),
      &memAllocSize,NULL);
   fprintf(stdout,"CL_DEVICE_MAX_MEM_ALLOC_SIZE: %ld\n",memAllocSize);

   clGetDeviceInfo(device,CL_DEVICE_LOCAL_MEM_SIZE,sizeof(localMemAllocSize),
      &localMemAllocSize,NULL);
   fprintf(stdout,"CL_DEVICE_LOCAL_MEM_SIZE: %ld\n",localMemAllocSize);
   
   
}
