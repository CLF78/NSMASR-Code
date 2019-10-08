#include <common.h>
#include <game.h>
#include <g3dhax.h>


//////////////////////////////////////////////////////////
//
//	How it works:
//
//		1) Skip down to line 70 - read the comments along the way if you like
//		2) Change the stuff inside " " to be what you want.
//		3) Copy paste an entire 'case' section of code, and change the number to change the setting it uses
//		4) give it back to Tempus to compile in
//



// This is the class allocator, you don't need to touch this
class dMakeYourOwn : public dStageActor_c {
	// Let's give ourselves a few functions
	int onCreate();
	int onDelete();
	int onExecute();
	int onDraw();

	static dMakeYourOwn *build();

	// And a model and an anmChr
	mHeapAllocator_c allocator;
	m3d::mdl_c bodyModel;
	nw4r::g3d::ResFile resFile;
	m3d::anmChr_c chrAnimation;

	nw4r::g3d::ResMdl mdl;

	// Some variables to use
	int model;
	bool isAnimating;
	float size;
	float zOrder;
	bool customZ;

	void setupAnim(const char* name, float rate);
	void setupModel(const char* arcName, const char* brresName, const char* mdlName);
};

// This sets up how much space we have in memory
dMakeYourOwn *dMakeYourOwn::build() {
	void *buffer = AllocFromGameHeap1(sizeof(dMakeYourOwn));
	return new(buffer) dMakeYourOwn;
}


// Saves space when we do it like this
void dMakeYourOwn::setupAnim(const char* name, float rate) {
	if (isAnimating) {
		nw4r::g3d::ResAnmChr anmChr;

		anmChr = this->resFile.GetResAnmChr(name);
		this->chrAnimation.setup(this->mdl, anmChr, &this->allocator, 0);
		this->chrAnimation.bind(&this->bodyModel, anmChr, 1);
		this->bodyModel.bindAnim(&this->chrAnimation, 0.0);
		this->chrAnimation.setUpdateRate(rate);
	}
}

void dMakeYourOwn::setupModel(const char* arcName, const char* brresName, const char* mdlName) {
	this->resFile.data = getResource(arcName, brresName);
	this->mdl = this->resFile.GetResMdl(mdlName);

	bodyModel.setup(mdl, &allocator, 0x224, 1, 0);
}


// This gets run when the sprite spawns!
int dMakeYourOwn::onCreate() {

	// Settings for your sprite!

	this->model = this->settings & 0xFF; 						// Sets nubble 12 to choose the model you want
	this->isAnimating = this->settings & 0x100;					// Sets nybble 11 to a checkbox for whether or not the model has an anmChr to use
	this->size = (float)((this->settings >> 24) & 0xFF) / 4.0; 	// Sets nybbles 5-6 to size. Size equals value / 4.


	float zLevels[16] = {-6500.0, -5000.0, -4500.0, -2000.0, 
						 -1000.0, 300.0, 800.0, 1600.0, 
						  2000.0, 3600.0, 4000.0, 4500.0, 
						  6000.0, 6500.0, 7000.0, 7500.0 };

	this->zOrder = zLevels[(this->settings >> 16) & 0xF];

	this->customZ = (((this->settings >> 16) & 0xF) != 0);

	// Setup the models inside an allocator
	allocator.link(-1, GameHeaps[0], 0, 0x20);


	// Makes the code shorter and clearer to put these up here

	// A switch case, add extra models in here
	switch (this->model) {

		// TITLESCREEN STUFF
		// DEFAULT 

		case 0:		// Mario NF

			setupModel("arrow", "g3d/0.brres", "0"); 
			SetupTextures_Player(&bodyModel, 0);
			this->pos.z = -3000.0;

			setupAnim("anim00", 1.0); 
			break;	
						
		case 1:		// Peach NF

			setupModel("arrow", "g3d/1.brres", "1"); 
			SetupTextures_Enemy(&bodyModel, 0);
			this->pos.z = -3000.0;

			setupAnim("anim01", 1.0); 
			break;	

		case 2:		// Luigi NF

			setupModel("arrow", "g3d/2.brres", "2"); 
			SetupTextures_Player(&bodyModel, 0);
			this->pos.z = 3000.0;

			setupAnim("anim02", 1.0); 
			break;	
			
		case 3:	 // Yellow Toad NF

			setupModel("arrow", "g3d/3.brres", "3"); 
			SetupTextures_Player(&bodyModel, 0);
			this->pos.z = 3000.0;

			setupAnim("anim03", 1.0); 
			break;	

		case 4:		// Blue Toad NF

			setupModel("arrow", "g3d/4.brres", "4"); 
			SetupTextures_Player(&bodyModel, 0);
			this->pos.z = 3000.0;

			setupAnim("anim04", 1.0); 
			break;	

		case 5:		// Cloud

			setupModel("arrow", "g3d/5.brres", "5"); 
			SetupTextures_Item(&bodyModel, 0);
			this->pos.z = -3300.0;

			setupAnim("anim05", 1.0); 
			break;	

		case 6:		// Presents

			setupModel("OpeningScene", "g3d/6.brres", "6"); 
			SetupTextures_MapObj(&bodyModel, 0);
			this->pos.z = 0.0;

			setupAnim("anim06", 1.0); 
			break;	

		case 7:		// Luigi OP

			setupModel("OpeningScene", "g3d/7.brres", "7"); 
			SetupTextures_Player(&bodyModel, 0);
			this->pos.z = 0.0;

			setupAnim("anim07", 1.0); 
			break;		

		case 8:		// Blue Toad OP+Castle

			setupModel("OpeningScene", "g3d/8.brres", "8"); 
			SetupTextures_Player(&bodyModel, 0);
			this->pos.z = 0.0;

			setupAnim("anim08", 1.0); 
			break;	

		case 9:		// Yellow Toad OP+Castle

			setupModel("OpeningScene", "g3d/9.brres", "9"); 
			SetupTextures_Player(&bodyModel, 0);
			this->pos.z = 0.0;

			setupAnim("anim09", 1.0); 
			break;	

		case 10:		// Cloud Night

			setupModel("arrow", "g3d/10.brres", "10"); 
			SetupTextures_Item(&bodyModel, 0);
			this->pos.z = -3300.0;

			setupAnim("anim10", 1.0); 
			break;	

		case 11:		// Snow Tree

			setupModel("arrow", "g3d/11.brres", "11"); 
			SetupTextures_Map(&bodyModel, 0);
			this->pos.z = 0.0;

			break;		

		case 12:		// Red Toad Castle

			setupModel("OpeningScene", "g3d/12.brres", "12"); 
			SetupTextures_Player(&bodyModel, 0);
			this->pos.z = 0.0;

			setupAnim("anim12", 1.0); 
			break;	

		case 13:		// Actual Yellow Toad Castle

			setupModel("OpeningScene", "g3d/13.brres", "13"); 
			SetupTextures_Player(&bodyModel, 0);
			this->pos.z = 0.0;

			setupAnim("anim13", 1.0); 
			break;	

		case 14:		// Green Toad Castle

			setupModel("OpeningScene", "g3d/14.brres", "14"); 
			SetupTextures_Player(&bodyModel, 0);
			this->pos.z = 0.0;

			setupAnim("anim14", 1.0); 
			break;	

		case 15:		// Light Blue Toad Castle

			setupModel("OpeningScene", "g3d/15.brres", "15"); 
			SetupTextures_Player(&bodyModel, 0);
			this->pos.z = 0.0;

			setupAnim("anim15", 1.0); 
			break;	

		case 16:		// Violet Toad Castle

			setupModel("OpeningScene", "g3d/16.brres", "16"); 
			SetupTextures_Player(&bodyModel, 0);
			this->pos.z = 0.0;

			setupAnim("anim16", 1.0); 
			break;	

		case 17:		// White Cloud

			setupModel("arrow", "g3d/17.brres", "17"); 
			SetupTextures_Item(&bodyModel, 0);
			this->pos.z = -3300.0;

			setupAnim("anim17", 1.0); 
			break;	

		case 18:		// Standalone Cage

			setupModel("cage_boss_koopa", "g3d/18.brres", "18"); 
			SetupTextures_MapObj(&bodyModel, 0);
			this->pos.z = 0.0;

			setupAnim("anim18", 1.0); 
			break;	
	}

	allocator.unlink();

	if (size == 0.0) {	// If the person has the size nybble at zero, make it normal sized
		this->scale = (Vec){1.0,1.0,1.0};	
	}
	else {				// Else, use our size
		this->scale = (Vec){size,size,size};	
	}
		
	this->onExecute();
	return true;
}


// YOU'RE DONE, no need to do anything below here.


int dMakeYourOwn::onDelete() {
	return true;
}

int dMakeYourOwn::onExecute() {
	if (isAnimating) {
		bodyModel._vf1C();	// Advances the animation one update

		if(this->chrAnimation.isAnimationDone()) {
			this->chrAnimation.setCurrentFrame(0.0);	// Resets the animation when it's done
		}
	}

	return true;
}

int dMakeYourOwn::onDraw() {
	if (customZ) {
		matrix.translation(pos.x, pos.y, this->zOrder); }	// Set where to draw the model : -5500.0 is the official behind layer 2, while 5500.0 is in front of layer 0.
	else {
		matrix.translation(pos.x, pos.y, pos.z - 6500.0); }	// Set where to draw the model : -5500.0 is the official behind layer 2, while 5500.0 is in front of layer 0.

	matrix.applyRotationYXZ(&rot.x, &rot.y, &rot.z);	// Set how to rotate the drawn model 

	bodyModel.setDrawMatrix(matrix);	// Apply matrix
	bodyModel.setScale(&scale);			// Apply scale
	bodyModel.calcWorld(true);			// Do some shit

	bodyModel.scheduleForDrawing();		// Add it to the draw list for the game
	return true;
}
