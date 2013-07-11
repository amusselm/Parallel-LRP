#!/bin/sh
cat psplatcombinedsize.h splat.h itwom3.0.cl itm_support.cl | sed -e '/\#include/d' > itm_all.cl
   
