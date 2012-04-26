// GameOverState.cpp
// 
// Authors: Daniel Setser
// Date created: April 7, 2012
//
// -------
// Contains implementation for the GameOverState class
//

#include "GameOverState.h"

#include "tonc_core.h"
#include "tonc_input.h"

#include "HighScoreManager.h"
#include "TextManager.h"
#include "GameStateManager.h"
#include "globals.h"
#include "OamManager.h"
#include "TitleScreenState.h"

#include "assert2.h"

GameOverState::GameOverState(GameStateManager* _gameStateMan) : GameState(_gameStateMan)
{
}

void GameOverState::init()
{
	blinkCounter = 0;

	// Draw a black background
	memset32(&se_mem[24], 0x00000000, (SBB_SIZE/4));
	
	// Check if the last score was a high score. If so, prompt the user for their initials and save the score
	highScorePos = -1;

	// NOTE: this goes from 0 to 8, leaving off the high score
	// this is so that Spencer always has the lowest high
	// score of 2.
	for(int i = 0; i < 9; ++i)
	{
		if(g_playerScore > getNthHighScore(i, 0))
		{
			highScorePos = i;
			break;
		}
	}

	if (highScorePos >= 0)
	{
		write("Congratulations!", Vec2(8, 2));
		write("You earned a high score", Vec2(5, 4));

		char str[10];
		gba_itoa(g_playerScore, str);
		write(str, Vec2(10, 6));

		// Prompt for initials
		write("Enter initials below:", Vec2(6, 10));

		// Init player's selected initials
		curInitials[0] = 'A';
		curInitials[1] = 'A';
		curInitials[2] = 'A';

		// Init display version of initials
		dispInitials[0] = curInitials[0];
		dispInitials[1] = ' ';
		dispInitials[2] = curInitials[1];
		dispInitials[3] = ' ';
		dispInitials[4] = curInitials[2];
		dispInitials[5] = '\0';

		write2(dispInitials, Vec2(13, 12));
		//write("_ _ _", Vec2(13, 12));
	}

	// Otherwise, it's just a game over
	else
	{
		write("GAME", Vec2(13, 9));
		write("OVER", Vec2(13, 11));
	}
}

void GameOverState::update()
{
	// START or A = finish inputing name, and go back to the title
	if (key_hit(KEY_START) || key_hit(KEY_A))
	{
		if(highScorePos >= 0)
		{
			// Save score
			newHighScore(g_playerScore, curInitials, highScorePos);
		}

		gameStateMan->changeState(g_titleState);
		return;
	}

	// UP/DOWN: Change your initial's letters
	if (key_hit(KEY_UP))
	{
		curInitials[curInitialSelect]++;
		if(curInitials[curInitialSelect] > '~')
			curInitials[curInitialSelect] = ' ';
	}
	if (key_hit(KEY_DOWN))
	{
		curInitials[curInitialSelect]--;
		if(curInitials[curInitialSelect] < ' ')
			curInitials[curInitialSelect] = '~';
	}

	// RIGHT/LEFT: Select a letter to change
	if (key_hit(KEY_LEFT))
	{
		curInitialSelect--;
		if(curInitialSelect < 0)
			curInitialSelect = 2;
	}
	if (key_hit(KEY_RIGHT))
	{
		curInitialSelect++;
		if(curInitialSelect > 2)
			curInitialSelect = 0;
	}

	// Update the display -- BUT ONLY IF YOU ACTUALLY HAVE A HIGH SCORE!
	if(highScorePos >= 0)
	{
		clearText2();
		dispInitials[0] = curInitials[0];
		dispInitials[2] = curInitials[1];
		dispInitials[4] = curInitials[2];

		// Implement blinking:
		if (blinkCounter > 4)
			dispInitials[curInitialSelect * 2] = ' ';
		if (blinkCounter > 8)
		{
			dispInitials[curInitialSelect * 2] = curInitials[curInitialSelect];
			blinkCounter = 0;
		}

		blinkCounter++;

		write2(dispInitials, Vec2(13, 12));
	}
}

void GameOverState::destroy()
{
	clearText();
	clearText2();
}

void GameOverState::suspend()
{
	clearText();
	clearText2();
}