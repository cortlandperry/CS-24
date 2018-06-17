#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#include "ffunc.h"


/* This function takes an array of single-precision floating point values,
 * and computes a sum in the order of the inputs.  Very simple.
 */
float fsum(FloatArray *floats) {
    float sum = 0;
    int i;

    for (i = 0; i < floats->count; i++)
        sum += floats->values[i];

    return sum;
}

/* This function calculates and returns the sum of a FloatArray of floats 
 * by using the pairwise sum algorithm */
float my_fsum(FloatArray *floats) {
	int size;
	int halfsize;
	float sum = 0;
	int i;
	size = floats->count; /* determine the size of our array */
	halfsize = size / 2;	/* calculates the size of half our array */
	
	
	
	
	/* Base Case: If our array is size 2 or less, we sum and return
	 * the elements in our FloatArray */
	if (size <= 2) {
		for (i = 0; i < floats->count; i++) {
			sum += floats->values[i];
		}
	}
	/* Recursively "Divide and Conquer": split our array into two halves,
	 * and run my_fsum on each half, then sum the sums of those resulting
	 * halves */
	else {
		
		/* allocate memory for the structures of our FloatArrays for the
		 * first and second half */
		FloatArray *half1 = (FloatArray *)malloc(sizeof(FloatArray));
		FloatArray *half2 = (FloatArray *)malloc(sizeof(FloatArray));
		
		/* make sure we allocate memory */
		if (half1 == NULL || half2 == NULL) {
			fprintf(stderr, "Fatal error: out of memory. "
					"Terminating program.\n");
			exit(1);
		}
		
		/* allocate memory for the arrays inside of our FloatArray
		 * structure */
		half1->values = (float *) malloc(sizeof(float) * (halfsize));
		half2->values = (float *) malloc(sizeof(float) * (size - halfsize));
		
		/* make sure we allocated memory correctly */
		if (half1->values == NULL || half2->values == NULL) {
			fprintf(stderr, "Fatal error: out of memory. "
					"Terminating program.\n");
			exit(1);
		}
		
		/* give the count of half1 and half2 correct values */
		half1->count = halfsize;
		half2->count = size - halfsize;
		
		/* place the values in our float array into our half1 and half2
		 * array */
		for (i = 0; i < halfsize; i++) {
			half1->values[i] = floats->values[i];
		}
		for (i = halfsize; i < size; i++) {
			half2->values[i - halfsize] = floats->values[i];
		}
		
		/* recursively call my_fsum and make it our sum */
		sum = my_fsum(half1) + my_fsum(half2);
		
		/* free the memory that we allocated */
		free(half1->values);
		free(half2->values);
		free(half1);
		free(half2);
		
	}
    return sum;
}


int main() {
    FloatArray floats;
    float sum1, sum2, sum3, my_sum;

    load_floats(stdin, &floats);
    printf("Loaded %d floats from stdin.\n", floats.count);

    /* Compute a sum, in the order of input. */
    sum1 = fsum(&floats);

    /* Use my_fsum() to compute a sum of the values.  Ideally, your
     * summation function won't be affected by the order of the input floats.
     */
    my_sum = my_fsum(&floats);

    /* Compute a sum, in order of increasing magnitude. */
    sort_incmag(&floats);
    sum2 = fsum(&floats);

    /* Compute a sum, in order of decreasing magnitude. */
    sort_decmag(&floats);
    sum3 = fsum(&floats);

    /* %e prints the floating-point value in full precision,
     * using scientific notation.
     */
    printf("Sum computed in order of input:  %e\n", sum1);
    printf("Sum computed in order of increasing magnitude:  %e\n", sum2);
    printf("Sum computed in order of decreasing magnitude:  %e\n", sum3);

    printf("My sum:  %e\n", my_sum);
    

    return 0;
}

