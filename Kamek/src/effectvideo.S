#include <asm_setup.S>

.extern LoadBreffKoopa, LoadBreffKoopaJr
.extern returnFromBreffLoadPatch

.global BreffLoadPatch

.text
.align 4

BreffLoadPatch:
	# Original instruction
	lhz r0, 0(r27)
	
	# Bowser Fireballs will load Bowser's effect files
	cmpwi r0, 449
	beq Koopa
	
	# Electric Line will load Bowser Jr's effect files
	cmpwi r0, 250
	beq KoopaJr
	
	# Otherwise keep doing the game's original checks
	b returnFromBreffLoadPatch

Koopa:
	b LoadBreffKoopa

KoopaJr:
	b LoadBreffKoopaJr
