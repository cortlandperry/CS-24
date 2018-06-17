#include <stdio.h>
#include "my_setjmp.h"

static jmp_buf env;

void long_jmp_0(jmp_buf buf);
void long_jmp_n(jmp_buf buf, int n);
void function_jumper1(jmp_buf buf);
void function_jumper2(jmp_buf buf);
void stack_corrupt(jmp_buf buf);

/* This will call all of our functions that we created */
int main(int argc, char ** argv) {
    long_jmp_0(env);
    long_jmp_n(env, 1);
    long_jmp_n(env, 7);
    function_jumper1(env);
    stack_corrupt(env);
}


/* This function will test that once we call longjmp(buf, 0), then setjmp will
* return 1. It will print messages for PASS and FAIL */
void long_jmp_0(jmp_buf buf) {
    int i;
    i = 0;

    /* on the first call of setjmp, change the value of i */
    if (setjmp(buf) == 0) {
        i = 1;
    }
    else if(setjmp(buf) == 1) {
        printf("PASSED: longjmp(buf, 0) returns 1 \n");
        return;
    }
    /* after the first call, call longjmp */
    if(i == 1) {
      longjmp(buf, 0);
    }
    printf("FAILED: longjmp(buf, 0) does not return 1 \n");
}

/* This function will test that once we call longjmp(buf, n), then setjmp will
* return n. It will print messages for PASS and FAIL */
void long_jmp_n(jmp_buf buf, int n) {
    int i;
    i = 0;
    /* run setjmp first and show that it is 0, then set our counter to 1 */
    if (setjmp(buf) == 0) {
        i = 1;
    }
    /* make sure that longjmp returns n */
    else if(setjmp(buf) == n) {
        printf("PASSED: longjmp(buf, %d) returns %d \n", n, n);
        return;
    }

    /* run long jmp, and have it throw back to setjmp, if setjmp was called */
    if(i == 1) {
      longjmp(buf, n);
    }
    printf("FAILED: longjmp(buf, %d) does not return %d \n", n, n);
}

/* this function tests whether or not longjmp can jump in between functions */
void function_jumper1(jmp_buf buf) {
    /* if it is the first time we are at setjmp, we return the next function
    that we have to do. When we come back after longjmp, we will not have
    setjmp equal to 0, so it will not go in this if statement. */
    if (setjmp(buf) == 0) {
        return function_jumper2(buf);
    }

    printf("PASSED: longjmp was able to jump accross functions \n");
    return;

}

/* this function is the second function that tests whether or not longjmp can
jump in between functions */
void function_jumper2(jmp_buf buf) {
    longjmp(buf, 10);
    /* if it reaches here, then we know that longjmp did not jump back to the
    other function, so it failed our jumping across functions test */
    printf("FAILED: longjmp was not able to jump accross functions \n");
    return;
}

/* This function checks whetheer or not the stack is corrupted after calling
our implementation of setjmp and longjmp */
void stack_corrupt(jmp_buf buf) {
    int v1;
    int v2;
    v1 = 1;
    v2 = 1;
    if (setjmp(buf) == 0) {
        v2 = 3;
    }
    else {
      if(v1 == 1 && v2 == 3) {
          printf("PASSED: The stack was not corrupted. Values at the front and back of the jmp_buf were not affected \n");
      }
      else {
          printf("FAILED: The stack was corrupted: Jmp_buf does not affect front and back of stack \n");
      }
      return;
    }
    longjmp(buf, 0);
}
