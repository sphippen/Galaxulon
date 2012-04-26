// GameStateManager.h
// 
// Authors: Aric Parkinson, Spencer Phippen
// Date created: March 23, 2012
//
// -------
// Defines interface for the GameStateManager class
//

#ifndef GAMESTATEMANAGER_H
#define GAMESTATEMANAGER_H

#include "GameState.h"

// Manages which GameState should be "updated" on VSync
class GameStateManager
{
public:
	GameStateManager();

	void update();
	void pushState(GameState* newState);
	void popState();
	void changeState(GameState* newState);

private:
	GameState* stateStack[5];
	int topState;
};

#endif