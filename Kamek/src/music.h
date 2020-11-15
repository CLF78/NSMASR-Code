#ifndef MUSIC_H
#define MUSIC_H
#include <game.h>
#include <sfx.h>
#include "fileload.h"

extern void *SoundRelatedClass;

struct HijackedStream {
	u32 stringOffset;
	u32 stringOffsetFast;
	u32 infoOffset;
	u8 originalID;
	int streamID;
};

struct Hijacker {
	HijackedStream stream[2];
	u8 currentStream;
	u8 currentCustomTheme;
};

struct CustomStream {
	const char* name;
	bool hasFast;
	bool hasDrums;
};

// The following offsets are from the start of the INFO block, not the start of the brsar.
// The first couple is for two-channel songs and the other is for four-channel ones

HijackedStream Athletic = {0x29638, 0x29678, 0x26404, 4, STRM_BGM_ATHLETIC};
HijackedStream Shiro = {0x2A028, 0x2A060, 0x26EA4, 10, STRM_BGM_SHIRO};
HijackedStream Chijou = {0x2957C, 0x295F4, 0x29560, 1, STRM_BGM_CHIJOU};
HijackedStream Chika = {0x295B8, 0x294C0, 0x2959C, 2, STRM_BGM_CHIKA};

Hijacker Hijackers[2] = {
	{Athletic, Shiro, 0, 0},
	{Chijou, Chika, 0, 0}
};

#endif /* MUSIC_H */
