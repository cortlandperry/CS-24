/* This file contains x86-64 assembly-language implementations of three
 * basic, very common math operations.
 * There is a common theme throughout all these functions. All of these 
 * functions try to avoid incorrectly predicting a branch, and to do this
 * they use conditional statements so that the computer processor can
 * overlap multiple instructions together. There will be no conditional
 * jumps in any of the below implementations, which thus makes them all
 * faster because once we get the compare or test statements, we can
 * predict where our code will go more efficiently.
 */

        .text

/*====================================================================
 * int f1(int x, int y)
 * This function takes inputs x and y, it compares x and y, and if x is 
 * greater than y, it returns x, and if x is not greater than y, it returns
 * y. Basically, it returns the maximum of the two inputs
 */
.globl f1
f1:
        movl    %edi, %edx      /* this moves x into edx */
        movl    %esi, %eax      /* this moves y into eax */
        cmpl    %edx, %eax      /* compares x:y */
        cmovg   %edx, %eax      /* if x > y, move x into  eax, otherwise y stays in eax */
        ret     /* result returned in eax */


/*====================================================================
 * int f2(int x)
 * This function computes the absolute value of x
 */
.globl f2
f2:
        movl    %edi, %eax  // moves x into eax
        movl    %eax, %edx  // moves x from eax into edx
        sarl    $31, %edx   
    // shifts edx 31 bits right,if x is negative we get all 1's, if it is
    //positive we get 0
        xorl    %edx, %eax  
//this flips all of the digits of x if x is negative, and keep them the 
//same if x is positive
        subl    %edx, %eax 
 //if x is positive, we subtract 0 from it so we get x, if x is negative, 
// we subtract 111....111 from it, and we get the negative of x, thus 
//giving us abs
        ret


/*====================================================================
 * int f3(int x)
 * This function computes the sign of x, and returns 1 if it is positive and 0 if it is 0 and -1 if it is negative. (checks the sign)
 */
.globl f3
f3:
        movl    %edi, %edx  // moves x into edx
        movl    %edx, %eax  // moves x into eax as well
        sarl    $31, %eax   //shifts eax 31 bits right, x negative:
// 1....1, x positive: 0
        testl   %edx, %edx  // this tests edx AND edx
        movl    $1, %edx    //move 1 into edx
        cmovg   %edx, %eax  //if edx AND edx was = 0 in the test, we dont
// do anything and we return 0 which was edx. if edx AND edx > 0, we will
// move 1 into eax because it will be greater. if edx AND edx < 0, we 
// will keep our negative bitmask in eax and it becomes -1. 
        ret

