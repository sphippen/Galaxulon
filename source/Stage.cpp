// Stage.cpp
// 
// Authors: David Setser
// Date created: April 4, 2012
//
// -------
// Contains implementation for Stage class.
//

#include "Stage.h"
#include "PlayState.h"
#include "assert2.h"

Stage::Stage(PlayState * _parent, StageEvent ** _events, int * _timer, int * _yOffset, int _count)
{
	events = _events;
	counter = 0;
	eventIndex = 0;
	parent = _parent;
	count = _count;
	timer = _timer;
	yOffset = _yOffset;
	difficulty = 0;
}

void Stage::update()
{
	assert2(eventIndex < count, "There was no end event in the stage.");
	StageEvent * currentEvent = events[eventIndex];
	if (counter == timer[eventIndex])
	{
		if (currentEvent->powerUp != StageEvent::none)
		{
			parent->spawnPowerUp(currentEvent->powerUp, yOffset[eventIndex]);
		}
		else if (currentEvent->endEvent)
		{
			parent->advanceStage();
			return;
		}
		else
		{
			parent->spawnEnemy(currentEvent->enemyCount, currentEvent->enemyType, currentEvent->xSpawnPos, currentEvent->ySpawnPos, yOffset[eventIndex], difficulty);
		}
		counter = 0;
		eventIndex++;
	}
	else
	{
		counter++;
	}
}

void Stage::reset()
{
	counter = 0;
	eventIndex = 0;
}

void Stage::increaseDifficulty()
{
	difficulty++;
}
