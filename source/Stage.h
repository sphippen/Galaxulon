// Stage.h
// 
// Authors: David Setser
// Date created: April 4, 2012
//
// -------
// Defines interface for the Stage class
//

#ifndef STAGE_H
#define STAGE_H

#include "StageEvent.h"

class PlayState;

class Stage
{
public:
	Stage(PlayState * parent, StageEvent ** events, int * timer, int * yOffset, int count);
	void update();
	void reset();
	void increaseDifficulty();

private:
	StageEvent ** events;
	int * timer;
	int * yOffset;
	int counter;
	int eventIndex;
	int count;
	int difficulty;

	PlayState* parent;
};

#endif