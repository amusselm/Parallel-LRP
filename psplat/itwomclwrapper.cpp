/*****************************************************************************\
*      Wrapper functions for the ITWOM Open CL kernel                        *
******************************************************************************
*             Project started in 1997 by John A. Magliacane, KD2BD           *
*             Parallel OpenCL modifications by Andrew J. Musselman, KI6UOC   *
******************************************************************************
*         Please consult the documentation for a complete list of            *
*             individuals who have contributed to this project.              *
******************************************************************************
*                                                                            *
*  This program is free software; you can redistribute it and/or modify it   *
*  under the terms of the GNU General Public License as published by the     *
*  Free Software Foundation; either version 2 of the License or any later    *
*  version.                                                                  *
*                                                                            *
*  This program is distributed in the hope that it will useful, but WITHOUT  *
*  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or     *
*  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License     *
*  for more details.                                                         *
*                                                                            *
\****************************************************************************/
#include"splat.h"
#include"itwomclwrapper.h"
void foo(){
   int foo = 1+2;
}

void PlotLRMap_cl1(
   struct site source,
   double max_north,
   double min_north,
   double max_west,
   double min_west,
   unsigned char *mask_value,
   struct LR LR,
   struct dem *dem,
   const double altitude,
   const double clutter,
   const double max_range,
   unsigned char got_elevation_pattern,
   unsigned char dbm){

   int err;

   cl_platform_id platforms[MAX_PLATFORM];
   cl_device_id devices[MAX_PLATFORM][MAX_DEVICE]; 
   cl_uint numDev[MAX_PLATFORM];
   cl_uint numPlatform;
   cl_context context;
   cl_program program[MAX_PLATFORM];

   // Get all OpenCL devices
   //(yes, that's a bad idea for a function)
   // Edits devices, numDev and NumPlatform
   err = getDevices(devices,MAX_PLATFORM,MAX_DEVICE,numDev,&numPlatform);
   if(err != 0){
      fprintf(stderr,"Error grabbing devices! Exiting!\n");
   }
   //We'll just grab a certain device for now
   context = clCreateContext(NULL, 1, &devices[0][0], NULL, NULL, &err);
   
   //Hardcoded filename - FIXME
   program[0] = build_program(context, devices[0][0], "itm_support.cl");
   if(err != 0){
      fprintf(stderr,"Error Compiling program! Exiting!\n");
   }

}
