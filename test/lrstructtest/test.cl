#define ARRAYSIZE 5092 
#include "lrtest.h"

__kernel void test(__global struct LR_min *LR,__global double* result) {
   printf("Run!\n");

   *result = LR->frq_mhz*LR->eps_dielect*3;
}

