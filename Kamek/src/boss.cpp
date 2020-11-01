#include "boss.h"

void SetupKameck(daBoss *actor, daKameckDemo *Kameck) {
	// Stop the BGM Music
	StopBGMMusic();

	// Set the necessary Flags and make Mario enter Demo Mode
	dStage32C_c::instance->freezeMarioBossFlag = 1;
	WLClass::instance->_4 = 4;
	WLClass::instance->_8 = 0;

	MakeMarioEnterDemoMode();

	// Make sure to use the correct position
	Vec pos = (Vec){actor->pos.x - 124.0, actor->pos.y + 104.0, 3564.0};
	S16Vec rot = (S16Vec){0, 0, 0};

	// Create And use Kameck
	actor->Kameck = (daKameckDemo*)actor->createChild(KAMECK_FOR_CASTLE_DEMO, (dStageActor_c*)actor, 0, &pos, &rot, 0);
	actor->Kameck->doStateChange(&daKameckDemo::StateID_DemoWait);
}

void CleanupKameck(daBoss *actor, daKameckDemo *Kameck) {
	// Clean up the flags and Kameck
	dStage32C_c::instance->freezeMarioBossFlag = 0;
	WLClass::instance->_8 = 1;

	MakeMarioExitDemoMode();
	StartBGMMusic();

	actor->Kameck->Delete(1);
}

bool GrowBoss(daBoss *actor, daKameckDemo *Kameck, float initialScale, float endScale, float yPosModifier, int timer) {
	if (timer == 130)
		actor->Kameck->doStateChange(&daKameckDemo::StateID_DemoSt);

	else if (timer == 150)
		PlaySound(actor, SE_BOSS_IGGY_WANWAN_TO_L);

	else if ((timer > 150) && (timer < 230)) {
		float scaleSpeed, modifier;

		scaleSpeed = (endScale - initialScale) / 80.0;
		modifier = initialScale + ((timer - 150) * scaleSpeed);

		actor->scale = (Vec){modifier, modifier, modifier};
		actor->pos.y = actor->pos.y + (yPosModifier/80.0);

	} else if (timer == 360) {
		Vec tempPos = (Vec){actor->pos.x - 40.0, actor->pos.y + 120.0, 3564.0};
		int EfList [7] = {563, 489, 490, 639, 605, 564, 608};
		for (int i = 0; i < 7; i++)
			SpawnEffectByNum(EfList[i], 0, &tempPos, &(S16Vec){0,0,0}, &(Vec){1.0, 1.0, 1.0});
	}

	else if (timer == 400)
		actor->Kameck->doStateChange(&daKameckDemo::StateID_DemoSt2);

	return (timer > 420);
}

void OutroSetup(daBoss *actor) {
	actor->removeMyActivePhysics();

	StopBGMMusic();

	WLClass::instance->_4 = 5;
	WLClass::instance->_8 = 0;
	dStage32C_c::instance->freezeMarioBossFlag = 1;

	nw4r::snd::SoundHandle handle;
	PlaySoundWithFunctionB4(SoundRelatedClass, &handle, SE_BOSS_CMN_DAMAGE_LAST, 1);
}

bool ShrinkBoss(daBoss *actor, Vec *pos, float scale, int timer) {
	// Adjust actor to equal the scale of your boss / 80.
	actor->scale.x -= scale / 80.0;
	actor->scale.y -= scale / 80.0;
	actor->scale.z -= scale / 80.0;

	if (timer == 30) {
		int EfList [3] = {648, 401, 607};
		for (int i = 0; i < 3; i++)
			SpawnEffectByNum(EfList[i], 0, pos, &(S16Vec){0,0,0}, &(Vec){2.0, 2.0, 2.0});
	}

	return (actor->scale.x < 0);
}

void BossExplode(daBoss *actor, Vec *pos) {
	actor->scale.x = 0.0;
	actor->scale.y = 0.0;
	actor->scale.z = 0.0;

	SpawnEffect("Wm_ob_keyget02", 0, pos, &(S16Vec){0,0,0}, &(Vec){2.0, 2.0, 2.0});
	actor->dying = 1;
	actor->timer = 0;

	nw4r::snd::SoundHandle handle;
	PlaySoundWithFunctionB4(SoundRelatedClass, &handle, STRM_BGM_SHIRO_BOSS_CLEAR, 1);

	BossGoalForAllPlayers();
}

void BossGoalForAllPlayers() {
	for (int i = 0; i < 4; i++) {
		daPlBase_c *player = GetPlayerOrYoshi(i);
		if (player)
			player->setAnimePlayStandardType(2);
	}
}

void PlayerVictoryCries(daBoss *actor) {
	UpdateGameMgr();
}
