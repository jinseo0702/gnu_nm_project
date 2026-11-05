.text
.local local_text
.type local_text, @function
local_text:
    nop
    ret
.size local_text, .-local_text

.globl global_text
.type global_text, @function
global_text:
    call missing_external
    ret
.size global_text, .-global_text

.data
.local local_data
.type local_data, @object
.size local_data, 4
local_data:
    .long 7

.globl global_data
.type global_data, @object
.size global_data, 8
global_data:
    .quad missing_external

.bss
.globl global_bss
.type global_bss, @object
.size global_bss, 16
global_bss:
    .zero 16

.section .note.GNU-stack,"",@progbits
