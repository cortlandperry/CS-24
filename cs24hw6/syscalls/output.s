#============================================================================
# output:  write the message that we have received to stdout. This is done
#           by setting up the register values and then calling syscall on
#           write() function, which has a syscall value of 1
#
# Arguments are called in this convention:
#             %rdi = the pointer to the data
#             %rsi = integer size
#
# The return value is returned in rax.
#

.globl output
output:
    push %rcx
    push %r11           # pushing rcx and r11 onto the stack to save registers
    mov $1, %rax        # move 1, our system call number for write into rax
    mov %rsi, %rdx      # move our value for the size of message into rdx (arg3)
    mov %rdi, %rsi      # move our value for the pointer buffer into rsi (arg2)
    mov $1, %rdi        # move the file descriptor (1) to rdi (arg1)
    syscall             # call our system call function
    pop %rcx
    pop %r11            # restore our values for rcx and r11
    ret
