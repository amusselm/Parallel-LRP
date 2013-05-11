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

void PlotLRPath_test(struct site source, 
   double altitude, 
   struct LR_min LRM, 
   const path_m path,
   const double clutter, const double max_range, 
   const unsigned char got_elevation_pattern, 
   const unsigned char dbm,
   int path_point,
   double *loss_result)
{
   *loss_result = 31337.2;
}

void PlotLRPath(struct site source, double altitude, 
   struct LR_min LR, 
   const double distanceInput,
   const int pathLength,
   double *terrainAlt, 
   const double clutter, const double max_range, 
   const unsigned char got_elevation_pattern, 
   const unsigned char dbm,
   int path_point,
   double *loss_result)
{
	/* This function plots the RF path loss between source and
	   destination points based on the ITWOM propagation model,
	   taking into account antenna pattern data, if available. */
   int y = path_point;

	int	x, ifs, ofs, errnum;
	char	block=0, strmode[100];
	double	loss, azimuth, pattern=0.0, xmtr_alt,
		dest_alt, xmtr_alt2, dest_alt2, cos_rcvr_angle,
		cos_test_angle=0.0, test_alt, elevation=0.0,
		distance=0.0, four_thirds_earth, rxp, dBm,
		field_strength=0.0;
   double elev[ARRAYSIZE+10]; /* Formerly a global variable */
	struct	site temp;

	four_thirds_earth=FOUR_THIRDS*EARTHRADIUS;

	/* Copy elevations plus clutter along path into the elev[] array. */

	for (x=1; x<pathLength-1; x++){
		elev[x+2]=(terrainAlt[x]==0.0?terrainAlt[x]*METERS_PER_FOOT:(clutter+terrainAlt[x])*METERS_PER_FOOT);
   }


	/* Copy ending points without clutter */

	elev[2]=terrainAlt[0]*METERS_PER_FOOT;
	elev[pathLength+1]=terrainAlt[pathLength-1]*METERS_PER_FOOT;

	/* Since the only energy the propagation model considers
	   reaching the destination is based on what is scattered
	   or deflected from the first obstruction along the path,
	   we first need to find the location and elevation angle
	   of that first obstruction (if it exists).  This is done
	   using a 4/3rds Earth radius to match the radius used by
	   the irregular terrain propagation model.  This information
	   is required for properly integrating the antenna's elevation
	   pattern into the calculation for overall path loss. */

	if(y<(pathLength-1) && distanceInput*y<=max_range)
	{
      distance=5280.0*distanceInput*y;
      xmtr_alt=four_thirds_earth+source.alt+terrainAlt[0];
      dest_alt=four_thirds_earth+altitude+terrainAlt[y];
      dest_alt2=dest_alt*dest_alt;
      xmtr_alt2=xmtr_alt*xmtr_alt;


      /* Determine attenuation for each point along
         the path using ITWOM's point_to_point mode
         starting at y=2 (number_of_points = 1), the
         shortest distance terrain can play a role in
         path loss. */

      elev[0]=y-1;  /* (number of points - 1) */

      /* Distance between elevation samples */

      elev[1]=METERS_PER_MILE*(distanceInput);

      point_to_point(elev,source.alt*METERS_PER_FOOT, 
      altitude*METERS_PER_FOOT, LR.eps_dielect,
      LR.sgm_conductivity, LR.eno_ns_surfref, LR.frq_mhz,
      LR.radio_climate, LR.pol, LR.conf, LR.rel, &loss,
      strmode, &errnum);


      *loss_result = loss;
   }
}

__kernel void PlotLRPaths_cl(
   __global struct site *source,//0 - Transmitter site
   __global double *altitude,//1 - Receiver Altitude
   __global double *terrainAlt, // 2 - Path Terrain Altitude
   __global double *distance,//3 - Distance Array
   __global int *length, //4 - Path Array Length Array
   __global struct LR_min *LR,//5 - Propigation attributes
   __global double *paths,//6 - Array of Paths
   __global double *pathDist,//7- Array of Path distances
   __global int *pathLength,//8- Array of Path distances
   __constant  const double *clutter,//9 - Clutter Height
   __constant const double *max_range,//10
   __global unsigned char *got_elevation_pattern,//11
   __global unsigned char *dbm,//12
   __global double *loss,//13 - Loss Results
   __global size_t *pathArraySize //14 - Size of each path 
   ){
   int pathId = get_global_id(0);
   int pointId = get_global_id(1);

   double pointLoss;
    
   PlotLRPath(*source,
      *altitude,
     *LR,
      paths[pathId],*clutter,*max_range,
      *got_elevation_pattern,*dbm,pointId,
      &pointLoss);

   loss[pathId*(*pathArraySize)+pointId] = pointLoss;
}

__kernel void Test_cl(   ){
   int id = get_global_id(0);
   printf("Id is: %d\n",id);
}

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
     
      itm_elev[0] = id*1.00;
      itm_elev[1] = path_dist;
      for(int i = 0; i < elev_size; i++){
         itm_elev[i+2]=elev[i];
      }
      point_to_point(itm_elev,tht_m, rht_m, eps_dielect, sgm_conductivity, 
         eno_ns_surfref, frq_mhz, radio_climate, pol, conf, rel, &loss,
         strmode,&errnum);
      dbloss[id] = loss;
}
