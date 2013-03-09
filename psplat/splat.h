/*
 * Parameters for 1 arc-second high resolution mode of operation
 * Generated by ./configure by amusselm on Sat Jul 7 16:34:52 PDT 2012
 * Modified by hand as part of the OpenCL reconfigureing...
 * Andrew J. Musselman, KI6UOC
*/
#ifndef splat_h
#define splat_h

/* We always use HD mode for testing */
#define HD_MODE 1
/* Smaller number of pages for testing */
#define MAXPAGES 1

/**
 * Bunch of preproscessor definations. These exist so that both host and device
 * code know how big the statically allocated arrays are
 */
#if HD_MODE==0
	#if MAXPAGES==4
	#define ARRAYSIZE 4950
	#endif

	#if MAXPAGES==9
	#define ARRAYSIZE 10870
	#endif

	#if MAXPAGES==16
	#define ARRAYSIZE 19240
	#endif

	#if MAXPAGES==25
	#define ARRAYSIZE 30025
	#endif

	#if MAXPAGES==36
	#define ARRAYSIZE 43217
	#endif

	#if MAXPAGES==49
	#define ARRAYSIZE 58813
	#endif

	#if MAXPAGES==64
	#define ARRAYSIZE 76810
	#endif

	#define IPPD 1200
#endif

#if HD_MODE==1
	#if MAXPAGES==1
	#define ARRAYSIZE 5092 
	#endif

	#if MAXPAGES==4
	#define ARRAYSIZE 14844 
	#endif

	#if MAXPAGES==9
	#define ARRAYSIZE 32600
	#endif

	#if MAXPAGES==16
	#define ARRAYSIZE 57713
	#endif

	#if MAXPAGES==25
	#define ARRAYSIZE 90072
	#endif

	#if MAXPAGES==36
	#define ARRAYSIZE 129650
	#endif

	#if MAXPAGES==49 
	#define ARRAYSIZE 176437
	#endif

	#if MAXPAGES==64
	#define ARRAYSIZE 230430
	#endif

	#define IPPD 3600
#endif

#include "psplatcombinedsize.h"

#ifndef PI
#define PI 3.141592653589793
#endif

#ifndef TWOPI
#define TWOPI 6.283185307179586
#endif

#ifndef HALFPI
#define HALFPI 1.570796326794896
#endif

#define DEG2RAD 1.74532925199e-02
#define EARTHRADIUS 20902230.97
#define	METERS_PER_MILE 1609.344
#define METERS_PER_FOOT 0.3048
#define	KM_PER_MILE 1.609344
#define FOUR_THIRDS 1.3333333333333


struct site {	
   double lat;
   double lon;
   float alt;
   char name[50];
   char filename[255];
};

/**
 * Represents the terrain data, and keeps track of which points have already been
 * calculated
 */
struct dem {	
   int min_north;
   int max_north;
   int min_west;
   int max_west;
   int max_el;
   int min_el;
   short data[IPPD][IPPD];
   unsigned char mask[IPPD][IPPD];
   unsigned char signal[IPPD][IPPD];
};	

/**
 * Stores the Longely-Rice paramaters
 */
struct LR {	
   double eps_dielect; 
   double sgm_conductivity; 
   double eno_ns_surfref;
   double frq_mhz; 
   double conf; 
   double rel;
   double erp;
   int radio_climate;  
   int pol;
   float antenna_pattern[361][1001];
};

/**
 * Longly-Rice Paramaters without antenna elevation pattern
 */
struct LR_min {	
   double eps_dielect; 
   double sgm_conductivity; 
   double eno_ns_surfref;
   double frq_mhz; 
   double conf; 
   double rel;
   double erp;
   int radio_climate;  
   int pol;
};


typedef struct path {	
   double lat[ARRAYSIZE];
   double lon[ARRAYSIZE];
   double elevation[ARRAYSIZE];
   double distance[ARRAYSIZE];
   int length;
} path_t;

typedef struct path_m {	
   double elevation[ARRAYSIZE];
   double distance;
   int length;
} path_m;

#endif
