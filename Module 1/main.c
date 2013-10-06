/*
 * main.c
 *
 *  Created on: 2013-10-01
 *      Author: htang
 */

#include "main.h"

// The game ball. Keep tracks of the positions.
Ball gameBall;

// current selection in the menu
int menu_selection = MENU_PLAY;

// state of the game
GameStatus gameStatus = MAIN_MENU;

int main()
{
	printf("Hello. Welcome.\n");

	// draw interface
	initializeVgaDisplay();

	//main loop
	while(1)
	{
		// we always start with the Menu
		printf("Initializing menu...\n");
		drawMenu();
		printf("Menu Drawn...\n");

		// menu loop
		while (gameStatus == MAIN_MENU)
		{
			menu_controller();
		}
		// main game loop
		while (gameStatus == PLAYING)
		{
			handleControllerInput();
		}

		// at the end, we go back to main menu and restart our loop
		gameStatus = MAIN_MENU;
	}
	return 0;
}

// main game initialization
void initializeGame()
{
	initializeGameBall();
	initializeBallMover();
	initializeScreenMover();
	initializeInfoBar();
}

// initializeGame helper method: initializes the gameBall
void initializeGameBall()
{
	gameBall.nw_x = (SCREEN_X_PLAY / 2) - 2;
	gameBall.w_x = (SCREEN_X_PLAY / 2) - 2;
	gameBall.sw_x = (SCREEN_X_PLAY / 2) - 2;
	gameBall.ne_x = (SCREEN_X_PLAY / 2);
	gameBall.e_x = (SCREEN_X_PLAY / 2);
	gameBall.se_x = (SCREEN_X_PLAY / 2);

	gameBall.nw_y = 0;
	gameBall.w_y = 1;
	gameBall.sw_y = 2;
	gameBall.ne_y = 0;
	gameBall.e_y = 1;
	gameBall.se_y = 2;
}

// Menu button control handler
void menu_controller()
{
	usleep(5000);
	if (!(IORD_ALTERA_AVALON_PIO_DATA(KEY_BASE) & 0x2))
	{
		// clears the menu screen
		clearScreen();
		// select
		selectMenu();
	}
	else if (!(IORD_ALTERA_AVALON_PIO_DATA(KEY_BASE) & 0x4))
	{
		// change option
		update_menu_selection(MENU_SCORE);
		menu_selection = MENU_SCORE;
		printf("High Scores.\n");
	}
	else if (!(IORD_ALTERA_AVALON_PIO_DATA(KEY_BASE) & 0x8))
	{
		// change option
		update_menu_selection(MENU_PLAY);
		menu_selection = MENU_PLAY;
		printf("Play.\n");
	}
	usleep(5000);
}

// Menu controller helper method: chooses a menu selection
// Changes the "state" of the game based on the current selection.
void selectMenu()
{
	switch (menu_selection)
	{
		// select Play Game
		case MENU_PLAY:
			printf("Select: Play.\n");
			gameStatus = PLAYING;
			//initialize the game here.
			initializeGame();
			break;
		// select High Score table
		case MENU_SCORE:
			printf("Select: High Score.\n");
			gameStatus = HIGH_SCORE;
			break;
	}
}
