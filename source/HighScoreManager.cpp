// HighScoreManager.cpp
//
// Authors: Aric Parkinson, Spencer Phippen
// Date created: April 13, 2012
//
// Contains implementations of high score-related functions

#include "tonc_memmap.h"

#include "HighScoreManager.h"
#include "assert2.h"

void setupDefaultHighScores();
void setupEasyHighScores();

int getNthHighScore(int scoreN, char* playerName)
{
	assert2(scoreN >= 0, "N must be non-negative");
	assert2(scoreN < 10, "N must be less than 10");

	// calculate offset
	// -4 byte header
	// -7 bytes per entry
	// -Entries stored sequentially
	u8* toRead = sram_mem + (4 + 7 * scoreN);

	// Read chars
	if(playerName != 0)
	{
		playerName[0] = *toRead++;
		playerName[1] = *toRead++;
		playerName[2] = *toRead++;
		playerName[3] = '\0';
	}
	else
	{
		toRead += 3;
	}

	// Read score
	int score = 0;
	u8* scoreAsChars = (u8*)&score;
	scoreAsChars[0] = *toRead++;
	scoreAsChars[1] = *toRead++;
	scoreAsChars[2] = *toRead++;
	scoreAsChars[3] = *toRead++;

	return score;
}

void initSRAM()
{
	u8* SRAM = sram_mem;

	u32 header;
	u8* headerAsChar = (u8*)&header;
	*headerAsChar++ = *SRAM++;
	*headerAsChar++ = *SRAM++;
	*headerAsChar++ = *SRAM++;
	*headerAsChar++ = *SRAM++;

	// If the header was not our magic number:
	if (header != 0x12345678)
	{
		setupDefaultHighScores();
	}
}

void initSRAM_easy()
{
	u8* SRAM = sram_mem;

	u32 header;
	u8* headerAsChar = (u8*)&header;
	*headerAsChar++ = *SRAM++;
	*headerAsChar++ = *SRAM++;
	*headerAsChar++ = *SRAM++;
	*headerAsChar++ = *SRAM++;

	// If the header was not our magic number:
	if (header != 0x87654321)
	{
		setupEasyHighScores();
	}
}

void saveNthHighScore(int score, const char* playerName, int scoreN)
{
	assert2(scoreN >= 0, "N must be non-negative");
	assert2(scoreN < 10, "N must be less than 10");

	// calculate offset
	// -4 byte header
	// -7 bytes per entry
	// -Entries stored sequentially
	u8* toWrite = sram_mem + (4 + 7 * scoreN);

	// write chars
	*toWrite++ = playerName[0];
	*toWrite++ = playerName[1];
	*toWrite++ = playerName[2];

	// write score
	u8* intAsChars = (u8*)&score;
	
	*toWrite++ = intAsChars[0];
	*toWrite++ = intAsChars[1];
	*toWrite++ = intAsChars[2];
	*toWrite++ = intAsChars[3];
}

void setupDefaultHighScores()
{
	u8* SRAM = sram_mem;

	// Set header - little endian
	*SRAM++ = 0x78;
	*SRAM++ = 0x56;
	*SRAM++ = 0x34;
	*SRAM++ = 0x12;

	// Top scores:
	//   - Score 0: ALT 1,000,000 PTS
	//   - Score 1: SPH 800,000 PTS
	//   - Score 2: DJS 500,000 PTS
	//   - Score 3: DES 250,000 PTS
	//   - Score 4: BUD 200,000 PTS
	//   - Score 5: DRK 190,000 PTS
	//   - Score 6: GUN 180,000 PTS
	//   - Score 7: MAN 175,000 PTS
	//   - Score 8: DUD 1,000 PTS
	//   - Score 9: SPH 2 PTS
	static const int scores[] = { 1000000, 800000, 500000, 250000, 200000, 190000, 180000, 175000, 1000, 2 };
	static const char* const names[] = { "ALT", "SPH", "DJS", "DES", "BUD", "DRK", "GUN", "MAN", "DUD", "SPH" };

	for(int i = 0; i < 10; ++i)
	{
		saveNthHighScore(scores[i], names[i], i);
	}
}

void setupEasyHighScores()
{
	u8* SRAM = sram_mem;

	// Set header - little endian
	*SRAM++ = 0x21;
	*SRAM++ = 0x43;
	*SRAM++ = 0x65;
	*SRAM++ = 0x87;

	// Top scores:
	//   - Score 0: ALT 1,000 PTS
	//   - Score 1: SPH 500 PTS
	//   - Score 2: DJS 450 PTS
	//   - Score 3: DES 350 PTS
	//   - Score 4: BUD 200 PTS
	//   - Score 5: DRK 150 PTS
	//   - Score 6: GUN 100 PTS
	//   - Score 7: MAN 100 PTS
	//   - Score 8: DUD 75 PTS
	//   - Score 9: SPH 2 PTS
	static const int scores[] = { 1000, 500, 450, 350, 200, 150, 100, 100, 75, 2 };
	static const char* const names[] = { "ALT", "SPH", "DJS", "DES", "BUD", "DRK", "GUN", "MAN", "DUD", "SPH" };

	for(int i = 0; i < 10; ++i)
	{
		saveNthHighScore(scores[i], names[i], i);
	}
}

void newHighScore(int score, const char* playerName, int scoreN)
{
	assert2(scoreN >= 0, "N must be non-negative");
	assert2(scoreN < 9, "N must be less than 9 for newHighScores");

	// calculate offsets
	// -4 byte header
	// -7 bytes per entry
	// -Entries stored sequentially
	// -We want to copy backwards, to avoid losing information
	// -Start copying 7th place to 8th, because we don't want to overwrite 9th (SPH -- 2 Points)
	u8* SRAM_read = sram_mem + (4 + 7 * 7) + 6;
	u8* SRAM_write = sram_mem + (4 + 7 * 8) + 6;
	
	// Perform byte copy
	for (int i = scoreN * 7; i < 8 * 7; ++i)
	{
		*SRAM_write-- = *SRAM_read--;
	}

	// Save the new Nth high score
	saveNthHighScore(score, playerName, scoreN);
}
