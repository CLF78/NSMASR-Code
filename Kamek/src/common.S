#include <asm_setup.S>

.global CheckExistingPowerup
.global Zero

.text
CheckExistingPowerup:
	lwz r3, 0x14E0(r3)
	blr

.data
.align 4
Zero:		.float 0.0