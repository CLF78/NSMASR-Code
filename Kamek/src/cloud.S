#include <asm_setup.S>

.global AirshipClouds
.extern returnFromAirshipCloudsNotGood, returnFromAirshipCloudsIsGood

.text
.align 4

AirshipClouds:
	lbz r3, 6(r28)
	cmpwi r3, 1
	beq IsGood

NotGood:
	b returnFromAirshipCloudsNotGood

IsGood:
	b returnFromAirshipCloudsIsGood
