#include <asm_setup.S>

.global ReturnNone
.global ReturnTrue
.global ReturnFalse

.text
ReturnTrue:
	li r3, 1
	blr

ReturnFalse:
	li r3, 0

ReturnNone:
	blr
