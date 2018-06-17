#include "fact.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

int main(int argc, char  **argv) {
    int arg;
    int result;
    // Verifying that we recieve one command line argument
    assert(argc == 2);
    // setting our arg for fact to the input
    arg = atoi(argv[1]);
    // Verifying that our argument is non-negative
    assert(arg >= 0);
    // setting our result to our factorial function
    result = fact(arg);
    //printing
    printf("Result is: %d \n", result);
}
