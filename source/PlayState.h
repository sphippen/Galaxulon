// PlayState.h
// 
// Authors: Aric Parkinson, Spencer Phippen, David Setser
// Date created: March 23, 2012
//
// -------
// Defines interface for the PlayState class
//

#ifndef PLAYSTATE_H
#define PLAYSTATE_H

#include "Player.h"
#include "Enemy.h"
#include "GameState.h"
#include "ObjectAllocator.h"
#include "tonc_types.h"
//#include "tonc_video.h"
#include "Bullet.h"
#include "Stage.h"
#include "PowerUp.h"

// Constants used for PlayState initialization and such
//	-- Player Constants --
#define PLAYER_ORIGIN			0, 76
#define PLAYER_SPRITE_SIZE		Sprite::size_32x16
#define PLAYER_SPRITE_POS		sprites
#define PLAYER_BASE_PALETTE		0
#define PLAYER_LEFT_OFFSET      3
#define PLAYER_RIGHT_OFFSET     27
#define PLAYER_TOP_OFFSET       4
#define PLAYER_BOTTOM_OFFSET    12

//  -- Player Bullet Constants --
#define PLAYER_BULLETS_SPRITE_POS	(PLAYER_SPRITE_POS + 1)
#define MAX_NUM_PLAYER_BULLETS		32

//  -- Enemy Bullet Constants --
#define ENEMY_BULLETS_SPRITE_POS	(PLAYER_BULLETS_SPRITE_POS + MAX_NUM_PLAYER_BULLETS)
#define MAX_NUM_ENEMY_BULLETS		32

//  -- Enemy Constants --
#define ENEMY_SPRITE_POS	(ENEMY_BULLETS_SPRITE_POS + MAX_NUM_ENEMY_BULLETS)
#define MAX_NUM_ENEMIES		     10
#define MAX_ENEMY_X              300
#define MIN_ENEMY_Y              -32
#define MAX_ENEMY_Y              159

// -- Enemy Type Constants --
#define JELLYFISH_PATH              g_straightLeftPath_slow
#define JELLYFISH_BULLET_PATH       g_straightLeftPath_med
#define JELLYFISH_HEALTH            (5 + difficulty)
#define JELLYFISH_BULLET_FREQUENCY  200
#define JELLYFISH_LEFT_OFFSET       0
#define JELLYFISH_RIGHT_OFFSET      16
#define JELLYFISH_TOP_OFFSET        0
#define JELLYFISH_BOTTOM_OFFSET     32

#define MINE_PATH                   g_straightLeftPath_slow
#define MINE_BULLET_PATH            g_straightLeftPath_med
#define MINE_HEALTH                 (3 + difficulty)
#define MINE_BULLET_FREQUENCY       9001
#define MINE_LEFT_OFFSET            0
#define MINE_RIGHT_OFFSET           8
#define MINE_TOP_OFFSET             0
#define MINE_BOTTOM_OFFSET          8

#define SQUID_PATH                  g_zigZagLeftPath_largeAmplitude
#define SQUID_BULLET_PATH           g_straightLeftPath_med
#define SQUID_HEALTH                (5 + difficulty)
#define SQUID_BULLET_FREQUENCY      200
#define SQUID_LEFT_OFFSET           0
#define SQUID_RIGHT_OFFSET          16
#define SQUID_TOP_OFFSET            0
#define SQUID_BOTTOM_OFFSET         16

#define CRAB_PATH                   g_straightLeftPath_slow
#define CRAB_BULLET_PATH            g_straightLeftPath_med
#define CRAB_HEALTH                 10 + (difficulty)
#define CRAB_BULLET_FREQUENCY       350
#define CRAB_LEFT_OFFSET            0
#define CRAB_RIGHT_OFFSET           32
#define CRAB_TOP_OFFSET             4
#define CRAB_BOTTOM_OFFSET          32

#define SHARK_PATH                  g_zigZagLeftPath_smallAmplitude
#define SHARK_BULLET_PATH           g_straightLeftPath_med
#define SHARK_HEALTH                3 + (difficulty)
#define SHARK_BULLET_FREQUENCY      150
#define SHARK_LEFT_OFFSET           0
#define SHARK_RIGHT_OFFSET          16
#define SHARK_TOP_OFFSET            -1
#define SHARK_BOTTOM_OFFSET         8

//  -- R_Triggered_Indicator Constants --
#define R_TRIG_INDIC_SPRITE_POS		(ENEMY_SPRITE_POS + MAX_NUM_ENEMIES)
#define R_TRIG_INDIC_ORIGIN			224, 0
#define R_TRIG_INDIC_BASE_PALETTE	0
#define R_TRIG_INDIC_SPRITE_SIZE	Sprite::size_16x8

//  -- Lives Constants --
#define Ith_LIFE_SPRITE_POS			(R_TRIG_INDIC_SPRITE_POS + 1 + i)
#define LIFE_SPRITE_SIZE			Sprite::size_32x16
#define Ith_LIFE_SPRITE_ORIGIN		208, 144 - (16 * i)

// -- PowerUp constants --
#define POWER_UP_SPRITE_POS         (R_TRIG_INDIC_SPRITE_POS + 1 + MAX_LIVES + 1)
#define POWER_UP_SPRITE_SIZE	    Sprite::size_8x8
#define POWERUP_X_VALUE             240

#define LIFE_DISP_SPRITE_POS		(POWER_UP_SPRITE_POS + 1)
#define LIFE_DISP_SPRITE_SIZE		Sprite::size_8x8
#define BOMB_DISP_SPRITE_POS		(LIFE_DISP_SPRITE_POS + 1)
#define BOMB_DISP_SPRITE_SIZE		Sprite::size_8x8

#define PLAYER_DEFAULT_FIRE_FREQUENCY 10

// -- Sprite Number --
#define NUM_SPRITES                 79

// -- Bomb animation length --
#define BOMB_ANIMATION_LENGTH		60

class GameStateManager;

typedef Vec2 (*Path)(int*);

// GameState which manages the actual gameplay - ships, movement,
// bullets, etc.
class PlayState : public GameState
{
public:
	PlayState(GameStateManager* _gameStateMan);
	virtual void init();
	virtual void update();
	virtual void refreshed();
	virtual void destroy();
    virtual void suspend();

	void spawnEnemy(int count, StageEvent::EnemyType * enemyTypes, int * xSpawn, int * ySpawn, int yOffset, int difficulty);
	void spawnPowerUp(StageEvent::PowerUpType type, int yOffset);
	void spawnBullet(int count, bool player, int * x, int * y, Path * path, int * damage, int * spriteChoice);
	void advanceStage();
private:
    void updateBullets();
    void checkInput();
    void updateEnemies();
	void updatePowerUp();
	void updatePlayer();
    void updateText();

	Player * getPlayerPointer() { return &player; }
	//void loseLife();

	OBJ_ATTR* sprites;
	Player player;

	ObjectAllocator<Bullet, 32> playerBullets;
	ObjectAllocator<Enemy, 10> enemies;
    ObjectAllocator<Bullet, 32> enemyBullets;


	bool R_triggered;
	Sprite R_triggered_indicator;

    Sprite lives[10];

	Sprite lifeIndic;
	Sprite bombIndic;

	Vec2 starsBG_Pos;
	int bgMoveCounter;

	Stage * currentStage;
	int stageCount;

	PowerUp powerUp;
	bool powerUpExists;
	bool poweredUp;
	int powerUpCount;
	int powerUpEnd;

	bool bombAnimation;
	int bombAnimationCount;
	int bombShakeCounter;
};

#endif