#include <asm_setup.S>

.global Fix38WithJumpCoin
.global FixFireballSensorSetup
.global FixScoreGlitch
.global PowEvent
.global Rrun
.global TextWriterStuff
.extern ContinueFromFixFireballSensorSetup
.extern continueFromFlagObjCheck
.extern instance__10dFlagMgr_c
.extern returnFromFlagObjCheck
.extern set__10dFlagMgr_cFUcibbbUi
.extern TextWriterStuffContinue

.text
FixScoreGlitch:
	lwz r0, 0x4A10(r3)
	lis r8, 0x8000
	cmplw r3, r8
	bgtlr
	lwz r31, 0xC(r1)
	lwz r0, 0x14(r1)
	mtlr r0
	addi r1, r1, 0x10
	blr

TextWriterStuff:
	lis r6, 0xFFFF
	ori r6, r6, 0xFF00
	lwz r0, 8(r5)
	or r0, r0, r6
	stw r0, 0x18(r1)
	lwz r0, 0xC(r5)
	or r0, r0, r6
	stw r0, 0x1C(r1)
	lwz r0, 0x10(r5)
	or r0, r0, r6
	stw r0, 0x20(r1)
	lwz r0, 0x14(r5)
	or r0, r0, r6
	stw r0, 0x24(r1)
	b TextWriterStuffContinue

PowEvent:
	subi r3, r3, 0x4CC
	lwz r3, 4(r3)
	clrlwi. r3, r3, 24
	beqlr
	subi r4, r3, 1
	lis r3, instance__10dFlagMgr_c@ha
	lwz r3, instance__10dFlagMgr_c@l(r3)
	li r5, 0
	li r6, 1
	li r7, 0
	li r8, 0
	li r9, 0
	b set__10dFlagMgr_cFUcibbbUi

FixFireballSensorSetup:
	mr r3, r30
	lwz r12, 0x60(r3)
	lwz r12, 0x11C(r12)
	mtctr r12
	bctrl
	mr r3, r30
	b ContinueFromFixFireballSensorSetup

Fix38WithJumpCoin:
	lhz r0, 8(r3)
	cmplwi r0, 403 	# EN_COIN_JUMP
	beq IsExemptedActor
	cmplwi r0, 401	# EN_COIN
	beq IsExemptedActor
	cmplwi r0, 221	# EN_HATENA_BALLOON
	beq IsExemptedActor
	li r3, 0 # Else, return 0
	b returnFromFlagObjCheck

IsExemptedActor:
	b continueFromFlagObjCheck

.data
Rrun: .string "Rrun"
.align 4
