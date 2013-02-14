#include "test_cl_header.h"

//elevation=((acos(cos_test_angle))/DEG2RAD)-90.0
// ^ double   ^returns double  ^ double  ^double ^--also a double
//The existance of that line in another file causes the Intel OpenCL compiler
//to fail silently. This is an attempt to reproduce this issue in a minimal
// envrioment. 

double getResult() {
   double foo = ((acos(.23))/TEST_NUM)-23.2; 
   return foo;
}

__kernel void test(__global double* result) {
   *result = getResult();//This proves that the code ran
}

__kernel void test2(__global double* result) {
  *result = 31337.77; 
}
