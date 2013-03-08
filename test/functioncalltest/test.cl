struct LR {	
   float antenna_pattern[361][1001];
};

void PlotLRPath_test(
   struct LR LRM, 
   double *loss_result
   )
{
   *loss_result = 31337.2;
}

__kernel void test(__global double* result) {
   struct LR aLRStruct;
   double foo;

//   PlotLRPath_test(aLRStruct,&foo); 
   *result = foo;
}

__kernel void test2(__global double* result) {
  *result = 31337.77; 
}
