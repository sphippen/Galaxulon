// HitBox.h
//
// Authors: Daniel Setser
// Date created: April 4, 2012
//
// -------
// Contains interface for HitBox class

#ifndef HITBOX_H
#define HITBOX_H

struct HitBox
{
public:
    HitBox();
    HitBox(int _top, int _bottom, int _right, int _left);
    void moveBox(int x, int y);
    bool checkCollision(const HitBox& box);

	int top;
    int bottom;
    int right;
    int left;
};

inline bool HitBox::checkCollision(const HitBox& box)
{
	// the value of bottom is _greater_ than the value of top, because positive y goes down the screen
	return (this->left < box.right) && (box.left < this->right) && (this->top < box.bottom) && (box.top < this->bottom);
}

#endif
