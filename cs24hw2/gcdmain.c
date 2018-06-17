#include "gcd.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

int main(int argc, char  **argv) {
    int a;
    int b;
    int placeholder;
    int result;
    placeholder = 0;
    // Verifying that we recieve one command line argument
    assert(argc == 3);
    // setting our a, b to input to input line
    a = atoi(argv[1]);
    b = atoi(argv[2]);
    // Verifying that our arguments are non-negative
    assert(a >= 0);
    assert (b >= 0);

    // make sure a is greater than b
    if (b > a) {
      placeholder = a;
      a = b;
      b = placeholder;
    }

    // setting our result to our factorial function
    result = gcd(a, b);
    //printing
    printf("Result is: %d \n", result);
}
