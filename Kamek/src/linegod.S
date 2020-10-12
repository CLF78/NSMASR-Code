#include <asm_setup.S>

.extern ContinueBgActorSpawn

.global BgActorSetInfoFix
.global BgActorSpawnFix

.text
.align 4

BgActorSpawnFix:
	mr r30, r3
	lbz r0, 7(r30)
	cmpwi r0, 1
	beq JustRet
	b ContinueBgActorSpawn

JustRet:	
	lwz r31, 0x3C(r1)
	lwz r30, 0x38(r1)
	lwz r0, 0x44(r1)
	mtlr r0
	addi r1, r1, 0x40
	blr

BgActorSetInfoFix:
	li r4, 0
	stb r4, 7(r3)
	blr
