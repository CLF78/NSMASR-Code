#include <asm_setup.S>

.global JrFloorFireHax

.text
JrFloorFireHax:
	stw r0, 0x120(r30)
	lwz r4, 4(r30)
	clrrwi. r4, r4, 28
	beqlr
	li r4, 0
	stw r4, 0x778(r30)
	lis r4, 0x100
	stw r4, 0x774(r30)
	stw r4, 0x77C(r30)
	blr
