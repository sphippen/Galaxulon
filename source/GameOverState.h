// GameOverState.h
// 
// Authors: Daniel Setser
// Date created: April 7, 2012
//
// -------
// Defines interface for the GameOverState class
//

#ifndef GAMEOVERSTATE_H
#define GAMEOVERSTATE_H

#include "GameState.h"
#include "Sprite.h"

class GameStateManager;

class GameOverState : public GameState
{
public:
	GameOverState(GameStateManager* _gameStateMan);
	virtual void init();
	virtual void update();
	virtual void destroy();
    virtual void suspend();

private:
	// Used for managing initials' selection when high scores are happening
		// Int from 0 to 2 representing which initial is currently selected.
	int curInitialSelect;
		// The string containing the initials, as well as the string to actually be displayed on screen
	char curInitials[3];
	char dispInitials[6];
		// Every so many frames, the letter that is selected blinks.
	int blinkCounter;
		// Position you ranked in the high score leader boards. Negative if you didn't rank.
	int highScorePos;
};

#endif