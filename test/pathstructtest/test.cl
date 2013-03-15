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
   *loss_result = 1337.2;
}

__kernel void test(__global double* result) {
   path_m testPath;
   double foo;

   PlotLRPath_test(testPath,&foo); 
   *result = foo;
}

__kernel void test2(__global double* result) {
  *result = 31337.77; 
}
