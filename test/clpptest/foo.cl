

void print_test_4(){
   printf("Test In 4!\n");
}


void print_test_3(){
   printf("Test In 3!\n");
   print_test_4();
}

void print_test_2(int eh){
   int i = 999;
   printf("Test In 2! %d\n",i);
   print_test_3();
}

void bail(int *place){
   printf("Works?");
   *place = 199;
}

int rail(){
   int i;
   bail(&i);
   return i;
}

int fail(){
   int i = rail();
   return i;
}

void print_test_intel(){
   int i = fail();
   printf("Test_intel, %d!\n",i);
   printf("Test_intel Eh?!\n");
   print_test_2(23);
}



