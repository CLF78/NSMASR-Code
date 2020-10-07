#include <asm_setup.S>

.global RedRingEventControl
.extern set__10dFlagMgr_cFUcibbbUi

.text
RedRingEventControl:
	lwz r10, 4(r31)
	srwi. r10, r10, 28
	beq dontResetRREDelay
	li r5, 0

dontResetRREDelay:
	b set__10dFlagMgr_cFUcibbbUi
