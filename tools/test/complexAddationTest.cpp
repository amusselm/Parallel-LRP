/*
 * A test of what happens when a complex to a double
 */

#include<complex>
#include<stdio.h>

using namespace std;

int main(int argc, char* argv[]){
   complex<double> foo(1.0,1.0);
   printf("real: %lf, imag %lf \n",foo.real(),foo.imag());
   foo = foo+1.0;
   printf("real: %lf, imag %lf \n",foo.real(),foo.imag());
   foo = foo/2;
   printf("real: %lf, imag %lf \n",foo.real(),foo.imag());

}
