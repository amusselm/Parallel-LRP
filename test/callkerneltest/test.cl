
__kernel void test(__global double* input,double input2, __global double* result) {

   *result = *input*2+input2;

}
