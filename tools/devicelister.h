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

void printPlatformInfo(cl_platform_id platform);
void printDeviceInfo(cl_device_id device);

#endif
