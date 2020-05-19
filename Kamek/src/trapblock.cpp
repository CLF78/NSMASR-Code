#include <common.h>
#include <game.h>
#include <g3dhax.h>
#include <sfx.h>
extern "C" void PlaySoundWithFunctionB4(void *spc, nw4r::snd::SoundHandle *handle, int id, int unk);

//Wrote by RedStoneMatt because CLF78 isn't able to do it :p


/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
// Trap Block

class daTrapBlock_c : public daEnBlockMain_c {
public:
	TileRenderer tile;
	Physics::Info physicsInfo;

	int wasIHit;
	int doOneTime;

	int onCreate();
	int onDelete();
	int onExecute();

	void calledWhenUpMoveExecutes();
	void calledWhenDownMoveExecutes();

	void blockWasHit(bool isDown);

	USING_STATES(daTrapBlock_c);
	DECLARE_STATE(Wait);

	static daTrapBlock_c *build();
};


CREATE_STATE(daTrapBlock_c, Wait);


int daTrapBlock_c::onCreate() {
	blockInit(pos.y);

	physicsInfo.x1 = -8;
	physicsInfo.y1 = 16;
	physicsInfo.x2 = 8;
	physicsInfo.y2 = 0;

	physicsInfo.otherCallback1 = &daEnBlockMain_c::OPhysicsCallback1;
	physicsInfo.otherCallback2 = &daEnBlockMain_c::OPhysicsCallback2;
	physicsInfo.otherCallback3 = &daEnBlockMain_c::OPhysicsCallback3;

	physics.setup(this, &physicsInfo, 3, currentLayerID);
	physics.flagsMaybe = 0x260;
	physics.callback1 = &daEnBlockMain_c::PhysicsCallback1;
	physics.callback2 = &daEnBlockMain_c::PhysicsCallback2;
	physics.callback3 = &daEnBlockMain_c::PhysicsCallback3;
	physics.addToList();

	TileRenderer::List *list = dBgGm_c::instance->getTileRendererList(0);
	list->add(&tile);

	tile.x = pos.x - 8;
	tile.y = -(16 + pos.y);
	if((this->settings >> 24 & 0xF) == 0) {
		tile.tileNumber = 0x0; //0x0 is invisible, 0x30 is brick and 0x31 is question
	}
	if((this->settings >> 24 & 0xF) == 1) {
		tile.tileNumber = 0x30; //0x0 is invisible, 0x30 is brick and 0x31 is question
	}
	if((this->settings >> 24 & 0xF) == 2) {
		tile.tileNumber = 0x31; //0x0 is invisible, 0x30 is brick and 0x31 is question
	}

	this->spriteSomeRectX = 6000.0f;

	doStateChange(&daTrapBlock_c::StateID_Wait);

	return true;
}


int daTrapBlock_c::onDelete() {
	TileRenderer::List *list = dBgGm_c::instance->getTileRendererList(0);
	list->remove(&tile);

	physics.removeFromList();

	return true;
}


int daTrapBlock_c::onExecute() {
	acState.execute();
	physics.update();
	blockUpdate();

	tile.setPosition(pos.x-8, -(16+pos.y), pos.z);
	tile.setVars(scale.x);

	return true;
}


daTrapBlock_c *daTrapBlock_c::build() {
	void *buffer = AllocFromGameHeap1(sizeof(daTrapBlock_c));
	return new(buffer) daTrapBlock_c;
}


void daTrapBlock_c::blockWasHit(bool isDown) {
	pos.y = initialY;
	int shroomSettings = isGroundPound ? 1 : 0;
	dStageActor_c *PoisonMushroom = dStageActor_c::create(AC_ITEM_KEY, shroomSettings, &this->pos, 0, 0); //Creating Poison Mushroom
	PoisonMushroom->pos.z = this->pos.z - 200;
	PlaySoundWithFunctionB4(SoundRelatedClass, &handle, SE_OBJ_ITEM_APPEAR, 1);
	this->wasIHit = 1;


	physics.setup(this, &physicsInfo, 3, currentLayerID);
	physics.addToList();
	
	doStateChange(&StateID_Wait);
}



void daTrapBlock_c::calledWhenUpMoveExecutes() {
	if (initialY >= pos.y)
		blockWasHit(false);
}

void daTrapBlock_c::calledWhenDownMoveExecutes() {
	if (initialY <= pos.y)
		blockWasHit(true);
}



void daTrapBlock_c::beginState_Wait() {
}

void daTrapBlock_c::endState_Wait() {
}

void daTrapBlock_c::executeState_Wait() {
	if(this->wasIHit == 0) {
		int result = blockResult();

		if (result == 0)
			return;

		if (result == 1) {
			if((this->settings >> 24 & 0xF) == 0) { tile.tileNumber = 0x31; }
			doStateChange(&daEnBlockMain_c::StateID_UpMove);
			anotherFlag = 2;
			isGroundPound = false;
		} else {
			if((this->settings >> 24 & 0xF) != 0) {
				doStateChange(&daEnBlockMain_c::StateID_DownMove);
				anotherFlag = 1;
				isGroundPound = true;
			}
		}
	}
	if(this->wasIHit == 1 && this->doOneTime == 0) { //Was already hit ? Change the tiles.
		tile.tileNumber = 0x32; //Brown Block
		this->doOneTime++;
	}
}




// virtual void daEnBlockMain_c::calledWhenUpMoveExecutes_new() {
// 	void calledWhenUpMoveExecutes = daEnBlockMain_c::calledWhenUpMoveExecutes();

// 	OSReport("-----\nEN_OBJ_CLEAR_BLOCK:\n");
// 	OSReport("_534: %d\n", this->_534);
// 	OSReport("_528: %d\n", this->_528);
// 	OSReport("_52C: %d\n", this->_52C);
// 	OSReport("_530: %d\n", this->_530);
// 	OSReport("_624: %d\n", this->_624);
// 	OSReport("_628: %d\n", this->_628);
// 	OSReport("_62C: %d\n", this->_62C);
// 	OSReport("_648: %d\n", this->_648);
// 	OSReport("_64C: %d\n", this->_64C);
// 	OSReport("_650: %d\n", this->_650);
// 	OSReport("_654: %d\n", this->_654);
// 	OSReport("_658: %d\n", this->_658);
// 	OSReport("_65C: %d\n", this->_65C);
// 	OSReport("_660: %d\n", this->_660);
// 	OSReport("_662: %d\n", this->_662);
// 	OSReport("_663: %d\n", this->_663);
// 	OSReport("_664: %d\n", this->_664);
// 	OSReport("_665: %d\n", this->_665);
// 	OSReport("_666: %d\n", this->_666);
// 	OSReport("_667: %d\n", this->_667);
// 	OSReport("_668: %d\n", this->_668);
// 	OSReport("_669: %d\n", this->_669);
// 	OSReport("_66A: %d\n", this->_66A);
// 	OSReport("_66B: %d\n", this->_66B);
// 	OSReport("_66C: %d\n", this->_66C);
// 	OSReport("_66D: %d\n", this->_66D);
// 	OSReport("_66E: %d\n", this->_66E);
// 	OSReport("_66F: %d\n", this->_66F);
// 	OSReport("_670: %d\n", this->_670);
// 	OSReport("_671: %d\n", this->_671);
// 	OSReport("_672: %d\n", this->_672);
// 	OSReport("_673: %d\n", this->_673);
// 	OSReport("_674: %d\n", this->_674);
// 	OSReport("_675: %d\n", this->_675);
// 	OSReport("_676: %d\n", this->_676);
// 	OSReport("_677: %d\n", this->_677);
// 	OSReport("_678: %d\n", this->_678);
// 	OSReport("_679: %d\n", this->_679);
// 	OSReport("_67A: %d\n", this->_67A);
// 	OSReport("_67B: %d\n", this->_67B);
// 	OSReport("_67C: %d\n", this->_67C);
// 	OSReport("_67D: %d\n", this->_67D);
// 	OSReport("_67E: %d\n", this->_67E);
// 	OSReport("_67F: %d\n", this->_67F);
// 	OSReport("_680: %d\n", this->_680);
// 	OSReport("_684: %d\n", this->_684);
// 	OSReport("_688: %d\n", this->_688);
// 	OSReport("_68B: %d\n", this->_68B);
// 	OSReport("_68C: %d\n", this->_68C);
// 	OSReport("_68D: %d\n", this->_68D);
// 	OSReport("_68E: %d\n", this->_68E);
// 	OSReport("_68F: %d\n", this->_68F);
// 	OSReport("_690: %d\n", this->_690);
// 	OSReport("_694: %d\n", this->_694);
// 	OSReport("-----\n");
// }