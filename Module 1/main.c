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

// mode switch counter
int mode_switch_counter;

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
		while (gameStatus == GAME_OVER)
		{
			waitForInput();
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
	initialize_modeSwitch_IRQ();
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

	gameBall.color = WHITE;
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

void stopGame()
{
	//disable all interrupts that run in the game
	alt_ic_irq_disable(SCREEN_TIMER_IRQ_INTERRUPT_CONTROLLER_ID, SCREEN_TIMER_IRQ);
	alt_ic_irq_disable(BALL_TIMER_IRQ_INTERRUPT_CONTROLLER_ID, BALL_TIMER_IRQ);
	alt_ic_irq_disable(MODESWITCH_TIMER_IRQ_INTERRUPT_CONTROLLER_ID, MODESWITCH_TIMER_IRQ);

	//clear the screen
	clearScreen();

	gameStatus = GAME_OVER;
	drawGameOverScreen();
	printf("Game Over.\n");
}

void waitForInput()
{
	usleep(5000);
	if (!(IORD_ALTERA_AVALON_PIO_DATA(KEY_BASE) & 0x1))
	{
		// clears the menu screen
		clearScreen();
		gameStatus = MAIN_MENU;
	}
	usleep(5000);
}

void switchPlayMode()
{
	if (gameBall.color == WHITE)
	{
		reverseClearPlayScreen();
		gameBall.color = BLACK;
		undraw_color = WHITE;
	}
	else
	{
		clearPlayScreen();
		gameBall.color = WHITE;
		undraw_color = BLACK;
	}
}

//initialize irq for the fall down timer
void initialize_modeSwitch_IRQ()
{
	mode_switch_counter = 0;
	printf("Initializing mode switch timer IRQ...\n");
	// setting up timer (Continuous, Interrupt enabled, Stopped)
	IOWR_ALTERA_AVALON_TIMER_CONTROL(MODESWITCH_TIMER_BASE, 0xb); //stop-start-cont-ito 1011

	// register isr
	alt_ic_isr_register(MODESWITCH_TIMER_IRQ_INTERRUPT_CONTROLLER_ID, MODESWITCH_TIMER_IRQ,
			modeSwitch_isr, 0x0, 0x0);

	// enable interrupt
	alt_ic_irq_enable(MODESWITCH_TIMER_IRQ_INTERRUPT_CONTROLLER_ID, MODESWITCH_TIMER_IRQ);

	//start timer
	IOWR_ALTERA_AVALON_TIMER_CONTROL(MODESWITCH_TIMER_BASE, 0x7); //stop-start-cont-ito 0111

	printf("Mode switch timer started...\n");
}

//handler for fall down interrupt
void modeSwitch_isr(void* context)
{
	if (mode_switch_counter <= 10)
	{
		mode_switch_counter++;
	}
	else
	{
		mode_switch_counter = 0;
		switchPlayMode();
	}

	//clear timeout bit
	IOWR_ALTERA_AVALON_TIMER_STATUS(MODESWITCH_TIMER_BASE, 0x0);
}

