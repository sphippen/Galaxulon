// OamManager.cpp
//
// Authors: Aric Parkinson, Spencer Phippen
// Date Created: March 23, 2012
//
// Contains implementation for OamManager class

#include "OamManager.h"
#include "assert2.h"

#define OAM_NUM_SPRITES 128

OamManager::OamManager() : topOfStack(0)
{
	// Clear shadowCopy
	oam_init(shadowCopy, OAM_NUM_SPRITES);
}

OBJ_ATTR* OamManager::allocSprites(int numSprites)
{
	// Check for bad input
	assert2(numSprites != 0, "cannot allocate 0 sprites");
	if (OAM_NUM_SPRITES - topOfStack < numSprites)
		return 0;

	topOfStack += numSprites;
	return shadowCopy + topOfStack - numSprites;
}

void OamManager::deallocSprites(OBJ_ATTR* toDealloc, int size)
{
	// Check for bad input
	assert2((shadowCopy + (topOfStack - size) == toDealloc), "Bad number of OBJ_ATTRs or OBJ_ATTR pointer");

	topOfStack -= size;
}

void OamManager::update()
{
	// We can't get away with only copying the allocated sprites, because deallocing might happen
	// between Vsyncs, in which case we miss an update to turn sprites invisible.
	oam_copy(oam_mem, shadowCopy, 128);
}