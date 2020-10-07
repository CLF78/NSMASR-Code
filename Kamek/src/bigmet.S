#include <asm_setup.S>

.global BigMetSlidingFix
.global BigMetPowFix
.extern OriginalPowBlockActivated

.text
BigMetSlidingFix:
	stwu r1, -0x10(r1)
	mflr r0
	stw r0, 0x14(r1)
	lwz r4, 4(r5)
	lwz r12, 0x60(r3)
	lwz r12, 0x220(r12)
	mtctr r12
	bctrl
	li r3, 1
	lwz r0, 0x14(r1)
	mtlr r0
	addi r1, r1, 0x10
	blr

BigMetPowFix:
	lwz r5, 4(r3)
	extrwi. r5, r5, 4,4
	beqlr
	b OriginalPowBlockActivated
