// TitleScreenState.cpp
// 
// Authors: Aric Parkinson, Spencer Phippen
// Date created: March 23, 2012
//
// -------
// Contains implementation for the TitleScrenState class
//

#include "TitleScreenState.h"

#include "tonc_core.h"
#include "tonc_input.h"

#include "TextManager.h"
#include "HighScoreManager.h"
#include "GameStateManager.h"
#include "OamManager.h"
#include "PlayState.h"
#include "globals.h"

TitleScreenState::TitleScreenState(GameStateManager* _gameStateMan) : GameState(_gameStateMan) {	}

void TitleScreenState::init()
{
	curHighScoreDisp = 0;

	// Draw our background
	memset32(&se_mem[24], 0x00000000, (SBB_SIZE/4));
	
	// Initialize high scores
	initSRAM();

	//write("Galaxulon", Vec2(10, 5));
	sprites = g_oamMan->allocSprites(2);

	title_sprite1.useImg(sprites + 1);
	title_sprite1.setTileIndex(g_galaxulon_titleTilesPos1);
	title_sprite1.setPalette(9);
	title_sprite1.setPos(TITLE_SPRITE_1_POS);
	title_sprite1.setSize(Sprite::size_64x64);
	title_sprite1.setVisible(true);

	title_sprite2.useImg(sprites + 2);
	title_sprite2.setTileIndex(g_galaxulon_titleTilesPos2);
	title_sprite2.setPalette(9);
	title_sprite2.setPos(TITLE_SPRITE_2_POS);
	title_sprite2.setSize(Sprite::size_64x64);
	title_sprite2.setVisible(true);
	
	write("Press start to play", Vec2(5, 10));

	// Write the top score to the screen
	char playerName[4];
	int topScore = getNthHighScore(curHighScoreDisp, playerName);
	char topScoreToString[20];
	gba_itoa(topScore, topScoreToString);

	write(playerName, Vec2(14, 13));
	write(topScoreToString, Vec2(12, 15));
}

void TitleScreenState::update()
{
	if (key_hit(KEY_START))
	{
		gameStateMan->changeState(g_playState);
	}
	else if (key_hit(KEY_RIGHT))
	{
		// Incriment high score display position
		++curHighScoreDisp;
		if(curHighScoreDisp == 10)
			curHighScoreDisp = 0;

		// Write the top score to the screen
		char playerName[4];
		int topScore = getNthHighScore(curHighScoreDisp, playerName);
		char topScoreToString[20];
		gba_itoa(topScore, topScoreToString);

		write(playerName, Vec2(14, 13));
		write("          ", Vec2(12, 15));	// Erase previous high-score display
		write(topScoreToString, Vec2(12, 15));
	}
	else if (key_hit(KEY_LEFT))
	{
		// Decrement high score display position
		--curHighScoreDisp;
		if(curHighScoreDisp == -1)
			curHighScoreDisp = 9;

		// Write the top score to the screen
		char playerName[4];
		int topScore = getNthHighScore(curHighScoreDisp, playerName);
		char topScoreToString[20];
		gba_itoa(topScore, topScoreToString);

		write(playerName, Vec2(14, 13));
		write("          ", Vec2(12, 15));	// Erase previous high-score display
		write(topScoreToString, Vec2(12, 15));
	}
}

void TitleScreenState::destroy()
{
	// Clear all text
	clearText();

	// Dealloc the title
	title_sprite1.setVisible(false);
	title_sprite1.unuseImg();
	title_sprite2.setVisible(false);
	title_sprite1.unuseImg();

	g_oamMan->deallocSprites(sprites, 2);
}

void TitleScreenState::suspend()
{
	clearText();
	title_sprite1.setVisible(false);
	title_sprite2.setVisible(false);
}