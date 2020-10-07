#include <asm_setup.S>

.global BooZOrderHack

.text
BooZOrderHack:
	lhz r0, 8(r3)
	cmplwi r0, 0xB0
	bne notRegularTeresa
	lfs f8, 0xB4(r30)
	lis r11, booZorderPlus@ha
	lfs f7, booZorderPlus@l(r11)
	fadds f8, f8, f7
	stfs f7, 0xB4(r30)
	stfs f7, 0x990(r30)
	stfs f7, 0x99C(r30)

notRegularTeresa:
	li r5, 1
	blr

.data
.align 4

booZorderPlus:
	.float 5500.0
