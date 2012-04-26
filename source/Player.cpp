// Player.cpp
//
// Authors: Aric Parkinson, Spencer Phippen, David Setser
// Date created: March 30, 2012
//
// -------
// Contains implementation for Player class

#include "Player.h"

#include "tonc_input.h"

#include "globals.h"
#include "sfx.h"
#include "AudioManager.h"
#include "PlayState.h"

#define SCREEN_BOUND_HORIZ 240
#define SCREEN_BOUND_VERT 160

Player::Player() : Sprite(Sprite::size_32x16, g_playerSpriteTilesPos), velocity(2), lightBulletCounter(0), lightBulletFrequency(DEFAULT_LIGHT_BULLET_FREQUENCY),
	heavyBulletCounter(0), heavyBulletFrequency(DEFAULT_HEAVY_BULLET_FREQUENCY), playerLightDamage(1), playerHeavyDamage(3), invincibilityTime(0), bombCount(1),
	lifeCount(INITIAL_LIFE_COUNT), splitFiresUp(true), doubleLightFire(false), fastChargingHeavyFire(false), hasShield(false),
    isShielded(false), shieldCounter(0)
{

}

void Player::update()
{
	int _x = key_tri_horz() * velocity + X();
	int _y = key_tri_vert() * velocity + Y();

	if (_x < 0)
		_x = 0;
	else if (_x > SCREEN_BOUND_HORIZ - width())
		_x = SCREEN_BOUND_HORIZ - width();
	if (_y < 0)
		_y = 0;
	else if (_y > SCREEN_BOUND_VERT - height())
		_y = SCREEN_BOUND_VERT - height();

	setPos(_x, _y);
	
	if (invincibilityTime > 0)
		invincibilityTime--;

    shieldCounter--;
    if(shieldCounter == 0)
    {
        isShielded = true;
    }

	// Display a different coloring if shields are up
    if(isInvincible())
        setPalette(7);
	else if(isShielded)
        setPalette(6);
	else
        setPalette(0);

	assert2(invincibilityTime >= 0, "The player's invincibility time is negative.");
	assert2(bombCount >=0, "The player's bomb count is negative.");
	assert2(bombCount <=3, "The player's bomb count is greater than 3.");
	assert2(playerLightDamage > 0, "The player's light damage is not greater than 0.");
	assert2(playerHeavyDamage > 0, "The player's heavy damage is not greater than 0.");
}

void Player::changeSpeed(int speedChange)
{
	velocity += speedChange;
}

void Player::shootLightWide()
{
	if (lightBulletCounter == lightBulletFrequency)
	{
		g_audioMan->playOneShot(shotNoise2, shotNoise2Size, 20);
		if (!doubleLightFire)
		{
			if (splitFiresUp)
			{
				splitFiresUp = false;
				int spawnPosX = X() + width();
				int spawnPosY = Y() + BULLET_Y_OFFSET_2;
				Path bulletPath = g_upRightBulletPath;
				int damage = playerLightDamage;
				int bulletSprite = 0;
				parent->spawnBullet(1, true, &spawnPosX, &spawnPosY, &bulletPath, &damage, &bulletSprite);
			}
			else
			{
				splitFiresUp = true;
				int spawnPosX = X() + width();
				int spawnPosY = Y() + BULLET_Y_OFFSET_1;
				Path bulletPath = g_downRightBulletPath;
				int damage = playerLightDamage;
				int bulletSprite = 0;
				parent->spawnBullet(1, true, &spawnPosX, &spawnPosY, &bulletPath, &damage, &bulletSprite);
			}
		}
		else
		{
			int spawnPosX[2];
			spawnPosX[0] = X() + width();
			spawnPosX[1] = X() + width();
			int spawnPosY[2];
			spawnPosY[0] = Y() + BULLET_Y_OFFSET_1;
			spawnPosY[1] = Y() + BULLET_Y_OFFSET_2;
			Path bulletPaths[2];
			bulletPaths[0] = g_downRightBulletPath;
			bulletPaths[1] = g_upRightBulletPath;
			int damage[2];
			damage[0] = playerLightDamage;
			damage[1] = playerLightDamage;
			int bulletSprite[2];
			bulletSprite[0] = 0;
			bulletSprite[1] = 0;
			parent->spawnBullet(2, true, spawnPosX, spawnPosY, bulletPaths, damage, bulletSprite);
		}
		lightBulletCounter = 0;
		heavyBulletCounter = 0;
	}
	else
		lightBulletCounter++;
}

void Player::shootLightFocus()
{
	if (lightBulletCounter == lightBulletFrequency)
	{
		g_audioMan->playOneShot(shotNoise1, shotNoise1Size, 20);
		if (!doubleLightFire)
		{
			int spawnPosX = X() + width();
			int spawnPosY = Y() + BULLET_Y_OFFSET_3;
			Path bulletPaths = g_horizontalRightBulletPath;
			int damage = playerLightDamage;
			int bulletSprite = 0;
			parent->spawnBullet(1, true, &spawnPosX, &spawnPosY, &bulletPaths, &damage, &bulletSprite);
		}
		else
		{
			int spawnPosX[2];
			spawnPosX[0] = X() + width();
			spawnPosX[1] = X() + width();
			int spawnPosY[2];
			spawnPosY[0] = Y() + BULLET_Y_OFFSET_1;
			spawnPosY[1] = Y() + BULLET_Y_OFFSET_2;
			Path bulletPaths[2];
			bulletPaths[0] = g_horizontalRightBulletPath;
			bulletPaths[1] = g_horizontalRightBulletPath;
			int damage[2];
			damage[0] = playerLightDamage;
			damage[1] = playerLightDamage;
			int bulletSprite[2];
			bulletSprite[0] = 0;
			bulletSprite[1] = 0;
			parent->spawnBullet(2, true, spawnPosX, spawnPosY, bulletPaths, damage, bulletSprite);
		}
		lightBulletCounter = 0;
		heavyBulletCounter = 0;
	}
	else
		lightBulletCounter++;
}

void Player::idleLight()
{
	lightBulletCounter = lightBulletFrequency;
}

void Player::shootHeavy()
{
	assert2(heavyBulletCounter <= heavyBulletFrequency, "The player's heavyBulletCounter was greater than its frequency.");
	assert2(heavyBulletCounter >= 0, "The player's heavybulletCounter was negative.");
	if (heavyBulletCounter == heavyBulletFrequency)
	{
		g_audioMan->playOneShot(shotNoise3, shotNoise3Size, 30);
		int spawnPosX = X() + width();
		int spawnPosY = Y() + BULLET_Y_OFFSET_HEAVY;
		Path bulletPaths = g_horizontalRightBulletPath;
		int damage = playerHeavyDamage;
		int bulletSprite = 1;
		parent->spawnBullet(1, true, &spawnPosX, &spawnPosY, &bulletPaths, &damage, &bulletSprite);
		heavyBulletCounter = 0;
	}
	else
		heavyBulletCounter++;
}

void Player::idleHeavy()
{
	if (heavyBulletCounter < heavyBulletFrequency)
		heavyBulletCounter++;
}

void Player::setParent(PlayState * _parent)
{
	parent = _parent;
}

void Player::setInvincibility(int time)
{
	invincibilityTime = time;
}

bool Player::isInvincible()
{
	assert2(invincibilityTime >= 0, "The player's invincibility time is negative.");
	return (invincibilityTime != 0);
}

int Player::setLightDamage(int damage)
{
	int temp = playerLightDamage;
	playerLightDamage = damage;
	return temp;
}

int Player::setHeavyDamage(int damage)
{
	int temp = playerHeavyDamage;
	playerHeavyDamage = damage;
	return temp;
}

int Player::addBomb()
{
	if (bombCount < 3)
		bombCount++;
	return bombCount;
}

int Player::subtractBomb()
{
	if (bombCount > 0)
		bombCount--;
	return bombCount;
}

bool Player::hasBomb()
{
	return !(0 == bombCount);
}

bool Player::killPlayer()
{	
	// update global to indicate player was hit
	g_playerHitThisStage = true;

    if(!isShielded)
    {
		g_audioMan->playOneShot(diedNoise, diedNoiseSize, 64);
        lifeCount--;
        if(hasShield)
            isShielded = true;
        return lifeCount != 0;
    }
    else
    {
        isShielded = false;
        shieldCounter = 600;
        return false;
    }
}

void Player::addLife()
{
	//if (lifeCount < MAX_LIVES)
		lifeCount++;
}

int Player::getLifeCount()
{
	return lifeCount;
}

void Player::upgradeLightFire()
{
	doubleLightFire = true;
}

void Player::upgradeHeavyFire()
{
	//fastChargingHeavyFire = true;
	heavyBulletFrequency = UPGRADED_HEAVY_BULLET_FREQUENCY;
	heavyBulletCounter = heavyBulletFrequency;
}

void Player::addShield()
{
	hasShield = true;
    isShielded = true;
}

void Player::reset()
{
	lifeCount = INITIAL_LIFE_COUNT;
	doubleLightFire = false;
	fastChargingHeavyFire = false;
	heavyBulletFrequency = DEFAULT_HEAVY_BULLET_FREQUENCY;
	heavyBulletCounter = heavyBulletFrequency;
	hasShield = false;
    isShielded = false;
    shieldCounter = 0;
    setInvincibility(false);
}
