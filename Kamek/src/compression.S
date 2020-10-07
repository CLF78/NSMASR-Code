#include <asm_setup.S>

.global GetFileEntrynumNew
.global InitCompression
.extern AllocFromGameHeap1
.extern DVDConvertPathToEntrynum
.extern DecompBufferPointer
.extern LHDecompClass
.extern StoreCompressionClassList
.extern TryAndFindCompressedFile

.text
.align 4

InitCompression:
	stwu sp, -0x10(sp)
	mflr r0
	stw r0, 0x14(sp)
	
	# Prolog
	lis r3, CCCL@h
	ori r3, r3, CCCL@l
	lis r4, CCCL_End@h
	ori r4, r4, CCCL_End@l
	bl StoreCompressionClassList

	# Allocate a buffer for decompression stuff
	li r3, 0x8B4
	bl AllocFromGameHeap1
	lis r4, DecompBufferPointer@h
	ori r4, r4, DecompBufferPointer@l
	stw r3, 0(r4)

	# Epilog
	li r3, 1
	lwz r0, 0x14(sp)
	mtlr r0
	addi sp, sp, 0x10
	blr

GetFileEntrynumNew:
	stwu sp, -0x20(sp)
	mflr r0
	stw r0, 0x24(sp)
	stw r31, 0x1C(sp)
	stw r30, 0x18(sp)
	stw r29, 0x14(sp)

	# Compression Type pointer (byte*)
	mr r30, r4

	# Filename
	mr r29, r3

	# First, try compression - don't bother checking 5278, it's always on in NSMBW anyway
	bl TryAndFindCompressedFile
	cmpwi r3, -1
	bne returnEntrynum

	# Didn't find it, so try the normal version
	mr r3, r29
	bl DVDConvertPathToEntrynum
	
	# Return value gets passed through
	cmpwi r30, 0
	beq returnEntrynum
	li r0, 0
	stb r0, 0(r30)

returnEntrynum:
	lwz r31, 0x1C(sp)
	lwz r30, 0x18(sp)
	lwz r29, 0x14(sp)
	lwz r0, 0x24(sp)
	mtlr r0
	addi sp, sp, 0x20
	blr

.data
CCCL: .long LHDecompClass
CCCL_End: .long 0
