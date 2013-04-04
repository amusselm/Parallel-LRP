
__kernel void test(__global double* input,__global double* result) {

   *result = *input*2;

}
