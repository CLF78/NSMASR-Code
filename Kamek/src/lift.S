#include <asm_setup.S>

.global FixLineLiftActivation
.global FixLineLift
.extern ContinueFromFixLineLiftActivation

.text
FixLineLift:
	stfs f0, 0x324(r28)
	lis r4, floaty@h
	ori r4, r4, floaty@l
	lfs f0, 0(r4)
	stfs f0, 0x318(r28)
	stfs f0, 0x31C(r28)
	blr

FixLineLiftActivation:
	stwu r1, -0x10(r1)
	lwz r6, 4(r3)
	extrwi. r6, r6, 4,24
	bne myLineLiftActivator
	b ContinueFromFixLineLiftActivation

myLineLiftActivator:
	mflr r0
	stw r0, 0x14(r1)
	stw r31, 0xC(r1)
	li r31, 0
	stw r30, 0x8(r1)
	lwz r30, 0x518(r3)
	b llaStartLoop

llaStartLoop:
	cmpwi r30, 0
	bne llaLoop
	cmpwi r31, 0
	beq llaReturnZero
	li r3, 1
	b llaReturn

llaLoop:
	lwz r3, 4(r30)
	lbz r0, 0x38C(r3)
	cmplwi r0, 1
	bne llaNotPlayer
	addi r31, r31, 1
	b llaNext

llaNotPlayer:
	cmplwi r0, 2
	bne llaNext
	lwz r12, 0x60(r3)
	lwz r12, 0x6C(r12)
	mtctr r12
	bctrl
	lbz r0, 0(r3)
	extsb r0, r0
	cmplwi r0, 4
	bge llaNext
	addi r31, r31, 1

llaNext:
	lwz r30, 0xC(r30)

llaReturnZero:
	li r3, 0

llaReturn:
	lwz r31, 0xC(r1)
	lwz r30, 8(r1)
	lwz r0, 0x14(r1)
	mtlr r0
	addi r1, r1, 0x10
	blr

.data
floaty: .float 512.0
