// PlayState.cpp
// 
// Authors: Aric Parkinson, Spencer Phippen, Daniel Setser, David Setser
// Date created: March 23, 2012
//
// -------
// Contains implementation for the PlayState class
//

#include "PlayState.h"

#include "tonc_core.h"
#include "tonc_input.h"
#include "tonc_types.h"
#include "tonc_video.h"

#include "assert2.h"
#include "globals.h"
#include "sfx.h"
#include "PauseState.h"
#include "GameOverState.h"
#include "HighScoreManager.h"
#include "StoreState.h"
#include "GameStateManager.h"
#include "TitleScreenState.h"
#include "StageEndState.h"
#include "AudioManager.h"
#include "OamManager.h"
#include "Sprite.h"
#include "globals.h"
#include "TextManager.h"

// Typedefs for inconveniently long Template types
typedef ObjectAllocator<Enemy, MAX_NUM_ENEMIES>::iterator EnemyIterator;
typedef ObjectAllocator<Bullet, MAX_NUM_PLAYER_BULLETS>::iterator PlayerBulletIterator;
typedef ObjectAllocator<Bullet, MAX_NUM_ENEMY_BULLETS>::iterator EnemyBulletIterator;



PlayState::PlayState(GameStateManager* _gameStateMan) : GameState(_gameStateMan), sprites(0), player(), playerBullets(), enemies(), enemyBullets(), 
	R_triggered(false), R_triggered_indicator(Sprite::size_16x8, g_bullet_selection1TilesPos), stageCount(0), bombAnimation(false), bombAnimationCount(0)
{
}

void PlayState::init()
{
	// Reset the GameState
	g_playerScore = 0;
	g_playerHitThisStage = false;
	g_playerStageBulletsHit = 0;
	g_playerStageBulletsTotal = 0;
	R_triggered = false;
	currentStage->reset();
	bombShakeCounter = 0;
	bombAnimationCount = 0;
	bgMoveCounter = 0;

	player.setBombCount(1);

	starsBG_Pos = Vec2(0, 0);
	REG_BG0VOFS = starsBG_Pos.y;
	REG_BG0HOFS = starsBG_Pos.x;

	// Init background
	memcpy16(&se_mem[24], g_demo_starsMap, (SBB_SIZE/2));

	// Get sprites from the OAM Manager
	//	- 1 for player
	//	- 10 for enemies
	//	- 32 for bullets
	//  - 32 for enemy bullets
	//	- 1 for R_triggered indicator
	//  - 1 for power up
	//  - 2 for displaying lives/bombs at bottom of screen
	sprites = g_oamMan->allocSprites(NUM_SPRITES);

	// Initialize the first stage
	currentStage = g_stage1;
    currentStage->reset();
	stageCount = 1;

	// Init player
	player.useImg(PLAYER_SPRITE_POS);
	player.setPos(PLAYER_ORIGIN);
	player.setPalette(PLAYER_BASE_PALETTE);
	player.setTileIndex(g_playerSpriteTilesPos);
	player.setSize(PLAYER_SPRITE_SIZE);
	player.setVisible(true);
	player.setParent(this);
	player.hitBox.left = player.X() + PLAYER_LEFT_OFFSET;
	player.hitBox.right = player.X() + PLAYER_RIGHT_OFFSET;
	player.hitBox.top = player.Y() + PLAYER_TOP_OFFSET;
	player.hitBox.bottom = player.Y() + PLAYER_BOTTOM_OFFSET;
	player.reset();

	// Give the objects in allocators sprites
	playerBullets.spriteInit(PLAYER_BULLETS_SPRITE_POS);
	enemyBullets.spriteInit(ENEMY_BULLETS_SPRITE_POS);
	enemies.spriteInit(ENEMY_SPRITE_POS);

	// Init the R_triggered indicator
	R_triggered_indicator.useImg(R_TRIG_INDIC_SPRITE_POS);
	R_triggered_indicator.setPos(R_TRIG_INDIC_ORIGIN);
	R_triggered_indicator.setPalette(R_TRIG_INDIC_BASE_PALETTE);
	R_triggered_indicator.setTileIndex(g_bullet_selection1TilesPos);
	R_triggered_indicator.setSize(R_TRIG_INDIC_SPRITE_SIZE);
	R_triggered_indicator.setVisible(true);

	bombAnimation = false;
	bombAnimationCount = 0;

	// Init display sprites at bottom
	lifeIndic.useImg(LIFE_DISP_SPRITE_POS);
	lifeIndic.setSize(LIFE_DISP_SPRITE_SIZE);
	lifeIndic.setTileIndex(g_extraLife_TilesPos);
	lifeIndic.setPalette(4);
	lifeIndic.setPos(0, 152);
	lifeIndic.setVisible(true);
	
	bombIndic.useImg(BOMB_DISP_SPRITE_POS);
	bombIndic.setSize(BOMB_DISP_SPRITE_SIZE);
	bombIndic.setTileIndex(g_extraBomb_TilesPos);
	bombIndic.setPalette(4);
	bombIndic.setPos(128, 152);
	bombIndic.setVisible(true);

	// Set up the PowerUp constants
	powerUpExists = false;
	poweredUp = false;

    // Power ups
    powerUp.useImg(POWER_UP_SPRITE_POS);
    powerUp.setVisible(false);
    powerUp.setSize(POWER_UP_SPRITE_SIZE);
    powerUp.setTileIndex(g_shield_TilesPos);

	//Set up store state
	g_storeState->setPlayerPointer(&player);

	//spawnPowerUp(StageEvent::bomb, 70);

	// Add in enemies.
	/*EnemyIterator itr = enemies.alloc(3);
	Enemy* curEnemy;
	int i = 0;
	while ((curEnemy = itr.next()) != 0)
	{
	curEnemy->setPos(100 + i * 20, 0 + i * 20);
	curEnemy->setPalette(0);
	curEnemy->setSize(Sprite::size_16x32);
	curEnemy->setTileIndex(g_jellyfishSpriteTilesPos);
	curEnemy->setVisible(true);
	curEnemy->setPath(g_verticalEnemyPath);
	curEnemy->resetTimer();
	++i;
	}*/
}

void PlayState::update()
{
	// Shake the background up and down when bomb is used
	if(bombAnimation)
	{
		bombAnimationCount++;
		bombShakeCounter++;
		// Up
		if(bombShakeCounter < 5)
		{
			starsBG_Pos.y -= 2;
		}
		// Down + Down
		else if(bombShakeCounter < 15)
		{
			starsBG_Pos.y += 2;
		}
		// Up
		else if(bombShakeCounter < 20)
		{
			starsBG_Pos.y -= 2;
		}
		// Reset
		else
		{
			bombShakeCounter = 0;
			--starsBG_Pos.y;
		}

		if(bombAnimationCount == BOMB_ANIMATION_LENGTH)
		{
			bombAnimationCount = 0;
			bombAnimation = false;
			//player.setVisible(true);
            //memcpy16(&se_mem[24], g_demo_starsMap, SBB_SIZE/2);
		}
	}

	// Move BG
	++bgMoveCounter;

	if(bgMoveCounter > 2)
	{
		++starsBG_Pos.x;
		bgMoveCounter = 0;
	}

	REG_BG0VOFS = starsBG_Pos.y;
	REG_BG0HOFS = starsBG_Pos.x;

	updateText();

	// Update the stage
	currentStage->update();

	// Checks for button pushes and updates accordingly
	checkInput();

	updatePlayer();
	
	// Udates the PowerUp and checks for collisions
	updatePowerUp();

	// Updates all of the enemies and has them fire.
	updateEnemies();

	// Updates all of the bullets and checks for collisions
	updateBullets();
}

void PlayState::refreshed()
{
    memcpy16(&se_mem[24], g_demo_starsMap, SBB_SIZE/2);
    
	REG_BG0VOFS = starsBG_Pos.x;
	REG_BG0HOFS = starsBG_Pos.y;

	// Get sprites from the OAM Manager
	//	- 1 for player
	//	- 10 for enemies
	//	- 32 for bullets
	//  - 32 for enemy bullets
	//	- 1 for R_triggered indicator
	//  - 9 for "lives"
    //  - 1 for power up
	sprites = g_oamMan->allocSprites(NUM_SPRITES);

	// Init display sprites at bottom
	lifeIndic.useImg(LIFE_DISP_SPRITE_POS);
	lifeIndic.setSize(LIFE_DISP_SPRITE_SIZE);
	lifeIndic.setTileIndex(g_extraLife_TilesPos);
	lifeIndic.setPalette(4);
	lifeIndic.setPos(0, 152);
	lifeIndic.setVisible(true);
	
	bombIndic.useImg(BOMB_DISP_SPRITE_POS);
	bombIndic.setSize(BOMB_DISP_SPRITE_SIZE);
	bombIndic.setTileIndex(g_extraBomb_TilesPos);
	bombIndic.setPalette(4);
	bombIndic.setPos(128, 152);
	bombIndic.setVisible(true);

	// Init player
	player.useImg(PLAYER_SPRITE_POS);
	player.setPalette(PLAYER_BASE_PALETTE);
	player.setTileIndex(g_playerSpriteTilesPos);
	player.setVisible(true);
	player.setParent(this);

	// Give the objects in allocators sprites
	playerBullets.spriteInit(PLAYER_BULLETS_SPRITE_POS);
	enemyBullets.spriteInit(ENEMY_BULLETS_SPRITE_POS);
	enemies.spriteInit(ENEMY_SPRITE_POS);

	// Init the R_triggered indicator
	R_triggered_indicator.useImg(R_TRIG_INDIC_SPRITE_POS);
	R_triggered_indicator.setPalette(R_TRIG_INDIC_BASE_PALETTE);
	R_triggered_indicator.setTileIndex(g_bullet_selection1TilesPos);
	R_triggered_indicator.setSize(R_TRIG_INDIC_SPRITE_SIZE);
	R_triggered_indicator.setVisible(true);

	// Initialize all lives
	/*for (int i = 0; i < MAX_LIVES; ++i)
	{
		lives[i].useImg(Ith_LIFE_SPRITE_POS);
		lives[i].setTileIndex(g_playerSpriteTilesPos);
	}*/

    // Power ups
    powerUp.useImg(POWER_UP_SPRITE_POS);
    powerUp.setSize(POWER_UP_SPRITE_SIZE);
    powerUp.setTileIndex(g_shield_TilesPos);

}

void PlayState::suspend()
{
	player.unuseImg();

	REG_BG0VOFS = 0;
	REG_BG0HOFS = 0;

	playerBullets.spriteDestroy();
	enemyBullets.spriteDestroy();
	enemies.spriteDestroy();

	R_triggered_indicator.unuseImg();
	
	// deinit display sprites at bottom
	lifeIndic.setVisible(false);
	lifeIndic.unuseImg();
	
	bombIndic.setVisible(false);
	bombIndic.unuseImg();

	int lifeCount = player.getLifeCount();
	for(int i = 0; i < lifeCount - 1; ++i)
	{
		lives[i].unuseImg();
	}
   
	powerUp.unuseImg();

    clearText();

	g_oamMan->deallocSprites(sprites, NUM_SPRITES);
}

void PlayState::spawnEnemy(int count, StageEvent::EnemyType * enemyTypes, int * xSpawn, int * ySpawn, int yOffset, int difficulty)
{
	EnemyIterator itr = enemies.alloc(count);
	Enemy * curEnemy;
	int i = 0;
	while ((curEnemy = itr.next()) != 0)
	{

		assert2(i < count, "The allocator gave more enemies than were suppose to be created.");
		switch(enemyTypes[i])
		{
		case StageEvent::jellyfish:
			{
				curEnemy->setPos(xSpawn[i], ySpawn[i] + yOffset);
				curEnemy->setPath(JELLYFISH_PATH);
				curEnemy->setBulletPath(JELLYFISH_BULLET_PATH);
				curEnemy->setMaxHealth(JELLYFISH_HEALTH + difficulty);
				curEnemy->setBulletFrequency(JELLYFISH_BULLET_FREQUENCY);
				curEnemy->setDamage(1);
				curEnemy->setParent(this);
				curEnemy->setSize(Sprite::size_16x32);
				curEnemy->setTileIndex(g_jellyfishTilesPos);
				curEnemy->hitBox.left = xSpawn[i] + JELLYFISH_LEFT_OFFSET;
				curEnemy->hitBox.right = xSpawn[i] + JELLYFISH_RIGHT_OFFSET;
				curEnemy->hitBox.top = ySpawn[i] + yOffset + JELLYFISH_TOP_OFFSET;
				curEnemy->hitBox.bottom = ySpawn[i] + yOffset + JELLYFISH_BOTTOM_OFFSET;
				break;
			}
		case StageEvent::mine:
			{
				curEnemy->setPos(xSpawn[i], ySpawn[i] + yOffset);
				curEnemy->setPath(MINE_PATH);
				curEnemy->setBulletPath(MINE_BULLET_PATH);
				curEnemy->setMaxHealth(MINE_HEALTH + difficulty);
				curEnemy->setBulletFrequency(MINE_BULLET_FREQUENCY);
				curEnemy->setDamage(1);
				curEnemy->setParent(this);
				curEnemy->setSize(Sprite::size_8x8);
				curEnemy->setTileIndex(g_mineTilesPos);
				curEnemy->hitBox.left = xSpawn[i] + MINE_LEFT_OFFSET;
				curEnemy->hitBox.right = xSpawn[i] + MINE_RIGHT_OFFSET;
				curEnemy->hitBox.top = ySpawn[i] + yOffset + MINE_TOP_OFFSET;
				curEnemy->hitBox.bottom = ySpawn[i] + yOffset + MINE_BOTTOM_OFFSET;
				break;
			}
		case StageEvent::squid:
			{
				curEnemy->setPos(xSpawn[i], ySpawn[i] + yOffset);
				curEnemy->setPath(SQUID_PATH);
				curEnemy->setBulletPath(SQUID_BULLET_PATH);
				curEnemy->setMaxHealth(SQUID_HEALTH);
				curEnemy->setBulletFrequency(SQUID_BULLET_FREQUENCY);
				curEnemy->setDamage(1);
				curEnemy->setParent(this);
				curEnemy->setSize(Sprite::size_16x16);
				curEnemy->setTileIndex(g_squidTilesPos);
				curEnemy->hitBox.left = xSpawn[i] + SQUID_LEFT_OFFSET;
				curEnemy->hitBox.right = xSpawn[i] + SQUID_RIGHT_OFFSET;
				curEnemy->hitBox.top = ySpawn[i] + yOffset + SQUID_TOP_OFFSET;
				curEnemy->hitBox.bottom = ySpawn[i] + yOffset + SQUID_BOTTOM_OFFSET;
				break;
			}
		case StageEvent::crab:
			{
				curEnemy->setPos(xSpawn[i], ySpawn[i] + yOffset);
				curEnemy->setPath(CRAB_PATH);
				curEnemy->setBulletPath(CRAB_BULLET_PATH);
				curEnemy->setMaxHealth(CRAB_HEALTH);
				curEnemy->setBulletFrequency(CRAB_BULLET_FREQUENCY);
				curEnemy->setDamage(1);
				curEnemy->setParent(this);
				curEnemy->setSize(Sprite::size_32x32);
				curEnemy->setTileIndex(g_crabTilesPos);
				curEnemy->hitBox.left = xSpawn[i] + CRAB_LEFT_OFFSET;
				curEnemy->hitBox.right = xSpawn[i] + CRAB_RIGHT_OFFSET;
				curEnemy->hitBox.top = ySpawn[i] + yOffset + CRAB_TOP_OFFSET;
				curEnemy->hitBox.bottom = ySpawn[i] + yOffset + CRAB_BOTTOM_OFFSET;
				break;
			}
		case StageEvent::shark:
			{
				curEnemy->setPos(xSpawn[i], ySpawn[i] + yOffset);
				curEnemy->setPath(SHARK_PATH);
				curEnemy->setBulletPath(SHARK_BULLET_PATH);
				curEnemy->setMaxHealth(SHARK_HEALTH);
				curEnemy->setBulletFrequency(SHARK_BULLET_FREQUENCY);
				curEnemy->setDamage(1);
				curEnemy->setParent(this);
				curEnemy->setSize(Sprite::size_16x8);
				curEnemy->setTileIndex(g_sharkTilesPos);
				curEnemy->hitBox.left = xSpawn[i] + SHARK_LEFT_OFFSET;
				curEnemy->hitBox.right = xSpawn[i] + SHARK_RIGHT_OFFSET;
				curEnemy->hitBox.top = ySpawn[i] + yOffset + SHARK_TOP_OFFSET;
				curEnemy->hitBox.bottom = ySpawn[i] + yOffset + SHARK_BOTTOM_OFFSET;
				break;
			}
		default:
			{
				assert2(false, "Invalid enemy type");
				break;
			}
		}
		curEnemy->setVisible(true);
		curEnemy->resetTimer();
		++i;
	}
}

void PlayState::spawnPowerUp(StageEvent::PowerUpType type, int yOffset)
{
	//assert2(type == StageEvent::firePower, "The type is firepower.");
	//if(powerUpExists || poweredUp) // Can change later if we add more than one power up on screen at a time.
	//	return;
	switch(type)
	{
	case StageEvent::none:
		{
			return;
		}
	case StageEvent::firePower:
		{
			powerUp.setPowerUpEnd(300);
			powerUp.setPowerUp(g_firePowerPowerUp);
			powerUp.setPowerDown(g_firePowerPowerDown);
			powerUp.setPos(POWERUP_X_VALUE, yOffset);
			powerUp.setVisible(true);
			powerUp.setPalette(4);
			powerUp.setTileIndex(g_primaryFireUp_TilesPos);
			powerUpExists = true;
			break;
		}
	case StageEvent::invincibility:
		{
			//powerUp.setPowerUpEnd(300);  The invincibility time is defined and then set in globals.cpp instead of here.
			powerUp.setPowerUp(g_invincibilityPowerUp);
			powerUp.setPowerDown(g_invincibilityPowerDown);
			powerUp.setPos(POWERUP_X_VALUE, yOffset);
			powerUp.setVisible(true);
			powerUp.setPalette(4);
			powerUp.setTileIndex(g_shield_TilesPos);
			powerUpExists = true;
			break;
		}
	case StageEvent::bomb:
		{
			powerUp.setPowerUp(g_bombPowerUp);
			powerUp.setPowerDown(g_bombPowerDown);
			powerUp.setPos(POWERUP_X_VALUE, yOffset);
			powerUp.setVisible(true);
			powerUp.setPalette(4);
			powerUp.setTileIndex(g_extraBomb_TilesPos);
			powerUpExists = true;
			break;
		}
	}
}

void PlayState::updateText()
{
	// Black BG for the text to show more visibly
	char blackStrip[30];
	for(int i = 0; i < 30; ++i)
	{
		blackStrip[i] = 127;
	}

	write2(blackStrip, Vec2(0, 0));
	write2(blackStrip, Vec2(0, 19));

    // Write lives
    char lifeCountToString[10];
    gba_itoa(player.getLifeCount(), lifeCountToString);
    write(" :", Vec2(0,19));

    write(lifeCountToString, Vec2(2, 19));

    // Write bombs
    char bombCountToString[10];
    gba_itoa(player.getBombCount(), bombCountToString);
    write(" :", Vec2(16, 19));
    write(bombCountToString, Vec2(18, 19));

	// Write Score
	char scoreToString[10];
	gba_itoa(g_playerScore, scoreToString);
    write("Score: ", Vec2(0, 0));
    write(scoreToString, Vec2(9, 0));


}

void PlayState::spawnBullet(int count, bool playerFired, int * x, int * y, Path * paths, int * damage, int * spriteChoice)
{
	if (playerFired)
	{
		// update global stage bullet counter
		g_playerStageBulletsTotal += count;

		PlayerBulletIterator bullets = playerBullets.alloc(count);
		int bulletCounter = 0;
		Bullet * toSpawn;
		while ((toSpawn = bullets.next()) != 0)
		{
			toSpawn->setPos(x[bulletCounter], y[bulletCounter]);
			toSpawn->setPath(paths[bulletCounter]);
			toSpawn->setDamage(damage[bulletCounter]);
			switch(spriteChoice[bulletCounter])
			{
			case 0:
				{
					toSpawn->setTileIndex(g_bulletSpriteTilesPos);
					break;
				}
			case 1:
				{
					toSpawn->setTileIndex(g_missileSpriteTilesPos);
					break;
				}
			default:
				{
					assert2(false, "The sprite choice was outside the cases given.");
					break;
				}
			}
			if(player.getPoweredUpFire())
				toSpawn->setPalette(1);
			else
				toSpawn->setPalette(0);

			toSpawn->setVisible(true);
			bulletCounter++;
		}
	}
	else
	{
		EnemyBulletIterator bullets = enemyBullets.alloc(count);
		int bulletCounter = 0;
		Bullet * toSpawn;
		while ((toSpawn = bullets.next()) != 0)
		{
			toSpawn->setPos(x[bulletCounter], y[bulletCounter]);
			toSpawn->setPath(paths[bulletCounter]);
			toSpawn->setDamage(damage[bulletCounter]);
			toSpawn->setTileIndex(g_bulletSpriteTilesPos);
			toSpawn->setPalette(8);
			toSpawn->setVisible(true);
			bulletCounter++;
		}
	}
}

void PlayState::destroy()
{
	// deinit display sprites at bottom
	lifeIndic.setVisible(false);
	lifeIndic.unuseImg();
	
	bombIndic.setVisible(false);
	bombIndic.unuseImg();

	starsBG_Pos = Vec2(0, 0);
	REG_BG0VOFS = starsBG_Pos.y;
	REG_BG0HOFS = starsBG_Pos.x;

	player.setVisible(false);
	player.unuseImg();

	PlayerBulletIterator playerBulletItr = playerBullets.begin();
	Bullet* curBullet;
	while((curBullet = playerBulletItr.next()) != 0)
	{
		curBullet->setVisible(false);
		playerBulletItr.dealloc();
	}

	EnemyBulletIterator enemyBulletItr = enemyBullets.begin();
	while((curBullet = enemyBulletItr.next()) != 0)
	{
		curBullet->setVisible(false);
		enemyBulletItr.dealloc();
	}

	EnemyIterator enemyItr = enemies.begin();
	Enemy* curEnemy;
	while((curEnemy = enemyItr.next()) != 0)
	{
		curEnemy->setVisible(false);
		enemyItr.dealloc();
	}

	playerBullets.spriteDestroy();
	enemyBullets.spriteDestroy();
	enemies.spriteDestroy();

	R_triggered_indicator.setVisible(false);
	R_triggered_indicator.unuseImg();
	
	int lifeCount = player.getLifeCount();
	for(int i = 0; i < lifeCount - 1; ++i)
	{
		lives[i].setVisible(false);
		lives[i].unuseImg();
	}

	powerUp.setVisible(false);
	powerUp.unuseImg();
	if (poweredUp)
	{
		powerUp.powerDown(&player);
	}

    clearText();

	g_oamMan->deallocSprites(sprites, NUM_SPRITES);
}

void PlayState::updateBullets()
{	
	PlayerBulletIterator playerBulletItr = playerBullets.begin();

	// Update each player bullet
	Bullet* toUpdate;
    bool bulletDestroyed;
	while ((toUpdate = playerBulletItr.next()) != 0)
	{
        bulletDestroyed = false;
		toUpdate->update();

		// Check if the bullet has gone off-screen. If it is, we hide it and dealloc.
		// Otherwise, we check for enemy collisions
		if(toUpdate->X() > (SCREEN_WIDTH + toUpdate->width()) || toUpdate->X() < (-1*toUpdate->width()) || toUpdate->Y() < (-1*toUpdate->height()) || toUpdate->Y() > SCREEN_HEIGHT)
		{
			toUpdate->setVisible(false);
			playerBulletItr.dealloc();
		}
		else
		{
			// Check if a bullet has collided with an enemy.
			EnemyIterator enemyItr = enemies.begin();
			Enemy* curEnemy;
			while ((curEnemy = enemyItr.next()) != 0)
			{
				if(toUpdate->hitBox.checkCollision(curEnemy->hitBox))
				{
                    if(!bulletDestroyed)
                    {
						g_playerStageBulletsHit++;
                        toUpdate->setVisible(false);
                        playerBulletItr.dealloc();
                        bulletDestroyed = true;
                    }
					if (curEnemy->damage(toUpdate->getDamage()))
					{
						curEnemy->setVisible(false);
						g_audioMan->playOneShot(enemyDiedNoise, enemyDiedNoiseSize, 32);
						enemyItr.dealloc();
						g_playerScore += 200;
					}
				}
			}
		}
	}

	EnemyBulletIterator enemyBulletItr = enemyBullets.begin();

	// Update each enemy bullet
	while ((toUpdate = enemyBulletItr.next()) != 0)
	{
		bulletDestroyed = false;
		toUpdate->update();

		// Check if a bullet has collided with the player.
		if(player.hitBox.checkCollision(toUpdate->hitBox))
		{
			if (!player.isInvincible())
			{
				// Decrement life count
                if(!player.getIsShielded())
                {
                    player.setPos(PLAYER_ORIGIN);
                    // Give temporary immunity to damage
                    player.setInvincibility(90);
                }
				player.killPlayer();
				int lifeCount = player.getLifeCount();
				lives[lifeCount - 1].setVisible(false);

				// If you're out of lives, the game is over
				if(lifeCount == 0)
				{
					gameStateMan->changeState(g_gameOverState);
					return;
				}

			}

			toUpdate->setVisible(false);
			if (!bulletDestroyed)
			{
				enemyBulletItr.dealloc();
				bulletDestroyed = true;
			}
		}

		// Check if the bullet has gone off-screen. If it is, we hide it and dealloc,
		// which effectively steps to the next iterator value. Otherwise, we'll step 
		// forward ourselves..
		if (toUpdate->X() > (SCREEN_WIDTH + toUpdate->width()) || toUpdate->X() < (-1*toUpdate->width()) ||
			toUpdate->Y() < (-1*toUpdate->height()) || toUpdate->Y() > SCREEN_HEIGHT)
		{
			toUpdate->setVisible(false);
			if (!bulletDestroyed)
			{
				enemyBulletItr.dealloc();
				bulletDestroyed = true;
			}
		}
	}
}

void PlayState::checkInput()
{
	// Start = pause game
	if (key_hit(KEY_START))
	{
		gameStateMan->pushState(g_pauseState);
	}

	// R-Button = swap weapon modes
	if (key_hit(KEY_R))
	{
		R_triggered = !R_triggered;
		R_triggered_indicator.setTileIndex(R_triggered ? g_bullet_selection2TilesPos : g_bullet_selection1TilesPos);
	}

	// Check for A button press
	//	First block checks for an immediate button press.
	//	Second block allows constant fire by holding a button. This is forced to a specific frequency.
	if (key_is_down(KEY_A))
	{
		if (R_triggered)
		{
			player.shootLightWide();
		}
		else
		{
			player.shootLightFocus();
		}
	}
	else
	{
		player.idleLight();
	}

	// secondary fire
	if (key_is_down(KEY_B))
	{
		player.shootHeavy();
	}
	else
	{
		player.idleHeavy();
	}

	// L-Button = bomb the screen, destroying all enemies and bullets
	if (key_hit(KEY_L) && player.hasBomb())
	{
		player.subtractBomb();

		// play the bomb sound
		g_audioMan->playOneShot(bombNoise, bombNoiseSize);

		// Destroy enemies
		EnemyIterator itr = enemies.begin();
		Enemy* curEnemy;
		while ((curEnemy = itr.next()) != 0)
		{
			curEnemy->setVisible(false);
			itr.dealloc();
			g_playerScore = g_playerScore + 200;
		}

		bombAnimation = true;
		//memset32(&se_mem[24], 0x00020002, (SBB_SIZE/4));
		//player.setVisible(false);

		// Destroy enemy bullets
		EnemyBulletIterator enemyBulletItr = enemyBullets.begin();
		Bullet* toUpdate;
		while ((toUpdate = enemyBulletItr.next()) != 0)
		{
			toUpdate->setVisible(false);
			enemyBulletItr.dealloc();
		}

		// Destroy player bullets
		PlayerBulletIterator bulletItr = playerBullets.begin();
		Bullet* playerToUpdate;
		while ((playerToUpdate = bulletItr.next()) != 0)
		{
			playerToUpdate->setVisible(false);
			bulletItr.dealloc();
		}

		powerUp.setVisible(false);
		powerUpExists = false;
	}
}

void PlayState::updatePowerUp()
{
	if (powerUpExists)
	{
		powerUp.update();
		if(powerUp.X() > (SCREEN_WIDTH + powerUp.width()) || powerUp.X() < (-1*powerUp.width()) || powerUp.Y() < (-1*powerUp.height()) || powerUp.Y() > (SCREEN_HEIGHT + powerUp.width()))
		{
			powerUp.setVisible(false);
			powerUp.setPos(POWERUP_X_VALUE, 0);
			powerUpExists = false;
		}
		if (powerUp.hitBox.checkCollision(player.hitBox))
		{
			powerUp.setVisible(false);
			powerUp.setPos(POWERUP_X_VALUE, 0);
			powerUpExists = false;
			powerUp.powerUp(&player);
			poweredUp = true;
			powerUpCount = 0;
			powerUpEnd = powerUp.getPowerUpEnd();
		}
		return;
	}
	else if (poweredUp)
	{
		++powerUpCount;
		if (powerUpCount == powerUpEnd)
		{
			powerUp.powerDown(&player);
			poweredUp = false;
		}
	}
}

void PlayState::updateEnemies()
{
	EnemyIterator itr = enemies.begin();
	Enemy* curEnemy;
	while ((curEnemy = itr.next()) != 0)
	{
		curEnemy->update();

		if (curEnemy->X() < (-1*curEnemy->width()) || curEnemy->X() > MAX_ENEMY_X || curEnemy->Y() < MIN_ENEMY_Y || curEnemy->Y() > MAX_ENEMY_Y)
		{
			curEnemy->setVisible(false);
			itr.dealloc();
		}

		else 
		{
			if(player.hitBox.checkCollision(curEnemy->hitBox))
			{
				//assert2(false, "The player crashed into an enemy.  If found, comment this out on line 854 in PlayState.cpp");
				if (!player.isInvincible())
				{
                    if(!player.getIsShielded())
                    {
                        player.setPos(PLAYER_ORIGIN);
                        player.setInvincibility(90);
                    }
					player.killPlayer();
					int lifeCount = player.getLifeCount();
					lives[lifeCount-1].setVisible(false);

					// If you're out of lives, the game is over.
					if(lifeCount == 0)
					{
						gameStateMan->changeState(g_gameOverState);
						return;
					}
				}
				g_playerScore += 200;
				curEnemy->setVisible(false);
				g_audioMan->playOneShot(enemyDiedNoise, enemyDiedNoiseSize, 32);
				itr.dealloc();
			}
			/*
			if(enemyBulletCounter == enemyBulletFrequency)
			{
				enemyBulletCounter = 0;

				// Hacked together quick enemy firing.
				EnemyBulletIterator newBullets = enemyBullets.alloc(1);
				Bullet* toModify;

				// Initialize new bullets
				int offset = -10;
				int dirSign = -1;
				while ((toModify = newBullets.next()) != 0)
				{
					toModify->setPos(curEnemy->X() + offset, curEnemy->Y() + 16);
					toModify->setTileIndex(g_bulletSpriteTilesPos);
					toModify->setVisible(true);
					toModify->setPath(g_horizontalLeftBulletPath);
					dirSign *= -1;
				}
			}
			else
				enemyBulletCounter++;
				*/
		}
	} 
}

void PlayState::advanceStage()
{
	EnemyIterator itr = enemies.begin();
	if(itr.next() == 0)
    {
        switch (stageCount)
        {
        case 1:
            {
				currentStage->increaseDifficulty();
                currentStage = g_stage2;
                currentStage->reset();
                stageCount = 2;
                break;
            }
        case 2:
            {
				currentStage->increaseDifficulty();
                currentStage = g_stage3;
                currentStage->reset();
                stageCount = 3;
                break;
            }
        case 3:
            {
				currentStage->increaseDifficulty();
                currentStage = g_stage1;
                currentStage->reset();
                stageCount = 1;
                break;
            }
        }
        //gameStateMan->pushState(g_pauseState);
        gameStateMan->pushState(g_stageEndState);
    }
}

void PlayState::updatePlayer()
{
	for(int i = 0; i < player.getLifeCount() - 1; i++)
	{
		lives[i].setVisible(true);
	}
	player.update();
}

/*
void PlayState::loseLife()
{
	// Decrement life count
	--lifeCount;
	lives[lifeCount-1].setVisible(false);

	// Give temporary immunity to damage
	player.setInvincibility(60);

	// If you're out of lives, the game is over
	if(lifeCount == 0)
	{
		gameStateMan->changeState(g_gameOverState);
		return;
	}

	player.setPos(PLAYER_ORIGIN);
}
*/