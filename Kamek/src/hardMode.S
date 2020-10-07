#include <asm_setup.S>

.global enableHardMode
.global OneHitKill
.global TimerAdjustments

.text
.align 4

OneHitKill:
	# Check if hard mode is enabled
	lis 	r10, enableHardMode@ha
	lbz 	r10, enableHardMode@l(r10)
	cmpwi 	r10, 0
	beq 	NoOneHitKill
	
	# Enable one hit kill
	li		r0, 0

NoOneHitKill:
	cmpwi r0, 0 # Original instruction
	blr

TimerAdjustments:
	# Check if hard mode is enabled
	lis 	r10, enableHardMode@ha
	lbz 	r10, enableHardMode@l(r10)
	cmpwi 	r10, 0
	beq 	NoTimerAdjust

	# Set the timer to 250, but only if the timer is higher than that
	li		r10, 250
	slwi	r10, r10, 0xC
	cmp		r3, r10
	blt		NoTimerAdjust
	mr		r3, r10

NoTimerAdjust:
	subic.	r0, r3, 92 # Original instruction
	blr

.data
.align 4
enableHardMode:
	.byte 0
