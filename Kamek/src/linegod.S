#include <asm_setup.S>

.extern ContinueBgActorSpawn, ContinueBgActorSpawn2

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
	b ContinueBgActorSpawn2

BgActorSetInfoFix:
	li r4, 0
	stb r4, 7(r3)
	blr
