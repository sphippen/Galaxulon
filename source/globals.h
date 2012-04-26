// globals.h
// 
// Authors: Aric Parkinson, Spencer Phippen
// Date created: March 23, 2012
//
// -------
// Contains declarations for global variables used in the project
//

#ifndef GLOBALS_H
#define GLOBALS_H

#include "Vector.h"

class PlayState;
class TitleScreenState;
class StageEndState;
class GameStateManager;
class OamManager;
class PauseState;
class Stage;
struct StageEvent;
class GameOverState;
class StoreState;
class Player;
class AudioManager;

// Game states
extern PlayState*			g_playState;
extern TitleScreenState*	g_titleState;
extern PauseState*			g_pauseState;
extern GameOverState*		g_gameOverState;
extern StoreState*          g_storeState;
extern StageEndState*		g_stageEndState;

// Various managers
extern GameStateManager*	g_gameStateMan;
extern OamManager*			g_oamMan;
extern AudioManager*		g_audioMan;

// Variables needed by multiple GameStates
extern int g_playerScore;
extern int g_playerStageBulletsHit;
extern int g_playerStageBulletsTotal;
extern bool g_playerHitThisStage;

// Sprite declarations
#define g_playerSpriteTilesLen 256
#define g_playerSpriteTilesPos 0
extern const unsigned short g_playerSpriteTiles[128];

#define g_jellyfishTilesLen 256
#define g_jellyfishTilesPos (g_playerSpriteTilesPos + 8)
extern const unsigned short g_jellyfishTiles[128];

#define g_bulletSpriteTilesLen 32
#define g_bulletSpriteTilesPos (g_jellyfishTilesPos + 8)
extern const unsigned short g_bulletSpriteTiles[16];

#define g_missileSpriteTilesLen 32
#define g_missileSpriteTilesPos (g_bulletSpriteTilesPos + 1)
extern const unsigned short g_missileSpriteTiles[16];

#define g_bullet_selection1TilesLen 64
#define g_bullet_selection1TilesPos (g_missileSpriteTilesPos + 1)
extern const unsigned short g_bullet_selection1Tiles[32];

#define g_bullet_selection2TilesLen 64
#define g_bullet_selection2TilesPos (g_bullet_selection1TilesPos + 2)
extern const unsigned short g_bullet_selection2Tiles[32];

#define g_mineTilesLen 32
#define g_mineTilesPos (g_bullet_selection2TilesPos + 2)
extern const unsigned short g_mineTiles[16];

#define g_sharkTilesLen 64
#define g_sharkTilesPos (g_mineTilesPos + 1)
extern const unsigned short g_sharkTiles[32];

#define g_squidTilesLen 128
#define g_squidTilesPos (g_sharkTilesPos + 2)
extern const unsigned short g_squidTiles[64];

#define g_crabTilesLen 512
#define g_crabTilesPos (g_squidTilesPos + 4)
extern const unsigned short g_crabTiles[256];


// -----------------
// Powerup Tiles
// -----------------

// Extra life
#define g_extraLife_TilesLen 32
#define g_extraLife_TilesPos (g_crabTilesPos + 16)
extern const unsigned short g_extraLife_Tiles[16];

// Extra bomb
#define g_extraBomb_TilesLen 32
#define g_extraBomb_TilesPos (g_extraLife_TilesPos + 1)
extern const unsigned short g_extraBomb_Tiles[16];

// Upgrade Primary Weapon
#define g_primaryFireUp_TilesLen 32
#define g_primaryFireUp_TilesPos (g_extraBomb_TilesPos + 1)
extern const unsigned short g_primaryFireUp_Tiles[16];

// Upgrade Secondary Weapon
#define g_secondFireUp_TilesLen 32
#define g_secondFireUp_TilesPos (g_primaryFireUp_TilesPos + 1)
extern const unsigned short g_secondFireUp_Tiles[16];

// Shield
#define g_shield_TilesLen 32
#define g_shield_TilesPos (g_secondFireUp_TilesPos + 1)
extern const unsigned short g_shield_Tiles[16];

//----------------------------
// Title screen logo tiles
//----------------------------
#define g_galaxulon_titleTilesLen 4096
#define g_galaxulon_titleTilesPos1 (g_shield_TilesPos + 1)
#define g_galaxulon_titleTilesPos2 (g_galaxulon_titleTilesPos1 + 64)
extern const unsigned short g_galaxulon_titleTiles[2048];

// -----------------
// BG Tiles
// -----------------

#define g_blankBG_TilesLen 32
#define g_blankBG_TilesPos 0
extern const unsigned short g_blankBG_Tiles[16];

#define g_assertTile_TilesLen 32
#define g_assertTile_TilesPos (g_blankBG_TilesPos + 1)
extern const unsigned short g_assertTile_Tiles[16];

#define g_galaxulonFont_TilesLen 3072
#define g_galaxulonFont_Offset 32
#define g_galaxulonFont_TilesPos (g_assertTile_TilesPos + 1)
extern const unsigned short g_galaxulonFont_Tiles[1536];

#define g_demo_starsTilesLen 288
#define g_demo_starsTilesPos (g_galaxulonFont_TilesPos + 96)
extern const unsigned short g_demo_starsTiles[144];


//------------
//BG Maps
//------------
#define g_demo_starsMapLen 2048
extern const unsigned short g_demo_starsMap[1024];

// Palette declarations
#define g_spritePalLen 512
extern const unsigned short g_spritePal[256];

#define g_bgPalLen 512
extern const unsigned short g_bgPal[256];

// Enemy path functions
// --------------------

// vertically up and down
Vec2 g_verticalEnemyPath(int* t);

// Bullet path functions
// ---------------------

// Horizontally to the right
Vec2 g_horizontalRightBulletPath(int* t);

// Horizontally to the left
Vec2 g_horizontalLeftBulletPath(int* t);

// Diagonally up/right
Vec2 g_upRightBulletPath(int* t);

// Diagonally down/right
Vec2 g_downRightBulletPath(int* t);

// Straight forward, fairly slowly
Vec2 g_straightLeftPath_slow(int* t);

// Straight forward, decent speed
Vec2 g_straightLeftPath_med(int* t);

// Straight forward, quick
Vec2 g_straightLeftPath_fast(int* t);

// Zig-zag to the left
Vec2 g_zigZagLeftPath_largeAmplitude(int* t);

// Zig-zag to the left with small amplitude
Vec2 g_zigZagLeftPath_smallAmplitude(int* t);

// StageEvents
extern StageEvent * g_endEvent;
extern StageEvent * g_event1;
extern StageEvent * g_event2;
extern StageEvent * g_event3;
extern StageEvent * g_event4;
extern StageEvent * g_event5;
extern StageEvent * g_event6;
extern StageEvent * g_firePowerPowerUpEvent;
extern StageEvent * g_invinciblePowerUpEvent;
extern StageEvent * g_bombPowerUpEvent;

// Stages
extern Stage * g_stage1;
extern Stage * g_stage2;
extern Stage * g_stage3;

// Power up functions
void g_firePowerPowerUp(Player*) ;
void g_firePowerPowerDown(Player*);
extern int baseLightDamage;
extern int baseHeavyDamage;
void g_invincibilityPowerUp(Player*) ;
void g_invincibilityPowerDown(Player*);
void g_bombPowerUp(Player*);
void g_bombPowerDown(Player*);

#endif