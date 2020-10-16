#include <asm_setup.S>

.extern BG_GM_ptr
.extern CameraLockEnabled
.extern ContinueFromEFBMagic2
.extern CreditsModeActive
.extern EFBMagic2__Fv
.extern isLockPlayerRotation
.extern StaffCreditsAddCoin
.extern continueAddToCoinCount
.extern continueAddToScore

.global AutoScrollInjector
.global CameraLockPosition
.global EFBStuff
.global PlayerRotInjector
.global PlayerRotInjector2
.global PlayerRotInjector3
.global StaffCreditsAddAllCoins
.global StaffCreditsNoScore
.global lockedPlayerRotation

.text
StaffCreditsAddAllCoins:
	lis r4, CreditsModeActive@ha
	lbz r4, CreditsModeActive@l(r4)
	cmpwi r4, 1
	beq isCredits
	stwu r1, -0x40(r1)
	b continueAddToCoinCount

isCredits:
	b StaffCreditsAddCoin

StaffCreditsNoScore:
	lis r9, CreditsModeActive@ha
	lbz r9, CreditsModeActive@l(r9)
	cmpwi r9, 1
	beqlr
	cmpwi r5, -1
	b continueAddToScore

AutoScrollInjector:
	lis r3, CameraLockEnabled@ha
	lbz r3, CameraLockEnabled@l(r3)
	cmpwi r3, 1
	bnelr
	lis r3, BG_GM_ptr@ha
	lwz r3, BG_GM_ptr@l(r3)
	lis r4, CameraLockPosition@h
	ori r4, r4, CameraLockPosition@l
	addis r3, r3, 9
	lfs f1, 0(r4)
	stfs f1, -0x178(r3)
	lfs f1, 4(r4)
	stfs f1, -0x174(r3)
	blr

PlayerRotInjector:
	lis r9, isLockPlayerRotation@ha 
	lbz r9, isLockPlayerRotation@l(r9)
	cmpwi r9, 1
	bne dontOverridePlayerRot
	lis r9, lockedPlayerRotation@ha
	lha r7, lockedPlayerRotation@l(r9)

dontOverridePlayerRot:
	sth r7, 0xA(r1)
	blr

PlayerRotInjector2:
	lis r9, isLockPlayerRotation@ha 
	lbz r9, isLockPlayerRotation@l(r9)
	cmpwi r9, 1
	bne dontOverridePlayerRot2
	lis r9, lockedPlayerRotation@ha
	lha r0, lockedPlayerRotation@l(r9)

dontOverridePlayerRot2:
	sth r0, 0xA(r1)
	blr

PlayerRotInjector3:
	lis r9, isLockPlayerRotation@ha 
	lbz r9, isLockPlayerRotation@l(r9)
	cmpwi r9, 1
	bne dontOverridePlayerRot3
	lis r9, lockedPlayerRotation@ha
	lha r7, lockedPlayerRotation@l(r9)

dontOverridePlayerRot3:
	sth r7, 0x12(r1)
	blr

EFBStuff:
	bl EFBMagic2__Fv
	lwz r3, 4(r30)
	b ContinueFromEFBMagic2

.data
lockedPlayerRotation: .short 0

.align 4
CameraLockPosition: .float 0.0, 0.0
