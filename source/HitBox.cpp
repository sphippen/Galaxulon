// HitBox.cpp
//
// Authors: Daniel Setser
// Date created: April 4, 2012
//
// -------
// Implementation for the HitBox class

#include "HitBox.h"

HitBox::HitBox() : top(0), bottom(0), right(0), left(0) {}

HitBox::HitBox(int _top, int _bottom, int _right, int _left) : top(_top), bottom(_bottom), right(_right), left(_left)
{}

void HitBox::moveBox(int x, int y)
{
    top += y;
    bottom += y;
    left += x;
    right += x;
}


