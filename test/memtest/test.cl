int getResult() {
   int foo[999999];
   for(int i = 0; i < 999999; i++){
      foo[i] = i;
   }
   foo[99999] = 2321;
   foo[32798] = 12;
   return foo[89999];
}

__kernel void test(__global int* result) {

   *result = getResult();//This proves that the code ran
}
