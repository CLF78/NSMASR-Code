#include <asm_setup.S>

.global ThwompNewAutoDestruction
.extern Delete__13dStageActor_cFUc

.text
ThwompNewAutoDestruction:
	lfs f1, 0xB0(r31)
	lis r3, ThwompDestroyAt@h
	ori r3, r3, ThwompDestroyAt@l
	lfs f2, 0(r3)
	fcmpo cr0, f1, f2
	bgt dontDestroyMe
	mr r3, r31
	lbz r4, 0x390(r3)
	bl Delete__13dStageActor_cFUc

dontDestroyMe:
	li r3, 1
	lwz r31, 0xC(r1)
	lwz r0, 0x14(r1)
	mtlr r0
	addi r1, r1, 0x10
	blr

.data
.align 4
ThwompDestroyAt: .float -7533.0
