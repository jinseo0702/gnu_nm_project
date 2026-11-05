.data
.globl zeta
.type zeta, @object
zeta:
    .byte 1
.size zeta, .-zeta

.globl _zulu
.type _zulu, @object
_zulu:
    .byte 2
.size _zulu, .-_zulu

.globl _alpha
.type _alpha, @object
_alpha:
    .byte 3
.size _alpha, .-_alpha

.globl Alpha
.type Alpha, @object
Alpha:
    .byte 4
.size Alpha, .-Alpha

.globl alpha
.type alpha, @object
alpha:
    .byte 5
.size alpha, .-alpha

.globl dot.symbol
.type dot.symbol, @object
dot.symbol:
    .byte 6
.size dot.symbol, .-dot.symbol

.globl dollar$symbol
.type dollar$symbol, @object
dollar$symbol:
    .byte 7
.size dollar$symbol, .-dollar$symbol

.section .note.GNU-stack,"",@progbits
