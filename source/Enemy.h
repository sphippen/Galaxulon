// Enemy.h
//
// Authors: Aric Parkinson, Spencer Phippen, David Setser
// Date created: April 2, 2012
//
// -------
// Contains interface for Enemy class

#ifndef ENEMY_H
#define ENEMY_H

#define BULLET_X_OFFSET -8
#define BULLET_Y_OFFSET -4 //(half the sprite height - half the bullet height)

#include "Sprite.h"
#include "Vector.h"

class PlayState;

class Enemy : public Sprite
{
public:
	Enemy();

	void update();
	void setPath(Vec2 (*_pathFn)(int*));
	bool damage(int amount); //Returns true if it survives; false if it dies
	void resetTimer();
	void setMaxHealth(int health);
	void setBulletFrequency(int frequency);
	void setParent(PlayState * parent);
	void setBulletPath(Vec2 (*_bulletPath)(int*));
	void setDamage(int damage);

private:
	// Function pointer that returns the velocity at that point in time.
	Vec2 (*pathFn)(int*);
	Vec2 (*bulletPath)(int*);
	int time;
	int maxHealth;
	int currentHealth;
	int enemyBulletCounter;
	int enemyBulletFrequency;
	int bulletDamage; //If bulletDamage is 0, the enemy does not fire
	PlayState * parent;
};

#endif