// Sprite.h
//
// Authors: Aric Parkinson, Spencer Phippen
// Date created: March 30, 2012
//
// -------
// Contains interface for Sprite class

#ifndef SPRITE_H
#define SPRITE_H

struct OBJ_ATTR;
#include "HitBox.h"

// Maintains OBJ_ATTR state for a sprite object. In particular, manages:
//	-x and y coordinates on screen,
//	-whether the sprite is horizontally and/or verticaly flipped,
//	-whether the sprite is visible,
//	-the start-index for tiles comprising the sprite,
//	-the dimensions of the sprite,
//	-which sub-palette the sprite is using.
//
// Presumes 1D mapping global mapping.
// Uses 4BPP and regular (non-affine) graphics
class Sprite
{
public:
	enum SpriteSize
	{ size_8x8 = 0, size_16x16, size_32x32, size_64x64,
	  size_16x8, size_32x8, size_32x16, size_64x32,
	  size_8x16, size_8x32, size_16x32, size_32x64	};

	Sprite(SpriteSize _size = size_8x8, int _tileIndex = 0, bool _visible = true, int _palette = 0,
			int _x = 0, int _y = 0, bool _horizFlip = false, bool _vertFlip = false);
	
	void useImg(OBJ_ATTR* _obj_img);
	void unuseImg();

	void moveBy(int dx, int dy);
	void setPos(int newX, int newY);
	void setSize(SpriteSize newSize);
	void flipHoriz();
	void flipVert();
	void setTileIndex(int index);
	void setPalette(int newPalette);
	void setVisible(bool newVisible);

	bool isFlippedHoriz();
	bool isFlippedVert();
	int X();
	int Y();
	int getTileIndex();
	int getPalette();
	bool isVisible();
	int width();
	int height();
    HitBox hitBox;
	SpriteSize getSpriteSize();

private:
	int x;
	int y;
	bool horizFlip;
	bool vertFlip;
	SpriteSize size;
	int tileIndex;
	int palette;
	bool visible;

	OBJ_ATTR* obj_img;
};

#endif