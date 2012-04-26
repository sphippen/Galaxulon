// GameState.cpp
//
// Authors: Spencer Phippen, Aric Parkinson
// Date created: March 29, 2012
//
// -------
// Contains implementation of certain functions for GameState

#include "GameState.h"

#include "GameStateManager.h"

GameState::GameState(GameStateManager* _gameStateMan) : gameStateMan(_gameStateMan) { }

void GameState::init() { }

void GameState::refreshed() { }

void GameState::destroy()	{	}

void GameState::suspend()	{	}