.text
.local local32
.type local32, @function
local32:
    nop
    ret
.size local32, .-local32

.globl function32
.type function32, @function
function32:
    call missing32
    ret
.size function32, .-function32

.data
.globl data32
.type data32, @object
.size data32, 4
data32:
    .long 42

.section .note.GNU-stack,"",@progbits
