/*
	"Chrome says"
	Programari desenvolupat per a l'assignatura d'AEV (GII - ETSINF - UPV) per:
	Salvador Llobet, Ignacio
*/

#include <nds.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>

// git outputs a nice header to reference data
#include "chromeP2.h"
#include "red.h"
#include "green.h"
#include "yellow.h"
#include "blue.h"
#include "topScreen.h"

// Font
#include "font.h"

// Basic menu
#include "mainMenu.h"

// Sound
#include <maxmod9.h>
#include "soundbank.h"
#include "soundbank_bin.h"

// Constants
#define WIN			12
#define CORRECT		2
#define INCORRECT	1
#define TIMER_SPEED (BUS_CLOCK/1024)

int seconds = 0, minutes = 1;
int end = 0;
char solution[WIN];
int last = 0;									//last void position
int length = sizeof(solution)/sizeof(char);
int correct = 1;

/*
	Initializes the array
*/
void initSol()
{
	int i;
	for(i = 0; i < length; i++)
		solution[i] = 'v';		// void
}

/*
	Delay function
*/
void sleep(int secs)
{
	uint ticks = 0;
	timerStart(1, ClockDivider_1024, /*0*/1, NULL);
	while((ticks/TIMER_SPEED) < secs)
	{
		ticks += timerPause(1);
		timerUnpause(1);
	}
	timerStop(1);
}

/*
	Increase or decrease the timer, depending of the first argument.
		inde: if 0 then decrease, else then increase
		qtt: value to increase or decrease to the timer
*/
void changeTimer(int inde, int qtt)
{
	if(inde){
		if(seconds + qtt > 59){
			minutes++;
			seconds = (seconds + qtt) - 60;
		}
		else seconds += qtt;
	}
	else{
		if(minutes >= 1 && seconds - qtt <= 0){
			minutes--;
			if(qtt == seconds)
				seconds = 0;
			else
				seconds = 60 - (qtt - seconds);
		}
		else if(minutes == 0 && seconds - qtt <= 0){
			seconds = 0;
		}
		else seconds -= qtt;
	}
}

/*
	Decreases the timer one time per second
*/
void updateTimer()
{
	if(minutes >= 1 && seconds == 0){
		minutes--;
		seconds = 59;
	}
	else if(minutes == 0 && seconds == 0){
		iprintf("S'ha acabat el temps\n");
		end = 1;
	}
	else seconds--;
	
	iprintf("\x1b[2J%02i:%02i", minutes, seconds);
}

/*
	Returns an integer in the range [0, n).
*/
int randint(int n)
{
	// We change the seed every time
	int v = 3;
	int memAddr = (int)&v;
	int rawValue = abs(memAddr);
	srand((rawValue % 100)+seconds);
	
	return rand() % n;
}

/*
	Returns a char depending of the value of argument, from 0 to 3
*/
char randchar(int n)
{
	// 0 = red
	// 1 = green
	// 2 = yellow
	// 3 = blue
	switch(n)
	{
		case 0:
		return 'r';
		case 1:
		return 'g';
		case 2:
		return 'y';
		case 3:
		return 'b';
	}
}

/*
	Appends a random char to the solution array
*/
void appendSol()
{
	solution[last] = randchar(randint(4));
	last++;
}

/*
	Load a different background depending of char's argument
*/
void loadBG(char x)
{
	switch(x)
	{
		case 'r':
		decompress(redBitmap, BG_GFX_SUB,  LZ77Vram);
		break;
		case 'g':
		decompress(greenBitmap, BG_GFX_SUB,  LZ77Vram);
		break;
		case 'y':
		decompress(yellowBitmap, BG_GFX_SUB,  LZ77Vram);
		break;
		case 'b':
		decompress(blueBitmap, BG_GFX_SUB,  LZ77Vram);
		break;
	}
}

/*
	Confirms if user's solution is correct or not
		x: user's solution
		pos: point of the correct answer
*/
int confirm(char x, int pos)
{
	if(solution[pos] == x)
		return 1;
	else
		return 0;
}

/*
	Load the BG and increase or decrease the timer
	Returns 1 if selection is correct, and 0 if it isn't
*/
int selection(char x, int pos)
{
	loadBG(x);
	if(confirm(x,pos)){
		if(pos + 1 >= last)changeTimer(1,CORRECT);
		sleep(5);
		decompress(chromeP2Bitmap, BG_GFX_SUB,  LZ77Vram);
		return 1;
	}
	else{
		changeTimer(0,INCORRECT);
		sleep(5);
		decompress(chromeP2Bitmap, BG_GFX_SUB,  LZ77Vram);
		return 0;
	}
}

/*
	Main
*/
int main()
{
	LOOP:
	
	initSol();									// Initializes the solution array

	touchPosition touch;

		// Load sounds
	mmInitDefaultMem((mm_addr)soundbank_bin);
	mmLoad( MOD_THEME );
	mmLoadEffect( SFX_R );
	mmLoadEffect( SFX_G );
	mmLoadEffect( SFX_Y );
	mmLoadEffect( SFX_B );
	mmLoadEffect( SFX_CHOOSE );
	mmLoadEffect( SFX_WRONG );
	
	mmStart( MOD_THEME, MM_PLAY_LOOP );
	mmSetModuleVolume( 250 );
	
	// Red
	mm_sound_effect r = {
		{ SFX_R } ,			// id
		(int)(1.0f * (1<<10)),	// rate
		0,		// handle
		255,	// volume
		255,	// panning
	};
	
	// Green
	mm_sound_effect g = {
		{ SFX_G } ,			// id
		(int)(1.0f * (1<<10)),	// rate
		0,		// handle
		255,	// volume
		255,	// panning
	};
	
	// Yellow
	mm_sound_effect y = {
		{ SFX_Y } ,			// id
		(int)(1.0f * (1<<10)),	// rate
		0,		// handle
		255,	// volume
		255,	// panning
	};
	
	// Blue
	mm_sound_effect b = {
		{ SFX_B } ,			// id
		(int)(1.0f * (1<<10)),	// rate
		0,		// handle
		255,	// volume
		255,	// panning
	};
	
	// Wrong
	mm_sound_effect wrong = {
		{ SFX_WRONG } ,			// id
		(int)(1.0f * (1<<10)),	// rate
		0,		// handle
		255,	// volume
		255,	// panning
	};
	
	// Choose
	mm_sound_effect choose = {
		{ SFX_CHOOSE } ,		// id
		(int)(1.0f * (1<<10)),	// rate
		0,		// handle
		255,	// volume
		255,	// panning
	};
	
	PrintConsole topScreen;
	PrintConsole bottomScreen;

	last = 0;
	end = 0;
	correct = 1;
	
	minutes = 1;
	seconds = 0;
	
	// We load the main menu
	loadMenu();
	mmEffectEx(&choose);
	sleep(5);
	
	//videoSetMode(MODE_0_2D);
	//videoSetModeSub(MODE_5_2D);
	videoSetMode(MODE_0_2D | MODE_4_2D);
	videoSetModeSub(MODE_4_2D);
	
	vramSetBankA(VRAM_A_MAIN_BG);
	vramSetBankB(VRAM_B_MAIN_BG);
	vramSetBankC(VRAM_C_SUB_BG);
	
	//consoleInit(&topScreen, 3,BgType_Text4bpp, BgSize_T_256x256, 31, 0, true, true);
	int bg = bgInit(3, BgType_Bmp16, BgSize_B16_256x256, 5,0);
	
	PrintConsole *console = consoleInit(&topScreen, 0, BgType_ExRotation, BgSize_ER_256x256, 20, 0, true, false);
	ConsoleFont font;

	font.gfx = (u16*)fontTiles;
	font.pal = (u16*)fontPal;
	font.numChars = 95;
	font.numColors =  fontPalLen / 2;
	font.bpp = 8;
	font.asciiOffset = 32;
	font.convertSingleColor = false;
	
	consoleSetFont(console, &font);
	
    decompress(topScreenBitmap, bgGetGfxPtr(bg), LZ77Vram);
    consoleInit(&topScreen, 1,BgType_Text4bpp, BgSize_T_256x256, 4, 0, true, true);
	consoleInit(&bottomScreen, 3, BgType_Bmp16, BgSize_B16_256x256, 0, 0, false, false);

	consoleSetFont(console, &font);
	//consoleSelect(&topScreen);
	
	decompress(chromeP2Bitmap, BG_GFX_SUB,  LZ77Vram);
	
	// Selecting topScreen
	consoleSelect(&topScreen);
	
	//Move text
	consoleSetWindow(&topScreen, 46, 8, 40, 40);
	
	//calls the updateTimer function 1 times per second
	timerStart(0, ClockDivider_1024, TIMER_FREQ_1024(1), updateTimer);

	while(/*1 &&*/ last <= WIN && !end)
	{
		int pos = 0;
		int i;
		char sel = 'v';
			
		swiWaitForVBlank();
		scanKeys();
			
		if(correct) appendSol();
			
		//AI
		for(i = 0; i < last; i++)
		{
			loadBG(solution[i]);
			switch(solution[i])
			{
				case 'r':
				mmEffectEx(&r);
				break;
				case 'g':
				mmEffectEx(&g);
				break;
				case 'y':
				mmEffectEx(&y);
				break;
				case 'b':
				mmEffectEx(&b);
				break;
			}
			sleep(5);
			decompress(chromeP2Bitmap, BG_GFX_SUB,  LZ77Vram);
		}
		//AI
			
		correct = 1;
			
		for(i = 0; (i < last) && correct && !end; i++)
		{
			while (sel == 'v' && !end){
					
				touchRead(&touch);
					
				//Red
				if(((touch.px >= 100 && touch.px <= 200) && (touch.py >= 25 && touch.py <= 56)) || ((touch.px >= 168 && touch.px <= 200) && (touch.py >= 25 && touch.py <= 90)) || ((touch.px >= 151 && touch.px <= 200) && (touch.py >= 25 && touch.py <= 61)))
				{	
					sel = 'r';
					mmEffectEx(&r);
				}
					
				//Green
				if(((touch.px >= 55 && touch.px <= 87) && (touch.py >= 25 && touch.py <= 146)) || ((touch.px >= 55 && touch.px <= 79) && (touch.py >= 146 && touch.py <= 158)))
				{	
					sel = 'g';
					mmEffectEx(&g);
				}
						
				//Yellow
				if(((touch.px >= 99 && touch.px <= 114) && (touch.py >= 154 && touch.py <= 168)) || ((touch.px >= 114 && touch.px <= 200) && (touch.py >= 136 && touch.py <= 168)) || ((touch.px >= 167 && touch.px <= 200) && (touch.py >= 108 && touch.py <= 168)))
				{
					sel = 'y';
					mmEffectEx(&y);
				}
						
				//Blue
				if(((touch.px >= 98 && touch.px <= 156) && (touch.py >= 66 && touch.py <= 125)))
				{
					sel = 'b';
					mmEffectEx(&b);
				}
					
			} // closes while
				
			if(sel != 'v')
				correct = selection(sel,pos);
			if(!correct)
			{
				mmEffectEx(&wrong);
				iprintf("\n\nERROR!");
				sleep(5);
			}
			pos++;
			sel = 'v';
		} // closes for
			
	}
		
		if(last >= WIN){
			loadFinal(1);
			mmEffectEx(&choose);
			timerStop(0);
			sleep(5);
			goto LOOP;
		} else {
			loadFinal(0);
			mmEffectEx(&choose);
			timerStop(0);
			sleep(5);
			goto LOOP;
		}
}