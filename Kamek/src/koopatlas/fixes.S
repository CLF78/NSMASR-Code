#include <asm_setup.S>

.global ClearLastPowerupStoreTypeAfterGameOver
.extern LastPowerupStoreType

.text
ClearLastPowerupStoreTypeAfterGameOver:
	li r5, 0
	lis r4, LastPowerupStoreType@ha
	stw r5, LastPowerupStoreType@l(r4)
	blr
