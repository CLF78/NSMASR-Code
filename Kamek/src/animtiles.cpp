#include <common.h>
#include <game.h>
#include "fileload.h"

struct AnimDef_Header {
	u32 magic;
	u32 entryCount;
};

struct AnimDef_Entry {
	u16 texNameOffset;
	u16 frameDelayOffset;
	u16 tileNum;
	u8 tilesetNum;
	u8 reverse;
};

FileHandle fh;

void DoTiles(void* self) {
	AnimDef_Header *header;
	
	header = (AnimDef_Header*)LoadFile(&fh, "/NewerRes/AnimTiles.bin");
	
	if (!header) {
		OSReport("File header missing\n");
		return;
	}
	
	if (header->magic != 'NWRa') {
		OSReport("Magic check failed\n");
		FreeFile(&fh);
		return;
	}
	
	AnimDef_Entry *entries = (AnimDef_Entry*)(header+1);
	
	for (int i = 0; i < header->entryCount; i++) {
		AnimDef_Entry *entry = &entries[i];
		char *name = (char*)fh.filePtr+entry->texNameOffset;
		char *frameDelays = (char*)fh.filePtr+entry->frameDelayOffset;
		
		char realName[0x40];
		snprintf(realName, 0x40, "BG_tex/%s", name);
		
		void *blah = BgTexMng__LoadAnimTile(self, entry->tilesetNum, entry->tileNum, realName, frameDelays, entry->reverse);
	}
}


void DestroyTiles(void *self) {
	FreeFile(&fh);
}
