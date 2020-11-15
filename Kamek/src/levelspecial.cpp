#include "levelspecial.h"
#define ACTIVATE	1
#define DEACTIVATE	0

extern "C" void dAcPy_vf294(void *Mario, dStateBase_c *state, u32 unk);

class dLevelSpecial_c : public dStageActor_c {
	int onCreate();
	int onExecute();
	int onDelete();
	int onDraw();

	u8 event;
	u32 setTime;
	u8 type;
	u8 effect;
	u8 lastEvState;

	static dLevelSpecial_c *build();
};

dLevelSpecial_c *dLevelSpecial_c::build() {
	void *buffer = AllocFromGameHeap1(sizeof(dLevelSpecial_c));
	return new(buffer) dLevelSpecial_c;
}

int dLevelSpecial_c::onCreate() {
	this->event = ((this->settings >> 24) & 0xFF) - 1;

	this->type = (this->settings) & 0xF;
	this->effect = (this->settings >> 4) & 0xF;
	this->setTime = (this->settings >> 8) & 0xFFFF;

	this->lastEvState = 0xFF;
	return true;
}

int dLevelSpecial_c::onExecute() {
	bool newEvState = (dFlagMgr_c::instance->active(this->event));

	if (newEvState == this->lastEvState)
		return true;

	if (newEvState == ACTIVATE) {
		switch (this->type) {
			case 1:											// Level Warper
				WarpWorld = this->settings >> 20 & 0xF;
				WarpLevel = this->settings >> 12 & 0xFF;
				WarpMapID = this->settings >> 4 & 0xFF;
				WarpIsEnabled = true;
				break;

			case 2:											// Stop Time
				TimeKeeper::instance->isPaused = true;
				break;

			case 3:											// Mario Gravity
				if (this->effect == 0) {					// Low grav
					MarioDescentRate = -2;
					MarioJumpArc = 0.5;
					MiniMarioJumpArc = 0.5;
					MarioJumpMax = 4.5;
				} else {									// Anti-grav
					MarioDescentRate = 0.5;
					MarioJumpArc = 4.0;
					MiniMarioJumpArc = 4.0;
					MarioJumpMax = 0.0;
				}
				break;

			case 4:											// Set Time
				TimeKeeper::instance->setTime(this->setTime);
				break;

			case 5:											// Global Enemy Size
				SizerOn = 3;

				GlobalSpriteSize = GlobalSizeFloatModifications[this->effect];
				GlobalRiderSize = GlobalRiderFloatModifications[this->effect];
				GlobalSpriteSpeed = GlobalRiderFloatModifications[this->effect];
				break;

			case 6:											// Individual Enemy Size
				SizerOn = (this->effect == 0) ? 2 : 1;
				break;

			case 7:											// Z Order Hack
				ZOrderOn = true;
				break;

			case 8:											// No Multiplayer Bubbling
				NoMichaelBuble = true;
				break;

			case 9:											// BG Scale Modifier
				BGScaleEnabled = true;
				BGScaleFront.x = BGScaleChoices[(this->settings >> 20) & 0xF];
				BGScaleFront.y = BGScaleChoices[(this->settings >> 16) & 0xF];
				BGScaleBack.x = BGScaleChoices[(this->settings >> 12) & 0xF];
				BGScaleBack.y = BGScaleChoices[(this->settings >> 8) & 0xF];
				break;

			default:
				break;
		}
	} else {
		switch (this->type) {
			case 1:											// Level Warper
				WarpIsEnabled = false;
				break;

			case 2:											// Stop Timer
				TimeKeeper::instance->isPaused = false;
				break;

			case 3:											// Mario Gravity
				MarioDescentRate = -4;
				MarioJumpArc = 2.5;
				MiniMarioJumpArc = 2.5;
				MarioJumpMax = 3.628;
				break;

			case 5:											// Global Enemy Size
				SizerOn = 0;

				GlobalSpriteSize = 1.0;
				GlobalRiderSize = 1.0;
				GlobalSpriteSpeed = 1.0;
				break;

			case 6:											// Individual Enemy Size
				SizerOn = 0;
				break;

			case 7:											// Z Order Hack
				ZOrderOn = false;
				break;

			case 8:											// No Multiplayer Bubbling
				NoMichaelBuble = false;
				break;

			case 9:											// BG Scale Modifier
				BGScaleEnabled = false;
				break;

			default:
				break;
		}
	}

	this->lastEvState = newEvState;
	return true;
}

int dLevelSpecial_c::onDelete() {
	return true;
}

int dLevelSpecial_c::onDraw() {
	return true;
}

/////////////////////
// Other Functions //
/////////////////////

void BubbleDisabler() {
	dCourse_c *course = dCourseFull_c::instance->get(GetAreaNum());

	bool thing = false;
	int zone = GetZoneNum();

	for (int i = 0; i < course->zoneSpriteCount[zone]; i++) {
		dCourse_c::sprite_s *spr = &course->zoneFirstSprite[zone][i];
		if (spr->type == 246 && (spr->settings & 0xF) == 8) {
			thing = true;
			break;
		}
	}

	if (thing) {
		OSReport("DISABLING EXISTING BUBBLES.\n");
		for (int i = 0; i < 4; i++)
			Player_Flags[i] &= ~4;
	}
}

void DoWarpZoneHack(int world, int level) {
	// First, save the progress of the level we warped from
	if (CurrentWorld < 10 && CurrentLevel < 42) {
		// Default to secret exit, as we don't need to enable it in LevelInfo
		SaveBlock *save = GetSaveFile()->GetBlock(-1);
		save->SetLevelCondition(CurrentWorld, CurrentLevel, COND_SECRET);

		// Save the star coins too, as those get lost during the switch.
		for (int i = 0; i < 3; i++) {
			if (StarCoinStatus[i] != 4)
				save->SetLevelCondition(CurrentWorld, CurrentLevel, 1 << i);
		}
	}

	// Start wipe
	ActivateWipe(WIPE_MARIO);
	hasWarped = true;

	// Setup the struct
	RESTART_CRSIN_LevelStartStruct.purpose = 0;
	RESTART_CRSIN_LevelStartStruct.world1 = world-1;
	RESTART_CRSIN_LevelStartStruct.world2 = world-1;
	RESTART_CRSIN_LevelStartStruct.level1 = level-1;
	RESTART_CRSIN_LevelStartStruct.level2 = level-1;
	RESTART_CRSIN_LevelStartStruct.areaMaybe = 0;
	RESTART_CRSIN_LevelStartStruct.entrance = 0xFF;
	RESTART_CRSIN_LevelStartStruct.unk4 = 0;

	// Go!
	DoSceneChange(RESTART_CRSIN, 0, 0);
	return;
}

void MarioStateChanger(void *Mario, dStateBase_c *state, u32 unk) {
	if ((strcmp(state->getName(), "dAcPy_c::StateID_Balloon") == 0) && (NoMichaelBuble))
		return;

	dAcPy_vf294(Mario, state, unk);
}

bool ResetAfterLevel(bool didItWork) {
	WarpWorld = 0;
	WarpLevel = 0;
	WarpIsEnabled = false;

	MarioDescentRate = -4;
	MarioJumpMax = 3.628;
	MarioJumpArc = 2.5;
	MiniMarioJumpArc = 2.5;

	GlobalRiderSize = 1.0;
	GlobalSpriteSize = 1.0;
	GlobalSpriteSpeed = 1.0;
	SizerOn = 0;
	ZOrderOn = false;

	BGScaleEnabled = 0;

	NoMichaelBuble = false;

	// Additional values here
	GlobalStarsCollected = 0;
	CameraLockEnabled = 0;
	isLockPlayerRotation = false;
	return didItWork;
}
