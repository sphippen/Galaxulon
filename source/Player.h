// Player.h
//
// Authors: Aric Parkinson, Spencer Phippen
// Date created: March 30, 2012
//
// -------
// Contains interface for Player class

#ifndef PLAYER_H
#define PLAYER_H

#include "Sprite.h"

#define BULLET_Y_OFFSET_1                -10
#define BULLET_Y_OFFSET_2                10
#define BULLET_Y_OFFSET_3                2
#define BULLET_Y_OFFSET_HEAVY            5
#define MAX_LIVES			             10
#define INITIAL_LIFE_COUNT               3
#define DEFAULT_HEAVY_BULLET_FREQUENCY   40
#define DEFAULT_LIGHT_BULLET_FREQUENCY   10
#define UPGRADED_HEAVY_BULLET_FREQUENCY  20

class PlayState;

class Player : public Sprite
{
public:
	Player();

	void update();
	void changeSpeed(int speedChange);
	void shootLightWide();
	void shootLightFocus();
	void idleLight();
	void shootHeavy();
	void idleHeavy();
	void setParent(PlayState * parent);
	void setInvincibility(int time);
	bool isInvincible();
	int setLightDamage(int damage);
	int setHeavyDamage(int damage);
	int addBomb();
	int subtractBomb();
	bool hasBomb();
	bool killPlayer(); //Returns true if the player has lives left.
	void addLife();
	int getLifeCount();
	void upgradeLightFire();
	void upgradeHeavyFire();
	void addShield();
	void reset();
	int getBombCount() { return bombCount; }
	void setBombCount(int bombs) { bombCount = bombs; }
	bool getIsShielded() { return isShielded; }
	bool getPoweredUpFire() { return poweredUpFire; }
	void setPoweredUpFire(bool poweredUp) { poweredUpFire = poweredUp; }
	
private:
	int velocity;
	int lightBulletCounter;
	int lightBulletFrequency;
	int heavyBulletCounter;
	int heavyBulletFrequency;
	int playerLightDamage;
	int playerHeavyDamage;
	PlayState * parent;
	int invincibilityTime;
	int bombCount;
	int lifeCount;
	bool splitFiresUp;
	bool doubleLightFire;
	bool fastChargingHeavyFire;
	bool hasShield;
	bool isShielded;
	int shieldCounter;
	bool poweredUpFire;
};

#endif