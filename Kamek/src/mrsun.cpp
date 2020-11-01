#include <common.h>
#include <game.h>
#include <g3dhax.h>
#include <sfx.h>
#include "boss.h"

#define ACTIVATE	1
#define DEACTIVATE	0

const char* MSarcNameList [] = {
	"mrsun",
	NULL
};

class daMrSun_c : public dEn_c {
	int onCreate();
	int onDelete();
	int onExecute();
	int onDraw();

	mHeapAllocator_c allocator;
	m3d::mdl_c bodyModel;
	m3d::mdl_c glowModel;

	bool isSun;
	u32 timer;

	float Baseline;
	float SpiralLoop;
	float xSpiralOffset;
	float ySpiralOffset;

	Vec swoopTarget;
	float swoopA;
	float swoopB;
	float swoopC;
	float swoopSpeed;

	float glowPos;
	short spinReduceZ;
	short spinReduceY;

	float dying;
	bool killFlag;

	u64 eventFlag;

	void dieFall_Execute();
	static daMrSun_c *build();

	void updateModelMatrices();

	void playerCollision(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat2_IceBall_15_YoshiIce(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat5_Mario(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCatA_PenguinMario(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat3_StarPower(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCatD_Drill(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat7_GroundPoundYoshi(ActivePhysics *apThis, ActivePhysics *apOther);

	USING_STATES(daMrSun_c);
	DECLARE_STATE(Follow);
	DECLARE_STATE(Swoop);
	DECLARE_STATE(Spiral);
	DECLARE_STATE(Spit);
	DECLARE_STATE(Spin);
	DECLARE_STATE(Wait);
};

daMrSun_c *daMrSun_c::build() {
	void *buffer = AllocFromGameHeap1(sizeof(daMrSun_c));
	return new(buffer) daMrSun_c;
}

CREATE_STATE(daMrSun_c, Follow);
CREATE_STATE(daMrSun_c, Swoop);
CREATE_STATE(daMrSun_c, Spiral);
CREATE_STATE(daMrSun_c, Spit);
CREATE_STATE(daMrSun_c, Spin);
CREATE_STATE(daMrSun_c, Wait);

// Player-damaging collision
void daMrSun_c::playerCollision(ActivePhysics *apThis, ActivePhysics *apOther) {
	DamagePlayer(this, apThis, apOther);
}

bool daMrSun_c::collisionCatD_Drill(ActivePhysics *apThis, ActivePhysics *apOther) {
	this->playerCollision(apThis, apOther);
	return true;
}

bool daMrSun_c::collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther) {
	return this->collisionCatD_Drill(apThis, apOther);
}

bool daMrSun_c::collisionCat7_GroundPoundYoshi(ActivePhysics *apThis, ActivePhysics *apOther) {
	return this->collisionCatD_Drill(apThis, apOther);
}

bool daMrSun_c::collisionCatA_PenguinMario(ActivePhysics *apThis, ActivePhysics *apOther) {
	return this->collisionCatD_Drill(apThis, apOther);
}

bool daMrSun_c::collisionCat5_Mario(ActivePhysics *apThis, ActivePhysics *apOther) {
	return this->collisionCatD_Drill(apThis, apOther);
}

// Enemy-damaging collision
bool daMrSun_c::collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther) {
	this->timer = 0;
	PlaySound(this, SE_EMY_DOWN);
	doStateChange(&StateID_DieFall);
	return true;
}

bool daMrSun_c::collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther) {
	return this->collisionCat9_RollingObject(apThis, apOther);
}

bool daMrSun_c::collisionCat3_StarPower(ActivePhysics *apThis, ActivePhysics *apOther) {
	return this->collisionCat9_RollingObject(apThis, apOther);
}

// Bunch of special cases
bool daMrSun_c::collisionCat2_IceBall_15_YoshiIce(ActivePhysics *apThis, ActivePhysics *apOther) {
	return (!this->isSun && apOther->owner->name == 0x76);  // It's a moon and the offending actor is BROS_ICEBALL
}

bool daMrSun_c::collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther) {
	return true;
}

void daMrSun_c::dieFall_Execute() {
	if (this->killFlag)
		return;

	this->timer++;
	this->dying += 0.15;

	this->pos.x += 0.15;
	this->pos.y -= ((-0.2 * (this->dying*this->dying)) + 5);

	this->dEn_c::dieFall_Execute();

	if (this->timer > 450) {
		if ((this->settings >> 28) > 0) {
			this->kill();
			this->pos.y += 800.0;
			this->killFlag = true;
			return;
		}

		dStageActor_c *Player;

		for (int i = 0; i < 4; i++) {
			Player = GetSpecificPlayerActor(i);
			if (Player)
				this->pos.x = Player->pos.x - 300;
				break;
		}

		if (!Player) {
			this->pos.x = 0;
			doStateChange(&StateID_Follow);
		}

		this->pos.y = this->Baseline;

		this->aPhysics.addToList();
		doStateChange(&StateID_Follow);
	}
}

int daMrSun_c::onCreate() {
	// Model setup
	allocator.link(-1, GameHeaps[0], 0, 0x20);

	if ((this->settings & 0xF) == 0) { // It's a sun
		isSun = true;

		nw4r::g3d::ResFile rf(getResource("mrsun", "g3d/sun.brres"));
		bodyModel.setup(rf.GetResMdl("Sun"), &allocator, 0x224, 1, 0);
		SetupTextures_Map(&bodyModel, 0);

		glowModel.setup(rf.GetResMdl("SunGlow"), &allocator, 0x224, 1, 0);
		SetupTextures_Boss(&glowModel, 0);
	}

	else { // It's a moon
		isSun = false;

		nw4r::g3d::ResFile rf(getResource("mrsun", "g3d/moon.brres"));
		bodyModel.setup(rf.GetResMdl("Moon"), &allocator, 0x224, 1, 0);
		SetupTextures_Map(&bodyModel, 0);
	}

	allocator.unlink();

	// Fix scale
	this->scale = (Vec){0.5, 0.5, 0.5};

	// Setup physics
	ActivePhysics::Info HitMeBaby;
	HitMeBaby.xDistToCenter = 0.0;
	HitMeBaby.yDistToCenter = 0.0;
	HitMeBaby.category1 = 3;
	HitMeBaby.category2 = 0;
	HitMeBaby.bitfield1 = 0x6F;

	if (isSun) { // It's a sun
		HitMeBaby.bitfield2 = 0xFFBAFFFC; 
		HitMeBaby.xDistToEdge = 24.0;
		HitMeBaby.yDistToEdge = 24.0;
	}	
	else { // It's a moon
		HitMeBaby.bitfield2 = 0xFFBAFFFE; 
		HitMeBaby.xDistToEdge = 12.0;
		HitMeBaby.yDistToEdge = 12.0;
	}

	HitMeBaby.unkShort1C = 0;
	HitMeBaby.callback = &dEn_c::collisionCallback;

	this->aPhysics.initWithStruct(this, &HitMeBaby);
	this->aPhysics.addToList();

	// Setup variables
	this->Baseline = this->pos.y;
	this->SpiralLoop = 0.0;
	this->timer = 0;
	this->xSpiralOffset = 0.0;
	this->ySpiralOffset = 0.0;
	this->dying = -5.0;
	this->killFlag = false;

	if (isSun)
		this->pos.z = 5750.0f; // sun
	else
		this->pos.z = 6000.0f; // moon

	// Setup event trigger
	char eventNum = (this->settings >> 16) & 0xFF;
	this->eventFlag = (u64)1 << (eventNum - 1);

	// State change
	doStateChange(&StateID_Follow);
	return true;
}

int daMrSun_c::onDelete() {
	return true;
}

int daMrSun_c::onExecute() {
	acState.execute();
	updateModelMatrices();

	if ((dFlagMgr_c::instance->flags & this->eventFlag) && !this->killFlag && acState.getCurrentState()->isNotEqual(&StateID_DieFall)) {
		this->kill();
		this->pos.y += 800.0;
		this->killFlag = true;
		doStateChange(&StateID_DieFall);
	}

	return true;
}

int daMrSun_c::onDraw() {
	bodyModel.scheduleForDrawing();

	if (isSun)
		glowModel.scheduleForDrawing();

	return true;
}


void daMrSun_c::updateModelMatrices() {
	// This won't work with wrap because I'm lazy.
	matrix.translation(pos.x, pos.y, pos.z);
	matrix.applyRotationYXZ(&rot.x, &rot.y, &rot.z);

	bodyModel.setDrawMatrix(matrix);
	bodyModel.setScale(&scale);
	bodyModel.calcWorld(false);

	if (isSun) {
		mMtx glowMatrix;
		short rotY;

		glowPos += 0.01666666666666;
		if (glowPos > 1)
			glowPos = 0;

		rotY = (1000 * sin(glowPos * 3.14)) + 500;

		glowMatrix.translation(pos.x, pos.y, pos.z);
		glowMatrix.applyRotationX(&rot.x);
		glowMatrix.applyRotationY(&rotY);

		glowModel.setDrawMatrix(glowMatrix);
		glowModel.setScale(&scale);
		glowModel.calcWorld(false);
	}
}

//////////////////
// Follow State //
//////////////////

void daMrSun_c::beginState_Follow() {
	this->timer = 0;
	this->rot.x = 18000;
	this->rot.y = 0;
	this->rot.z = 0;
}

void daMrSun_c::executeState_Follow() {
	if (this->timer > 200)
		this->doStateChange(&StateID_Wait);

	this->direction = dSprite_c__getXDirectionOfFurthestPlayerRelativeToVEC3(this, this->pos);

	float speedDelta;
	if (isSun)
		speedDelta = 0.1; // It's a sun
	else
		speedDelta = 0.15; // It's a moon

	if (this->direction == 0) {
		this->speed.x += speedDelta;

		if (this->speed.x < 0)
			this->speed.x += (speedDelta / 2);

		if (this->speed.x < 6.0)
			this->speed.x += speedDelta;
	} else {
		this->speed.x -= speedDelta;

		if (this->speed.x > 0)
			this->speed.x -= (speedDelta / 2);

		if (this->speed.x > 6.0)
			this->speed.x -= speedDelta;
	}

	this->HandleXSpeed();

	this->speed.y = (this->Baseline - this->pos.y) / 8;
	this->HandleYSpeed();

	this->UpdateObjectPosBasedOnSpeedValuesReal();

	this->timer++;
}

void daMrSun_c::endState_Follow() {
	this->speed.y = 0;
}

/////////////////
// Swoop State //
/////////////////

void daMrSun_c::beginState_Swoop() {
	// Not enough space to swoop, spit instead.
	if (this->swoopTarget.y < (this->pos.y - 50))
		doStateChange(&StateID_Spit);
	
	if (((this->pos.x - 96) < this->swoopTarget.x) && (this->swoopTarget.x < (this->pos.x + 96)))
		doStateChange(&StateID_Spit);

	// Moon hits closer to home...
	if (isSun)
		this->swoopTarget.y -= 16;
	else
		this->swoopTarget.y -= 4;

	float x1, x2, x3, y1, y2, y3;

	x1 = this->pos.x - this->swoopTarget.x;
	x2 = 0;
	x3 = -x1;

	y1 = this->pos.y - this->swoopTarget.y;
	y2 = 0;
	y3 = y1;

	float denominator = (x1 - x2) * (x1 - x3) * (x2 - x3);
	this->swoopA      = (x3 * (y2 - y1) + x2 * (y1 - y3) + x1 * (y3 - y2)) / denominator;
	this->swoopB      = (x3*x3 * (y1 - y2) + x2*x2 * (y3 - y1) + x1*x1 * (y2 - y3)) / denominator;
	this->swoopC      = (x2 * x3 * (x2 - x3) * y1 + x3 * x1 * (x3 - x1) * y2 + x1 * x2 * (x1 - x2) * y3) / denominator;

	this->swoopSpeed = x3 * 2 / 75;

	PlaySound(this, SE_PLY_PRPL_FLY);
}

void daMrSun_c::executeState_Swoop() {
	// Everything is calculated up top, just need to modify it.
	this->pos.x += this->swoopSpeed;

	this->pos.y = (this->swoopA * (this->pos.x - this->swoopTarget.x) * (this->pos.x - this->swoopTarget.x) + this->swoopB * (this->pos.x - this->swoopTarget.x) + this->swoopC) + this->swoopTarget.y;

	if (this->pos.y > this->Baseline)
		doStateChange(&StateID_Follow);
}

void daMrSun_c::endState_Swoop() {
	this->speed.y = 0;
}

//////////////////
// Spiral State //
//////////////////

void daMrSun_c::beginState_Spiral() {
	this->SpiralLoop = 0;
	this->xSpiralOffset = this->pos.x;
	this->ySpiralOffset = this->pos.y;

	PlaySound(this, SE_PLY_PRPL_FLY);
}

void daMrSun_c::executeState_Spiral() {
	float Loops;
	float Magnitude;
	float Period;

	Loops = 6.0;
	Magnitude = 11.0;

	if (isSun)
		Period = 0.1; // It's a sun
	else
		Period = 0.125; // It's a moon

	this->pos.x = this->xSpiralOffset + Magnitude*((this->SpiralLoop * cos(this->SpiralLoop)));
	this->pos.y = this->ySpiralOffset + Magnitude*((this->SpiralLoop * sin(this->SpiralLoop)));

	this->SpiralLoop += Period;
	
	if (this->SpiralLoop > (3.14 * Loops))
		doStateChange(&StateID_Follow);
}

void daMrSun_c::endState_Spiral() {
}

////////////////
// Spit State //
////////////////

void daMrSun_c::beginState_Spit() {
	this->timer = 0;
}

void daMrSun_c::executeState_Spit() {
	if (this->timer == 10) {
		PlaySound(this, SE_EMY_PAKKUN_FIRE);

		this->direction = dSprite_c__getXDirectionOfFurthestPlayerRelativeToVEC3(this, this->pos);

		float neg = -1.0;
		if (this->direction == 0)
			neg = -neg;

		if (isSun) { // It's a sun
			dStageActor_c *spawner = CreateActor(106, 0, this->pos, 0, 0);
			spawner->speed.x = 6.0 * neg;
			spawner->speed.y = -2.5;
			spawner->pos.z = 5550.0;
			
			spawner = CreateActor(106, 0, this->pos, 0, 0);
			spawner->speed.x = 0.0;
			spawner->speed.y = -6.0;
			spawner->pos.z = 5550.0;
		
			spawner = CreateActor(106, 0, this->pos, 0, 0);
			spawner->speed.x = 3.5 * neg;
			spawner->speed.y = -6.0;
			spawner->pos.z = 5550.0;

		} else { // It's a moon
			dStageActor_c *spawner = CreateActor(118, 0, this->pos, 0, 0);
			spawner->speed.x = 6.0 * neg;
			spawner->speed.y = -2.5;
			spawner->pos.z = 5550.0;
			*((u32 *) (((char *) spawner) + 0x3DC)) = this->id;
			
			spawner = CreateActor(118, 0, this->pos, 0, 0);
			spawner->speed.x = 0.0;
			spawner->speed.y = -6.0;
			spawner->pos.z = 5550.0;
			*((u32 *) (((char *) spawner) + 0x3DC)) = this->id;
		
			spawner = CreateActor(118, 0, this->pos, 0, 0);
			spawner->speed.x = 3.5 * neg;
			spawner->speed.y = -6.0;
			spawner->pos.z = 5550.0;
			*((u32 *) (((char *) spawner) + 0x3DC)) = this->id;
		}
	}
	
	else if (this->timer > 30)
		doStateChange(&StateID_Follow);

	this->timer++;
}

void daMrSun_c::endState_Spit() {
}

////////////////
// Spin State //
////////////////

void daMrSun_c::beginState_Spin() {
	this->spinReduceZ = 0;
	this->spinReduceY = 0;
}

void daMrSun_c::executeState_Spin() {
	PlaySound(this, SE_PLY_PRPL_LETDOWN_SPIN);

	this->direction = dSprite_c__getXDirectionOfFurthestPlayerRelativeToVEC3(this, this->pos);

	if (!this->direction) {
		this->speed.x += 0.2;

		if (this->speed.x < 0)
			this->speed.x += 0.2 / 2;

		if (this->speed.x < 80.0)
			this->speed.x += 0.2 * 2;

	} else {
		this->speed.x -= 0.2;

		if (this->speed.x > 0)
			this->speed.x -= 0.2 / 2;
		
		if (this->speed.x > 80.0)
			this->speed.x -= 0.2 * 2;
	}

	this->HandleXSpeed();
	this->UpdateObjectPosBasedOnSpeedValuesReal();

	this->timer++;

	short rotBonus;
	if (this->timer < 60)
		rotBonus = this->timer;
	else
		rotBonus = 120 - this->timer;

	this->rot.z += (55.1 * rotBonus);
	this->rot.y += (18.4 * rotBonus);

	float spitspeed;
	if (isSun)
		spitspeed = 3.0; // It's a sun
	else
		spitspeed = 4.0; // It's a moon

	int randomBall;
	randomBall = GenerateRandomNumber(8);
	if (randomBall == 1) {
		int direction;
		direction = GenerateRandomNumber(8);

		float xlaunch;
		float ylaunch;

		if (direction == 0) {      // E
			xlaunch = spitspeed;
			ylaunch = 0.0; }
		else if (direction == 1) { // SE
			xlaunch = spitspeed;
			ylaunch = spitspeed; }
		else if (direction == 2) { // S
			xlaunch = 0.0;
			ylaunch = spitspeed; }
		else if (direction == 3) { // SW
			xlaunch = -spitspeed;
			ylaunch = spitspeed; }
		else if (direction == 4) {	// W
			xlaunch = -spitspeed;
			ylaunch = 0.0; }
		else if (direction == 5) {	// NW
			xlaunch = -spitspeed;
			ylaunch = -spitspeed; }
		else if (direction == 6) {	// N
			xlaunch = 0.0;
			ylaunch = -spitspeed; }
		else if (direction == 7) {	// NE
			xlaunch = spitspeed;
			ylaunch = -spitspeed; }

		PlaySound(this, SE_EMY_PAKKUN_FIRE);

		if (isSun) { // It's a sun
			dStageActor_c *spawner = CreateActor(106, 0, this->pos, 0, 0);
			spawner->speed.x = xlaunch;
			spawner->speed.y = ylaunch;
			spawner->pos.z = 5550.0;
		}

		else { // It's a moon
			dStageActor_c *spawner = CreateActor(118, 0, this->pos, 0, 0);
			spawner->speed.x = xlaunch;
			spawner->speed.y = ylaunch;
			spawner->pos.z = 5550.0;
			*((u32 *) (((char *) spawner) + 0x3DC)) = this->id;
		}
	}

	if (this->timer > 120)
		this->doStateChange(&StateID_Follow);
}

void daMrSun_c::endState_Spin() {
	this->rot.x = 18000;
	this->rot.y = 0;
	this->rot.z = 0;

	this->speed.x = 0;
}

////////////////
// Wait State //
////////////////

void daMrSun_c::beginState_Wait() {
	this->timer = 0;
	this->speed.x = 0.0;

	dStageActor_c *Player;

	for (int i = 0; i < 4; i++) {
		Player = GetSpecificPlayerActor(i);
		if (Player)
			this->swoopTarget = Player->pos;
			break;
	}
	
	if (Player == 0)
		doStateChange(&StateID_Follow);
}

void daMrSun_c::executeState_Wait() {
	int Choice;
	int TimerMax;

	if (isSun)
		TimerMax = 60; // It's a sun
	else
		TimerMax = 30; // It's a moon

	if (this->timer > TimerMax) {
		Choice = GenerateRandomNumber(9);

		if (Choice == 0)
			doStateChange(&StateID_Spit);
		else if (Choice == 1)
			doStateChange(&StateID_Spit);
		else if (Choice == 2)
			doStateChange(&StateID_Spin);
		else if (Choice == 3)
			doStateChange(&StateID_Spiral);
		else
			doStateChange(&StateID_Swoop);
	}

	this->timer++;
}

void daMrSun_c::endState_Wait() {
	this->timer = 0;
}
