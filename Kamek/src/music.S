#include <asm_setup.S>

.global AutoBRSAR
.global EndSCM

.extern OSReport

.text
AutoBRSAR:
	lis r31, 0x7FFF
	ori r31, r31, 0xFFFF
	stw r31, 0x74(r29) # Original instruction
	blr

EndSCM:
	lis r3, SCMs@h
	ori r3, r3, SCMs@l
	mr r4, r31
	crclr 4*cr1+eq
	bl OSReport

	lwz r31, 0xC(r1)
	lwz r0, 0x14(r1)
	mtlr r0
	addi r1, r1, 0x10
	blr

.data
SCMs: .string "Setting music to %d\n"
.align 4
