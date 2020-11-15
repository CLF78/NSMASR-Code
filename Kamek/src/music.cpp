#include "music.h"

// name, hasFast, hasDrums
const CustomStream SongNameList [] = {
	{"SMBGRASS", true, false},
	{"SMBUNDERGROUND", true, false},
	{"SMBUNDERWATER", true, false},
	{"SMBBONUS", true, false},
	{"SMBCASTLE", true, false},
	{"INTRO", false, false},
	{"SMBSNOW", true, false},
	{"SMBSNOWNIGHT", true, false},
	{"SMBGRASSNIGHT", true, false},
	{NULL, NULL, NULL}
};

inline char *BrsarInfoOffset(u32 offset) {
	return (char*)(*(u32*)(((u32)SoundRelatedClass) + 0x5CC)) + offset;
}

u8 hijackMusicWithSongName(const char *songName, int themeID, bool hasFast, int channelCount, int trackCount, int *wantRealStreamID) {
	Hijacker *hj = &Hijackers[channelCount==4?1:0];

	// If it's just an area transition where both areas are using the same song, we don't want the music to restart
	if ((themeID >= 0) && hj->currentCustomTheme == themeID)
		return hj->stream[hj->currentStream].originalID;

	// Which one do we use this time...?
	int toUse = (hj->currentStream + 1) & 1;

	hj->currentStream = toUse;
	hj->currentCustomTheme = themeID;

	// Write the stream's info
	HijackedStream *stream = &hj->stream[hj->currentStream];

	if (stream->infoOffset) {
		u16 *thing = (u16*)(BrsarInfoOffset(stream->infoOffset) + 4);
		OSReport("Modifying stream info, at offset %x which is at pointer %x\n", stream->infoOffset, thing);
		OSReport("It currently has: channel count %d, track bitfield 0x%x\n", thing[0], thing[1]);
		thing[0] = channelCount;
		thing[1] = (1 << trackCount) - 1;
		OSReport("It has been set to: channel count %d, track bitfield 0x%x\n", thing[0], thing[1]);
	}

	sprintf(BrsarInfoOffset(stream->stringOffset), "stream/%s.brstm", songName);
	sprintf(BrsarInfoOffset(stream->stringOffsetFast), hasFast?"stream/%s_F.brstm":"stream/%s.brstm", songName);

	// Done!
	if (wantRealStreamID)
		*wantRealStreamID = stream->streamID;

	return stream->originalID;
}

u8 GetMusicForZone(u8 realThemeID) {
	if (realThemeID < 100)
		return realThemeID;

	// Failsafe that checks the size of the song list to avoid invalid IDs
	if (realThemeID - 100 > (sizeof(SongNameList) / sizeof(CustomStream)) - 2)
		return 0;

	CustomStream entry = SongNameList[realThemeID - 100];
	return hijackMusicWithSongName(entry.name, realThemeID, entry.hasFast, entry.hasDrums?4:2, entry.hasDrums?2:1, 0);
}
