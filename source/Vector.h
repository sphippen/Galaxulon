// Vector.h
// 
// Authors: Daniel Setser
// Date created: April 5, 2012
//
// -------
// Contains interface for Vec2 class, a 2-component vector.
//

#ifndef VECTOR_H
#define VECTOR_H

struct Vec2
{
    Vec2(int _x = 0, int _y = 0);
    int x, y;
};

inline Vec2::Vec2(int _x, int _y) : x(_x), y(_y) { }

#endif
