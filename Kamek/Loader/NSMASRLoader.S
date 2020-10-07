# (c)Ninji 2010-2013, CLF78 2020

# Constants
.set MASTERHI, 0x8000
.set MASTERLO, 0x1800
TEST_REGION = 0x80768D50

# Macros
.macro callFunc offset
	lwz r12, \offset(r31)
	mtctr r12
	bctrl
.endm

# Offsets in system function table
OSReport =                  0x0
OSFatal =                   0x4
DVDConvertPathToEntrynum =  0x8
DVDFastOpen =               0xC
DVDReadPrio =               0x10
DVDClose =                  0x14
HeapAlloc =                 0x18
HeapFree =                  0x1C
GameHeapArray =             0x20
ArchiveHeap =               0x24
sprintf =                   0x28

# Begin code!
.text
.align 4
.global JumpToBootstrap
JumpToBootstrap:
	b start

# Offsets to system calls
EUAddr:
	.long 0x8015F870 # OSReport_EU
	.long 0x801AF710 # OSFatal_EU
	.long 0x801CA7C0 # DVDConvertPathToEntrynum_EU
	.long 0x801CAAD0 # DVDFastOpen_EU
	.long 0x801CAC60 # DVDReadPrio_EU
	.long 0x801CAB40 # DVDClose_EU
	.long 0x802B8E00 # HeapAlloc_EU
	.long 0x802B90B0 # HeapFree_EU
	.long 0x80377F48 # GameHeapArray_EU
	.long 0x8042A72C # ArchiveHeap_EU
	.long 0x802E1ACC # sprintf_EU
	.string "EU"
	.byte 0
USAddr:
	.long 0x8015F730 # OSReport_US
	.long 0x801AF5D0 # OSFatal_US
	.long 0x801CA680 # DVDConvertPathToEntrynum_US
	.long 0x801CA990 # DVDFastOpen_US
	.long 0x801CAB20 # DVDReadPrio_US
	.long 0x801CAA00 # DVDClose_US
	.long 0x802B8CC0 # HeapAlloc_US
	.long 0x802B8F70 # HeapFree_US
	.long 0x80377C48 # GameHeapArray_US
	.long 0x8042A44C # ArchiveHeap_US
	.long 0x802E17DC # sprintf_US
	.string "US"
	.byte 0
JPAddr:
	.long 0x8015F540 # OSReport_JP
	.long 0x801AF3E0 # OSFatal_JP
	.long 0x801CA490 # DVDConvertPaJP
	.long 0x801CA7A0 # DVDFastOpen_JP
	.long 0x801CA930 # DVDReadPrio_JP
	.long 0x801CA810 # DVDClose_JP
	.long 0x802B8AD0 # HeapAlloc_JP
	.long 0x802B8D80 # HeapFree_JP
	.long 0x803779C8 # GameHeapArraJP
	.long 0x8042A16C # ArchiveHeap_JP
	.long 0x802E15EC # sprintf_JP
	.string "JP"
	.byte 0
KRAddr:
	.long 0x8015FC70 # OSReport_KR
	.long 0x801AFB10 # OSFatal_KR
	.long 0x801CABC0 # DVDConvertPathToEntrynum_KR
	.long 0x801CAED0 # DVDFastOpen_KR
	.long 0x801CB060 # DVDReadPrio_KR
	.long 0x801CAF40 # DVDClose_KR
	.long 0x802B9200 # HeapAlloc_KR
	.long 0x802B94B0 # HeapFree_KR
	.long 0x80384948 # GameHeapArray_KR
	.long 0x804370EC # ArchiveHeap_KR
	.long 0x802E1D1C # sprintf_KR
	.string "KR"
	.byte 0
TWAddr:
	# Need to check these ports later
	.long 0x8015FC70 # OSReport_TW
	.long 0x801AFB10 # OSFatal_TW
	.long 0x801CABC0 # DVDConvertPathToEntrynum_TW
	.long 0x801CAED0 # DVDFastOpen_TW
	.long 0x801CB060 # DVDReadPrio_TW
	.long 0x801CAF40 # DVDClose_TW
	.long 0x802B9050 # HeapAlloc_TW
	.long 0x802B9300 # HeapFree_TW
	.long 0x80382D48 # GameHeapArray_TW
	.long 0x804354EC # ArchiveHeap_TW
	.long 0x802E1E14 # sprintf_TW
	.string "TW"
	.byte 0

NewerSystem: .string "/NewerRes/Systemxx_x.bin"
NewerDlcode: .string "/NewerRes/DLCodexx_x.bin"
NewerDlrel:  .string "/NewerRes/DLRelocsxx_x.bin"
.align 4

start:
	# Reserve 60 bytes on the stack and backup Link Register
	stwu      r1, -0x60(r1)
	mflr      r0

	# Backup GPRs
	stw       r0, 0x64(r1)
	stw       r31, 0x5C(r1)
	stw       r30, 0x58(r1)
	stw       r29, 0x54(r1)

	# Assuming version 1
	li        r6, '1'
	lis       r5, TEST_REGION@h
	ori       r5, r5, TEST_REGION@l

	# Read where bl to ~dStateMgrBase_c points to
	lwz       r3, 0(r5)
	clrlwi    r3, r3, 16
	cmpwi     r3, 0x6DE1
	beq       setEU
	cmpwi     r3, 0x6CA1
	beq       setUS
	cmpwi     r3, 0x6AB1
	beq       setJP
	cmpwi     r3, 0x71E1
	beq       checkKor
	cmpwi     r3, 0x14
	bne-      Return # Unknown version
	
	# Try testing for version 2
	# In version 2, the function is offset by 0x40
	li        r6, '2'
	lwz       r3, 0x40(r5)
	clrlwi    r3, r3, 16
	cmpwi     r3, 0x6DA1
	beq       setEU
	cmpwi     r3, 0x6C61
	beq       setUS
	cmpwi     r3, 0x6A71
	beq       setJP
	b         Return # There's nothing else we can do here...

checkKor:
	# The original address has the same value for both these regions, so we load 0x807689BE instead
	li        r6, '3'
	lwz       r3, -0x394(r5)
	clrlwi    r3, r3, 16
	cmpwi     r3, 0x7128
	beq       setKR
	cmpwi     r3, 0x5528
	beq       setTW
	b         Return # Unknown version once again

setEU:
	lis       r31, MASTERHI
	ori       r31, r31, MASTERLO+EUAddr@l
	b         RegionDetected
setUS:
	lis       r31, MASTERHI
	ori       r31, r31, MASTERLO+USAddr@l
	b         RegionDetected
setJP:
	lis       r31, MASTERHI
	ori       r31, r31, MASTERLO+JPAddr@l
	b         RegionDetected
setKR:
	lis       r31, MASTERHI
	ori       r31, r31, MASTERLO+KRAddr@l
	b         RegionDetected
setTW:
	lis       r31, MASTERHI
	ori       r31, r31, MASTERLO+TWAddr@l

RegionDetected:
	#-------------------------------------------------
	# Step 0: Insert region and version into filenames
	#-------------------------------------------------
	# System
	lis       r3, MASTERHI
	ori       r3, r3, MASTERLO+NewerSystem@l
	li        r4, 0x10 # Characters to replace are after 0x10
	bl        InsertRegionIntoFileName

	# DLCode (XX is in the same position, so no extra li needed)
	lis       r3, MASTERHI
	ori       r3, r3, MASTERLO+NewerDlcode@l
	bl        InsertRegionIntoFileName

	# DLRelocs
	lis       r3, MASTERHI
	ori       r3, r3, MASTERLO+NewerDlrel@l
	li        r4, 0x12 # Characters to replace are after 0x12
	bl        InsertRegionIntoFileName

	#-------------------------
	# Step 1: Load system file
	#-------------------------
	# Announcement
	lis       r3, MASTERHI
	ori       r3, r3, MASTERLO+Step1@l
	lis       r4, MASTERHI
	ori       r4, r4, MASTERLO+NewerSystem@l
	callFunc  OSReport

	# Locate file
	lis       r3, MASTERHI
	ori       r3, r3, MASTERLO+NewerSystem@l
	callFunc  DVDConvertPathToEntrynum
	cmpwi     r3, -1
	beq       ErrorFileLoad
	
	# Open file
	addi      r4, r1, 8
	callFunc  DVDFastOpen
	cmpwi     r3, 1
	bne       ErrorOpenFile

	#-------------------------
	# Step 2: Read system file
	#-------------------------
	# Announcement
	lis       r3, MASTERHI
	ori       r3, r3, MASTERLO+Step2@l
	lwz       r4, 0x3C(r1)
	callFunc  OSReport

	# Allocate memory
	lwz       r3, 0x3C(r1) # Size
	addi      r3, r3, 0x1F
	clrrwi    r3, r3, 5
	li        r4, 0x20 # Unk
	lwz       r5, 0x24(r31) # Pointer to heap
	lwz       r5, 0(r5)
	callFunc  HeapAlloc
	mr        r30, r3
	cmpwi     r30, 0
	beq       ErrorAllocMem
	
	# Read file
	addi      r3, r1, 8 # Pointer to DVDHandle
	mr        r4, r30 # Pointer to buffer
	lwz       r5, 0x3C(r1) # Length
	addi      r5, r5, 0x1F
	clrrwi    r5, r5, 5
	li        r6, 0 # Offset
	li        r7, 2 # Unk
	callFunc  DVDReadPrio
	
	#-----------------------------
	# Step 3: Apply system patches
	#-----------------------------
	# Announcement
	lis       r3, MASTERHI
	ori       r3, r3, MASTERLO+Step3@l
	callFunc  OSReport
	
	# Do patches
	mr        r3, r30
	bl        Patch
	
	#-------------------------
	# Step 4: Close system file
	#-------------------------
	# Announcement
	lis       r3, MASTERHI
	ori       r3, r3, MASTERLO+Step4@l
	callFunc  OSReport
	
	# Free heap
	mr        r3, r30 # Pointer to file
	lwz       r4, 0x24(r31) # Pointer to heap
	lwz       r4, 0(r4)
	callFunc  HeapFree
	
	# Close file
	addi      r3, r1, 8 # Pointer to DVDHandle
	callFunc  DVDClose

	#-------------------------
	# Step 5: Load dylink code
	#-------------------------
	# Announcement
	lis       r3, MASTERHI
	ori       r3, r3, MASTERLO+Step5@l
	lis       r4, MASTERHI
	ori       r4, r4, MASTERLO+NewerDlcode@l
	callFunc  OSReport
	
	# Locate file
	lis       r3, MASTERHI
	ori       r3, r3, MASTERLO+NewerDlcode@l
	callFunc  DVDConvertPathToEntrynum
	
	# Open file
	addi      r4, r1, 8
	callFunc  DVDFastOpen
	
	# Read file
	lwz       r3, 0x3C(r1)
	addi      r3, r3, 0x1F
	clrrwi    r3, r3, 5
	li        r4, 0x20
	lwz       r5, 0x20(r31)
	lwz       r5, 0(r5)
	callFunc  HeapAlloc
	mr        r29, r3
	addi      r3, r1, 8
	mr        r4, r29
	lwz       r5, 0x3C(r1)
	addi      r5, r5, 0x1F
	clrrwi    r5, r5, 5
	li        r6, 0
	li        r7, 2
	callFunc  DVDReadPrio
	
	# Close file
	addi      r3, r1, 8
	callFunc  DVDClose
	lis       r3, MASTERHI
	ori       r3, r3, MASTERLO+Step5DLCode@l
	mr        r4, r29
	callFunc  OSReport
	lis       r3, MASTERHI
	ori       r3, r3, MASTERLO+NewerDlrel@l
	callFunc  DVDConvertPathToEntrynum
	addi      r4, r1, 8
	callFunc  DVDFastOpen
	lwz       r3, 0x3C(r1)
	addi      r3, r3, 0x1F
	clrrwi    r3, r3, 5
	li        r4, 0x20
	lwz       r5, 0x24(r31)
	lwz       r5, 0(r5)
	callFunc  HeapAlloc
	mr        r30, r3
	#-------------------------
	# Step 6: Load relocs
	#-------------------------
	lis       r3, MASTERHI
	ori       r3, r3, MASTERLO+Step6@l
	callFunc  OSReport
	addi      r3, r1, 8
	mr        r4, r30
	lwz       r5, 0x3C(r1)
	addi      r5, r5, 0x1F
	clrrwi    r5, r5, 5
	li        r6, 0
	li        r7, 2
	callFunc  DVDReadPrio
	#-------------------------
	# Step 7: Link
	#-------------------------
	lis       r3, MASTERHI
	ori       r3, r3, MASTERLO+Step7@l
	callFunc  OSReport
	mr        r3, r29
	mr        r4, r30
	bl        DLPatch
	mr        r3, r30
	lwz       r4, 0x24(r31)
	lwz       r4, 0(r4)
	callFunc  HeapFree
	addi      r3, r1, 8
	callFunc  DVDClose
	#-----------------------
	# Step 8: Return to game
	#-----------------------
	lis       r3, MASTERHI
	ori       r3, r3, MASTERLO+Step8@l
	callFunc  OSReport

Return:
	li        r3, 1
	lwz       r31, 0x5C(r1)
	lwz       r30, 0x58(r1)
	lwz       r29, 0x54(r1)
	lwz       r0, 0x64(r1)
	mtlr      r0
	addi      r1, r1, 0x60
	blr

InsertRegionIntoFileName:
	# Inserts the region and version into the bin files
	add       r3, r3, r4
	lbz       r5, 0x2C(r31)
	stb       r5, 0(r3)
	lbz       r5, 0x2D(r31)
	stb       r5, 1(r3)
	stb       r6, 3(r3)
	blr

#-------------------------
# Static Memory Patch Code
#-------------------------
Patch:
	# Reserve 20 bytes on the stack and backup Link Register
	stwu      r1, -0x20(r1)
	mflr      r0

	# Backup GPRs
	stw       r0, 0x24(r1)
	stw       r31, 0x1C(r1)
	stw       r30, 0x18(r1)
	stw       r29, 0x14(r1)
	addi      r31, r3, -4

PatchLoop:
	# Load next 4 bytes
	lwzu      r30, 4(r31)
	
	# EoF is marked with 0xFFFFFFFF
	cmpwi     r30, -1
	beq       PatchDone

	# Determine patch size
	lis       r29, 0x8000
	and.      r29, r29, r30
	beq       PatchSetLength
	
	# Else, patch a single address
	lwzu      r29, 4(r31)
	stw       r29, 0(r30)
	clrrwi    r3, r30, 5
	dcbst     0, r3
	sync
	icbi      0, r3
	b         PatchLoop

PatchSetLength:
	mtctr     r30
	lwzu      r30, 4(r31)
	addi      r30, r30, -4

PatchAddressLoop:
	lwzu      r29, 4(r31)
	stwu      r29, 4(r30)
	clrrwi    r3, r30, 5
	dcbst     0, r3
	sync
	icbi      0, r3
	bdnz      PatchAddressLoop
	b         PatchLoop

PatchDone:
	sync
	isync
	lwz       r29, 0x14(r1)
	lwz       r30, 0x18(r1)
	lwz       r31, 0x1C(r1)
	lwz       r0, 0x24(r1)
	mtlr      r0
	addi      r1, r1, 0x20
	blr

#------------------------
# Dynamic Link Patch Code
#------------------------
DLPatch:
	lwz       r11, 8(r4)
	add       r11, r4, r11
	addi      r4, r4, 0xC
	b         DLPatchLoop

DLPatchBegin:
	lwz       r5, 0(r4)
	lwz       r7, 4(r4)
	addi      r4, r4, 8
	clrlslwi  r6, r5, 8,2
	srwi      r5, r5, 24
	lwzx      r6, r11, r6
	srwi.     r8, r6, 31
	bne       DLPatchContinue
	add       r6, r3, r6

DLPatchContinue:
	srwi.     r8, r7, 31
	bne       DLType
	add       r7, r3, r7

DLType:
	# R_PPC_ADDR32
	cmpwi     r5, 1
	beq       DLType1
	
	# R_PPC_ADDR16_LO
	cmpwi     r5, 4
	beq       DLType4
	
	# R_PPC_ADDR16_HI
	cmpwi     r5, 5
	beq       DLType5
	
	# R_PPC_ADDR16_HA
	cmpwi     r5, 6
	beq       DLType6
	
	# R_PPC_REL24
	cmpwi     r5, 0xA
	beq       DLType10
	
	# Unsupported relocation type, skip
	b         DLPatchFinalize

DLType1:
	stw       r6, 0(r7)
	b         DLPatchFinalize

DLType4:
	sth       r6, 0(r7)
	b         DLPatchFinalize

DLType5:
	srwi      r6, r6, 16
	sth       r6, 0(r7)
	b         DLPatchFinalize

DLType6:
	extrwi    r8, r6, 1,16
	srwi      r6, r6, 16
	add       r6, r6, r8
	sth       r6, 0(r7)
	b         DLPatchFinalize

DLType10:
	subf      r9, r7, r6
	clrlwi    r9, r9, 6
	lwz       r8, 0(r7)
	or        r8, r8, r9
	stw       r8, 0(r7)

DLPatchFinalize:
	dcbst     0, r7
	sync
	icbi      0, r7

DLPatchLoop:
	cmpw      r4, r11
	blt       DLPatchBegin
	sync
	isync
	blr

#-------------------
# Error-related code
#-------------------

ErrorFileLoad:
	lis       r3, MASTERHI
	ori       r3, r3, MASTERLO+sprinf_buffer@l
	lis       r4, MASTERHI
	ori       r4, r4, MASTERLO+ErrorFileMissing@l
	lis       r5, MASTERHI
	ori       r5, r5, MASTERLO+NewerSystem@l
	lis       r6, 0x8000
	lbz       r7, 3(r6)
	lbz       r6, 7(r6)
	callFunc  sprintf
	lis       r3, MASTERHI
	ori       r3, r3, MASTERLO+sprinf_buffer@l
	b         PanicWithMessage

ErrorOpenFile:
	lis       r3, MASTERHI
	ori       r3, r3, MASTERLO+ErrorOpenSys@l
	b         PanicWithMessage

ErrorAllocMem:
	lis       r3, MASTERHI
	ori       r3, r3, MASTERLO+ErrorAlloc@l

PanicWithMessage:
	mr        r5, r3
	lis       r3, MASTERHI
	ori       r3, r3, MASTERLO+OSFatalData@l
	lis       r4, MASTERHI+4
	ori       r4, r4, MASTERLO+OSFatalData@l+4
	callFunc  OSFatal

sprinf_buffer:
.org 0x850
.align 4

OSFatalData:
	.long 0xFFFFFFFF
	.long 0x00000000
Step1: .string "1: Loading system file: %s\n"
Step2: .string "2: File successfully opened. %d bytes. Reading...\n"
Step3: .string "3: File read. Now Patching\n"
Step4: .string "4: Patch applied\n"
Step5: .string "5: Loading dylink file: %s\n"
Step5DLCode: .string "DLCode @ %p\n"
Step6: .string "6: Loading relocs\n"
Step7: .string "7: Linking\n"
Step8: .string "8: Hacks loaded!\n"
ErrorOpenSys: .string "FATAL ERROR: System file could not be opened."
ErrorAlloc: .string "FATAL ERROR: Failed to allocate memory (%d)."
ErrorFileMissing:
	.ascii "Newer Super Mario All Stars Revived\n"
	.ascii "\n"
	.ascii "Something went wrong while installing.\n"
	.ascii "The following file is required, but can't be found on the SD card or USB drive:\n"
	.ascii "/NSMASDemo%s\n"
	.ascii "\n"
	.ascii "For troubleshooting and installation\n"
	.ascii "instructions, see:\n"
	.ascii "    https://discord.gg/A4WSCRZ\n"
	.ascii "\n"
	.ascii "Disc version: %d\n"
	.ascii "Game region: %c"
	.align 4