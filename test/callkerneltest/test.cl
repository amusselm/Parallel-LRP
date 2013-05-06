#include "test2.h"

__kernel void test(__global double* input, // - 0
                  double input2, // - 1
                  double input3, // - 2
                  double input4, // - 3
                  double input5, // - 4
                  double input6, // - 5
                  double input7, // - 6 
                  __global double* result) {
   //This line isn't going to work as expected due to limitations of the debug printf statement. 
   printf("input is: %lf input2 is: %lf input3 is: %lf\n",*input,input2,input3); 

   *result = *input*2+input2+TESTVAL*1000000;

}
