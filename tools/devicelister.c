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
   cl_device_id devices[MAX_DEVICE]; 
   int err;
   int numDev;
   cl_uint numPlatform;
   int ii;

   /* Identify a platform */
   err = clGetPlatformIDs(MAX_PLATFORM, &platforms, &numPlatform);
   if(err < 0) {
      perror("Couldn't identify a platform");
      exit(1);
   }
   
   if(numPlatform > MAX_PLATFORM){
      fprintf(stderr,"Truncating number of devices to %d\n",MAX_PLATFORM);
      numPlatform = MAX_PLATFORM;
   }
   
   printf("Platforms:\n");
   for(ii = 0; ii < numPlatform; ii++){
      printPlatformInfo(platforms[ii]);
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
