// TextManager.h
// 
// Authors: Aric Parkinson, Spencer Phippen
// Date created: April 13, 2012
//
// -------
// Contains interface declaration for text-related functions.
//

#ifndef TEXTMANAGER_H
#define TEXTMANAGER_H

#include "Vector.h"

// Writes the specified string to the given position on screen, in terms of tile-coordinates
//	ie, writing "Abc" to position (0, 1) draws "Abc" on screen starting flush-left, 1 tile down
//
//	NOTE: undefined behavior for escape characters.
void write(const char* string, Vec2 tilePos);

// The same as write, but does it on the bottom text background instead.
void write2(const char* string, Vec2 tilePos);

// Wipes all text from the top text screen
void clearText();

// Wipes all text from the bottom text screen
void clearText2();

// Converts an integer to a char*, storing in the provided memory location
// DOES NOT WORK FOR NEGATIVE NUMBERS
void gba_itoa(int value, char* str);

#endif
