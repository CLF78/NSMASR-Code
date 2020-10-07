#include <asm_setup.S>

.global ClownCarDetectAdjacents
.extern ContinueFromClownCarDetectAdjacent
.extern calculateAdjacentCollision__14collisionMgr_cFPf
.extern Zero

.text
.align 4

ClownCarDetectAdjacents:
	addi r3, r30, 0x1EC
	li r4, 0
	bl calculateAdjacentCollision__14collisionMgr_cFPf
	cmpwi r3, 0
	beq notAdjacent

	# What direction are we in?
	lfs f1, 0xE8(r30) # X speed
	lis r4, Zero@ha
	lfs f2, Zero@l(r4)
	lbz r4, 0x348(r30)
	cmpwi r4, 1
	beq isLeft

	# Right
	fcmpo cr0, f1, f2 # X Speed > 0
	blt notAdjacent
	stfs f2, 0xE8(r30)
	b notAdjacent

isLeft:
	# Left
	fcmpo cr0, f1, f2 # X Speed < 0
	bgt notAdjacent
	stfs f2, 0xE8(r30)

notAdjacent:
	# End here
	mr r3, r30
	b ContinueFromClownCarDetectAdjacent
