.globl gcd
gcd:
    orl     $0, %esi       # Sets zero-flag if b is equal to 0
    jnz     gcd_continue   #compute gcd if b is nonzero
    movl    %edi, %eax     # otherwise return a
    jmp     gcd_return

gcd_continue:
    movl    %edi, %eax    #set up for division
    cltd                  #set up for division
    idivl   %esi          # do 1-argument division by b
    movl    %esi, %edi    # move b into input slot for a
    movl    %edx, %esi    # move a mod b into input slot for b
    call    gcd           #calls our gcd function recursively

gcd_return:
    ret                   #finished
