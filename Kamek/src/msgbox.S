#include <asm_setup.S>

.extern Global5758

.global MessageBoxIsShowing
.global Query5758Replacement

.text
Query5758Replacement:
	mr r5, r3				# Save param
	lis r4, Global5758@ha	# Get 5758
	lwz r0, Global5758@l(r4)
	and r3, r0, r3			# And it with param
	andi. r5, r5, 1			# If param & 1 == 0...
	beqlr					# ...then return
	
	lis r4, MessageBoxIsShowing@ha
	lwz r4, MessageBoxIsShowing@l(r4)
	or r3, r3, r4			# Otherwise, OR with MessageBoxIsShowing
	blr						# Then return

.data
MessageBoxIsShowing:
	.byte 0
