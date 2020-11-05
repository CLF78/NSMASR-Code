#include <common.h>
#include <game.h>
#include <dCourse.h>

// Level Warper
extern bool WarpIsEnabled;
extern bool hasWarped;
extern char CurrentLevel;
extern char CurrentWorld;
extern char WarpLevel;
extern char WarpMapID;
extern char WarpWorld;
extern u32 StarCoinStatus[3];

// Time Stop/Set
extern u32 GameTimer;

// Gravity Modifier
extern float MarioDescentRate;
extern float MarioJumpArc;
extern float MarioJumpMax;
extern float MiniMarioJumpArc;

// Enemy Resizer
extern bool ZOrderOn;
extern char SizerOn;
extern float GlobalRiderSize;
extern float GlobalSpriteSize;
extern float GlobalSpriteSpeed;
static const float BGScaleChoices[] = {0.1f, 0.15f, 0.25f, 0.375f, 0.5f, 0.625f, 0.75f, 0.9f, 1.0f, 1.125f, 1.25f, 1.5f, 1.75f, 2.0f, 2.25f, 2.5f};
static const float GlobalRiderFloatModifications [] = {1, 0.6, 0.7, 0.9, 1, 1, 1, 1.1, 1.25, 1.5, 2, 2.5, 3, 3.5, 4, 5};
static const float GlobalSizeFloatModifications [] = {1, 0.25, 0.5, 0.75, 1.25, 1.5, 1.75, 2, 2.5, 3, 4, 5, 6, 7, 8, 10 };

// No Multiplayer Bubbling
extern bool NoMichaelBuble;

// BG Scale Modifier
extern VEC2 BGScaleBack;
extern VEC2 BGScaleFront;
extern char BGScaleEnabled;

// The following is unrelated to the sprite itself, and is just reset by ResetAfterLevel
extern VEC2 CameraLockPosition;
extern char CameraLockEnabled;
extern char isLockPlayerRotation;
extern int GlobalStarsCollected;
