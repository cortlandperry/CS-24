#============================================================================
# my_setjmp:  move all of our enviornment into our jump_buf, and return 0. Like
#             the regular setjmp.
#
# Arguments are called in this convetion:
#             %rdi = the jmp_buf argument. This is where the current enviornment
#              will be stored.
#
# The return value is returned in eax.
#


.globl my_setjmp
my_setjmp:
    mov   %rbp, (%rdi)        # move callee-save registers into rdi.
    mov   %rbx, 8(%rdi)
    mov   %r12, 16(%rdi)
    mov   %r13, 24(%rdi)
    mov   %r14, 32(%rdi)
    mov   %r15, 40(%rdi)
    mov   %rsp, 48(%rdi)      # save the stack pointer into rdi (jmp_buf)

    mov   (%rsp), %r10        # save the return address in rdi have to reference
    mov   %r10, 56(%rdi)      # to another register to do this

    mov   $0, %eax            # move zero into the return address
    ret


#============================================================================
# my_longjmp:  restore our execution state from jump_buf. Thus this appears as
#              if setjmp is running a second time. It returns any value but 0.
#
# Arguments are called in this convetion:
#             %rdi = the jmp_buf argument. This is where the current enviornment
#              will be stored.
#             %rsi = the number that will be returned
#
# The return value is returned in eax.
#


.globl my_longjmp
my_longjmp:
    mov   (%rdi), %rbp          #restoring values from our jmp_buf into values
    mov   8(%rdi), %rbx
    mov   16(%rdi), %r12
    mov   24(%rdi), %r13
    mov   32(%rdi), %r14
    mov   40(%rdi), %r15
    mov   48(%rdi), %rsp

    mov   56(%rdi), %r10        #restore the return address. Need to dereference
    mov   %r10, (%rsp)          # the pointer and mix it.

    cmp   $0, %esi              # if our return is 0, we return 1, otherwise
    je    esi_zero              # we return what is stored in esi
    mov   %esi, %eax
    ret
esi_zero:
    mov $1, %eax
    ret
