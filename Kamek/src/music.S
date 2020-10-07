#include <asm_setup.S>

.global EndSCM
.global newClimbSounds
.global newClimbSoundsJump
.global newClimbSoundsJumpS
.global newClimbSoundsJumpSS
.extern OSReport
.extern StagePtr

.text
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

newClimbSounds:
	li r7, 0x134
	b effectNewClimbSounds

newClimbSoundsJump:
	li r7, 0x136
	b effectNewClimbSounds

newClimbSoundsJumpS:
	li r7, 0x137
	b effectNewClimbSounds

newClimbSoundsJumpSS:
	li r7, 0x138
	b effectNewClimbSounds

.global newClimbSoundsLand
newClimbSoundsLand:
	li r7, 0x139
	b effectNewClimbSounds

.global newClimbSoundsAttack
newClimbSoundsAttack:
	li r7, 0x13A
	b effectNewClimbSounds

effectNewClimbSounds:
	lis r6, StagePtr@h
	ori r6, r6, StagePtr@l
	lwz r6, 0(r6)
	lhz r6, 0x120C(r6)
	cmpwi r6, 14
	beq pngn
	cmpwi r6, 0x903
	beq pngn
	cmpwi r6, 0x40E
	beq climb
	mr r4, r7
	blr

pngn:
	li r4, 0xE1
	blr

climb:
	li r4, 0xC9
	blr

.data
SCMs: .string "Setting music to %d\n"
.align 4
