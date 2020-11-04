#include <common.h>
#include <game.h>
#include <g3dhax.h>
#include <sfx.h>
#include "boss.h"

extern "C" int SmoothRotation(short* rot, u16 amt, int unk2);
extern "C" int SomeStrangeModification(dStageActor_c* actor);
extern "C" void *StageScreen;
extern "C" void dAcPy_vf3F8(void* player, dEn_c* monster, int t);
extern "C" void DoStuffAndMarkDead(dStageActor_c *actor, Vec vector, float unk);
extern "C" void stunPlayer(void *, int);
extern "C" void unstunPlayer(void *);

const char* MGarcNameList [] = {
	"kuriboBig",
	"kuriboBoss",
	NULL
};

class daMegaGoomba_c : public dEn_c {
	public:
		int onCreate();
		int onDelete();
		int onExecute();
		int onDraw();

		mHeapAllocator_c allocator;
		nw4r::g3d::ResFile resFile;
		m3d::mdl_c bodyModel;
		m3d::anmChr_c animationChr;

		float timer;
		float dying;

		lineSensor_s belowSensor, adjacentSensor;

		ActivePhysics leftTrapAPhysics, rightTrapAPhysics;
		ActivePhysics stalkAPhysics;

		HermiteKey keysX[4];

		char lives;
		bool already_hit;
		bool longerStun;
		float XSpeed;

		bool takeHit(char count);

		void dieFall_Begin();
		void dieFall_Execute();
		static daMegaGoomba_c *build();

		void setupBodyModel();

		void updateModelMatrices();

		void stunPlayers();
		void unstunPlayers();

		bool playerStunned[4];

		void removeMyActivePhysics();

		int tryHandleJumpedOn(ActivePhysics *apThis, ActivePhysics *apOther);

		void spriteCollision(ActivePhysics *apThis, ActivePhysics *apOther);
		void playerCollision(ActivePhysics *apThis, ActivePhysics *apOther);

		bool collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther);
		bool collisionCat2_IceBall_15_YoshiIce(ActivePhysics *apThis, ActivePhysics *apOther);
		bool collisionCat3_StarPower(ActivePhysics *apThis, ActivePhysics *apOther);
		bool collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther);
		bool collisionCat7_GroundPoundYoshi(ActivePhysics *apThis, ActivePhysics *apOther);
		bool collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther);
		bool collisionCatA_PenguinMario(ActivePhysics *apThis, ActivePhysics *apOther);
		bool collisionCatD_Drill(ActivePhysics *apThis, ActivePhysics *apOther);
		bool collisionCat11_PipeCannon(ActivePhysics *apThis, ActivePhysics *apOther);
		bool collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther);
		bool collisionCat14_YoshiFire(ActivePhysics *apThis, ActivePhysics *apOther);
		void addScoreWhenHit(void *other);
		bool _vf120(ActivePhysics *apThis, ActivePhysics *apOther);
		bool _vf110(ActivePhysics *apThis, ActivePhysics *apOther);
		bool _vf108(ActivePhysics *apThis, ActivePhysics *apOther);

		void powBlockActivated(bool isNotMPGP);

		void dieOther_Begin();
		void dieOther_Execute();
		void dieOther_End();

		USING_STATES(daMegaGoomba_c);
		DECLARE_STATE(Shrink);
		DECLARE_STATE(Walk);
		DECLARE_STATE(Turn);
};

daMegaGoomba_c *daMegaGoomba_c::build() {
	void *buffer = AllocFromGameHeap1(sizeof(daMegaGoomba_c));
	return new(buffer) daMegaGoomba_c;
}

CREATE_STATE(daMegaGoomba_c, Shrink);
CREATE_STATE(daMegaGoomba_c, Walk);
CREATE_STATE(daMegaGoomba_c, Turn);

/////////////////////////
// Collision Functions //
/////////////////////////

void daMegaGoomba_c::removeMyActivePhysics() {
	aPhysics.removeFromList();
	stalkAPhysics.removeFromList();
	leftTrapAPhysics.removeFromList();
	rightTrapAPhysics.removeFromList();
}

void setNewActivePhysicsRect(daMegaGoomba_c *actor, Vec *scale) {
	float amtX = scale->x * 0.5f;
	float amtY = scale->y * 0.5f;

	actor->belowSensor.flags = SENSOR_LINE;
	actor->belowSensor.lineA = s32((amtX * -28.0f) * 4096.0f);
	actor->belowSensor.lineB = s32((amtX * 28.0f) * 4096.0f);
	actor->belowSensor.distanceFromCenter = 0;

	actor->adjacentSensor.flags = SENSOR_LINE;
	actor->adjacentSensor.lineA = s32((amtY * 4.0f) * 4096.0f);
	actor->adjacentSensor.lineB = s32((amtY * 32.0f) * 4096.0f);
	actor->adjacentSensor.distanceFromCenter = s32((amtX * 46.0f) * 4096.0f);

	u8 cat1 = 3, cat2 = 0;
	u32 bitfield1 = 0x6F, bitfield2 = 0xFFBAFFFE;

	ActivePhysics::Info info = {0.0f, amtY*57.0f, amtX*20.0f, amtY*31.0f, cat1, cat2, bitfield1, bitfield2, 0, &dEn_c::collisionCallback};
	actor->aPhysics.initWithStruct(actor, &info);

	// Original trapezium was -12,12 to -48,48
	ActivePhysics::Info left = {amtX*-32.0f, amtY*55.0f, amtX*12.0f, amtY*30.0f, cat1, cat2, bitfield1, bitfield2, 0, &dEn_c::collisionCallback};
	actor->leftTrapAPhysics.initWithStruct(actor, &left);
	actor->leftTrapAPhysics.trpValue0 = amtX * 12.0f;
	actor->leftTrapAPhysics.trpValue1 = amtX * 12.0f;
	actor->leftTrapAPhysics.trpValue2 = amtX * -12.0f;
	actor->leftTrapAPhysics.trpValue3 = amtX * 12.0f;
	actor->leftTrapAPhysics.collisionCheckType = 3;

	ActivePhysics::Info right = {amtX*32.0f, amtY*55.0f, amtX*12.0f, amtY*30.0f, cat1, cat2, bitfield1, bitfield2, 0, &dEn_c::collisionCallback};
	actor->rightTrapAPhysics.initWithStruct(actor, &right);
	actor->rightTrapAPhysics.trpValue0 = amtX * -12.0f;
	actor->rightTrapAPhysics.trpValue1 = amtX * -12.0f;
	actor->rightTrapAPhysics.trpValue2 = amtX * -12.0f;
	actor->rightTrapAPhysics.trpValue3 = amtX * 12.0f;
	actor->rightTrapAPhysics.collisionCheckType = 3;

	ActivePhysics::Info stalk = {0.0f, amtY*12.0f, amtX*28.0f, amtY*12.0f, cat1, cat2, bitfield1, bitfield2, 0, &dEn_c::collisionCallback};
	actor->stalkAPhysics.initWithStruct(actor, &stalk);
}

bool daMegaGoomba_c::takeHit(char count) {
	OSReport("Taking a hit!\n");

	if (!this->already_hit) {
		if (count < 0)
			count = -count;

		this->lives -= count;
		doStateChange(&StateID_Shrink);
		this->already_hit = true;
	}
	return (lives <= 0);
}

int daMegaGoomba_c::tryHandleJumpedOn(ActivePhysics *apThis, ActivePhysics *apOther) {
	// Temporarily alter the enemy bounce value
	float saveBounce = EnemyBounceValue;
	EnemyBounceValue = 5.2f;
	char ret = usedForDeterminingStatePress_or_playerCollision(this, apThis, apOther, 2);
	EnemyBounceValue = saveBounce;

	if (ret == 1 || ret == 3) {
		apOther->someFlagByte |= 2;
		if (this->takeHit(1)) {
			VEC2 eSpeed = {speed.x, speed.y};
			killWithSpecifiedState(apOther->owner, &eSpeed, &dEn_c::StateID_DieOther);
		}
	}
	return ret;
}

/////////////////////////
// Collision Callbacks //
/////////////////////////

void daMegaGoomba_c::playerCollision(ActivePhysics *apThis, ActivePhysics *apOther) {
	if (apThis == &stalkAPhysics) {
		dEn_c::playerCollision(apThis, apOther);
		return;
	}

	if (tryHandleJumpedOn(apThis, apOther) == 0) {
		this->dEn_c::playerCollision(apThis, apOther);
		this->_vf220(apOther->owner);
		this->counter_504[apOther->owner->which_player] = 180;
	}
}

bool daMegaGoomba_c::collisionCat3_StarPower(ActivePhysics *apThis, ActivePhysics *apOther) {
	if (tryHandleJumpedOn(apThis, apOther) == 0) {
		dAcPy_vf3F8(apOther->owner, this, 3);
		this->counter_504[apOther->owner->which_player] = 0xA;
	}
	return true;
}

bool daMegaGoomba_c::collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther) {
	if (this->counter_504[apOther->owner->which_player] > 0)
		return false;

	VEC2 eSpeed = {speed.x, speed.y};
	killWithSpecifiedState(apOther->owner, &eSpeed, &dEn_c::StateID_DieOther);
	return true;
}

bool daMegaGoomba_c::collisionCat7_GroundPoundYoshi(ActivePhysics *apThis, ActivePhysics *apOther) {
	return collisionCat7_GroundPound(apThis, apOther);
}

bool daMegaGoomba_c::collisionCatD_Drill(ActivePhysics *apThis, ActivePhysics *apOther) {
	return collisionCat7_GroundPound(apThis, apOther);
}

bool daMegaGoomba_c::collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther) {
	if (this->takeHit(1))
		doStateChange(&StateID_DieFall);
	return true;
}

bool daMegaGoomba_c::collisionCatA_PenguinMario(ActivePhysics *apThis, ActivePhysics *apOther) {
	return this->collisionCat9_RollingObject(apThis, apOther);
}

bool daMegaGoomba_c::collisionCat14_YoshiFire(ActivePhysics *apThis, ActivePhysics *apOther) {
	return this->collisionCat9_RollingObject(apThis, apOther);
}

bool daMegaGoomba_c::collisionCat11_PipeCannon(ActivePhysics *apThis, ActivePhysics *apOther) {
	return true;
}

bool daMegaGoomba_c::collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther) {
	return true;
}

bool daMegaGoomba_c::_vf120(ActivePhysics *apThis, ActivePhysics *apOther) {
	return true; // Replicate existing broken behaviour
}

bool daMegaGoomba_c::_vf110(ActivePhysics *apThis, ActivePhysics *apOther) {
	return true; // Replicate existing broken behaviour
}

bool daMegaGoomba_c::_vf108(ActivePhysics *apThis, ActivePhysics *apOther) {
	return true; // Replicate existing broken behaviour
}

bool daMegaGoomba_c::collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther) {
	return false;
}

bool daMegaGoomba_c::collisionCat2_IceBall_15_YoshiIce(ActivePhysics *apThis, ActivePhysics *apOther) {
	return false;
}

void daMegaGoomba_c::spriteCollision(ActivePhysics *apThis, ActivePhysics *apOther) {
}

void daMegaGoomba_c::addScoreWhenHit(void *other) {
}

////////////////////
// Base Functions //
////////////////////

void daMegaGoomba_c::setupBodyModel() {
	allocator.link(-1, GameHeaps[0], 0, 0x20);

	this->resFile.data = getResource(MGarcNameList[1], "g3d/kuriboBoss.brres");
	nw4r::g3d::ResMdl mdl = this->resFile.GetResMdl(MGarcNameList[0]);
	bodyModel.setup(mdl, &allocator, 0x224, 1, 0);
	SetupTextures_Enemy(&bodyModel, 0);

	nw4r::g3d::ResAnmChr anmChr = this->resFile.GetResAnmChr("walk");
	this->animationChr.setup(mdl, anmChr, &this->allocator, 0);
	this->animationChr.bind(&this->bodyModel, anmChr, 1);
	this->bodyModel.bindAnim(&this->animationChr, 0.0);
	this->animationChr.setUpdateRate(0.2);

	allocator.unlink();
}

int daMegaGoomba_c::onCreate() {
	// Setup Settings
	longerStun = (this->settings & 0xF);

	// Setup Model
	this->setupBodyModel();
	this->animationChr.setCurrentFrame(69.0);

	// Position
	this->pos.y -= 16.0;
	this->pos.z = (!this->appearsOnBackFence) ? 1500.0 : -2500.0;

	// Position Delta
	this->pos_delta2.x = 0.0;
	this->pos_delta2.y = 16.0;
	this->pos_delta2.z = 0.0;

	// Scale
	scale.x = 4.0f;
	scale.y = 4.0f;
	scale.z = 4.0f;

	// Speed
	this->XSpeed = 0.2;
	speed.y = 0.0;

	// Max Speed
	this->max_speed.y = -4.0;

	// Speed Increase
	this->x_speed_inc = 0.1;

	// Direction
	this->direction = dSprite_c__getXDirectionOfFurthestPlayerRelativeToVEC3(this, this->pos);

	// Rotation
	this->rot.y = (this->direction) ? 0xE000 : 0x2000;
	rot.x = rot.z = 0;

	// Additional Flags
	this->_120 |= 0x200;
	this->_36D = 0;
	this->_518 = 2;

	// Own Flags
	dying = 0.0;
	lives = 3;
	already_hit = false;

	// Collider
	this->collMgr.init(this, &belowSensor, 0, &adjacentSensor);

	// ActivePhysics
	aPhysics.addToList();
	stalkAPhysics.addToList();
	leftTrapAPhysics.addToList();
	rightTrapAPhysics.addToList();
	setNewActivePhysicsRect(this, &this->scale);

	// Zone Boundary Check
	checkZoneBoundaries(0);

	// State Change
	doStateChange(&StateID_Walk);
	return true;
}

int daMegaGoomba_c::onDelete() {
	unstunPlayers();
	return true;
}

int daMegaGoomba_c::onExecute() {
	acState.execute();
	updateModelMatrices();
	return true;
}

int daMegaGoomba_c::onDraw() {
	bodyModel.scheduleForDrawing();
	return true;
}

void daMegaGoomba_c::updateModelMatrices() {
	// This won't work with wrap because I'm lazy.
	matrix.translation(pos.x, pos.y, pos.z);
	matrix.applyRotationYXZ(&rot.x, &rot.y, &rot.z);

	bodyModel.setDrawMatrix(matrix);
	bodyModel.setScale(&scale);
	bodyModel.calcWorld(false);
}

//////////////////
// Shrink State //
//////////////////

void daMegaGoomba_c::beginState_Shrink() {
	this->timer = 1.0;

	keysX[0] = (HermiteKey){  0.0, this->scale.y,        0.5 };
	keysX[1] = (HermiteKey){ 10.0, this->scale.y - 0.75, 0.5 };
	keysX[2] = (HermiteKey){ 20.0, this->scale.y - 0.35, 0.5 };
	keysX[3] = (HermiteKey){ 39.0, this->scale.y - 0.75, 0.5 };

	// Disable being hit
	Vec tempVec = (Vec){0.0, 0.0, 0.0};
	setNewActivePhysicsRect(this, &tempVec);
}

void daMegaGoomba_c::executeState_Shrink() {
	this->timer += 1.0;

	float modifier = GetHermiteCurveValue(this->timer, this->keysX, 4);
	this->scale = (Vec){modifier, modifier, modifier};

	if (this->timer == 2.0)
		PlaySound(this, SE_EMY_KURIBO_L_DAMAGE_02);

	else if (this->timer > 40.0)
		doStateChange(&StateID_Walk);
}

void daMegaGoomba_c::endState_Shrink() {
	// Enable being hit
	setNewActivePhysicsRect(this, &this->scale);
	this->already_hit = false;
}

////////////////
// Turn State //
////////////////

void daMegaGoomba_c::beginState_Turn() {
	this->direction ^= 1;
	this->speed.x = 0.0;
}

void daMegaGoomba_c::executeState_Turn() {
	this->bodyModel._vf1C();

	this->HandleYSpeed();
	this->doSpriteMovement();

	int ret = SomeStrangeModification(this);

	if (ret & 1)
		this->speed.y = 0.0;
	if (ret & 4)
		this->pos.x = this->last_pos.x;

	DoStuffAndMarkDead(this, this->pos, 1.0);

	u16 amt = (this->direction == 0) ? 0x2000 : 0xE000;

	if (SmoothRotation(&this->rot.y, amt, 0x80))
		this->doStateChange(&StateID_Walk);

	int frame = (int)(this->animationChr.getCurrentFrame() * 5.0);

	if ((frame == 100) || (frame == 325) || (frame == 550) || (frame == 775)) {
		ShakeScreen(StageScreen, 0, 1, 0, 0);
		stunPlayers();
		PlaySound(this, SE_BOSS_MORTON_GROUND_SHAKE);
	}

	else if (longerStun) {
		if ((frame == 250) || (frame == 500) || (frame == 700) || (frame == 900))
			unstunPlayers();
	}

	else if ((frame == 200) || (frame == 425) || (frame == 650) || (frame == 875))
		unstunPlayers();
}

void daMegaGoomba_c::endState_Turn() {
	this->max_speed.x = (this->direction) ? -this->XSpeed : this->XSpeed;
}

////////////////
// Walk State //
////////////////

void daMegaGoomba_c::beginState_Walk() {
	this->speed.x = this->speed.z = 0.0;
	this->speed.y = -4.0;

	this->max_speed.x = (this->direction) ? -this->XSpeed : this->XSpeed;
	this->y_speed_inc = -0.1875;
}

void daMegaGoomba_c::executeState_Walk() {
	this->bodyModel._vf1C();

	this->HandleXSpeed();
	this->HandleYSpeed();
	this->doSpriteMovement();

	u16 amt = (this->direction == 0) ? 0x2000 : 0xE000;
	SmoothRotation(&this->rot.y, amt, 0x200);

	if(SomeStrangeModification(this) & 1)
		this->speed.y = 0.0;

	u32 bitfield = this->collMgr.outputMaybe;
	if (bitfield & (0x15<<this->direction)) {
		this->pos.x = this->last_pos.x;
		this->doStateChange(&StateID_Turn);
	}

	DoStuffAndMarkDead(this, this->pos, 1.0);

	int frame = (int)(this->animationChr.getCurrentFrame() * 5.0);

	if ((frame == 100) || (frame == 325) || (frame == 550) || (frame == 775)) {
		ShakeScreen(StageScreen, 0, 1, 0, 0);
		stunPlayers();
		PlaySound(this, SE_BOSS_MORTON_GROUND_SHAKE);
	}

	else if (longerStun) {
		if ((frame == 250) || (frame == 500) || (frame == 700) || (frame == 900))
			unstunPlayers();
	}

	else if ((frame == 200) || (frame == 425) || (frame == 650) || (frame == 875))
		unstunPlayers();

	if (this->animationChr.isAnimationDone()) {
		this->animationChr.setCurrentFrame(0.0);

		int newdir = dSprite_c__getXDirectionOfFurthestPlayerRelativeToVEC3(this, pos);
		if (this->direction != newdir)
			doStateChange(&StateID_Turn);
	}
}

void daMegaGoomba_c::endState_Walk() {
}

////////////////////
// DieOther State //
////////////////////

void daMegaGoomba_c::dieOther_Begin() {
	animationChr.bind(&bodyModel, resFile.GetResAnmChr("damage"), true);
	bodyModel.bindAnim(&animationChr, 2.0f);

	speed.x = speed.y = speed.z = 0.0f;
	removeMyActivePhysics();

	PlaySound(this, SE_EMY_KURIBO_L_SPLIT_HPDP);

	rot.y = 0;
	counter_500 = 60;
}

void daMegaGoomba_c::dieOther_Execute() {
	bodyModel._vf1C();

	if (counter_500 == 0) {
		SpawnEffect("Wm_ob_icebreaksmk", 0, &pos, &(S16Vec){0,0,0}, &(Vec){5.0f, 5.0f, 5.0f});
		Delete(1);
	}
}

void daMegaGoomba_c::dieOther_End() {
	dEn_c::dieOther_End();
}

///////////////////
// DieFall State //
///////////////////

void daMegaGoomba_c::dieFall_Begin() {
	this->dEn_c::dieFall_Begin();
	PlaySound(this, SE_EMY_KURIBO_L_DAMAGE_03);
}

void daMegaGoomba_c::dieFall_Execute() {
	this->timer += 1.0;

	this->dying += 0.15;

	this->pos.x += 0.15;
	this->pos.y += ((-0.2 * this->dying * this->dying) + 5);

	this->dEn_c::dieFall_Execute();
}

/////////////////////
// Other Functions //
/////////////////////

void daMegaGoomba_c::stunPlayers() {
	for (int i = 0; i < 4; i++) {
		dStageActor_c *player = GetSpecificPlayerActor(i);
		if (player && player->collMgr.isOnTopOfTile() && player->currentZoneID == currentZoneID) {
				stunPlayer(player, 1);
				playerStunned[i] = true;
		}
	}
}

void daMegaGoomba_c::unstunPlayers() {
	for (int i = 0; i < 4; i++) {
		dStageActor_c *player = GetSpecificPlayerActor(i);
		if (player && playerStunned[i]) {
			unstunPlayer(player);
			playerStunned[i] = false;
		}
	}
}

void daMegaGoomba_c::powBlockActivated(bool isNotMPGP) {
}
