// StageEvent.h
// 
// Authors: David Setser
// Date created: April 4, 2012
//
// -------
// Defines interface for the StageEvent struct
//

#ifndef STAGEEVENT_H
#define STAGEEVENT_H

struct StageEvent
{
	enum EnemyType
	{
		jellyfish = 0, mine, shark, squid, crab,
	};

	enum PowerUpType
	{
		none = 0, firePower, invincibility, bomb
	};

	EnemyType enemyType[5];
	int xSpawnPos[5];
	int ySpawnPos[5];
	int enemyCount;
	bool endEvent;
	PowerUpType powerUp;
};

#endif
