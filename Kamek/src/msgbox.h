#ifndef MSGBOX_H
#define MSGBOX_H 

#include <common.h>
#include <game.h>
#include <sfx.h>

extern bool MessageBoxIsShowing;

class dMsgBoxManager_c : public dStageActor_c {
	public:
		dMsgBoxManager_c() : state(this, &StateID_LoadRes) { }

		int onCreate();
		int onDelete();
		int onExecute();
		int onDraw();

		m2d::EmbedLayout_c layout;
		dDvdLoader_c msgDataLoader;

		bool layoutLoaded;
		bool visible;

		bool canCancel;
		int delay;

		static dMsgBoxManager_c *instance;
		static dMsgBoxManager_c *build();

		void showMessage(int id, bool canCancel=true, int delay=-1);

		dStateWrapper_c<dMsgBoxManager_c> state;
		USING_STATES(dMsgBoxManager_c);
		DECLARE_STATE(LoadRes);
		DECLARE_STATE(Wait);
		DECLARE_STATE(BoxAppearWait);
		DECLARE_STATE(ShownWait);
		DECLARE_STATE(BoxDisappearWait);

	private:
		struct entry_s {
			u32 id;
			u32 titleOffset;
			u32 msgOffset;
		};

		struct header_s {
			u32 count;
			entry_s entry[1];
		};
};

class daEnMsgBlock_c : public daEnBlockMain_c {
	public:
		TileRenderer tile;
		Physics::Info physicsInfo;

		int onCreate();
		int onDelete();
		int onExecute();

		void calledWhenUpMoveExecutes();
		void calledWhenDownMoveExecutes();

		void blockWasHit();

		USING_STATES(daEnMsgBlock_c);
		DECLARE_STATE(Wait);

		static daEnMsgBlock_c *build();
};

#endif
