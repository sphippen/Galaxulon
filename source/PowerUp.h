// PowerUp.h
// 
// Authors: David Setser
// Date created: April 11, 2012
//
// -------
// Contains definition for PowerUp class, that represents a power up the user
// can get while playing in-stage.
//

#ifndef POWERUP_H
#define POWERUP_H

#include "Player.h"
#include "assert2.h"

#define POWERUP_X_VELOCITY -1
#define POWERUP_Y_VELOCITY 0

class PowerUp : public Sprite
{
public:
	PowerUp();
	void update();
	void powerUp(Player * player);
	void powerDown(Player * player);
	void setPowerUp(void (*_powerUpInternal)(Player*)) { powerUpInternal = _powerUpInternal; }
	void setPowerDown(void (*_powerDownInternal)(Player*)) { powerDownInternal = _powerDownInternal; }
	void setPowerUpEnd(int end) { powerUpEnd = end; }
	int getPowerUpEnd() { return powerUpEnd; }
private:
	void (*powerUpInternal)(Player*);
	void (*powerDownInternal)(Player*);
	int powerUpEnd;
};

#endif
