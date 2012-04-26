// OamManager.h
// 
// Authors: Aric Parkinson, Spencer Phippen
// Date created: March 23, 2012
//
// -------
// Contains interface for OamManager
//

#ifndef OAMMANAGER_H
#define OAMMANAGER_H

#include "tonc_oam.h"

// Used to interact with OAM: inserting sprites, etc.
// GameStates can "allocate" OBJ_ATTRs in OAM shadow memory
// through the OamManager.
class OamManager
{
public:
	OamManager();

	// Allocates numSprites sprites to the requestor. Returns an array of OBJ_ATTRs pointing
	// to those sprites. NULL if not enough sprites remain in memory, or if zero sprites are
	// requested.
	OBJ_ATTR* allocSprites(int numSprites);

	// Deallocates the sprites given to the user. Requires the number of sprites that are to
	// deallocated. Expected to be deallocated in order that sprites were originally allocated.
	// Undefined behavior otherwise.
	void deallocSprites(OBJ_ATTR* toDealloc, int size);

	// Updates OAM with current contents of shadowCopy. SHOULD ONLY BE CALLED DURING VBLANK.
	void update();
	
private:
	// Copy of OAM used to update things before Vblank
	OBJ_ATTR shadowCopy[128];
	
	// Keeps track of first non-allocated sprite slot in shadowCopy/OAM
	int topOfStack;
};

#endif