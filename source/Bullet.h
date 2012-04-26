// Bullet.h
//
// Authors: Aric Parkinson, Spencer Phippen
// Date created: March 30, 2012
//
// -------
// Contains interface for Bullet class

#ifndef BULLET_H
#define BULLET_H

#include "Sprite.h"
#include "Vector.h"

class Bullet : public Sprite
{
public:
	Bullet();

	void update();

    void setPath(Vec2 (*_pathFn)(int*));
	void setDamage(int _damage) { damage = _damage; }
	int getDamage() { return damage; }
private:
    Vec2 (*pathFn)(int*);
    int time;
	int damage;
};

#endif