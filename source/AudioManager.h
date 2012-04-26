// AudioManager.h
//
// Authors: Spencer Phippen
// Date created: April 15, 2012
//
// Contains class declaration for AudioManager, the class responsible
// for managing low-level details of audio output.

#ifndef AUDIO_MANAGER_H
#define AUDIO_MANAGER_H

#include "tonc_types.h"

class AudioManager
{
public:
	// Intializes the audio manager.
	// NOTE: the AudioManager assumes control of timer 0,
	// as well as DMA 1. You cannot use them.
	AudioManager();

	// method to perform mixing as necessary
	// manually call every vsync
	void sndMix();

	// play a sample located at "data" with the specified length
	// (# of samples) and volume
	// if no channels are free, sample is not played
	// length cannot be larger than 1024 * 1024
	// vol ranges from 0 to 64
	void playOneShot(const s8* data, int length, int vol = 64);

	// VBlank interrupt method to update buffers
	// DO NOT CALL MANUALLY
	static void sndVBlank();

private:
	// the base pointer for the two buffers
	s8* mixBufferBase;
	// the pointer to the buffer currently being mixed,
	// i.e. NOT playing
	s8* curMixBuffer;
	// the size of each buffer
	u32 mixBufferSize;
	// the currently playing buffer = 0 or 1
	u8 activeBuffer;

	// Disallow copying/assignment
	AudioManager(const AudioManager&);
	AudioManager& operator=(const AudioManager&);
};

#endif