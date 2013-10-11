/*
 * main.c
 *
 *  Created on: 2013-10-01
 *      Author: EECE381 Group 11
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

/*
 * Main loop, where it all comes together
 */
int main()
{
	//initialize VGA
	initializeVgaDisplay();

	//draw loading screen
	drawLoadingScreen();

	//initialize LCD, SD card, audio
	initializeLCD();
	initializeSDCardController();
	initialize_audio();

	//play intro sound
	playGreet();

	//main loop
	while(1)
	{
		// we always start with the Menu
		drawMenu();
		drawStatus("FALL DOWN 381");
		drawMode("Main Menu");

		// menu loop
		while (gameStatus == MAIN_MENU)
		{
			menu_controller();
		}
		// high score loop
		while (gameStatus == HIGH_SCORE)
		{
			waitForInput();
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

/*
 * Game initialization
 * Initializes all timer interrupts, UI components, game objects
 */
void initializeGame()
{
	initializeGameBall();
	initializeBallMover();
	initializeScreenMover();
	initializeInfoBar();
	initialize_modeSwitch_IRQ();
}

/*
 * Initializes the game ball's position and color
 */
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

/*
 * The controller method for the Main Menu
 * Checks for the button presses and acts accordingly
 * KEY3 selects Play
 * KEY2 selects High Score
 * KEY1 selects the current selection
 * KEY0 does nothing
 */
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
			//change state of game
			gameStatus = PLAYING;
			//play Game Start sound
			playStart();
			//wait for sound to finish
			usleep(1000000);
			//initialize the game
			initializeGame();
			//start playing background music
			playBackground();
			//draw status on LCD
			drawStatus("Let's Begin.");
			drawMode("Normal");
			break;
		// select High Score table
		case MENU_SCORE:
			//draw status on LCD
			drawMode("High Scores");
			//change state of game
			gameStatus = HIGH_SCORE;
			//load up highscore UI
			drawHighScoreScreen();
			break;
	}
}

/*
 * Stops the game when game is over.
 * Disables all interrupts.
 * Clears the play screen and draws the Game Over screen.
 * Changes the state of the game to Game Over.
 */
void stopGame()
{
	//disable all interrupts that run in the game
	alt_irq_disable(SCREEN_TIMER_IRQ);
	alt_irq_disable(BALL_TIMER_IRQ);
	alt_irq_disable(MODESWITCH_TIMER_IRQ);
	alt_irq_disable(AUDIO_IRQ);

	//clear the screen
	clearScreen();

	playLose();
	drawGameOverScreen();

	gameStatus = GAME_OVER;
	drawMode("Game Over");
}

/*
 * Method to wait for a key press (KEY0) from user
 * When pressed, the status of the game changes to Main Menu
 * Essentially waits for the user to press a key before going back to Main Menu.
 */
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

/*
 * Switches the mode of play from Normal to Reverse and vice-versa
 * All black colors become white, and all white colors become black.
 * All directional controls are also reversed.
 */
void switchPlayMode()
{
	//printf("Mode Switch!!\n");
	if (gameBall.color == WHITE)
	{
		gameBall.color = BLACK;
		undraw_color = WHITE;
		reverseClearPlayScreen();
		drawMode("Reversed");
	}
	else
	{
		gameBall.color = WHITE;
		undraw_color = BLACK;
		clearPlayScreen();
		drawMode("Normal");
	}
	reverseControllerInput();
}

/*
 * Initializes the mode switch timer irq
 * Ticks every 1 second.
 */
void initialize_modeSwitch_IRQ()
{
	mode_switch_counter = 9;
	printf("Initializing mode switch timer IRQ...\n");
	// setting up timer (Continuous, Interrupt enabled, Stopped)
	IOWR_ALTERA_AVALON_TIMER_CONTROL(MODESWITCH_TIMER_BASE, 0xb); //stop-start-cont-ito 1011

	// register isr
	alt_irq_register(MODESWITCH_TIMER_IRQ, 0x0, modeSwitch_isr);

	// enable interrupt
	alt_irq_enable(MODESWITCH_TIMER_IRQ);

	//start timer
	IOWR_ALTERA_AVALON_TIMER_CONTROL(MODESWITCH_TIMER_BASE, 0x7); //stop-start-cont-ito 0111

	printf("Mode switch timer started...\n");
}

/*
 * Handler for the mode switch irq
 * After 10 ticks, this method reverses the play mode by calling switchPlayMode
 */
void modeSwitch_isr(void* context, alt_u32 id)
{
	if (mode_switch_counter != 0)
	{
		printf("%d\n", mode_switch_counter);
		mode_switch_counter--;
	}
	else
	{
		mode_switch_counter = 9;
		switchPlayMode();
	}

	//clear timeout bit
	IOWR_ALTERA_AVALON_TIMER_STATUS(MODESWITCH_TIMER_BASE, 0x0);
}

