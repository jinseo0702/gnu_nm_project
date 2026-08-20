.text
.globl low_function
.type low_function, @function
low_function:
    nop
    ret
.size low_function, .-low_function

.globl high_absolute
.set high_absolute, 0x100000123

.section .note.GNU-stack,"",@progbits
