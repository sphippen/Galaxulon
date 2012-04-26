// AudioManager.cpp
//
// Authors: Spencer Phippen
// Date created: April 15, 2012
//
// Contains implementation of AudioManager.

#include "AudioManager.h"

#include "tonc_core.h"
#include "tonc_irq.h"

#include "assert2.h"
#include "globals.h"
#include "sfx.h"

#define SND_BUF_SIZE 304
#define CHAN_NUM_POW2 2

// do not modify this define
// modify CHAN_NUM_POW2 instead
#define CHAN_NUM (1 << CHAN_NUM_POW2)

// two back-to-back 304-sample buffers, used
// for double buffering
static s8 soundBuffers[SND_BUF_SIZE * 2];

// intermediate buffer used for mixing
static s16 tmpMixBuffer[SND_BUF_SIZE];

typedef struct
{
	const s8* data;
	u32 pos; // 20.12 fixed point - current position in the sound
	u32 inc; // 20.12 fixed point - how far to move forward for each 18157 Hz sample
	u32 vol; // 0 to 64 - 0 is lowest volume, 64 is highest
	u32 length; // 20.12 fixed point - total length of sample
	u32 loopLength; // 20.12 fixed point - length of loop to be used
} SOUND_CHANNEL;

SOUND_CHANNEL channels[CHAN_NUM];

AudioManager::AudioManager() : mixBufferBase(soundBuffers), curMixBuffer(soundBuffers),
	mixBufferSize(SND_BUF_SIZE), activeBuffer(1)
{
	// clear sound buffers
	for (int i = 0; i < (SND_BUF_SIZE * 2); ++i)
	{
		soundBuffers[i] = 0;
	}

	// clear all channels
	for (int i = 0; i < CHAN_NUM; ++i)
	{
		channels[i].data = 0;
	}

	// set up sound control registers
	REG_SNDSTAT = SSTAT_ENABLE;

	// enable direct sound channel @ 100% volume, in both L/R channels,
	// and use timer 0 for
	REG_SNDDSCNT = SDS_DMG100 | SDS_A100 | SDS_AR | SDS_AL | SDS_ATMR0;

	// set up timer 1 for sound
	// this is 65536 - (CPU clock / sample freq)
	// sample freq = 18157 Hz
	REG_TM0D = 64612;

	// set timer increment to once every 1 cycle,
	// and enable the timer
	REG_TM0CNT = TM_FREQ_1 | TM_ENABLE;

	// set up dma 1 for sound
	// copy FROM our buffer
	REG_DMA1SAD = (u32)(&(soundBuffers[SND_BUF_SIZE]));
	// copy INTO directsound a's FIFO
	REG_DMA1DAD = (u32)(&(REG_FIFO_A));
	// in order:
	// fixed dest (a's FIFO), increment source (move through the buffer), 
	// repeat copies, one word at a time, use sound mode, DON'T enable DMA -
	// the VBlank interrupt will do that
	REG_DMA1CNT = DMA_DST_FIXED | DMA_SRC_INC | DMA_REPEAT | DMA_32 | DMA_AT_REFRESH;

	// setup VSync interrupts
	// call sndVSync on the VBlank interrupt
	REG_DISPSTAT = DSTAT_VBL_IRQ;
	irq_set(II_VBLANK, AudioManager::sndVBlank, ISR_DEF);
}

void AudioManager::sndMix()
{
	// first, blank the mix buffer in case nothing
	// gets copied in this frame
	for (u32 i = 0; i < mixBufferSize; ++i)
	{
		tmpMixBuffer[i] = 0;
	}

	// now, mix every channel
	for (u32 curChan = 0; curChan < CHAN_NUM; ++curChan)
	{
		SOUND_CHANNEL* chanPtr = &channels[curChan];
		if (chanPtr->data != 0) // the channel is active
		{
			// the below if statement is here solely for optimization
			// we only need to include the code to check for running off the edge
			// of a sample if that is even possible in this frame.
			// so, if this test fails, we can use the optimized form (see the "else" statement)
			// savings: a few hundred samples per channel in most cases
			if (chanPtr->pos + mixBufferSize * chanPtr->inc >= chanPtr->length)
			{
				for (u32 i = 0; i < mixBufferSize; ++i)
				{
					tmpMixBuffer[i] += (chanPtr->data[chanPtr->pos >> 12]) * chanPtr->vol;
					chanPtr->pos += chanPtr->inc;

					if (chanPtr->pos >= chanPtr->length)
					{
						// we ran past the end of the sample
						if (chanPtr->loopLength == 0) // there is no loop
						{
							// stop playing the sample
							chanPtr->data = 0;
							break;
						}
						else // there is a loop
						{
							// move the pos back to before the end
							while (chanPtr->pos >= chanPtr->length)
							{
								chanPtr->pos -= chanPtr->loopLength;
							}
						}
					} // end sample runover
				} // end loop over samples
			}
			else
			{
				for (u32 i = 0; i < mixBufferSize; ++i)
				{
					tmpMixBuffer[i] += (chanPtr->data[chanPtr->pos >> 12]) * chanPtr->vol;
					chanPtr->pos += chanPtr->inc;
				} // end loop over samples
			}
		}
	} // end loop over channels

	// copy tmp buffer into finalized mix buffer
	for (u32 i = 0; i < mixBufferSize; ++i)
	{
		// 6 to accomodate for the volume, and CHAN_NUM_POW2 to prevent overflow
		// with multiple channels
		curMixBuffer[i] = tmpMixBuffer[i] >> (6 + CHAN_NUM_POW2);
	}
}

void AudioManager::sndVBlank()
{
	if (g_audioMan->activeBuffer == 1)
	{
		// the second buffer is playing, swap back to the first one

		// same DMA settings as constructor, but enable it this time
		REG_DMA1CNT = 0;
		REG_DMA1SAD = (u32)(&(soundBuffers[0]));
		REG_DMA1CNT = DMA_DST_FIXED | DMA_SRC_INC | DMA_REPEAT | DMA_32 | DMA_AT_REFRESH | DMA_ENABLE;

		g_audioMan->activeBuffer = 0;
		// the buffer to mix is now the second buffer
		g_audioMan->curMixBuffer = g_audioMan->mixBufferBase + g_audioMan->mixBufferSize;
	}
	else
	{
		// the first buffer is playing, it'll roll over into the next one just fine
		g_audioMan->activeBuffer = 1;
		g_audioMan->curMixBuffer = g_audioMan->mixBufferBase;
	}
}

void AudioManager::playOneShot(const s8* data, int length, int vol)
{
	SOUND_CHANNEL* chanPtr = channels;
	int curChan = 0;
	while (curChan < CHAN_NUM && chanPtr->data != 0)
	{
		chanPtr++;
		curChan++;
	}

	if (curChan == CHAN_NUM) // all channels are being used
	{
		return;
	}
	else // this channel is free
	{
		chanPtr->data = data;
		chanPtr->inc = 1 << 12;
		chanPtr->length = length << 12;
		chanPtr->loopLength = 0;
		chanPtr->pos = 0;
		chanPtr->vol = vol;
	}
}