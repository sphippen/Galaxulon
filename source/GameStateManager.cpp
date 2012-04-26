// GameStateManager.cpp
// 
// Authors: Aric Parkinson, Spencer Phippen
// Date created: March 23, 2012
//
// -------
// Contains implementation for the GameStateManager class
//

#include "GameStateManager.h"
#include "GameState.h"
#include "assert2.h"

#define MAX_NUM_GAME_STATES 5
#define EMPTY_STACK -1

GameStateManager::GameStateManager() : topState(EMPTY_STACK)
{
	for (int i = 0; i < MAX_NUM_GAME_STATES; ++i)
	{
		stateStack[i] = 0;
	}
}

void GameStateManager::update()
{
	assert2(topState != EMPTY_STACK, "State stack is empty");

	stateStack[topState]->update();
}

void GameStateManager::pushState(GameState* newState)
{
	assert2(newState != 0, "NULL state");
	assert2(topState != MAX_NUM_GAME_STATES-1, "State stack is full");

    // We don't want to suspend a state that isn't there!
    if(topState != EMPTY_STACK)
        stateStack[topState]->suspend();

	newState->init();
	stateStack[++topState] = newState;
}

void GameStateManager::popState()
{
	assert2(topState != EMPTY_STACK, "State stack is empty");

	stateStack[topState]->destroy();
	--topState;
	stateStack[topState]->refreshed();
}

void GameStateManager::changeState(GameState* newState)
{
	assert2(newState != 0, "NULL state");
	assert2(topState != EMPTY_STACK, "State stack is empty");

	stateStack[topState]->destroy();
	newState->init();
	stateStack[topState] = newState;
}