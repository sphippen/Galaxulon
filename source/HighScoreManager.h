// HighScoreManager.h
//
// Authors: Aric Parkinson, Spencer Phippen
// Date created: April 13, 2012
//
// Contains declarations of high score-related functions

#ifndef HIGHSCOREMANAGER_H
#define HIGHSCOREMANAGER_H

// Returns the Nth high score (ie, 0 is highest, 1 is next highest, and so on).
// Fills playerName with the null-terminated 3-character initials of the player
// associated with that score. Expects playerName to have size 4.
//
// If scoreN > 9, program fails (high scores saved to top 10)
int getNthHighScore(int scoreN, char* playerName);

// Initializes SRAM if it hasn't been done already. Call before reading or
// writing high scores.
void initSRAM();

// Initializes SRAM with easy scores to beat - used for debugging purposes
void initSRAM_easy();

// Saves the Nth high score for future access. Expects playerName to be 3
// characters.
//
// If scoreN > 9, program fails (high scores saved up to top 10)
void saveNthHighScore(int score, const char* playerName, int scoreN);

// Takes the provided high score, and inserts it into place N. It assumes that
// the score given is actually supposed to go in that slot. Other players in
// the list are shifted down - excluding the score SPH -- 2 Points.
void newHighScore(int score, const char* playerName, int scoreN);

#endif
