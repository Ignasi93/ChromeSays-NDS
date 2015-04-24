#include <nds.h>
#include <stdio.h>

// git outputs a nice header to reference data
#include "menu.h"
#include "portada.h"
#include "win.h"
#include "lose.h"
#include "back.h"

/*
	Load the main menu and it will be loaded until the user press start
*/
void loadMenu(void) {
	
	touchPosition touch1;

	PrintConsole bottomScreen1;
	
	videoSetMode(MODE_5_2D);
	videoSetModeSub(MODE_5_2D);

	vramSetBankB(VRAM_B_MAIN_BG);
	vramSetBankH(VRAM_H_SUB_BG);

	consoleInit(&bottomScreen1, 3, BgType_Bmp16, BgSize_B16_256x256, 0, 0, false, false);

	bgInit(3, BgType_Bmp16, BgSize_B16_256x256, 0,0);
	decompress(portadaBitmap, BG_GFX, LZ77Vram);
	
	consoleSelect(&bottomScreen1);
	decompress(menuBitmap, BG_GFX_SUB,  LZ77Vram);
	
	while(1) {
	
		swiWaitForVBlank();
		scanKeys();
		
		touchRead(&touch1);

		if(((touch1.px >= 16 && touch1.px <= 240) && (touch1.py >= 72 && touch1.py <= 116)) || keysDown() & KEY_A)
			break;
	}
}

/*
	Load the final menu, the victory or loss one
*/
void loadFinal(int v)
{
	touchPosition touch1;

	PrintConsole bottomScreen1;
	
	videoSetMode(MODE_5_2D);
	videoSetModeSub(MODE_5_2D);

	vramSetBankB(VRAM_B_MAIN_BG);
	vramSetBankH(VRAM_H_SUB_BG);

	consoleInit(&bottomScreen1, 3, BgType_Bmp16, BgSize_B16_256x256, 0, 0, false, false);

	bgInit(3, BgType_Bmp16, BgSize_B16_256x256, 0,0);
	if(!v)
		decompress(loseBitmap, BG_GFX, LZ77Vram);
	else
		decompress(winBitmap, BG_GFX, LZ77Vram);
	
	consoleSelect(&bottomScreen1);
	decompress(backBitmap, BG_GFX_SUB,  LZ77Vram);
	
	while(1) {
	
		swiWaitForVBlank();
		scanKeys();
		
		touchRead(&touch1);

		if(((touch1.px >= 51 && touch1.px <= 207) && (touch1.py >= 20 && touch1.py <= 174)) || keysDown() & KEY_A)
			break;
	}
}