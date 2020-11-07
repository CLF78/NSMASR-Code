#include <common.h>
#include <game.h>
#include <g3dhax.h>

#define ANMCHR 1
#define ANMSRT 2
#define ANMPAT 4
#define ANMCLR 8

const char* RYOMFileList [] = {
	"models",
	NULL	
};

//////////////////////////////////////////////////////////
//
//	How it works:
//
//		1) Skip down to line 70 - read the comments along the way if you like
//		2) Change the stuff inside " " to be what you want.
//		3) Copy paste an entire 'case' section of code, and change the number to change the setting it uses
//

// This is the class allocator, you don't need to touch this
class dMakeYourOwn : public dStageActor_c {
	// Let's give ourselves a few functions
	int onCreate();
	int onDelete();
	int onExecute();
	int onDraw();

	static dMakeYourOwn *build();

	// Add a model and the eventual animations
	mHeapAllocator_c allocator;
	nw4r::g3d::ResFile resFile;
	m3d::mdl_c bodyModel;
	m3d::anmChr_c chrAnimation;
	m3d::anmTexSrt_c srtAnimation;
	m3d::anmTexPat_c patAnimation;
	m3d::anmClr_c clrAnimation;

	nw4r::g3d::ResMdl mdl;

	// Some variables to use
	int model;
	u8 animtype;
	float size;
	float zOrder;
	bool customZ;

	void setupAnim(const char* name, float rate);
	void setupModel(int arcIndex, const char* brresName, const char* mdlName);
};

// This sets up how much space we have in memory
dMakeYourOwn *dMakeYourOwn::build() {
	void *buffer = AllocFromGameHeap1(sizeof(dMakeYourOwn));
	return new(buffer) dMakeYourOwn;
}

// Sets up each animation if enabled
void dMakeYourOwn::setupAnim(const char* name, float rate) {
	if (animtype & ANMCHR) {
		nw4r::g3d::ResAnmChr anmChr;

		anmChr = this->resFile.GetResAnmChr(name);
		this->chrAnimation.setup(this->mdl, anmChr, &this->allocator, 0);
		this->chrAnimation.bind(&this->bodyModel, anmChr, 1);
		this->bodyModel.bindAnim(&this->chrAnimation, 0.0);
		this->chrAnimation.setUpdateRate(rate);
	}

	if (animtype & ANMSRT) {
		nw4r::g3d::ResAnmTexSrt anmSrt;

		anmSrt = this->resFile.GetResAnmTexSrt(name);
		this->srtAnimation.setup(this->mdl, anmSrt, &this->allocator, 0, 1);
		this->srtAnimation.bindEntry(&this->bodyModel, anmSrt, 0, 0);
		this->bodyModel.bindAnim(&this->srtAnimation, 1.0);
		this->srtAnimation.setFrameForEntry(1.0, 0);
		this->srtAnimation.setUpdateRateForEntry(rate, 0);
	}

	if (animtype & ANMPAT) {
		nw4r::g3d::ResAnmTexPat anmPat;

		anmPat = this->resFile.GetResAnmTexPat(name);
		this->patAnimation.setup(this->mdl, anmPat, &this->allocator, 0, 1);
		this->patAnimation.bindEntry(&this->bodyModel, &anmPat, 0, 0);
		this->bodyModel.bindAnim(&this->patAnimation, 1.0);
		this->patAnimation.setFrameForEntry(1.0, 0);
		this->patAnimation.setUpdateRateForEntry(rate, 0);
	}

	if (animtype & ANMCLR) {
		nw4r::g3d::ResAnmClr anmClr;

		anmClr = this->resFile.GetResAnmClr(name);
		this->clrAnimation.setup(this->mdl, anmClr, &this->allocator, 0, 1);
		this->clrAnimation.bind(&this->bodyModel, anmClr, 0, 0);
		this->bodyModel.bindAnim(&this->clrAnimation, 1.0);
		this->clrAnimation.setFrameForEntry(1.0, 0);
		this->clrAnimation.setUpdateRateForEntry(rate, 0);
	}
}

void dMakeYourOwn::setupModel(int arcIndex, const char* brresName, const char* mdlName) {
	this->resFile.data = getResource(RYOMFileList[arcIndex], brresName);
	this->mdl = this->resFile.GetResMdl(mdlName);

	bodyModel.setup(mdl, &allocator, 0x224, 1, 0);
}

// This gets run when the sprite spawns!
int dMakeYourOwn::onCreate() {
	// Settings for your sprite!
	this->model = this->settings & 0xFF; 						// Sets nubble 12 to choose the model you want
	this->animtype = this->settings >> 8 & 0xF;					// Sets nybble 10 to a series of checkbox for which anim should be used
	this->size = (float)((this->settings >> 24) & 0xFF) / 4.0; 	// Sets nybbles 5-6 to size. Size equals value / 4.

	float zLevels[16] = {-6500.0, -5000.0, -4500.0, -2000.0, -1000.0, 300.0, 800.0, 1600.0, 2000.0, 3600.0, 4000.0, 4500.0, 6000.0, 6500.0, 7000.0, 7500.0 };

	this->zOrder = zLevels[(this->settings >> 16) & 0xF];
	this->customZ = (((this->settings >> 16) & 0xF) != 0);

	// Setup the models inside an allocator
	allocator.link(-1, GameHeaps[0], 0, 0x20);

	// A switch case, add extra models in here
	switch (this->model) {
		case 0:		// Mario NF

			setupModel(0, "g3d/0.brres", "0");
			SetupTextures_Player(&bodyModel, 0);
			this->pos.z = -3000.0;

			setupAnim("anim00", 1.0);
			break;

		case 1:		// Peach NF

			setupModel(0, "g3d/1.brres", "1");
			SetupTextures_Enemy(&bodyModel, 0);
			this->pos.z = -3000.0;

			setupAnim("anim01", 1.0);
			break;

		case 2:		// Luigi NF

			setupModel(0, "g3d/2.brres", "2");
			SetupTextures_Player(&bodyModel, 0);
			this->pos.z = 3000.0;

			setupAnim("anim02", 1.0);
			break;

		case 3:	 // Yellow Toad NF

			setupModel(0, "g3d/3.brres", "3");
			SetupTextures_Player(&bodyModel, 0);
			this->pos.z = 3000.0;

			setupAnim("anim03", 1.0);
			break;

		case 4:		// Blue Toad NF

			setupModel(0, "g3d/4.brres", "4");
			SetupTextures_Player(&bodyModel, 0);
			this->pos.z = 3000.0;

			setupAnim("anim04", 1.0);
			break;

		case 5:		// Cloud

			setupModel(0, "g3d/5.brres", "5");
			SetupTextures_Item(&bodyModel, 0);
			this->pos.z = -3300.0;

			setupAnim("anim05", 1.0);
			break;

		case 6:		// Presents

			setupModel(0, "g3d/6.brres", "6");
			SetupTextures_MapObj(&bodyModel, 0);
			this->pos.z = 0.0;

			setupAnim("anim06", 1.0);
			break;

		case 7:		// Luigi OP

			setupModel(0, "g3d/7.brres", "7");
			SetupTextures_Player(&bodyModel, 0);
			this->pos.z = 0.0;

			setupAnim("anim07", 1.0);
			break;

		case 8:		// Blue Toad OP

			setupModel(0, "g3d/4.brres", "4");
			SetupTextures_Player(&bodyModel, 0);
			this->pos.z = 0.0;

			setupAnim("anim08", 1.0);
			break;

		case 9:		// Yellow Toad OP

			setupModel(0, "g3d/3.brres", "3");
			SetupTextures_Player(&bodyModel, 0);
			this->pos.z = 0.0;

			setupAnim("anim09", 1.0);
			break;

		case 10:		// Cloud Night

			setupModel(0, "g3d/10.brres", "10");
			SetupTextures_Item(&bodyModel, 0);
			this->pos.z = -3300.0;

			setupAnim("anim10", 1.0);
			break;

		case 11:		// Snow Tree

			setupModel(0, "g3d/11.brres", "11");
			SetupTextures_Map(&bodyModel, 0);
			this->pos.z = 0.0;

			break;

		case 17:		// White Cloud

			setupModel(0, "g3d/17.brres", "17");
			SetupTextures_Item(&bodyModel, 0);
			this->pos.z = -3300.0;

			setupAnim("anim17", 1.0);
			break;

		case 18:		// Standalone Cage

			setupModel(0, "g3d/18.brres", "18");
			SetupTextures_MapObj(&bodyModel, 0);
			this->pos.z = 0.0;

			setupAnim("anim18", 1.0);
			break;
	}

	allocator.unlink();

	if (size == 0.0)	// If the person has the size nybble at zero, make it normal sized
		this->scale = (Vec){1.0,1.0,1.0};
	else				// Else, use our size
		this->scale = (Vec){size,size,size};

	return true;
}

// No need to do anything below here.
int dMakeYourOwn::onDelete() {
	return true;
}

int dMakeYourOwn::onExecute() {
	// Process and reset the animation(s) when done

	if (this->animtype & ANMCHR) {
		this->chrAnimation.process();
		if(this->chrAnimation.isAnimationDone())
			this->chrAnimation.setCurrentFrame(0.0);
	}
	
	if (this->animtype & ANMSRT) {
		this->srtAnimation.process();
		if(this->srtAnimation.isEntryAnimationDone(0))
			this->srtAnimation.setFrameForEntry(1.0, 0);
	}

	if (this->animtype & ANMPAT) {
		this->patAnimation.process();
		if(this->patAnimation.isEntryAnimationDone(0))
			this->patAnimation.setFrameForEntry(1.0, 0);
	}

	if (this->animtype & ANMCLR) {
		this->clrAnimation.process();
		if(this->clrAnimation.isEntryAnimationDone(0))
			this->clrAnimation.setFrameForEntry(1.0, 0);
	}

	return true;
}

int dMakeYourOwn::onDraw() {
	if (customZ)
		matrix.translation(pos.x, pos.y, this->zOrder);	// Set where to draw the model : -5500.0 is the official behind layer 2, while 5500.0 is in front of layer 0.
	else
		matrix.translation(pos.x, pos.y, pos.z - 6500.0);	// Set where to draw the model : -5500.0 is the official behind layer 2, while 5500.0 is in front of layer 0.

	matrix.applyRotationYXZ(&rot.x, &rot.y, &rot.z);	// Set how to rotate the drawn model

	bodyModel.setDrawMatrix(matrix);	// Apply matrix
	bodyModel.setScale(&scale);			// Apply scale
	bodyModel.calcWorld(true);			// Do some shit

	bodyModel.scheduleForDrawing();		// Add it to the draw list for the game
	return true;
}
