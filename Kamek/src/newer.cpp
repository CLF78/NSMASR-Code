#include <game.h>
#include <newer.h>
#include "levelinfo.h"

struct GEIFS {
	int starCoins, exits;
};

int lastLevelIDs[] = {
	-1, /*no world*/
	27, 27, 27, 27, 27, 27, 27, 25,
	10,
	24, 24, 21, 24, 3
};

int getStarCoinCount() {
	SaveBlock *save = GetSaveFile()->GetBlock(-1);
	int coinsEarned = 0;

	for (int w = 0; w < 10; w++) {
		for (int l = 0; l < 42; l++) {
			u32 conds = save->GetLevelCondition(w, l);

			if (conds & COND_COIN1)
				coinsEarned++;
			if (conds & COND_COIN2)
				coinsEarned++;
			if (conds & COND_COIN3)
				coinsEarned++;
		}
	}

	return coinsEarned;
}

extern "C" GEIFS *GrabExitInfoForFileSelect(GEIFS *out, SaveBlock *save) {
	out->starCoins = 0;
	out->exits = 0;

	for (int i = 0; i < dLevelInfo_c::s_info.sectionCount(); i++) {
		dLevelInfo_c::section_s *section = dLevelInfo_c::s_info.getSectionByIndex(i);

		for (int j = 0; j < section->levelCount; j++) {
			dLevelInfo_c::entry_s *l = &section->levels[j];
			if (l->flags & 2) {
				u32 cond = save->GetLevelCondition(l->worldSlot, l->levelSlot);
				if ((l->flags & 0x10) && (cond & COND_NORMAL))
					out->exits++;
				if ((l->flags & 0x20) && (cond & COND_SECRET))
					out->exits++;
				if (cond & COND_COIN1)
					out->starCoins++;
				if (cond & COND_COIN2)
					out->starCoins++;
				if (cond & COND_COIN3)
					out->starCoins++;
			}
		}
	}

	OSReport("Done, got %d coins and %d exits\n", out->starCoins, out->exits);
	return out;
}
