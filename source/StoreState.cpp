// StoreState.cpp
// 
// Authors: Daniel Setser, David Setser
// Date created: April 13, 2012
//
// -------
// Contains implementation for the StoreState class
//

#include "tonc_core.h"
#include "tonc_input.h"

#include "TextManager.h"
#include "GameStateManager.h"
#include "globals.h"
#include "TitleScreenState.h"
#include "PauseState.h"
#include "Player.h"

#include "assert2.h"
#include "StoreState.h"

#define EXTRA_LIFE    0
#define EXTRA_BOMB    1
#define UPGRADE_LIGHT 2
#define UPGRADE_HEAVY 3
#define ADD_SHIELD    4

StoreState::StoreState(GameStateManager* _gameStateMan) : GameState(_gameStateMan)
{
    spriteCount = 5;
	selected = EXTRA_LIFE;
}

void StoreState::init()
{
	// update end-of-stage globals
	g_playerHitThisStage = false;
	g_playerStageBulletsHit = 0;
	g_playerStageBulletsTotal = 0;

	// Draw a black background
	memset32(&se_mem[24], 0x00000000, (SBB_SIZE/4));
	
    spriteObjs = g_oamMan->allocSprites(SPRITE_NUM);

    write("Upgrades:", Vec2(10, 1));

    player.useImg(SELECTOR_SPRITE_POS);
	player.setPos(0,SELECTOR_Y_SPAWN);
	player.setPalette(SELECTOR_BASE_PALETTE);
	player.setTileIndex(g_playerSpriteTilesPos);
	player.setSize(SELECTOR_SPRITE_SIZE);
	player.setVisible(true);

    for(int i = 0; i < spriteCount; i++)
    {
        sprites[i].useImg(UPGRADE_SPRITE_POS+i);
        sprites[i].setVisible(true);
        sprites[i].setSize(UPGRADE_SPRITE_SIZE);
        sprites[i].setPos(UPGRADE_X_POS + UPGRADE_X_INCREMENT, UPGRADE_Y_POS + UPGRADE_Y_INCREMENT*i);
		sprites[i].setPalette(4);
    }
	
	sprites[0].setTileIndex(g_extraLife_TilesPos);
	sprites[1].setTileIndex(g_extraBomb_TilesPos);
	sprites[2].setTileIndex(g_primaryFireUp_TilesPos);
	sprites[3].setTileIndex(g_secondFireUp_TilesPos);
	sprites[4].setTileIndex(g_shield_TilesPos);

	// Fourth upgrade has it's own subpalette
	sprites[3].setPalette(5);

	selected = EXTRA_LIFE;

    //Life, bomb, light fire upgrade, heavy fire upgrade, shield
    write("Extra life", Vec2(7, 4));
    write("Extra Bomb", Vec2(7, 7));
    write("Upgrade Light Fire", Vec2(7, 10));
    write("Upgrade Heavy Fire", Vec2(7, 13));
    write("Shield", Vec2(7, 16));
}

void StoreState::update()
{
	assert2(playerPointer != 0, "The store's player pointer is null.");
	if (key_hit(KEY_UP))
    {
        if(player.Y() != SELECTOR_Y_SPAWN)
		{
            player.moveBy(0, -1*UPGRADE_Y_INCREMENT);
			selected--; //Moves this to the appropriate value as described in the .h
		}
	}

    if (key_hit(KEY_DOWN))
	{
        if(player.Y() != (SELECTOR_Y_SPAWN + (4*UPGRADE_Y_INCREMENT)))
		{
            player.moveBy(0, UPGRADE_Y_INCREMENT);
			selected++; //Moves this to the appropriate value as described in the .h
		}
	}
	if (key_hit(KEY_A) || key_hit(KEY_START))
	{
		switch(selected)
		{	
		case EXTRA_LIFE:
			{
				playerPointer->addLife();
				break;
			}
		case EXTRA_BOMB:
			{
				playerPointer->addBomb();
				break;
			}
		case UPGRADE_LIGHT:
			{
				playerPointer->upgradeLightFire();
				break;
			}
		case UPGRADE_HEAVY:
			{
				playerPointer->upgradeHeavyFire();
				break;
			}
		case ADD_SHIELD:
			{
				playerPointer->addShield();
				break;
			}
		}
		gameStateMan->popState();
	}
}

void StoreState::destroy()
{
	clearText();
    for(int i = 0; i < spriteCount; i++)
    {
        sprites[i].unuseImg();
    }
    player.unuseImg();
    g_oamMan->deallocSprites(spriteObjs, SPRITE_NUM);
}

void StoreState::refreshed()
{
    init();
}

void StoreState::suspend()
{
    clearText();
    for(int i = 0; i < spriteCount; i++)
    {
        sprites[i].unuseImg();
    }
    player.unuseImg();
    g_oamMan->deallocSprites(spriteObjs, SPRITE_NUM);
}