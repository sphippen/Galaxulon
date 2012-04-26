// Bullet.cpp
//
// Authors: Aric Parkinson, Spencer Phippen
// Date created: March 30, 2012
//
// -------
// Contains implementation for Bullet class

#include "Bullet.h"
#include "globals.h"
#include "assert2.h"

Bullet::Bullet() : Sprite(Sprite::size_8x8, g_bulletSpriteTilesPos, false), time(0){   }

void Bullet::update()
{
	if(isVisible())
	{
        assert2(pathFn != 0, "No bullet path set");
        Vec2 velocity = pathFn(&time);
        moveBy(velocity.x, velocity.y);
        time++;
    }
}

void Bullet::setPath(Vec2(*_pathFn)(int*))
{
    pathFn = _pathFn;
}