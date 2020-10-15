#include <asm_setup.S>

# Sys functions
.extern DVDClass
.extern GetRes
.extern OSReport
.extern strcmp

# Code
.extern HasFlowers, NoFlowers
.extern returnFromFlowerBuild1
.extern returnFromFlowerBuild2, returnFromFlowerBuild2ItsGrassland, returnFromFlowerBuild2ItsDaishizen
.extern returnFromFlowerBuild3, returnFromFlowerBuild3ItsGrassland, returnFromFlowerBuild3ItsDaishizen
.extern returnFromFlowerBuild4

# Data
.extern ObjHanaBrres
.extern ObjKusa, ObjKusaBrres
.extern Pa1Daishizen

# Globals
.global FlowerCheck
.global FlowerBuild1, FlowerBuild2, FlowerBuild3, FlowerBuild4

.text
.align 4

# This function checks whether the flowers should be loaded. To make
# your own block, copy the instructions from "mr r3, r26" to "beq true",
# add your tileset's name in the lis and ori instructions, then add
# your tileset to the tileset name list at the end of this file.

FlowerCheck:
	lis r3, M1@h
	ori r3, r3, M1@l
	crclr 4*cr1+eq
	bl OSReport
	mr r3, r26
	lis r4, Pa1NoharaSmb@h
	ori r4, r4, Pa1NoharaSmb@l
	bl strcmp
	cmpwi r3, 0
	beq true
	
	# No flowers detected, sir.
	b NoFlowers

true:
	b HasFlowers


# First branch in the ObjGrass onCreate, which checks the tileset name (again)
# Not gonna write an essay here as well.

FlowerBuild1:
	# Copy of the game's original compare, don't use this as a base!
	mr r26, r3
	addi r4, r31, Pa1Daishizen@l
	bl strcmp
	cmpwi r3, 0
	beq IsPa1Daishizen

	# Use this one instead ;)
	mr r3, r26
	lis r4, Pa1NoharaSmb@h
	ori r4, r4, Pa1NoharaSmb@l
	bl strcmp
	cmpwi r3, 0
	beq IsPa1NoharaSmb
	
	# Return otherwise
	b returnFromFlowerBuild1

# The value in r28 will be reused in later checks, so add a copy of this as well
IsPa1Daishizen:
	li r28, 1
	b returnFromFlowerBuild1

IsPa1NoharaSmb:
	lis r3, M2@h
	ori r3, r3, M2@l
	crclr 4*cr1+eq
	bl OSReport
	li r28, 2
	b returnFromFlowerBuild1


# First of the three arc file loaders. Loads the arc file based on r28's content.
FlowerBuild2:
	cmpwi r28, 0
	beq ItsGrassland
	cmpwi r28, 1
	beq ItsDaishizen
	cmpwi r28, 2
	beq ItsNoharaSmb
	b returnFromFlowerBuild2

ItsGrassland:
	b returnFromFlowerBuild2ItsGrassland

ItsDaishizen:
	b returnFromFlowerBuild2ItsDaishizen

# Copy this block!
ItsNoharaSmb:
	lis r3, M3@h
	ori r3, r3, M3@l
	crclr 4*cr1+eq
	bl OSReport
	lis r3, DVDClass@ha
	lwz r3, DVDClass@l(r3)
	addi r3, r3, 4
	lis r4, ObjHanaSmb@h
	ori r4, r4, ObjHanaSmb@l
	lis r5, ObjHanaBrres@h
	ori r5, r5, ObjHanaBrres@l
	bl GetRes
	b returnFromFlowerBuild2


# Another arc file swapper, no idea why there are two.
FlowerBuild3:
	cmpwi r28, 0
	beq ItsGrassland3
	cmpwi r28, 1
	beq ItsDaishizen3
	cmpwi r28, 2
	beq ItsNoharaSmb3
	b returnFromFlowerBuild2

ItsGrassland3:
	b returnFromFlowerBuild3ItsGrassland

ItsDaishizen3:
	b returnFromFlowerBuild3ItsDaishizen

ItsNoharaSmb3:
	lis r3, M4@h
	ori r3, r3, M4@l
	crclr 4*cr1+eq
	bl OSReport
	lis r3, DVDClass@ha
	lwz r3, DVDClass@l(r3)
	addi r3, r3, 4
	lis r4, ObjHanaSmb@h
	ori r4, r4, ObjHanaSmb@l
	lis r5, ObjHanaBrres@h
	ori r5, r5, ObjHanaBrres@l
	bl GetRes
	b returnFromFlowerBuild3

# This check is used for the grass. Since the game originally doesn't perform
# a check on r28, we'll do it ourselves.
FlowerBuild4:
	cmpwi r28, 1
	ble ItsGrassOrDaishizen
	cmpwi r28, 2
	beq ItsNoharaSmb4
	b returnFromFlowerBuild2

# Copypasted code, do NOT use!
ItsGrassOrDaishizen:
	lis r3, DVDClass@ha
	lwz r3, DVDClass@l(r3)
	addi r3, r3, 4
	lis r4, ObjKusa@h
	ori r4, r4, ObjKusa@l
	lis r5, ObjKusaBrres@h
	ori r5, r5, ObjKusaBrres@l
	bl GetRes
	b returnFromFlowerBuild4

# Use this block instead ;)
ItsNoharaSmb4:
	lis r3, M5@h
	ori r3, r3, M5@l
	crclr 4*cr1+eq
	bl OSReport
	lis r3, DVDClass@ha
	lwz r3, DVDClass@l(r3)
	addi r3, r3, 4
	lis r4, ObjKusaSmb@h
	ori r4, r4, ObjKusaSmb@l
	lis r5, ObjKusaBrres@h
	ori r5, r5, ObjKusaBrres@l
	bl GetRes
	b returnFromFlowerBuild4


.data
.align 4

# Tilesets go here
Pa1NoharaSmb: .string 'Pa1_nohara_smb'

.align 4
# Flower arc files go here
ObjHanaSmb: .string 'obj_hana_smb'

.align 4
# Grass arc files go here
ObjKusaSmb: .string 'obj_kusa_smb'

.align 4
# FUCK
M1: .string "Checking tileset name...\n"
M2: .string "Compare for noharasmb returned true\n"
M3: .string "Did first arc check on noharasmb, tryna load the file...\n"
M4: .string "Did second arc check on noharasmb, tryna load the file...\n"
M5: .string "The game wants the grass arc for noharasmb, tryna load the file...\n"
