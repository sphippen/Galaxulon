// GameState.h
// 
// Authors: Aric Parkinson, Spencer Phippen
// Date created: March 23, 2012
//
// -------
// Defines standard interface for GameState objects.
//

#ifndef GAMESTATE_H
#define GAMESTATE_H

class GameStateManager;

// Interface for all GameState objects defined in this project.
// Not to be instantiated directly.
class GameState
{
public:
	GameState(GameStateManager* _gameStateMan);

	// Called immediately before this state is added to the
	// state stack for the first time.
	virtual void init();

	// Called on VBlank
	virtual void update() = 0;

	// Called when the state on top of this one is removed
	// from the stack.
	virtual void refreshed();

	// Called when this state is removed from the stack
	virtual void destroy();

    // Called when this state has something pushed onto it
    virtual void suspend();
protected:
	GameStateManager* gameStateMan;
};

#endif