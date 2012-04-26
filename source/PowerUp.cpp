// PowerUp.cpp
// 
// Authors: David Setser, Spencer Phippen
// Date created: April 11, 2012
//
// -------
// Contains definition for PowerUp methods.
//

#include "PowerUp.h"

PowerUp::PowerUp() : powerUpInternal(0), powerDownInternal(0) { }

void PowerUp::update()
{
	this->moveBy(POWERUP_X_VELOCITY, POWERUP_Y_VELOCITY);
}

void PowerUp::powerUp(Player * _player)
{
	assert2(powerUpInternal != 0, "The player collected a power up that didn't have a method.");
	powerUpInternal(_player);
}

void PowerUp::powerDown(Player * _player)
{
	assert2(powerDownInternal != 0, "The player collected a power up that didn't have a method.");
	powerDownInternal(_player);
}
