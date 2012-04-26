// Sprite.cpp
//
// Authors: Aric Parkinson, Spencer Phippen
// Date created: March 30, 2012
//
// -------
// Contains implementation for Sprite class

#include "tonc_types.h"
#include "tonc_oam.h"

#include "Sprite.h"
#include "assert2.h"

Sprite::Sprite(SpriteSize _size, int _tileIndex, bool _visible, int _palette,
				int _x, int _y, bool _horizFlip, bool _vertFlip)
				: x(_x), y(_y), horizFlip(_horizFlip), vertFlip(_vertFlip),
				size(_size), tileIndex(_tileIndex), palette(_palette), visible(_visible), obj_img(0)
{
	assert2(palette >= 0 && palette <= 15, "Palette index out of range");
	assert2(tileIndex >= 0 && tileIndex <= 1023, "Tile index out of range");

	hitBox.left = 0;
	hitBox.top = 0;

	switch (size)
	{
		case size_8x8:
			hitBox.right = 8;
			hitBox.bottom = 8;
			break;
		case size_8x16:
			hitBox.right = 8;
			hitBox.bottom = 16;
			break;
		case size_8x32:
			hitBox.right = 8;
			hitBox.bottom = 32;
			break;
		case size_16x8:
			hitBox.right = 16;
			hitBox.bottom = 8;
			break;
		case size_16x16:
			hitBox.right = 16;
			hitBox.bottom = 16;
			break;
		case size_16x32:
			hitBox.right = 16;
			hitBox.bottom = 32;
			break;
		case size_32x8:
			hitBox.right = 32;
			hitBox.bottom = 8;
			break;
		case size_32x16:
			hitBox.right = 32;
			hitBox.bottom = 16;
			break;
		case size_32x32:
			hitBox.right = 32;
			hitBox.bottom = 32;
			break;
		case size_32x64:
			hitBox.right = 32;
			hitBox.bottom = 64;
			break;
		case size_64x32:
			hitBox.right = 64;
			hitBox.bottom = 32;
			break;
		case size_64x64:
			hitBox.right = 64;
			hitBox.bottom = 64;
			break;
		default:
			assert2(false, "bad switch case");
	}
}

void Sprite::useImg(OBJ_ATTR* _obj_img)
{
	assert2(_obj_img != 0, "Null OBJ_ATTR passed");
	obj_img = _obj_img;

	// set values in the OBJ_ATTR
	obj_img->attr0 = (ATTR0_Y_MASK & y) 
				   | (visible ? ATTR0_REG : ATTR0_HIDE)
				   | (ATTR0_4BPP)
				   | ((size & 0xC) << (ATTR0_SHAPE_SHIFT - 2));	// the third and fourth bits of the enum value map directly
																// into ATTR0_SHAPE

	obj_img->attr1 = (ATTR1_X_MASK & x)
				   | (horizFlip ? ATTR1_HFLIP : 0)
				   | (vertFlip ? ATTR1_VFLIP : 0)
				   | ((size & 0x3) << ATTR1_SIZE_SHIFT);		// the first and second bits of the enum value map directly
																// into ATTR1_SIZE

	obj_img->attr2 = (ATTR2_ID(tileIndex))
				   | (ATTR2_PRIO(0))							// Priority currently not implemented
				   | (ATTR2_PALBANK(palette));

}

void Sprite::unuseImg()
{
	if (obj_img != 0)
	{
		obj_img->attr0 = (obj_img->attr0 & ~ATTR0_MODE_MASK) | ATTR0_HIDE;
	}
	obj_img = 0;
}

void Sprite::moveBy(int dx, int dy)
{
	assert2(dx != 1, "Dx was 1.");
	x += dx;
	y += dy;

	hitBox.moveBox(dx, dy); // Update the position of the hit box

	if (obj_img != 0)
	{
		obj_img->attr0 = (obj_img->attr0 & ~ATTR0_Y_MASK) | (ATTR0_Y_MASK & y);
		obj_img->attr1 = (obj_img->attr1 & ~ATTR1_X_MASK) | (ATTR1_X_MASK & x);
	}
}

void Sprite::setPos(int newX, int newY)
{
	int dy = newY - y;
	int dx = newX - x;

	y += dy;
	x += dx;

	hitBox.moveBox(dx, dy);

	if (obj_img != 0)
	{
		obj_img->attr0 = (obj_img->attr0 & ~ATTR0_Y_MASK) | (ATTR0_Y_MASK & y);
		obj_img->attr1 = (obj_img->attr1 & ~ATTR1_X_MASK) | (ATTR1_X_MASK & x);
	}
}

void Sprite::setSize(SpriteSize newSize)
{
	size = newSize;
	if (obj_img != 0)
	{
		// The third and fourth bits of size map directly to ATTR0_SHAPE
		obj_img->attr0 = (obj_img->attr0 & ~ATTR0_SHAPE_MASK) | ((size & 0xC) << (ATTR0_SHAPE_SHIFT - 2));
		// The first and second bits of size map directly to ATTR1_SIZE
		obj_img->attr1 = (obj_img->attr1 & ~ATTR1_SIZE_MASK) | ((size & 0x3) << ATTR1_SIZE_SHIFT);
	}
}

void Sprite::flipHoriz()
{
	horizFlip = !horizFlip;
	if (obj_img != 0)
	{
		obj_img->attr1 = (obj_img->attr1 & ~ATTR1_HFLIP) | (horizFlip ? ATTR1_HFLIP : 0);
	}
}

void Sprite::flipVert()
{
	vertFlip = !vertFlip;
	if (obj_img != 0)
	{
		obj_img->attr1 = (obj_img->attr1 & ~ATTR1_VFLIP) | (vertFlip ? ATTR1_VFLIP : 0);
	}
}

void Sprite::setTileIndex(int index)
{
	assert2(index >= 0 && index <= 1023, "Tile index out of range");
	tileIndex = index;
	if (obj_img != 0)
	{
		obj_img->attr2 = (obj_img->attr2 & ~ATTR2_ID_MASK) | (ATTR2_ID(tileIndex));
	}
}

void Sprite::setPalette(int newPalette)
{
	assert2(newPalette >= 0 && newPalette <= 15, "Palette index out of range");
	palette = newPalette;
	if (obj_img != 0)
	{
		obj_img->attr2 = (obj_img->attr2 & ~ATTR2_PALBANK_MASK) | (ATTR2_PALBANK(palette));
	}
}

void Sprite::setVisible(bool newVisible)
{
	visible = newVisible;
	if (obj_img != 0)
	{
		obj_img->attr0 = (obj_img->attr0 & ~ATTR0_MODE_MASK) | (visible ? ATTR0_REG : ATTR0_HIDE);
	}
}

bool Sprite::isFlippedHoriz()
{
	return horizFlip;
}

bool Sprite::isFlippedVert()
{
	return vertFlip;
}

int Sprite::X()
{
	return x;
}

int Sprite::Y()
{
	return y;
}

int Sprite::getTileIndex()
{
	return tileIndex;
}

int Sprite::getPalette()
{
	return palette;
}

bool Sprite::isVisible()
{
	return visible;
}

Sprite::SpriteSize Sprite::getSpriteSize()
{
	return size;
}

int Sprite::width()
{
	switch (size)
	{
		case size_8x8:
		case size_8x16:
		case size_8x32:
			return 8;
		case size_16x8:
		case size_16x16:
		case size_16x32:
			return 16;
		case size_32x8:
		case size_32x16:
		case size_32x32:
		case size_32x64:
			return 32;
		case size_64x32:
		case size_64x64:
			return 64;
		default:
			assert2(false, "bad switch case");
			return -1; // this line is never reached
	}
}

int Sprite::height()
{
	switch (size)
	{
		case size_8x8:
		case size_16x8:
		case size_32x8:
			return 8;
		case size_8x16:
		case size_16x16:
		case size_32x16:
			return 16;
		case size_8x32:
		case size_16x32:
		case size_32x32:
		case size_64x32:
			return 32;
		case size_32x64:
		case size_64x64:
			return 64;
		default:
			assert2(false, "bad switch case");
			return -1; // this line is never reached
	}
}
