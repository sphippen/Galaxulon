// StoreState.h
// 
// Authors: Daniel Setser, David Setser
// Date created: April 13, 2012
//
// -------
// Defines interface for the StoreState class
//

#ifndef STORESTATE_H
#define STORESTATE_H

#define SPRITE_NUM                 13 // 12 upgrades and 1 player

#define SELECTOR_SPRITE_SIZE	   Sprite::size_32x16
#define SELECTOR_SPRITE_POS		   (spriteObjs)
#define SELECTOR_BASE_PALETTE	   0
#define SELECTOR_Y_SPAWN           27

#define UPGRADE_SPRITE_POS         (spriteObjs+1)
#define UPGRADE_SPRITE_SIZE	       Sprite::size_8x8
#define UPGRADE_X_POS              40
#define UPGRADE_Y_POS              32
#define UPGRADE_Y_INCREMENT        24
#define UPGRADE_X_INCREMENT        0

#define EXTRA_LIFE    0
#define EXTRA_BOMB    1
#define UPGRADE_LIGHT 2
#define UPGRADE_HEAVY 3
#define ADD_SHIELD    4

#include "GameState.h"
#include "Sprite.h"
#include "OamManager.h"


class GameStateManager;

class StoreState : public GameState
{
public:
	StoreState(GameStateManager* _gameStateMan);
	virtual void init();
	virtual void update();
	virtual void destroy();
    virtual void suspend();
    virtual void refreshed();
	void setPlayerPointer(Player * _player) { playerPointer = _player; }

private:
    Sprite sprites[12];
    int spriteCount;
    Sprite player;
    OBJ_ATTR* spriteObjs;
	Player * playerPointer;
	int selected;
};

#endif