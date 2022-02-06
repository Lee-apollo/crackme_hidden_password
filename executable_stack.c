// gcc executable_stack.c && ./a.out 

// Example source code from StackOverflow - shows how to create an executable memory location using dynamic memory allocation.
// This does not require any special compiler switch.

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include <sys/mman.h>

int function1(int x){
   return x-5;
}

int function2(int x){
  return x+5;
}


int main(){
  int diff = (&function2 - &function1);
  printf("pagesize: %d, diff: %d\n",getpagesize(),diff);

  int (*fptr)(int);

  void *memfun = aligned_alloc(4096, 4096);

  if (mprotect(memfun, 4096, PROT_READ|PROT_EXEC|PROT_WRITE) == -1) {
      perror ("mprotect");
  }

  memcpy(memfun, (const void*)&function2, diff);

  fptr = &function1;
  printf("native: %d\n",(*fptr)(6));
  fptr = memfun;
  printf("memory: %d\n",(*fptr)(6) );
  fptr = &function1;
  printf("native: %d\n",(*fptr)(6));

  free(memfun);
  return 0;
}

