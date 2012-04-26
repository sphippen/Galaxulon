// Enemy.cpp
//
// Authors: Aric Parkinson, Spencer Phippen, David Setser
// Date created: April 2, 2012
//
// -------
// Implementation for the Enemy class

#include "Enemy.h"
#include "globals.h"
#include "assert2.h"
#include "PlayState.h"

Enemy::Enemy() : Sprite(Sprite::size_16x32, g_jellyfishTilesPos, false), time(0), enemyBulletCounter(0), enemyBulletFrequency(150)	{	} 

void Enemy::update()
{
	// Enemy AI strat: STAY WHERE I AM BECAUSE I'M TOO SCARED TO MOVE ;___;

	// New Enemy AI strat: Move exactly where we tell it like a good slave but never shoot. Run away from bullets. T_T
	assert2(pathFn != 0, "no enemy path set");
	Vec2 velocity = pathFn(&time);
	//assert2(bulletDamage == 100 && velocity.y != 1, "The enemy exists and its y velocity is 1.");
	moveBy(velocity.x, velocity.y);
	time++;
	if (!(bulletDamage == 0))
	{
		if (enemyBulletCounter == enemyBulletFrequency)
		{
			int bulletPosX = X() + BULLET_X_OFFSET;
			int bulletPosY = Y() + height() + BULLET_Y_OFFSET;
			int bulletSprite = 0;
			parent->spawnBullet(1, false, &bulletPosX, &bulletPosY, &bulletPath, &bulletDamage, &bulletSprite);
			enemyBulletCounter = 0;
		}
		else
			enemyBulletCounter++;
	}
}

void Enemy::setPath(Vec2(*_pathFn)(int*))
{
	pathFn = _pathFn;
}

bool Enemy::damage(int amount)
{
	currentHealth -= amount;
	return (currentHealth <= 0);
}

void Enemy::resetTimer()
{
	time = 0;
}

void Enemy::setMaxHealth(int health)
{
	maxHealth = health;
	currentHealth = maxHealth;
}

void Enemy::setBulletFrequency(int frequency)
{
	enemyBulletFrequency = frequency;
	enemyBulletCounter = (frequency >> 1) & 127;
}

void Enemy::setParent(PlayState * _parent)
{
	parent = _parent;
}

void Enemy::setBulletPath(Vec2 (*_bulletPath)(int*))
{
	bulletPath = _bulletPath;
}

void Enemy::setDamage(int _damage)
{
	bulletDamage = _damage;
}