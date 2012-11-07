#define mem_size 203430*4 
//^ That number should be far larger than the amount of memory that 
// the LongelyRice code is creating for all of it's operations.

int getResult() {
   int foo[mem_size];
   for(int i = 0; i < mem_size; i++){
      foo[i] = i;
   }
   foo[99999] = 2321;
   foo[32798] = 12;
   return foo[89999];
}

__kernel void test(__global int* result) {

   *result = getResult();//This proves that the code ran
}
