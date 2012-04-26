// StageEndState.h
// 
// Authors: Spencer Phippen
// Date created: April 17, 2012
//
// -------
// Defines interface for the StageEndState class
//

#ifndef STAGEENDSTATE_H
#define STAGEENDSTATE_H

#include "GameState.h"

class GameStateManager;

class StageEndState : public GameState
{
public:
	StageEndState(GameStateManager* _gameStateMan);
	virtual void init();
	virtual void destroy();
	virtual void update();
	virtual void refreshed();
	virtual void suspend();

private:
};

#endif