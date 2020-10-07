#include <asm_setup.S>

.global BGScale
.global BGScaleFix
.global BGScaleFront, BGScaleBack, BGScaleEnabled
.global CameraLockEnabled
.global GlobalStarsCollected
.global isLockPlayerRotation
.global NoMichaelBuble

.text
BGScaleFix:
	lis r3, BGScale@h
	ori r3, r3, BGScale@l
	lbz r4, 0x10(r3)
	cmpwi r4, 0
	beq NoBGScale
	lwz r4, 0x4E4(r31)
	cmplwi r4, 0
	beq DontUseBack
	addi r3, r3, 8

DontUseBack:
	lfs f1, 0(r3)
	stfs f1, 0xDC(r31)
	lfs f1, 4(r3)
	stfs f1, 0xE0(r31)

NoBGScale:
	lwz r31, 0x5C(r1)
	lwz r0, 0x64(r1)
	mtlr r0
	addi r1, r1, 0x60
	blr

.data
.align 4

BGScale:
BGScaleFront: .float 1.0, 1.0
BGScaleBack: .float 1.0, 1.0

BGScaleEnabled: .byte 0

CameraLockEnabled: .byte 0

GlobalStarsCollected: .int 0

isLockPlayerRotation: .byte 0

NoMichaelBuble: .byte 0