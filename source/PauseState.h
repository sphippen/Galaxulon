// PauseState.h
// 
// Authors: Aric Parkinson, Spencer Phippen
// Date created: March 29, 2012
//
// -------
// Defines interface for the PauseState class
//

#ifndef PAUSESTATE_H
#define PAUSESTATE_H

#include "GameState.h"

class GameStateManager;

// Defines the Title Screen state
class PauseState : public GameState
{
public:
	PauseState(GameStateManager* _gameStateMan);
	virtual void init();
	virtual void update();
	virtual void destroy();
    virtual void suspend();
};

#endif