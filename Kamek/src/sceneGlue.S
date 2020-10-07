#include <asm_setup.S>

.global ExitStageReal
.extern ExitStageRealRest

.text
ExitStageReal:
	stwu r1, -0x20(r1)
	b ExitStageRealRest
