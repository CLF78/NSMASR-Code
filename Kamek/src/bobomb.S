#include <asm_setup.S>

.global BomheiForceFacingDirection
.global BomheiCollisionCallback
.extern collisionCallback__5dEn_cFP13ActivePhysicsP13ActivePhysics

.text
BomheiForceFacingDirection:
	stb r3, 0x348(r30)
	lwz r6, 4(r30)
	clrlwi r6, r6, 28
	cmpwi r6, 0
	beqlr
	subi r6, r6, 1
	stb r6, 0x348(r30)
	blr

BomheiCollisionCallback:
	lwz r5, 4(r4)
	lhz r5, 8(r5)
	cmpwi r5, 0x70 # Hammer Bros
	beqlr
	cmpwi r5, 0x72 # Lift Hammer Bros
	beqlr
	b collisionCallback__5dEn_cFP13ActivePhysicsP13ActivePhysics
