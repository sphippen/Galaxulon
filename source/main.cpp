// main.cpp
// 
// Authors: Aric Parkinson, Spencer Phippen
// Date created: March 23, 2012
//
// -------
// Initializes system, enters game loop
//

#include "tonc_video.h"
#include "tonc_memmap.h"
#include "tonc_core.h"
#include "tonc_oam.h"
#include "tonc_input.h"
#include "tonc_irq.h"
#include "tonc_bios.h"

#include "assert2.h"
#include "globals.h"
#include "AudioManager.h"
#include "GameStateManager.h"
#include "OamManager.h"
#include "PlayState.h"
#include "TitleScreenState.h"
#include "PauseState.h"
#include "GameOverState.h"
#include "StoreState.h"
#include "StageEndState.h"
#include "TextManager.h"

void videoInit();
void fillEventsStage1(StageEvent * events[], int * timer, int * yOffset);
void fillEventsStage2(StageEvent * events[], int * timer, int * yOffset);
void fillEventsStage3(StageEvent * events[], int * timer, int * yOffset);
void initializeEvents();
	
int main()
{
	// initialize IRQ (interrupts)
	// this must come before everything else
	IRQ_INIT();

	// Initialize global pointers
	GameStateManager gameStateMan;
	OamManager oamMan;
	AudioManager audioMan;
	PlayState playState(&gameStateMan);
	TitleScreenState titleState(&gameStateMan);
	PauseState pauseState(&gameStateMan);
	GameOverState gameOverState(&gameStateMan);
	StoreState storeState(&gameStateMan);
	StageEndState stageEndState(&gameStateMan);
	
	g_gameStateMan = &gameStateMan;
	g_oamMan = &oamMan;
	g_playState = &playState;
	g_titleState = &titleState;
	g_pauseState = &pauseState;
	g_gameOverState = &gameOverState;
	g_storeState = &storeState;
	g_stageEndState = &stageEndState;
	g_audioMan = &audioMan;

	// create stage events
	StageEvent endEvent;
	StageEvent event1;
	StageEvent event2;
	StageEvent event3;
	StageEvent event4;
	StageEvent event5;
	StageEvent event6;
	StageEvent firePowerPowerUpEvent;
	StageEvent invinciblePowerUpEvent;
	StageEvent bombPowerUpEvent;
	g_endEvent = &endEvent;
	g_event1 = &event1;
	g_event2 = &event2;
	g_event3 = &event3;
	g_event4 = &event4;
	g_event5 = &event5;
	g_event6 = &event6;
	g_firePowerPowerUpEvent = &firePowerPowerUpEvent;
	g_invinciblePowerUpEvent = &invinciblePowerUpEvent;
	g_bombPowerUpEvent = &bombPowerUpEvent;

	initializeEvents();

	StageEvent * stage1Events[24];
	int stage1Timing[24];
	int stage1yOffset[24];
	fillEventsStage1(stage1Events, stage1Timing, stage1yOffset);
	Stage stage1(&playState, stage1Events, stage1Timing, stage1yOffset, 24);
	g_stage1 = &stage1;

	StageEvent * stage2Events[20];
	int stage2Timing[20];
	int stage2yOffset[20];
	fillEventsStage2(stage2Events, stage2Timing, stage2yOffset);
	Stage stage2(&playState, stage2Events, stage2Timing, stage2yOffset, 20);
	g_stage2 = &stage2;
	
	StageEvent * stage3Events[20];
	int stage3Timing[20];
	int stage3yOffset[20];
	fillEventsStage3(stage3Events, stage3Timing, stage3yOffset);
	Stage stage3(&playState, stage3Events, stage3Timing, stage3yOffset, 20);
	g_stage3 = &stage3;

	videoInit();

	g_gameStateMan->pushState(g_titleState);

#ifdef DEBUG
	// timers used for debug display
	REG_TM1D = 0x10000 - 2808; // overflow into timer 2 every 2808 cycles, approx. 1% of a screen refresh
	REG_TM2D = 0;
	REG_TM2CNT = TM_CASCADE | TM_ENABLE;
	REG_TM1CNT = TM_FREQ_1 | TM_ENABLE;
	int oldPercent, diffPercent, oldFrac, diffFrac;
	char buf[15];
#endif // DEBUG

	while(true)
	{
		// wait until next VBlank
		// prefer this over vid_vsync() - it's
		// better for power consumption
		VBlankIntrWait();

#ifdef DEBUG
		// grab current percentage
		oldPercent = REG_TM2D;
		oldFrac = REG_TM1D;
#endif // DEBUG

		// update shadow OAM to real OAM
		g_oamMan->update();

		// mix the next frame's audio
		g_audioMan->sndMix();
		
		// poll keys - do not do this in other places
		key_poll();

		// update the game state
		g_gameStateMan->update();

#ifdef DEBUG
		// grab current percentage, and write it out
		diffPercent = REG_TM2D - oldPercent;
		diffFrac = REG_TM1D - oldFrac;

		// round the percent based on the fractional amount
		if (diffFrac > 1404)
		{
			diffPercent++;
		}
		else if (diffFrac < -1404)
		{
			diffPercent--;
		}

		gba_itoa(diffPercent, buf);

		// clear out characters from the last write
		write("  ", Vec2(0, 0));
		write(buf, Vec2(0, 0));

		// reset timer 2 to 0
		REG_TM2CNT = 0; // disable timer
		REG_TM2D = 0; // set new value to 0
		REG_TM2CNT = TM_CASCADE | TM_ENABLE; // reenable timer
#endif // DEBUG
	}
}

void videoInit()
{
	// Blank screen for faster loading
	REG_DISPCNT = DCNT_BLANK;

	// Completely clear VRAM
	memset16(vid_mem, 0x0000, 49152);

	// BACKGROUND SETUP //
	// Set background 0 control register
	//	-Render this BG on bottom
	//  -Tile indexing starts in charblock 0
	//  -Screen-entry indexing starts in charblock 3 / screenblock 24
	//  -4 bits per pixel
	//  -512 x 512 pixels - 32 x 32 tiles - 1 x 1 screenblocks
	REG_BG0CNT = BG_PRIO(3) | BG_CBB(0) | BG_SBB(24) | BG_4BPP | BG_REG_32x32;
	
	// Set background 2 control register - Bottom text screen
	//	-Render this BG on top
	//  -Tile indexing starts in charblock 0
	//  -Screen-entry indexing starts in charblock 3 / screenblock 29
	//  -4 bits per pixel
	//  -256 x 256 pixels - 32 x 32 tiles - 1 x 1 screenblocks
	REG_BG2CNT = BG_PRIO(1) | BG_CBB(0) | BG_SBB(29) | BG_4BPP | BG_REG_32x32;

	// Set background 3 control register - Top Text Screen
	//	-Render this BG on top
	//  -Tile indexing starts in charblock 0
	//  -Screen-entry indexing starts in charblock 3 / screenblock 28
	//  -4 bits per pixel
	//  -256 x 256 pixels - 32 x 32 tiles - 1 x 1 screenblocks
	REG_BG3CNT = BG_PRIO(0) | BG_CBB(0) | BG_SBB(28) | BG_4BPP | BG_REG_32x32;

	// Load background palette
	memcpy16(&pal_bg_mem[0], g_bgPal, g_bgPalLen >> 1);

	// Load background tiles

	// Load Font tiles
	memcpy16(&tile_mem[0][g_galaxulonFont_TilesPos], g_galaxulonFont_Tiles, g_galaxulonFont_TilesLen >> 1);

	// Load other BG tiles
	memcpy16(&tile_mem[0][g_blankBG_TilesPos], g_blankBG_Tiles, g_blankBG_TilesLen >> 1);
	memcpy16(&tile_mem[0][g_assertTile_TilesPos], g_assertTile_Tiles, g_assertTile_TilesLen >> 1);
	memcpy16(&tile_mem[0][g_demo_starsTilesPos], g_demo_starsTiles, g_demo_starsTilesLen >> 1);

	// Clear Background screenmaps - set the four background screenblocks
	memset32(&se_mem[24], 0x00000000, (SBB_SIZE/4));
	
	// OAM SETUP //
	// Load OAM palette
	memcpy16(pal_obj_mem, g_spritePal, g_spritePalLen >> 1);

	// Load OAM tiles
	// Init Player sprite
	memcpy16(&tile_mem_obj[0][g_playerSpriteTilesPos], g_playerSpriteTiles, g_playerSpriteTilesLen >> 1);

	// Init bullet sprites
	memcpy16(&tile_mem_obj[0][g_bulletSpriteTilesPos], g_bulletSpriteTiles, g_bulletSpriteTilesLen >> 1);
	memcpy16(&tile_mem_obj[0][g_missileSpriteTilesPos], g_missileSpriteTiles, g_missileSpriteTilesLen >> 1);

	// Init display sprites
	memcpy16(&tile_mem_obj[0][g_bullet_selection1TilesPos], g_bullet_selection1Tiles, g_bullet_selection1TilesLen >> 1);
	memcpy16(&tile_mem_obj[0][g_bullet_selection2TilesPos], g_bullet_selection2Tiles, g_bullet_selection2TilesLen >> 1);

	// Init enemy sprites
	memcpy16(&tile_mem_obj[0][g_jellyfishTilesPos], g_jellyfishTiles, g_jellyfishTilesLen >> 1);
	memcpy16(&tile_mem_obj[0][g_mineTilesPos], g_mineTiles, g_mineTilesLen >> 1);
	memcpy16(&tile_mem_obj[0][g_sharkTilesPos], g_sharkTiles, g_sharkTilesLen >> 1);
	memcpy16(&tile_mem_obj[0][g_squidTilesPos], g_squidTiles, g_squidTilesLen >> 1);
	memcpy16(&tile_mem_obj[0][g_crabTilesPos], g_crabTiles, g_crabTilesLen >> 1);

	// Init upgrade sprites
	memcpy16(&tile_mem_obj[0][g_extraLife_TilesPos], g_extraLife_Tiles, g_extraLife_TilesLen >> 1);
	memcpy16(&tile_mem_obj[0][g_extraBomb_TilesPos], g_extraBomb_Tiles, g_extraBomb_TilesLen >> 1);
	memcpy16(&tile_mem_obj[0][g_primaryFireUp_TilesPos], g_primaryFireUp_Tiles, g_primaryFireUp_TilesLen >> 1);
	memcpy16(&tile_mem_obj[0][g_secondFireUp_TilesPos], g_secondFireUp_Tiles, g_secondFireUp_TilesLen >> 1);
	memcpy16(&tile_mem_obj[0][g_shield_TilesPos], g_shield_Tiles, g_shield_TilesLen >> 1);

	// Load Title screen logo
	memcpy16(&tile_mem_obj[0][g_galaxulon_titleTilesPos1], g_galaxulon_titleTiles, g_galaxulon_titleTilesLen >> 1);

	// Clear OAM
	oam_init(oam_mem, 128);

	// Setup Display Control Register
	//	-Video mode 0 - tiled
	//	-BG 0, BG 2, and BG 3 enabled
	//	-Enable sprites
	//	-1D sprite-mapping mode
	REG_DISPCNT = DCNT_MODE0 | DCNT_OBJ_1D | DCNT_BG0 | DCNT_BG2 | DCNT_BG3 | DCNT_OBJ;
}

void fillEventsStage1(StageEvent * events[], int * timing, int * yOffset)
{
	events[0] =  g_event1;    yOffset[0] =  14;     timing[0] =  250;
	events[1] =  g_event1;    yOffset[1] =  22;   timing[1] = 250;
	events[2] =  g_event3;    yOffset[2] =  40;    timing[2] =  250;
	events[3] =  g_event1;	  yOffset[3] =  30;    timing[3] =  250;
	events[4] =  g_event2;	  yOffset[4] =  9;    timing[4] =  250;
	events[5] =  g_event2;	  yOffset[5] =  60;    timing[5] =  250;
	events[6] =  g_event1;	  yOffset[6] =  18;    timing[6] =  250;
	events[7] =  g_event1;	  yOffset[7] =  7;    timing[7] =  250;
	events[8] =  g_event6;	  yOffset[8] =  0;    timing[8] =  250;
	events[9] =  g_event2;    yOffset[9] =  9;    timing[9] =  250;
	events[10] = g_event3;	  yOffset[10] = 45;    timing[10] = 250;
	events[11] = g_firePowerPowerUpEvent;	  yOffset[11] = 40;    timing[11] = 250;
	events[12] = g_event3;	  yOffset[12] = 44;    timing[12] = 250;
	events[13] = g_event1;	  yOffset[13] = 7;    timing[13] = 250;
	events[14] = g_event1;	  yOffset[14] = 30;    timing[14] = 300;
	events[15] = g_event3;	  yOffset[15] = 44;    timing[15] = 250;
	events[16] = g_event6;	  yOffset[16] = 0;    timing[16] = 250;
	events[17] = g_invinciblePowerUpEvent;	  yOffset[17] = 9;    timing[17] = 250;
	events[18] = g_event3;    yOffset[18] = 9;    timing[18] = 250;
	events[19] = g_event3;    yOffset[19] = 44;    timing[19] = 250;
	events[20] = g_event1;    yOffset[20] = 0;    timing[20] = 250;
	events[21] = g_event1;    yOffset[21] = 0;    timing[21] = 250;
	events[22] = g_event6;    yOffset[22] = 0;    timing[22] = 250;
	events[23] = g_endEvent;  yOffset[23] = 0;    timing[23] = 400;
}

void fillEventsStage2(StageEvent * events[], int * timing, int * yOffset)
{
	events[0] =  g_event4;    yOffset[0] =  50;     timing[0] =  250;
	events[1] =  g_event1;    yOffset[1] =  22;   timing[1] = 250;
	events[2] =  g_event5;    yOffset[2] =  107;    timing[2] =  250;
	events[3] =  g_event5;	  yOffset[3] =  43;    timing[3] =  150;
	events[4] =  g_event5;	  yOffset[4] =  77;    timing[4] =  150;
	events[5] =  g_bombPowerUpEvent;	  yOffset[5] =  60;    timing[5] =  250;
	events[6] =  g_event1;	  yOffset[6] =  18;    timing[6] =  250;
	events[7] =  g_event1;	  yOffset[7] =  7;    timing[7] =  250;
	events[8] =  g_event1;	  yOffset[8] =  30;    timing[8] =  250;
	events[9] =  g_event2;    yOffset[9] =  9;    timing[9] =  250;
	events[10] = g_event6;	  yOffset[10] = 0;    timing[10] = 250;
	events[11] = g_event2;	  yOffset[11] = 9;    timing[11] = 250;
	events[12] = g_event3;	  yOffset[12] = 44;    timing[12] = 250;
	events[13] = g_firePowerPowerUpEvent;	  yOffset[13] = 10;    timing[13] = 250;
	events[14] = g_event1;	  yOffset[14] = 30;    timing[14] = 250;
	events[15] = g_event3;	  yOffset[15] = 44;    timing[15] = 250;
	events[16] = g_event1;	  yOffset[16] = 18;    timing[16] = 250;
	events[17] = g_event2;	  yOffset[17] = 9;    timing[17] = 250;
	events[18] = g_event6;    yOffset[18] = 9;    timing[18] = 250;
	events[19] = g_endEvent;  yOffset[19] = 0;    timing[19] = 400;
}

void fillEventsStage3(StageEvent * events[], int * timing, int * yOffset)
{
	events[0] =  g_event6;    yOffset[0] =  0;    timing[0] =  250;
	events[1] =  g_event4;    yOffset[1] =  33;    timing[1] =  250;
	events[2] =  g_event3;    yOffset[2] =  44;    timing[2] =  250;
	events[3] =  g_event3;	  yOffset[3] =  50;    timing[3] =  250;
	events[4] =  g_event3;	  yOffset[4] =  38;    timing[4] =  250;
	events[5] =  g_invinciblePowerUpEvent;	  yOffset[5] =  38;    timing[5] =  250;
	events[6] =  g_event6;	  yOffset[6] =  0;    timing[6] =  250;
	events[7] =  g_event1;	  yOffset[7] =  10;    timing[7] =  250;
	events[8] =  g_event2;	  yOffset[8] =  55;    timing[8] =  250;
	events[9] =  g_event2;	  yOffset[9] =  65;    timing[9] =  0;
	events[10] = g_event4;	  yOffset[10] = 50;    timing[10] = 250;
	events[11] = g_event5;	  yOffset[11] = 75;    timing[11] = 250;
	events[12] = g_invinciblePowerUpEvent;	  yOffset[12] = 40;    timing[12] = 250;
	events[13] = g_event5;	  yOffset[13] = 107;    timing[13] = 250;
	events[14] = g_event5;	  yOffset[14] = 75;    timing[14] = 250;
	events[15] = g_event5;	  yOffset[15] = 43;    timing[15] = 250;
	events[16] = g_event6;	  yOffset[16] = 0;    timing[16] = 250;
	events[17] = g_event1;	  yOffset[17] = 30;    timing[17] = 250;
	events[18] = g_event6;    yOffset[18] = 0;    timing[18] = 250;
	events[19] = g_endEvent;  yOffset[19] = 0;    timing[19] = 400;
}

void initializeEvents()
{
	g_endEvent->endEvent = true;

	g_event1->enemyType[0] = StageEvent::jellyfish;
	g_event1->xSpawnPos[0] = 250;
	g_event1->ySpawnPos[0] = 0;
	g_event1->enemyType[1] = StageEvent::jellyfish;
	g_event1->xSpawnPos[1] = 250;
	g_event1->ySpawnPos[1] = 50;
	g_event1->enemyType[2] = StageEvent::jellyfish;
	g_event1->xSpawnPos[2] = 250;
	g_event1->ySpawnPos[2] = 90;
	g_event1->enemyCount = 3;
	g_event1->endEvent = false;
	g_event1->powerUp = StageEvent::none;
	//Offset range: 7 - 30

	g_event2->enemyType[0] = StageEvent::mine;
	g_event2->xSpawnPos[0] = 250;
	g_event2->ySpawnPos[0] = 0;
	g_event2->enemyType[1] = StageEvent::mine;
	g_event2->xSpawnPos[1] = 250;
	g_event2->ySpawnPos[1] = 33;
	g_event2->enemyCount = 2;
	g_event2->endEvent = false;
	g_event2->powerUp = StageEvent::none;
	//Offset range: 9 - 110

	g_event3->enemyType[0] = StageEvent::shark;
	g_event3->xSpawnPos[0] = 280;
	g_event3->ySpawnPos[0] = 0;
	g_event3->enemyType[1] = StageEvent::shark;
	g_event3->xSpawnPos[1] = 250;
	g_event3->ySpawnPos[1] = 40;
	g_event3->enemyType[2] = StageEvent::shark;
	g_event3->xSpawnPos[2] = 280;
	g_event3->ySpawnPos[2] = 80;
	g_event3->enemyCount = 3;
	g_event3->endEvent = false;
	g_event3->powerUp = StageEvent::none;
	//Offset range: 35 - 53
	
	g_event4->enemyType[0] = StageEvent::crab;
	g_event4->xSpawnPos[0] = 250;
	g_event4->ySpawnPos[0] = 0;
	g_event4->enemyType[1] = StageEvent::crab;
	g_event4->xSpawnPos[1] = 250;
	g_event4->ySpawnPos[1] = 60;
	g_event4->enemyCount = 2;
	g_event4->endEvent = false;
	g_event4->powerUp = StageEvent::none;
	//Offset range: 5 - 61

	g_event5->enemyType[0] = StageEvent::squid;
	g_event5->xSpawnPos[0] = 250;
	g_event5->ySpawnPos[0] = 0;
	g_event5->enemyCount = 1;
	g_event5->endEvent = false;
	g_event5->powerUp = StageEvent::none;
	//Offset range: 43 - 107

	g_event6->enemyType[0] = StageEvent::jellyfish;
	g_event6->xSpawnPos[0] = 250;
	g_event6->ySpawnPos[0] = 2;
	g_event6->enemyType[1] = StageEvent::jellyfish;
	g_event6->xSpawnPos[1] = 250;
	g_event6->ySpawnPos[1] = 30;
	g_event6->enemyType[2] = StageEvent::jellyfish;
	g_event6->xSpawnPos[2] = 250;
	g_event6->ySpawnPos[2] = 58;
	g_event6->enemyType[3] = StageEvent::jellyfish;
	g_event6->xSpawnPos[3] = 250;
	g_event6->ySpawnPos[3] = 86;
	g_event6->enemyType[4] = StageEvent::jellyfish;
	g_event6->xSpawnPos[4] = 250;
	g_event6->ySpawnPos[4] = 114;
	g_event6->enemyCount = 5;
	g_event6->endEvent = false;
	g_event6->powerUp = StageEvent::none;
	//Offset range: 0

	g_firePowerPowerUpEvent->endEvent = false;
	g_firePowerPowerUpEvent->powerUp = StageEvent::firePower;
	
	g_invinciblePowerUpEvent->endEvent = false;
	g_invinciblePowerUpEvent->powerUp = StageEvent::invincibility;
	
	g_bombPowerUpEvent->endEvent = false;
	g_bombPowerUpEvent->powerUp = StageEvent::bomb;
}
