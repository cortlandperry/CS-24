# __add_bigint(a, b, size) - computes the sum of bigints a and b, each with
# size size.
# Arguments:
#  rdi: a
#  rsi: b
#  rdx: size
#
# rax: return

.globl __add_bigint
__add_bigint:
    mov %rdx, %r10      # put the size into our loop register
    cmp $0, %r10        # if r10 is 0, we just return 1 otherwise go to add_loop
    jne add_loop
    mov $1, %eax        # if r10 was 0, we return 1
    ret

add_loop:
    mov (%rdi), %r11
    adc %r11, (%rsi)      # add with the carry flag b += a
    leaq 8(%rdi), %rdi    # incrementing the pointer -- doesnt affect CF
    leaq 8(%rsi), %rsi    # incrementing the pointer -- doesnt affect CF

    dec %r10              # decrement our loop variable
    jnz add_loop          # if our loop variable isnt zero, go to top

finish_loop:
    jnc done              # if there is no carry, it was succses!
    mov $0, %eax          # otherwise, move 0 into our return value
    ret

done:
    mov $1, %eax          # move 1 into our return value
    ret
