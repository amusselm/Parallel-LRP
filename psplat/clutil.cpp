/**
 * A program to instincate cl_devices for devices instaled on a system.
 * Draws heavily from http://www.drdobbs.com/parallel/231002854
 *
 * @author Andrew J. Musselman
 */

#include "devicelister.h"

#define maxPlatform 64
#define MAX_DEVICE 1024 
#define INFO_BUFFER_SIZE 2048

int getDevices(cl_device_id devices[][], size_t maxPlatform, size_t maxDevice,
   int* numTotalDevices){
   cl_device_id devices[maxPlatform][maxDevice]; 
   int err = 0;
   cl_uint numDev[maxPlatform];
   cl_uint numPlatform;
   int ii,jj;

   /* Identify a platform */
   err = clGetPlatformIDs(maxPlatform, platforms, &numPlatform);
   if(err < 0) {
      perror("Couldn't identify a platform");
      exit(1);
   }
   
   if(numPlatform >= maxPlatform){
      fprintf(stderr,"Truncating number of devices to %d\n",maxPlatform);
      
      numPlatform = maxPlatform;
   }

   for(ii = 0; ii < numPlatform; ii++){
      err = clGetDeviceIDs(platforms[ii], CL_DEVICE_TYPE_ALL, maxDevice, 
         devices[ii],&numDev[ii]);
      if(err < 0) {
         perror("Couldn't access any devices");
         exit(1);   
      }
      if(numDev[ii] >= maxDevice){
         fprintf(stderr,"Truncating number of devices to %d for platform %d \n",
            maxPlatform,ii);
         numDev[ii] = maxDevice;
      }
   }
   *numTotalDevices= 0;
   for(ii = 0; ii < numPlatform; ii++){
      *numTotalDevices+=numDev[ii];
   }
   

   return err;

