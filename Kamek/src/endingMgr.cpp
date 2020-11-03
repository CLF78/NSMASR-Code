#include <game.h>
#include <playerAnim.h>
#include <sfx.h>
#include <stage.h>

extern void *SoundRelatedClass;

static float manipFourPlayerPos(int id, float pos) {
	return pos - ((3 - id) * 20.0f);
}

static daEnBossKoopaDemoPeach_c *getPeach() {
	return (daEnBossKoopaDemoPeach_c*)dEn_c::search(EN_BOSS_KOOPA_DEMO_PEACH);
}

class dEndingMgr_c : public daBossDemo_c {
	int onCreate();
	int onDelete();

	void init();

	USING_STATES(dEndingMgr_c);
	DECLARE_STATE_VIRTUAL(DemoSt);
	DECLARE_STATE(GoRight);
	DECLARE_STATE(LookUp);
	DECLARE_STATE(JumpOntoSwitch);
	DECLARE_STATE(ThanksPeach);

	dAcPy_c *players[4];

	int timer;

	static dEndingMgr_c *build();
};

dEndingMgr_c *dEndingMgr_c::build() {
	void *buf = AllocFromGameHeap1(sizeof(dEndingMgr_c));
	return new(buf) dEndingMgr_c;
}

CREATE_STATE(dEndingMgr_c, DemoSt);
CREATE_STATE(dEndingMgr_c, GoRight);
CREATE_STATE(dEndingMgr_c, LookUp);
CREATE_STATE(dEndingMgr_c, JumpOntoSwitch);
CREATE_STATE(dEndingMgr_c, ThanksPeach);

int dEndingMgr_c::onCreate() {
	if (StageE4::instance->currentBossDemo) {
		fBase_c::Delete();
		return false;
	}

	StageE4::instance->currentBossDemo = this;

	init();

	acState.setState(&StateID_DemoSt);
	acState.executeNextStateThisTick();

	return true;
}

int dEndingMgr_c::onDelete() {
	nw4r::snd::SoundHandle *fanfare = (nw4r::snd::SoundHandle*)(((u32)SoundRelatedClass) + 0x900);

	if (fanfare->Exists())
		fanfare->Stop(60);

	daBossDemo_c::onDelete();
	WLClass::instance->disableDemoControl(true);
	return true;
}

void dEndingMgr_c::init() {
	dStageActorMgr_c::instance->_BCA = true;
	WLClass::instance->demoControlAllPlayers();
	BalloonRelatedClass::instance->_20 = 1;
}

////////////////
// Demo State //
////////////////

void dEndingMgr_c::beginState_DemoSt() {
	_360 = 1;
}

void dEndingMgr_c::executeState_DemoSt() {
	for (int i = 0; i < 4; i++) {
		dAcPy_c *player;
		if (player = dAcPy_c::findByID(i)) {
			if (!player->isReadyForDemoControlAction())
				return;
		}
	}
	acState.setState(&StateID_GoRight);
}

void dEndingMgr_c::endState_DemoSt() {
}

////////////////////
// Go Right State //
////////////////////

void dEndingMgr_c::beginState_GoRight() {
	timer = 0;

	// Sort the players into a list
	// We shall first find the rightmost player, then the second rightmost, then...

	float maxBound = 50000.0f;
	for (int targetOffs = 3; targetOffs >= 0; targetOffs--) {
		float maxX = 0.0f;
		dAcPy_c *maxPlayer = 0;

		for (int check = 0; check < 4; check++) {
			if (dAcPy_c *player = dAcPy_c::findByID(check)) {
				if (player->pos.x >= maxBound)
					continue;

				if (player->pos.x > maxX) {
					maxX = player->pos.x;
					maxPlayer = player;
				}
			}
		}

		maxBound = maxX;
		players[targetOffs] = maxPlayer;
	}

	// And now move them
	float speed = 2.0f;
	for (int i = 0; i < 4; i++) {
		if (dAcPy_c *player = players[i]) {
			float target = manipFourPlayerPos(i, 1060.0f);
			player->moveInDirection(&target, &speed);
		}
	}
}

void dEndingMgr_c::executeState_GoRight() {
	for (int i = 0; i < 4; i++) {
		if (dAcPy_c *player = players[i]) {
			if (player->pos.x > manipFourPlayerPos(i, 920.0f))
				player->demoMoveSpeed *= 0.994f;
		}
	}

	// Can we leave this state yet?
	for (int i = 0; i < 4; i++) {
		if (dAcPy_c *player = players[i]) {
			if (!player->isReadyForDemoControlAction())
				return;
		}
	}

	// We can leave
	timer++;
	if (timer >= 20)
		acState.setState(&StateID_LookUp);
}

void dEndingMgr_c::endState_GoRight() {
}

///////////////////
// Look Up State //
///////////////////

void dEndingMgr_c::beginState_LookUp() {
	_120 |= 8;
	lookAtMode = 2; // Higher maximum distance
	timer = 0;
}

void dEndingMgr_c::executeState_LookUp() {
	timer++;

	if (timer == 60)
		_120 &= ~8;

	else if (timer >= 90)
		acState.setState(&StateID_JumpOntoSwitch);
}

void dEndingMgr_c::endState_LookUp() {
}

//////////////////////////
// Jump on Switch State //
//////////////////////////

void dEndingMgr_c::beginState_JumpOntoSwitch() {
	float speed = 2.0f;
	for (int i = 0; i < 4; i++) {
		if (dAcPy_c *player = players[i]) {
			float target = manipFourPlayerPos(i, 1144.0f);
			player->moveInDirection(&target, &speed);
		}
	}

	timer = 0;
}

void dEndingMgr_c::executeState_JumpOntoSwitch() {
	dAcPy_c *player = players[3];

	if (timer > 60)
		acState.setState(&StateID_ThanksPeach);
	else if (player->isReadyForDemoControlAction())
		player->input.setTransientForcedButtons(WPAD_DOWN);
	else if (timer > 30)
		player->input.unsetPermanentForcedButtons(WPAD_TWO);
	else if (timer > 15)
		player->input.setPermanentForcedButtons(WPAD_TWO);

	timer++;
}

void dEndingMgr_c::endState_JumpOntoSwitch() {
}

////////////////////////
// Thanks Peach State //
////////////////////////

void dEndingMgr_c::beginState_ThanksPeach() {
	timer = -1;
	getPeach()->doStateChange(&daEnBossKoopaDemoPeach_c::StateID_Turn);
}

void dEndingMgr_c::executeState_ThanksPeach() {
	daEnBossKoopaDemoPeach_c *peach = getPeach();
	dStateBase_c *peachSt = peach->acState.getCurrentState();

	if (peach->stage == 1 && peachSt == &peach->StateID_Turn) {
		timer++;
		if (timer == 1)
			// This plays the Peach harp fanfare
			sub_8019C390(_8042A788, 6);

		else if (timer > 20) {
			peach->doStateChange(&peach->StateID_Open);
			peach->_120 |= 8;
			timer = -1;
		}
	}

	else if (peach->stage == 1 && peachSt == &peach->StateID_Open)
		peach->doStateChange(&peach->StateID_Rescue);

	else if (peach->stage == 3 && peachSt == &peach->StateID_Rescue)
		peach->doStateChange(&peach->StateID_Thank);

	else if (peachSt == &peach->StateID_Thank) {
		// 1. Freeze Peach by changing to Stage 8 as soon as she's almost done
		// 2. Do our thing
		// 3. Go back!
		if (peach->stage == 0 && peach->counter == 33 && timer == -1) {
			peach->stage = 8;
			timer = 0;
		} else if (peach->stage == 8) {
			timer++;
			if (timer == 8) {
				float target = peach->pos.x - 20.0f;
				float speed = 1.5f;
				players[3]->moveInDirection(&target, &speed);
			} else if (timer == 90) {
				peach->stage = 0;
				players[3]->setAnimePlayWithAnimID(dm_escort);
			}
		} else if (peach->stage == 7) {
			// All the players are glad
			timer = 0;
			peach->stage = 9;
			for (int i = 0; i < 4; i++) {
				if (dAcPy_c *player = players[i]) {
					OSReport("Going to dmglad\n");
					OSReport("Player %d states: %s and %s\n", i, player->states2.getCurrentState()->getName(), player->demoStates.getCurrentState()->getName());
					player->setAnimePlayWithAnimID(dm_glad);

					static const int vocs[4] = {
						SE_VOC_MA_SAVE_PRINCESS,
						SE_VOC_LU_SAVE_PRINCESS,
						SE_VOC_KO_SAVE_PRINCESS,
						SE_VOC_KO2_SAVE_PRINCESS
					};

					nw4r::snd::SoundHandle handle;
					PlaySoundWithFunctionB4(SoundRelatedClass, &handle, vocs[player->settings & 0xF], 1);

					int powerup = *((u32*)( 0x1090 + ((u8*)player) ));
					handle.SetPitch(powerup == 3 ? 1.5f : 1.0f);
				}
			}
		} else if (peach->stage == 9) {
			timer++;
			if (timer == 90) {
				RESTART_CRSIN_LevelStartStruct.purpose = 0;
				RESTART_CRSIN_LevelStartStruct.world1 = 6;
				RESTART_CRSIN_LevelStartStruct.world2 = 6;
				RESTART_CRSIN_LevelStartStruct.level1 = 40;
				RESTART_CRSIN_LevelStartStruct.level2 = 40;
				RESTART_CRSIN_LevelStartStruct.areaMaybe = 0;
				RESTART_CRSIN_LevelStartStruct.entrance = 0xFF;
				RESTART_CRSIN_LevelStartStruct.unk4 = 0; // Does not load replay
				DontShowPreGame = true;
				ExitStage(RESTART_CRSIN, 0, BEAT_LEVEL, MARIO_WIPE);
			}
		}
	}
}

void dEndingMgr_c::endState_ThanksPeach() {
}
