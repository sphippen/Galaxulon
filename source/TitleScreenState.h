// TitleScreenState.h
// 
// Authors: Aric Parkinson, Spencer Phippen
// Date created: March 23, 2012
//
// -------
// Defines interface for the TitleScreenState class
//

#ifndef TITLESCREENSTATE_H
#define TITLESCREENSTATE_H

#define TITLE_SPRITE_1_POS 60, 10
#define TITLE_SPRITE_2_POS 124, 10

#include "tonc_video.h"

#include "Sprite.h"
#include "GameState.h"

class GameStateManager;

// Defines the Title Screen state
class TitleScreenState : public GameState
{
public:
	TitleScreenState(GameStateManager* _gameStateMan);
	virtual void init();
	virtual void update();
	virtual void destroy();
    virtual void suspend();

private:
	int curHighScoreDisp;
	Sprite title_sprite1;
	Sprite title_sprite2;
	OBJ_ATTR* sprites;
};

#endif