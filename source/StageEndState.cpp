// StageEndState.cpp
// 
// Authors: Spencer Phippen
// Date created: April 17, 2012
//
// -------
// Contains implementation for the StageEndState class
//

#include "StageEndState.h"

#include "tonc_core.h"
#include "tonc_input.h"

#include "assert2.h"
#include "TextManager.h"
#include "GameStateManager.h"
#include "globals.h"
#include "Player.h"
#include "StoreState.h"
#include "Vector.h"

StageEndState::StageEndState(GameStateManager* _gameStateMan) : GameState(_gameStateMan) { }

void StageEndState::init()
{
	// Draw a black background
	memset32(&se_mem[24], 0x00000000, (SBB_SIZE/4));

	// determine player bonuses
	// give no-miss if player did not get hit that stage
	bool noMissBonus = !g_playerHitThisStage;

	// give accuracy bonus if player had 87.5% accuracy
	// i.e. if # of bullets that didn't hit was < (total bullets / 8)
	bool accBonus = (g_playerStageBulletsTotal - g_playerStageBulletsHit) <= (g_playerStageBulletsTotal >> 3);
	int totalBonus = 0;

	// output player bonuses
	int curRow = 1;
	const int bonusCol = 3;
	if (noMissBonus)
	{
		write("NO-MISS:     5000", Vec2(bonusCol, curRow));
		curRow += 2;
		totalBonus += 5000;
	}
	if (accBonus)
	{
		write("ACCURACY:    2000", Vec2(bonusCol, curRow));
		curRow += 2;
		totalBonus += 2000;
	}
	write("TOTAL BONUS: ", Vec2(bonusCol, curRow));

	// expensive fn call, I know
	char intOut[15];
	gba_itoa(totalBonus, intOut);

	write(intOut, Vec2(bonusCol + 13, curRow));
	g_playerScore += totalBonus;
}

void StageEndState::update()
{
	if (key_hit(KEY_A) || key_hit(KEY_START))
	{
		gameStateMan->changeState(g_storeState);
	}
}

void StageEndState::refreshed()
{
	init();
}

void StageEndState::suspend()
{
	clearText();
}

void StageEndState::destroy()
{
	clearText();
}