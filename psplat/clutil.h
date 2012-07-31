#ifndef devicelister_h
#define devicelister_h
/**
 * Header file for devicelister, borrowed in part from 
 * http://www.drdobbs.com/go-parallel/article/231002854?pgno=3
 * (which is a great refrence)
 */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef MAC
#include <OpenCL/cl.h>
#else
#include <CL/cl.h>
#endif

#define MAX_PLATFORM 64
#define MAX_DEVICE 1024 
#define INFO_BUFFER_SIZE 2048
#define ARRAY_SIZE 64

int getDevices(cl_device_id devices[MAX_PLATFORM][MAX_DEVICE], size_t maxPlatform, 
   size_t maxDevice, cl_uint numDev[MAX_PLATFORM], cl_uint *numPlatforms);
cl_program build_program(cl_context ctx, cl_device_id dev, const char* filename);
#endif
