#include <game.h>
#include <sfx.h>
#include "koopatlas/mapmusic.h"
#include "music.h"

extern "C" void PlaySoundWithFunctionB4(void *spc, nw4r::snd::SoundHandle *handle, int id, int unk);

static nw4r::snd::StrmSoundHandle s_handle;
static bool s_playing = false;

static nw4r::snd::SoundHandle s_starHandle;
static bool s_starPlaying = false;

static int s_song = -1;
static int s_nextSong = -1;

static int s_countdownToSwitch = -1;

#define FADE_OUT_LEN 30

u8 hijackMusicWithSongName(const char *songName, int themeID, bool hasFast, int channelCount, int trackCount, int *wantRealStreamID);

void dKPMusic::play(int id) {
	if (s_playing) {
		// Switch track
		OSReport("Trying to switch to song %d (Current one is %d)...\n", id, s_song);
		if ((s_song == id && s_nextSong == -1) || s_nextSong == id) {
			OSReport("This song is already playing or is scheduled. Not gonna do that.\n");
		} else if (s_countdownToSwitch >= 0) {
			OSReport("We were already going to switch tracks, but the rstm hasn't been changed yet, so the next song is being changed to this one\n");
			s_nextSong = id;
		} else {
			OSReport("Will switch; Fading out current track 2 over %d frames\n", FADE_OUT_LEN);

			if (s_handle.Exists())
				s_handle.SetTrackVolume(1<<1, FADE_OUT_LEN, 0.0f);
				s_handle.Stop(FADE_OUT_LEN);

			s_countdownToSwitch = FADE_OUT_LEN;

			OSReport("Playing song %d from the start.\n", id);

			int realStreamID;
			char brstmName[8];
			sprintf(brstmName, "map%d", id);
			hijackMusicWithSongName(brstmName, -1, false, 2, 1, &realStreamID);

			PlaySoundWithFunctionB4(SoundRelatedClass, &s_handle, realStreamID, 1);

			s_song = id;
			s_nextSong = -1;
		}

	} else {
		// New track
		OSReport("Playing song %d from the start.\n", id);

		int realStreamID;
		char brstmName[8];
		sprintf(brstmName, "map%d", id);
		hijackMusicWithSongName(brstmName, -1, false, 2, 1, &realStreamID);

		PlaySoundWithFunctionB4(SoundRelatedClass, &s_handle, realStreamID, 1);

		s_playing = true;
		s_song = id;
		s_nextSong = -1;
	}
}

#include "fileload.h"

void dKPMusic::execute() {
	if (!s_playing)
		return;

	if (s_handle.GetSound() == 0) {
		nw4r::db::Exception_Printf_("SOUND IS NOT PLAYING!\n");
		return;
	}

	if (s_countdownToSwitch >= 0) {
		s_countdownToSwitch--;
		if (s_countdownToSwitch == 0) {
			nw4r::db::Exception_Printf_("Switching brstm files...\n");
		}
	}
}

void dKPMusic::stop() {
	if (!s_playing)
		return;

	OSReport("Stopping song\n");

	s_playing = false;
	s_song = -1;
	s_nextSong = -1;
	s_countdownToSwitch = -1;

	if (s_handle.Exists())
		s_handle.Stop(30);

	if (s_starHandle.Exists())
		s_starHandle.Stop(15);
}


void dKPMusic::playStarMusic() {
	if (s_starPlaying)
		return;

	PlaySoundWithFunctionB4(SoundRelatedClass, &s_starHandle, SE_BGM_CS_STAR, 1);
	s_starPlaying = true;
}
