#include <common.h>
#include <game.h>
#include <g3dhax.h>
#include <stage.h>
#include "boss.h"

extern "C" void *BowserDamageAnmClr(dEn_c *);
extern "C" void *BowserDamageStepTwo(dEn_c *);
extern "C" void *BowserDamageNormal(dEn_c *);
extern "C" void *BowserDamageKill(dEn_c *);
extern "C" void *BowserDamageEnd(dEn_c *);

int BridgeBowserHP = 2;
int lastBomb = 0;

extern bool HackyBombDropVariable;

void BowserDoomSpriteCollision(dEn_c *bowser, ActivePhysics *apThis, ActivePhysics *apOther) {
	// If you collide with something or other, call the fireball collision
	if (apOther->owner->name == 674) {
		if ((lastBomb == apOther->owner->id) || (!HackyBombDropVariable))
			return;

		HackyBombDropVariable = false;
		OSReport("HP: %d", BridgeBowserHP);
		
		*(int*)(((u32)bowser) + 0x540) = 0x28;
		BowserDamageAnmClr(bowser);

		if (BridgeBowserHP <= 0) {
			BridgeBowserHP = 0;

			BowserDamageStepTwo(bowser);
			BowserDamageKill(bowser);
			BowserDamageEnd(bowser);

			daBossKoopa_c *BowserClass = (daBossKoopa_c*)bowser;
			OSReport("Koopa Controller: %x", BowserClass);
			BowserClass->doStateChange(&daBossKoopa_c::StateID_Fall);
			dFlagMgr_c::instance->set(3, 0, true, false, false);

			BridgeBowserHP = 2;
		}

		else {
			BowserDamageNormal(bowser);
			BridgeBowserHP -= 1;
		}

		lastBomb = apOther->owner->id;

		dEn_c * bomb = (dEn_c*)apOther->owner;
		dFlagMgr_c::instance->set(bomb->settings & 0xFF, 0, true, false, false);
		bomb->kill();
	}

	return;
}

void BowserDoomStart(dStageActor_c *Controller) {
	OSReport("Here we go!");

	dEn_c *Bowser = (dEn_c*)FindActorByType(EN_BOSS_KOOPA, 0);
	Bowser->Delete(1);
	lastBomb = 0;
}

void BowserDoomExecute(dStageActor_c *Controller) {
	dFlagMgr_c::instance->set(2, 0, true, false, false);
	Controller->Delete(1);
}

void BowserDoomEnd(dStageActor_c *Controller) {
	OSReport("Bye bye everybody");
	Controller->Delete(1);
}

void BowserStartEnd(dStageActor_c *Controller) {
	dFlagMgr_c::instance->set(1, 0, true, false, false);
}
