#include <asm_setup.S>

.global ReplaceExceptionOccurred

.text
ReplaceExceptionOccurred:
	lis r3, NewExceptionOccurred@h
	ori r3, r3, NewExceptionOccurred@l
	blr

.data
NewExceptionOccurred:
.string "Mamma mia, the game has crashed!\nPlease take note of when the crash occurred\nand bug CLF78 so he fixes it.\nYou can find him at https://discord.gg/A4WSCRZ.\n"
.align 4
