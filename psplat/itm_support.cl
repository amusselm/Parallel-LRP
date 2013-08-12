/****************************************************************************\
*         SPLAT!: An RF Signal Path Loss And Terrain Analysis Tool          *
******************************************************************************
*           Project started in 1997 by John A. Magliacane, KD2BD            *
*             Parallel OpenCL modifications by Andrew J. Musselman, KI6UOC   *
******************************************************************************
*         Please consult the documentation for a complete list of           *
*           individuals who have contributed to this project.                  *
******************************************************************************
*                                                                            *
*  This program is free software; you can redistribute it and/or modify it   *
*  under the terms of the GNU General Public License as published by the     *
*  Free Software Foundation; either version 2 of the License or any later    *
*  version.                                                     *
*                                                            *
*  This program is distributed in the hope that it will useful, but WITHOUT  *
*  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or     *
*  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License     *
*  for more details.                                               
*                                                           *
******************************************************************************
* This file contains various support functions for the ITWOM. These functions *
* inclue the OpenCL _kernel function, which is the interface between the host *
* and device code, 0n addation to code that generates a Path from the terrain *
* data structure
\****************************************************************************/
#include "psplatcombinedsize.h"

#include "itwom3.0.cl"

__kernel void point_to_point_cl(
   int elev_size, //0 - Number of terreain hieghts in the elev array
   __global double *elev, //1 - Eevation array
   double path_dist, //2 - Total length of the path
   double tht_m, //3 - Source altitude (meters) 
   double rht_m, //4 - Destination Altitude 
   double eps_dielect,//5 
   double sgm_conductivity,//6 
   double eno_ns_surfref,//7 
   double frq_mhz, //8 - Frequency in Mhz 
   int radio_climate, //9 
   int pol,//10 - Polarity 
   double conf,// 11 
   double rel,// 12
   __global double *dbloss// 13 - Path loss results 
   ){
      int id = get_global_id(0);
      double itm_elev[ARRAYSIZE+2];
      double loss;
      char strmode[10000];
      int errnum;
     
      itm_elev[0] = (id-1)*1.00;
      itm_elev[1] = path_dist;
      for(int i = 0; i < elev_size; i++){
         itm_elev[i+2]=elev[i];
      }
      point_to_point(itm_elev,tht_m, rht_m, eps_dielect, sgm_conductivity, 
         eno_ns_surfref, frq_mhz, radio_climate, pol, conf, rel, &loss,
         strmode,&errnum);
      dbloss[id] = loss;
}
