#include <game.h>
#include <stage.h>

extern "C" void EnterOpeningLevel() {
	DontShowPreGame = true;
	RESTART_CRSIN_LevelStartStruct.purpose = 0;
	RESTART_CRSIN_LevelStartStruct.world1 = 1;
	RESTART_CRSIN_LevelStartStruct.world2 = 1;
	RESTART_CRSIN_LevelStartStruct.level1 = 40;
	RESTART_CRSIN_LevelStartStruct.level2 = 40;
	RESTART_CRSIN_LevelStartStruct.areaMaybe = 0;
	RESTART_CRSIN_LevelStartStruct.entrance = 0xFF;
	RESTART_CRSIN_LevelStartStruct.unk4 = 0; // Not "load replay" that's for sure
	DoSceneChange(RESTART_CRSIN, 0, 0);
}
