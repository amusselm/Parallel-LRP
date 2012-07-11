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

/* This function returns the elevation (in feet) of any location
 *  represented by the digital elevation model data in memory.
 *  Function returns -5000.0 for locations not found in memory.  
 * @param location Location to get elevation data for 
 * @param dem Pointer to the elevation data structure
 *
 */
double GetElevation(struct site location, struct dem dem[])
{

	char	found;
	int	x, y, indx;
	double	elevation;

	for (indx=0, found=0; indx<MAXPAGES && found==0;)
	{
		x=(int)rint(ppd*(location.lat-dem[indx].min_north));
		y=mpi-(int)rint(ppd*(LonDiff(dem[indx].max_west,location.lon)));

		if (x>=0 && x<=mpi && y>=0 && y<=mpi)
			found=1;
		else
			indx++;
	}

	if (found)
		elevation=3.28084*dem[indx].data[x][y];
	else
		elevation=-5000.0;
	
	return elevation;
}

double Azimuth(struct site source, struct site destination)
{
	/* This function returns the azimuth (in degrees) to the
	   destination as seen from the location of the source. */

	double	dest_lat, dest_lon, src_lat, src_lon,
		beta, azimuth, diff, num, den, fraction;

	dest_lat=destination.lat*DEG2RAD;
	dest_lon=destination.lon*DEG2RAD;

	src_lat=source.lat*DEG2RAD;
	src_lon=source.lon*DEG2RAD;
		
	/* Calculate Surface Distance */

	beta=acos(sin(src_lat)*sin(dest_lat)+cos(src_lat)*cos(dest_lat)*cos(src_lon-dest_lon));

	/* Calculate Azimuth */

	num=sin(dest_lat)-(sin(src_lat)*cos(beta));
	den=cos(src_lat)*sin(beta);
	fraction=num/den;

	/* Trap potential problems in acos() due to rounding */

	if (fraction>=1.0)
		fraction=1.0;

	if (fraction<=-1.0)
		fraction=-1.0;

	/* Calculate azimuth */

	azimuth=acos(fraction);

	/* Reference it to True North */

	diff=dest_lon-src_lon;

	if (diff<=-PI)
		diff+=TWOPI;

	if (diff>=PI)
		diff-=TWOPI;

	if (diff>0.0)
		azimuth=TWOPI-azimuth;

	return (azimuth/DEG2RAD);		
}

/** 
 * This function generates a sequence of latitude and
 *  longitude positions between source and destination
 *  locations along a great circle path, and stores
 *  elevation and distance information for points
 *   along that path in the "path" structure. 
 * @param source 
 * @param destination
 * @param path The path structure to read information into
 * @param dem Terrain Elevation data structure
 */

void ReadPath(struct site source, struct site destination, struct *path, 
   const struct dem dem[])
{

	int	c;
	double	azimuth, distance, lat1, lon1, beta, den, num,
		lat2, lon2, total_distance, dx, dy, path_length,
		miles_per_sample, samples_per_radian=68755.0;
	struct	site tempsite;

	lat1=source.lat*DEG2RAD;
	lon1=source.lon*DEG2RAD;

	lat2=destination.lat*DEG2RAD;
	lon2=destination.lon*DEG2RAD;

	if (ppd==1200.0)
		samples_per_radian=68755.0;

	if (ppd==3600.0)
		samples_per_radian=206265.0;

	azimuth=Azimuth(source,destination)*DEG2RAD;

	total_distance=Distance(source,destination);

	if (total_distance>(30.0/ppd))		/* > 0.5 pixel distance */
	{
		dx=samples_per_radian*acos(cos(lon1-lon2));
		dy=samples_per_radian*acos(cos(lat1-lat2));

		path_length=sqrt((dx*dx)+(dy*dy));		/* Total number of samples */

		miles_per_sample=total_distance/path_length;	/* Miles per sample */
	}

	else
	{
		c=0;
		dx=0.0;
		dy=0.0;
		path_length=0.0;
		miles_per_sample=0.0;
		total_distance=0.0;

		lat1=lat1/DEG2RAD;
		lon1=lon1/DEG2RAD;

		path->lat[c]=lat1;
		path->lon[c]=lon1;
		path->elevation[c]=GetElevation(source,dem);
		path->distance[c]=0.0;
	}

	for (distance=0.0, c=0; (total_distance!=0.0 && distance<=total_distance && c<ARRAYSIZE); c++, distance=miles_per_sample*(double)c)
	{
		beta=distance/3959.0;
		lat2=asin(sin(lat1)*cos(beta)+cos(azimuth)*sin(beta)*cos(lat1));
		num=cos(beta)-(sin(lat1)*sin(lat2));
		den=cos(lat1)*cos(lat2);

		if (azimuth==0.0 && (beta>HALFPI-lat1))
			lon2=lon1+PI;

		else if (azimuth==HALFPI && (beta>HALFPI+lat1))
				lon2=lon1+PI;

		else if (fabs(num/den)>1.0)
				lon2=lon1;

		else
		{
			if ((PI-azimuth)>=0.0)
				lon2=lon1-arccos(num,den);
			else
				lon2=lon1+arccos(num,den);
		}
	
		while (lon2<0.0)
			lon2+=TWOPI;

		while (lon2>TWOPI)
			lon2-=TWOPI;
 
		lat2=lat2/DEG2RAD;
		lon2=lon2/DEG2RAD;

		path->lat[c]=lat2;
		path->lon[c]=lon2;
		tempsite.lat=lat2;
		tempsite.lon=lon2;
		path->elevation[c]=GetElevation(tempsite,dem);
		path->distance[c]=distance;
	}

	/* Make sure exact destination point is recorded at path.length-1 */

	if (c<ARRAYSIZE)
	{
		path.lat[c]=destination.lat;
		path.lon[c]=destination.lon;
		path.elevation[c]=GetElevation(destination,dem);
		path.distance[c]=total_distance;
		c++;
	}

	if (c<ARRAYSIZE)
		path.length=c;
	else
		path.length=ARRAYSIZE-1;
}



void PlotLRPath(struct site source, struct site destination, unsigned char mask_value, struct LR LR)
{
	/* This function plots the RF path loss between source and
	   destination points based on the ITWOM propagation model,
	   taking into account antenna pattern data, if available. */

	int	x, y, ifs, ofs, errnum;
	char	block=0, strmode[100];
	double	loss, azimuth, pattern=0.0, xmtr_alt,
		dest_alt, xmtr_alt2, dest_alt2, cos_rcvr_angle,
		cos_test_angle=0.0, test_alt, elevation=0.0,
		distance=0.0, four_thirds_earth, rxp, dBm,
		field_strength=0.0;
   double elev[ARRAYSIZE+10];//Formerly a global variable
   struct path;
	struct	site temp;

	ReadPath(source,destination,&path);

	four_thirds_earth=FOUR_THIRDS*EARTHRADIUS;

	/* Copy elevations plus clutter along path into the elev[] array. */

	for (x=1; x<path.length-1; x++)
		elev[x+2]=(path.elevation[x]==0.0?path.elevation[x]*METERS_PER_FOOT:(clutter+path.elevation[x])*METERS_PER_FOOT);

	/* Copy ending points without clutter */

	elev[2]=path.elevation[0]*METERS_PER_FOOT;
	elev[path.length+1]=path.elevation[path.length-1]*METERS_PER_FOOT;

	/* Since the only energy the propagation model considers
	   reaching the destination is based on what is scattered
	   or deflected from the first obstruction along the path,
	   we first need to find the location and elevation angle
	   of that first obstruction (if it exists).  This is done
	   using a 4/3rds Earth radius to match the radius used by
	   the irregular terrain propagation model.  This information
	   is required for properly integrating the antenna's elevation
	   pattern into the calculation for overall path loss. */

	for (y=2; (y<(path.length-1) && path.distance[y]<=max_range); y++)
	{
		/* Process this point only if it
		   has not already been processed. */
      //THIS NEEDS TO BE SYNCHRONIZED ACROSS THREADS!!!!!!

		if ((GetMask(path.lat[y],path.lon[y])&248)!=(mask_value<<3))
		{
			distance=5280.0*path.distance[y];
			xmtr_alt=four_thirds_earth+source.alt+path.elevation[0];
			dest_alt=four_thirds_earth+destination.alt+path.elevation[y];
			dest_alt2=dest_alt*dest_alt;
			xmtr_alt2=xmtr_alt*xmtr_alt;

			/* Calculate the cosine of the elevation of
			   the receiver as seen by the transmitter. */

			cos_rcvr_angle=((xmtr_alt2)+(distance*distance)-(dest_alt2))/(2.0*xmtr_alt*distance);

			if (cos_rcvr_angle>1.0)
				cos_rcvr_angle=1.0;

			if (cos_rcvr_angle<-1.0)
				cos_rcvr_angle=-1.0;

			if (got_elevation_pattern || fd!=NULL)
			{
				/* Determine the elevation angle to the first obstruction
				   along the path IF elevation pattern data is available
				   or an output (.ano) file has been designated. */

				for (x=2, block=0; (x<y && block==0); x++)
				{
					distance=5280.0*path.distance[x];

					test_alt=four_thirds_earth+(path.elevation[x]==0.0?path.elevation[x]:path.elevation[x]+clutter);

					/* Calculate the cosine of the elevation
					   angle of the terrain (test point)
					   as seen by the transmitter. */

					cos_test_angle=((xmtr_alt2)+(distance*distance)-(test_alt*test_alt))/(2.0*xmtr_alt*distance);

					if (cos_test_angle>1.0)
						cos_test_angle=1.0;

					if (cos_test_angle<-1.0)
						cos_test_angle=-1.0;

					/* Compare these two angles to determine if
					   an obstruction exists.  Since we're comparing
					   the cosines of these angles rather than
					   the angles themselves, the sense of the
					   following "if" statement is reversed from
				  	   what it would be if the angles themselves
					   were compared. */

					if (cos_rcvr_angle>=cos_test_angle)
						block=1;
				}

				if (block)
					elevation=((acos(cos_test_angle))/DEG2RAD)-90.0;
				else
					elevation=((acos(cos_rcvr_angle))/DEG2RAD)-90.0;
			}

			/* Determine attenuation for each point along
			   the path using ITWOM's point_to_point mode
			   starting at y=2 (number_of_points = 1), the
			   shortest distance terrain can play a role in
			   path loss. */
 
			elev[0]=y-1;  /* (number of points - 1) */

			/* Distance between elevation samples */

			elev[1]=METERS_PER_MILE*(path.distance[y]-path.distance[y-1]);

         point_to_point(elev,source.alt*METERS_PER_FOOT, 
         destination.alt*METERS_PER_FOOT, LR.eps_dielect,
         LR.sgm_conductivity, LR.eno_ns_surfref, LR.frq_mhz,
         LR.radio_climate, LR.pol, LR.conf, LR.rel, loss,
         strmode, errnum);

			temp.lat=path.lat[y];
			temp.lon=path.lon[y];

			azimuth=(Azimuth(source,temp));

			/* Integrate the antenna's radiation
			   pattern into the overall path loss. */

			x=(int)rint(10.0*(10.0-elevation));

			if (x>=0 && x<=1000)
			{
				azimuth=rint(azimuth);

				pattern=(double)LR.antenna_pattern[(int)azimuth][x];

				if (pattern!=0.0)
				{
					pattern=20.0*log10(pattern);
					loss-=pattern;
				}
			}

			if (LR.erp!=0.0)
			{
				if (dbm)
				{
					/* dBm is based on EIRP (ERP + 2.14) */

					rxp=LR.erp/(pow(10.0,(loss-2.14)/10.0));

					dBm=10.0*(log10(rxp*1000.0));

					if (fd!=NULL)
						fprintf(fd,"%.3f",dBm);

					/* Scale roughly between 0 and 255 */

					ifs=200+(int)rint(dBm);

					if (ifs<0)
						ifs=0;

					if (ifs>255)
						ifs=255;

					ofs=GetSignal(path.lat[y],path.lon[y]);

					if (ofs>ifs)
						ifs=ofs;

					PutSignal(path.lat[y],path.lon[y],(unsigned char)ifs);
				}

				else
				{
					field_strength=(139.4+(20.0*log10(LR.frq_mhz))-loss)+(10.0*log10(LR.erp/1000.0));

					ifs=100+(int)rint(field_strength);

					if (ifs<0)
						ifs=0;

					if (ifs>255)
						ifs=255;

					ofs=GetSignal(path.lat[y],path.lon[y]);

					if (ofs>ifs)
						ifs=ofs;

					PutSignal(path.lat[y],path.lon[y],(unsigned char)ifs);
	
					if (fd!=NULL)
						fprintf(fd,"%.3f",field_strength);
				}
			}

			else
			{
				if (loss>255)
					ifs=255;
				else
					ifs=(int)rint(loss);

				ofs=GetSignal(path.lat[y],path.lon[y]);

				if (ofs<ifs && ofs!=0)
					ifs=ofs;

				PutSignal(path.lat[y],path.lon[y],(unsigned char)ifs);
			}

			if (fd!=NULL)
			{
				if (block)
					fprintf(fd," *");

				fprintf(fd,"\n");
			}

			/* Mark this point as having been analyzed */

			PutMask(path.lat[y],path.lon[y],(GetMask(path.lat[y],path.lon[y])&7)+(mask_value<<3));
		}
	}
}
