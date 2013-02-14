#define TEST_NUM 10

//elevation=((acos(cos_test_angle))/DEG2RAD)-90.0
// ^ double   ^returns double  ^ double  ^double ^--also a double
//The existance of that line in another file causes the Intel OpenCL compiler
//to fail silently. This is an attempt to reproduce this issue in a minimal
// envrioment. 

int getResult() {
   return 10001/TEST_NUM+1; 
}

__kernel void test(__global int* result) {
   *result = getResult();//This proves that the code ran
}
