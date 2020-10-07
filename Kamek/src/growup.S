#include <asm_setup.S>

.extern returnFromGlobalSizes
.extern returnFromGlobalSpeedUpdatesA, returnFromGlobalSpeedUpdatesB, returnFromGlobalSpeedUpdatesB2
.extern returnFromModifyCollision, returnFromModifyCollision2
.extern returnFromRiderInitA, returnFromRiderInitB, returnFromRiderInitC, returnFromRiderInitD

.global GlobalRiderSize
.global GlobalSpriteSize
.global GlobalSpriteSpeed
.global GlobalZOrderDeath, GlobalZOrderDamage
.global InheritSettings, InheritSettings_B, InheritSettings_D, InheritSettings_D_r31
.global ModifyCollision
.global ModifyGlobalSizes
.global ModifyRiderInitA, ModifyRiderInitB, ModifyRiderInitC, ModifyRiderInitD
.global ModifySpeedUpdatesA, ModifySpeedUpdatesB
.global PokeyZOrderDeath, PokeyZOrderDeathFreeze
.global SizerOn
.global TooBigToBurn, TooBigToFreeze
.global ZOrderOn

.text
.align 4

####################
# LoadFromSettings #
####################

# Notes:
# *F variant is for Nybble 5, normal one is for Nybble 7
# *We can trash r12 for now, it's not supposed to be guaranteed anyway
# *Srwi instruction gets the sprite settings

LoadFromSettingsS:
	lwz r12, 4(r3)
	srwi r12, r12, 20
	b LoadFromSettingsEnd

LoadFromSettingsSF:
	lwz r12, 4(r3)
	srwi r12, r12, 28
	b LoadFromSettingsEnd

LoadFromSettingsC:
	lwz r12, 4(r4)
	srwi r12, r12, 20
	b LoadFromSettingsEnd

LoadFromSettingsCF:
	lwz r12, 4(r4)
	srwi r12, r12, 28
	b LoadFromSettingsEnd

LoadFromSettingsH:
	lwz r12, 4(r30)
	srwi r12, r12, 20
	b LoadFromSettingsEnd

LoadFromSettingsHF:
	lwz r12, 4(r30)
	srwi r12, r12, 28
	b LoadFromSettingsEnd

LoadFromSettingsL:
	lwz r12, 8(r30)
	srwi r12, r12, 20
	b LoadFromSettingsEnd

LoadFromSettingsLF:
	lwz r12, 8(r30)
	srwi r12, r12, 28
	b LoadFromSettingsEnd

LoadFromSettingsEnd:
	# First, get the highest nybble as r12
	andi. r12, r12, 0xF

	# Get the value from the lookup table
	lis r11, LookupTable@h
	ori r11, r11, LookupTable@l

	# Size *= 4 (size of float)
	slwi r12, r12, 2

	# Get it from the array
	lfsx f8, r11, r12
	blr

######################
# Model Size Changer #
######################

ModelNoChanges:
	b returnFromGlobalSizes

ModifyGlobalSizes:
	# Go back if it's 60
	lhz r10, 8(r3)
	cmpwi r10, 60
	beq ModelNoChanges

	# Go back if SizerOn is 0 (off)
	lis r10, SizerOn@ha
	lbz r10, SizerOn@l(r10)
	cmpwi r10, 0
	beq ModelNoChanges

	# Else, use Global
    lis r11, GlobalSpriteSize@ha
    lfs f8, GlobalSpriteSize@l(r11)

	# Backup LR to r9
	mflr r9

	# Load from Nyb5 Settings if SizerOn is 1
	cmpwi r10, 1
	beql LoadFromSettingsSF

	# Load from Nyb7 Settings if SizerOn is 2
	cmpwi r10, 2
	beql LoadFromSettingsS

	# Restore LR
	mtlr r9

	# Apply changes (includes part of the original code due to the extra fmuls)
	lwz r4, 0x47C(r3)

	lfs f0, 0xDC(r4)
	fmuls f0, f0, f8
	stfs f0, 0xDC(r3)
	stfs f0, 0x464(r3)

	lfs f0, 0xE0(r4)
	fmuls f0, f0, f8
	stfs f0, 0xE0(r3)
	stfs f0, 0x468(r3)

	lfs f0, 0xE4(r4)
	fmuls f0, f0, f8
	stfs f0, 0xE4(r3)
	stfs f0, 0x46C(r3)

	# Off we go!
	blr

###################
# Speed Changer A #
###################

SpeedUpdatesNoChangesA:
	lfs f3, 0xB0(r4)
	b returnFromGlobalSpeedUpdatesA

ModifySpeedUpdatesA:
	# Go back if type is a Player, Yoshi, or World Map stuff
	lhz r10, 8(r4)
	cmpwi r10, 0x12
	blt SpeedUpdatesNoChangesA

	# Go back if it's 60
	cmpwi r10, 60
	beq SpeedUpdatesNoChangesA

	# Go back if SizerOn is 0 (off)
	lis r10, SizerOn@ha
	lbz r10, SizerOn@l(r10)
	cmpwi r10, 0
	beq SpeedUpdatesNoChangesA

	# Else, use Global
    lis r11, GlobalSpriteSize@ha
    lfs f8, GlobalSpriteSize@l(r11)

	# Backup LR to r9
	mflr r9

	# Load from Nyb5 Settings if SizerOn is 1
	cmpwi r10, 1
	beql LoadFromSettingsCF

	# Load from Nyb7 Settings if SizerOn is 2
	cmpwi r10, 2
	beql LoadFromSettingsC

	# Restore LR
	mtlr r9

	# Apply changes (includes part of the original code due to the extra fmuls)
	lfs f3, 0xB0(r4)
	lfs f2, 0xD4(r4)
	lfs f1, 0xAC(r4)
	lfs f0, 0xD0(r4)
	fmuls f2, f8, f2
	fadds f2, f3, f2
	fmuls f0, f8, f0
	fadds f0, f1, f0
	stfs f2, 4(r3)
	stfs f0, 0(r3)
	blr

###################
# Speed Changer B #
###################

SpeedUpdatesNoChangesB:
	lfs     f3, 0xC(r1)
	b returnFromGlobalSpeedUpdatesB

ModifySpeedUpdatesB:
	# Go back if type is a Player, Yoshi, or World Map stuff
	lhz r10, 8(r30)
	cmpwi r10, 0x12
	blt SpeedUpdatesNoChangesB

	# Go back if it's 60
	cmpwi r10, 60
	beq SpeedUpdatesNoChangesB

	# Go back if SizerOn is 0 (off)
	lis r10, SizerOn@ha
	lbz r10, SizerOn@l(r10)
	cmpwi r10, 0
	beq SpeedUpdatesNoChangesB

	# Else, use Global
    lis r11, GlobalSpriteSize@ha
    lfs f8, GlobalSpriteSize@l(r11)

	# Backup LR to r9
	mflr r9

	# Load from Nyb5 Settings if SizerOn is 1
	cmpwi r10, 1
	beql LoadFromSettingsHF

	# Load from Nyb7 Settings if SizerOn is 2
	cmpwi r10, 2
	beql LoadFromSettingsH

	# Restore LR
	mtlr r9

	# Apply changes (includes part of the original code due to the extra fmuls)
	lfs     f3, 0xC(r1)
	lfs     f1, 0x440(r30)
	lfs     f2, 0x10(r1)
	fadds   f6, f3, f1
	lfs     f0, 0x444(r30)
	lfs     f4, 0x14(r1)
	fadds   f5, f2, f0
	lfs     f3, 0x448(r30)
	lfs     f1, 0xAC(r30) #Actor.x_pos
	fadds   f3, f4, f3
	lfs     f0, 0xB0(r30) #Actor.y_pos
	fmuls   f6, f8, f6
	fadds   f2, f1, f6
	fmuls   f5, f8, f5
	fadds   f1, f0, f5
	lfs     f0, 0xB4(r30) #Actor.z_pos
	stfs    f2, 0xAC(r30) #Actor.x_pos
	fmuls   f3, f8, f3
	b returnFromGlobalSpeedUpdatesB2

##################
# Fireball Patch #
##################

NotTooBigToBurn:
    lwz       r4, 4(r5)
    blr

TooBigToBurn:
	# Go back if type is a Player, Yoshi, or World Map stuff
	lhz r10, 8(r31)
	cmpwi r10, 0x12
	blt NotTooBigToBurn

	# Go back if it's 60
	cmpwi r10, 60
	beq NotTooBigToBurn

	# Go back if SizerOn is 0 (off)
	lis r10, SizerOn@ha
	lbz r10, SizerOn@l(r10)
	cmpwi r10, 0
	beq NotTooBigToBurn

	cmpwi r10, 3
	beq GlobalCompare

	# Use nyb 7
	cmpwi r10, 1
	bne UseNyb7

	# Else use nyb 5
	lwz r6, 4(r31)
	srwi r6, r6, 28
	b GotNyb

UseNyb7:
	lwz r6, 4(r31)
	extrwi r6, r6, 4, 8

GotNyb:
	cmpwi r6, 7
	bge Dont
	b NotTooBigToBurn

GlobalCompare:
	lis r6, GlobalSpriteSize@ha
	lwz r6, GlobalSpriteSize@l(r6)
	lis r7, 0x3F80
	cmpw r6, r7
	beq NotTooBigToBurn

Dont:
	lwz r31, 0x4C(r1)
	lwz r30, 0x48(r1)
	lwz r29, 0x44(r1)
	lwz r0, 0x54(r1)
	mtlr r0
	addi r1, r1, 0x50
	blr

#################
# Iceball Patch #
#################

NotTooBigToFreeze:
    lwz       r0, 0x490(r3)
    blr

TooBigToFreeze:
	# Go back if type is a Player, Yoshi, or World Map stuff
	lhz r10, 8(r28)
	cmpwi r10, 0x12
	blt NotTooBigToFreeze

	# Go back if it's 60
	cmpwi r10, 60
	beq NotTooBigToFreeze

	# Go back if SizerOn is 0 (off)
	lis r10, SizerOn@ha
	lbz r10, SizerOn@l(r10)
	cmpwi r10, 0
	beq NotTooBigToFreeze

	cmpwi r10, 3
	beq GlobalCompare_2

	# Use nyb 7
	cmpwi r10, 1
	bne UseNyb7_2

	# Else use nyb 5
	lwz r6, 4(r28)
	srwi r6, r6, 28
	b GotNyb_2

UseNyb7_2:
	lwz r6, 4(r28)
	extrwi r6, r6, 4, 8

GotNyb_2:
	cmpwi r6, 7
	bge Dont_2
	b NotTooBigToFreeze

GlobalCompare_2:
	lis r6, GlobalSpriteSize@ha
	lwz r6, GlobalSpriteSize@l(r6)
	lis r7, 0x3F80
	cmpw r6, r7
	beq NotTooBigToFreeze

Dont_2:
	lwz r31, 0x1C(r1)
	li r3, 1
	lwz r30, 0x18(r1)
	lwz r29, 0x14(r1)
	lwz r28, 0x10(r1)
	lwz r0, 0x24(r1)
	mtlr r0
	addi r1, r1, 0x20
	blr

#####################
# Collision Changer #
#####################

CollisionNoChanges:
	stw r4, 4(r3)
	b returnFromModifyCollision

ModifyCollision:
	# Go back if it's 60
	lhz r10, 8(r4)
	cmpwi r10, 60
	beq CollisionNoChanges

	# Go back if SizerOn is 0 (off)
	lis r10, SizerOn@ha
	lbz r10, SizerOn@l(r10)
	cmpwi r10, 0
	beq CollisionNoChanges

	# Else, use Global
    lis r11, GlobalSpriteSize@ha
    lfs f8, GlobalSpriteSize@l(r11)

	# Backup LR to r9
	mflr r9

	# Load from Nyb5 Settings if SizerOn is 1
	cmpwi r10, 1
	beql LoadFromSettingsCF

	# Load from Nyb7 Settings if SizerOn is 2
	cmpwi r10, 2
	beql LoadFromSettingsC

	# Restore LR
	mtlr r9

	# This is the old ActivePhysics_InitWithStruct function
	stw r4, 4(r3)
	li r0, 0

	# x_dist_to_center
	lfs f0, 0(r5)			# Load it
 	fmuls f0, f0, f8		# Multiply it by the setting
	stfs f0, 0x1C(r3)		# Store it

	# y_dist_to_center
	lfs f0, 4(r5)
	fmuls f0, f0, f8
	stfs f0, 0x20(r3)

	# x_dist_to_edge
	lfs f0, 8(r5)
	fmuls f0, f0, f8
	stfs f0, 0x24(r3)

	# y_dist_to_edge
	lfs f0, 0xC(r5)
	fmuls f0, f0, f8
	stfs f0, 0x28(r3)

	# Off we go!
	b returnFromModifyCollision2

################
# Rider Init A #
################

RiderInitANoChanges:
	lfs f0, 0(r4)
	b returnFromRiderInitA

ModifyRiderInitA:

	# Go back if SizerOn is 0 (off)
	lis r10, SizerOn@ha
	lbz r10, SizerOn@l(r10)
	cmpwi r10, 0
	beq RiderInitANoChanges

	# Else, use Global
    lis r11, GlobalSpriteSize@ha
    lfs f8, GlobalSpriteSize@l(r11)

	# Backup LR to r9
	mflr r9

	# Load from Nyb5 Settings if SizerOn is 1
	cmpwi r10, 1
	beql LoadFromSettingsSF

	# Load from Nyb7 Settings if SizerOn is 2
	cmpwi r10, 2
	beql LoadFromSettingsS

	# Restore LR
	mtlr r9

	# Apply changes (includes part of the original code due to the extra fmuls)
	lfs f0, 0(r4)
	fmuls f0, f0, f8
	stfs f0, 0x4C(r3)

	lfs f0, 4(r4)
	fmuls f0, f0, f8
	stfs f0, 0x50(r3)

	lfs f0, 0(r5)
	fmuls f0, f0, f8
	stfs f0, 0x54(r3)

	lfs f0, 4(r5)
	fmuls f0, f0, f8
	stfs f0, 0x58(r3)

	# Off we go!
	blr

################
# Rider Init B #
################

RiderInitBNoChanges:
	lfs f0, 0(r5)
	b returnFromRiderInitB

ModifyRiderInitB:
	# Go back if SizerOn is 0 (off)
	lis r10, SizerOn@ha
	lbz r10, SizerOn@l(r10)
	cmpwi r10, 0
	beq RiderInitBNoChanges

	# Else, use Global
	lis r11, GlobalSpriteSize@ha
	lfs f8, GlobalSpriteSize@l(r11)

	# Backup LR to r9
	mflr r9

	# Load from Nyb5 Settings if SizerOn is 1
	cmpwi r10, 1
	beql LoadFromSettingsSF

	# Load from Nyb7 Settings if SizerOn is 2
	cmpwi r10, 2
	beql LoadFromSettingsS

	# Restore LR
	mtlr r9

	# Apply changes (includes part of the original code due to the extra fmuls)
	lfs f0, 0(r5)
	fmuls f0, f0, f8
	stfs f0, 0x4C(r3)

	lfs f0, 4(r5)
	fmuls f0, f0, f8
	stfs f0, 0x50(r3)

	lfs f0, 0(r4)
	fmuls f0, f0, f8
	stfs f0, 0x54(r3)

	lfs f0, 4(r4)
	fmuls f0, f0, f8
	stfs f0, 0x58(r3)

	# Off we go!
	blr

################
# Rider Init C #
################

RiderInitCNoChanges:
	stfs f1, 0x4C(r3)
	b returnFromRiderInitC

ModifyRiderInitC:
	# Go back if SizerOn is 0 (off)
	lis r10, SizerOn@ha
	lbz r10, SizerOn@l(r10)
	cmpwi r10, 0
	beq RiderInitCNoChanges

	# Else, use Global
	lis r11, GlobalSpriteSize@ha
	lfs f8, GlobalSpriteSize@l(r11)

	# Backup LR to r9
	mflr r9

	# Load from Nyb5 Settings if SizerOn is 1
	cmpwi r10, 1
	beql LoadFromSettingsSF

	# Load from Nyb7 Settings if SizerOn is 2
	cmpwi r10, 2
	beql LoadFromSettingsS

	# Restore LR
	mtlr r9

	# Apply changes
	fmuls f1, f1, f8
	fmuls f2, f2, f8
	fmuls f3, f3, f8
	fmuls f4, f4, f8

	# Return
	b RiderInitCNoChanges

################
# Rider Init D #
################

RiderInitDNoChanges:
	stfs f3, 0x4C(r3)
	b returnFromRiderInitD

ModifyRiderInitD:
	# Go back if SizerOn is 0 (off)
	lis r10, SizerOn@ha
	lbz r10, SizerOn@l(r10)
	cmpwi r10, 0
	beq RiderInitDNoChanges

	# Else, use Global
	lis r11, GlobalSpriteSize@ha
	lfs f8, GlobalSpriteSize@l(r11)

	# Backup LR to r9
	mflr r9

	# Load from Nyb5 Settings if SizerOn is 1
	cmpwi r10, 1
	beql LoadFromSettingsSF

	# Load from Nyb7 Settings if SizerOn is 2
	cmpwi r10, 2
	beql LoadFromSettingsS

	# Restore LR
	mtlr r9

	# Apply changes
	fmuls f1, f1, f8
	fmuls f2, f2, f8
	fmuls f3, f3, f8
	fmuls f4, f4, f8

	# Return
	b RiderInitDNoChanges

####################
# Inherit Settings #
####################

InheritSettings:
	lwz r4, 4(r3)
	andis. r4, r4, 0xFFF0
	blr

InheritSettings_B:
	lwz r5, 4(r3)
	andis. r5, r5, 0x0FF0
	blr

InheritSettings_D:
	lwz r0, 4(r3)
	b InheritSettings_D_Continue

InheritSettings_D_r31:
	lwz r0, 4(r31)

InheritSettings_D_Continue:
	andis. r0, r0, 0xFFF0
	or r4, r4, r0
	blr

#################
# Global ZOrder #
#################

GlobalZOrderDeath:
	lis r10, ZOrderOn@ha 
	lbz r10, ZOrderOn@l(r10)
	cmpwi r10, 0
	beq NormalZorder

	lis r11, InFront@ha
	lfs f8, InFront@l(r11)
	stfs f8, 0xB4(r28)
	stfs f8, 0xC(r1)

NormalZorder:
	mtlr r0
	addi r1, r1, 0x60
	blr

GlobalZOrderDamage:
	lis r10, ZOrderOn@ha 
	lbz r10, ZOrderOn@l(r10)
	cmpwi r10, 0
	beq NormalZorderDamage

	lis r11, InFront@ha
	lfs f0, InFront@l(r11)
	stfs f0, 0xB4(r31)

NormalZorderDamage:
	mr r3, r31
	blr


###############
# Pokey Fixes #
###############

PokeyZOrderDeath:
	lis r10, ZOrderOn@ha
	lbz r10, ZOrderOn@l(r10)
	cmpwi r10, 0
	beq NormalPokeyZorder

	lis r11, InFront@ha
	lfs f0, InFront@l(r11)
	stfs f0, 0xB4(r31)

NormalPokeyZorder:
	lwz r0, 0x5D0(r31)
	blr

PokeyZOrderDeathFreeze:
	lis r10, ZOrderOn@ha
	lbz r10, ZOrderOn@l(r10)
	cmpwi r10, 0
	beq NormalPokeyZorderFreeze

	lis r11, InFront@ha
	lfs f0, InFront@l(r11)
	stfs f0, 0xB4(r3)

NormalPokeyZorderFreeze:
	lwz r0, 0x604(r3)
	blr

###############
# Gabon Fixes #
###############

.global GabonRockZOrderDeath, GabonRockZOrderDeathDrop
GabonRockZOrderDeath:
	lis r10, ZOrderOn@ha
	lbz r10, ZOrderOn@l(r10)
	cmpwi r10, 0
	beq NormalGabonRockZorder

	lis r11, InFront@ha
	lfs f0, InFront@l(r11)
	stfs f0, 0xB4(r3)

NormalGabonRockZorder:
	stfs f0, 0xE8(r3)
	blr

GabonRockZOrderDeathDrop:
	lis r10, ZOrderOn@ha
	lbz r10, ZOrderOn@l(r10)
	cmpwi r10, 0
	beq NormalGabonRockZorderDrop

	lis r11, InFront@ha
	lfs f0, InFront@l(r11)
	stfs f0, 0xB4(r31)

NormalGabonRockZorderDrop:
	mr r31, r3
	blr

.data
.align 4
GlobalSpriteSize:
	.float 1.0

GlobalSpriteSpeed:
	.float 1.0

GlobalRiderSize:
	.float 1.0

SizerOn:
	.byte 0

ZOrderOn:
	.byte 0

.align 4
LookupTable:
	.float 1, 1, 1, 1, 0.25, 0.5, 0.75, 1.25, 1.5, 2, 2.5, 3, 4, 5, 8, 10

InFront:
	.float 5500.0
