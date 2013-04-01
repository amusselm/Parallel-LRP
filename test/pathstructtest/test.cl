#define ARRAYSIZE 5092 
typedef struct path_m {	
   double elevation[ARRAYSIZE];
   double distance;
   int length;
} path_m;


void PlotLRPath_test(
   const path_m testPath,
   double *loss_result
   )
{
   *loss_result = testPath.distance;
}

__kernel void test(__global double* result) {
   path_m testPath[2];
   double foo;
   testPath[0].distance = 123.4;
   testPath[1].distance = 567.8;
   printf("Run!\n");

   PlotLRPath_test(testPath[1],&foo); 
   *result = foo;
}

__kernel void test2(__global double* result) {
  *result = 31337.77; 
}
