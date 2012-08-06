#ifndef itwomclwrapper_h
#define itwomclwrapper_h

#ifdef MAC
#include <OpenCL/cl.h>
#else
#include <CL/cl.h>
#endif

#include "clutil.h"

void foo();

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
   unsigned char dbm);
#endif
