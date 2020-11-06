#include <common.h>
#include <game.h>

#define LINEGOD_FUNC_ACTIVATE	0
#define LINEGOD_FUNC_DEACTIVATE	1

struct BgActor {
	u16 def_id;		// 0x00
	u16 x;			// 0x02
	u16 y;			// 0x04
	u8 layer;		// 0x06
	u8 EXTRA_off;	// 0x07
	u32 actor_id;	// 0x08
};

struct BgActorDef {
	u32 tilenum;
	u16 actor;
	u8 _06[2];
	float x;
	float y;
	float z;
	float width;
	float height;
	u32 extra_var;
};

struct dBgActorManager_c {
	u32 vtable;		// 0x00
	u8 _04[0x34];	// 0x04
	BgActor *array;	// 0x38
	u32 count;		// 0x3C
	u32 type;		// 0x40
};

struct BG_GM_hax {
	u8 _00[0x8FE64];
	float _0x8FE64;
	float _0x8FE68;
	float _0x8FE6C;
	float _0x8FE70;
};

extern dBgActorManager_c *dBgActorManager;
extern BgActorDef *BgActorDefs;
extern BG_GM_hax *BG_GM_ptr;
extern u16 *GetPointerToTile(BG_GM_hax *self, u16 x, u16 y, u16 layer, short *blockID_p, bool unused);
extern fBase_c *FindActorByID(u32 id);

class dLineGod_c : public dStageActor_c {
	int onCreate();
	int onExecute();
	int onDelete();
	int onDraw();

	u8 eventNum;
	u8 func;
	u8 width;
	u8 height;
	u8 lastEvState;
	BgActor *ac[8];

	bool appendToList(BgActor *ac);
	void buildList();

	static dLineGod_c *build();
};

dLineGod_c *dLineGod_c::build() {
	void *buffer = AllocFromGameHeap1(sizeof(dLineGod_c));
	return new(buffer) dLineGod_c;
}

int dLineGod_c::onCreate() {
	this->eventNum = ((this->settings >> 24) & 0xFF) - 1;

	this->func = (this->settings) & 1;
	this->width = (this->settings >> 4) & 0xF;
	this->height = (this->settings >> 8) & 0xF;

	this->lastEvState = 0xFF;

	buildList();
	return true;
}

int dLineGod_c::onExecute() {
	u8 newEvState = (dFlagMgr_c::instance->active(this->eventNum));

	if (newEvState == this->lastEvState)
		return true;

	u16 x_bias = (BG_GM_ptr->_0x8FE64 / 16);
	u16 y_bias = -(BG_GM_ptr->_0x8FE6C / 16);

	u8 offState = (this->func == LINEGOD_FUNC_ACTIVATE) ? newEvState : !newEvState;

	for (int i = 0; i < 8; i++) {
		if (this->ac[i] != 0) {
			BgActor *ac = this->ac[i];

			ac->EXTRA_off = offState;

			if (offState == 1 && ac->actor_id != 0) {
				fBase_c *assoc_ac = FindActorByID(ac->actor_id);
				if (assoc_ac)
					assoc_ac->Delete();
				ac->actor_id = 0;
			}

			u16 *tile = GetPointerToTile(BG_GM_ptr, (ac->x + x_bias) * 16, (ac->y + y_bias) * 16, 0, 0, 0);
			if (offState == 1)
				*tile = 0;
			else
				*tile = BgActorDefs[ac->def_id].tilenum;
		}
	}

	this->lastEvState = newEvState;
	return true;
}

int dLineGod_c::onDelete() {
	return true;
}

int dLineGod_c::onDraw() {
	return true;
}

void dLineGod_c::buildList() {
	for (int clearIdx = 0; clearIdx < 8; clearIdx++)
		this->ac[clearIdx] = 0;

	float gLeft = this->pos.x - (BG_GM_ptr->_0x8FE64 - fmod(BG_GM_ptr->_0x8FE64, 16));
	float gTop = this->pos.y - (BG_GM_ptr->_0x8FE6C - fmod(BG_GM_ptr->_0x8FE6C, 16));

	// 1 unit padding to avoid catching stuff that is not in our rectangle
	Vec grect1 = (Vec){gLeft + 1, gTop - (this->height * 16) + 1, 0};
	Vec grect2 = (Vec){gLeft + (this->width * 16) - 1, gTop - 1, 0};

	for (int i = 0; i < dBgActorManager->count; i++) {
		BgActor *ac = &dBgActorManager->array[i];
		BgActorDef *def = &BgActorDefs[ac->def_id];

		// The def width/heights are padded with 8 units on each side 
		// except for one of the steep slopes, which differs for no reason

		float aXCentre = (ac->x * 16) + def->x;
		float aYCentre = (-ac->y * 16) + def->y;

		float xDistToCentre = (def->width - 16) / 2;
		float yDistToCentre = (def->height - 16) / 2;

		Vec arect1 = (Vec){aXCentre - xDistToCentre, aYCentre - yDistToCentre, 0};
		Vec arect2 = (Vec){aXCentre + xDistToCentre, aYCentre + yDistToCentre, 0};

		if (RectanglesOverlap(&arect1, &arect2, &grect1, &grect2))
			appendToList(ac);
	}
}

bool dLineGod_c::appendToList(BgActor *ac) {
	for (int i = 0; i < 8; i++) {
		if (this->ac[i] == 0) {
			this->ac[i] = ac;
			return true;
		}
	}

	return false;
}
