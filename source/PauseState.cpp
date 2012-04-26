// PauseState.cpp
// 
// Authors: Aric Parkinson, Spencer Phippen
// Date created: March 29, 2012
//
// -------
// Contains implementation for the PauseState class
//

#include "PauseState.h"

#include "tonc_core.h"
#include "tonc_input.h"

#include "TextManager.h"
#include "GameStateManager.h"
#include "globals.h"

PauseState::PauseState(GameStateManager* _gameStateMan) : GameState(_gameStateMan)
{
}

void PauseState::init()
{
	// Draw our background
	memset32(&se_mem[24], 0x00000000, (SBB_SIZE/4));

	write("PAUSE", Vec2(12, 10));
}

void PauseState::update()
{
	if (key_hit(KEY_START))
	{
		gameStateMan->popState();
	}
}

void PauseState::destroy()
{
	clearText();
}

void PauseState::suspend()
{
    clearText();
}