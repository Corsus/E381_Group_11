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
	initializeKeyboardController();
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
		drawStatus(intro_1);
		drawMode(intro_2);
		menu_selection = MENU_PLAY;

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
			gameOverInput();
		}
		usleep(100000);
		while (gameStatus == SUBMIT_SCORE)
		{
			waitForInput();
		}
		usleep(100000);

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
 * Valid input: L ARROW, R ARROW, ENTER
 */
void menu_controller()
{
	usleep(5000);
	int direction;
	direction = readMenuControl();

	if (direction == LEFT_BUTTON)
	{
		// change option
		update_menu_selection(MENU_PLAY);
		menu_selection = MENU_PLAY;
		//printf("Play.\n");
	}
	else if (direction == RIGHT_BUTTON)
	{
		// change option
		update_menu_selection(MENU_SCORE);
		menu_selection = MENU_SCORE;
		//printf("High Scores.\n");
	}
	else if (direction == ENTER_BUTTON)
	{
		// clears the menu screen
		clearScreen();
		// select
		selectMenu();
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
			//initialize the game
			initializeGame();
			//start playing background music
			playBackground();
			//draw status on LCD
			drawStatus(game_status_string_1);
			drawMode(play_mode_string_1);
			break;
		// select High Score table
		case MENU_SCORE:
			//draw status on LCD
			drawMode(high_score_menu_string);
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
	alt_irq_disable(AUDIO_IRQ);

	//clear the screen
	clearScreen();

	playLose();
	drawMode(game_over_string);
	drawGameOverScreen();
	gameStatus = GAME_OVER;

}

/*
 * Method to wait for a key press (ENTER) from user
 * When pressed, the status of the game changes to Main Menu
 * Essentially waits for the user to press a key before going back to Main Menu.
 */
void waitForInput()
{
	usleep(5000);
	int enter = readMenuControl();
	if (enter == ENTER_BUTTON)
	{
		// clears the menu screen
		clearScreen();
		gameStatus = MAIN_MENU;
	}
	usleep(5000);
}

/*
 * Method to wait for a key press (ENTER) from user
 * When pressed, the status of the game changes to Main Menu
 * Essentially waits for the user to press a key before going back to Main Menu.
 */
void gameOverInput()
{
	usleep(5000);
	int enter = readMenuControl();
	if (enter == ENTER_BUTTON)
	{
		// clears the menu screen
		clearScreen();

		if (game_score > readHighscore(10))
		{
			playCongrat();
			drawSubmitScoreScreen();
			gameStatus = SUBMIT_SCORE;
			drawStatus(sub_score_string_1);
			drawMode(sub_score_string_2);
		}
		else
		{
			gameStatus = MAIN_MENU;
		}
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
	if (gameBall.color == WHITE)
	{
		gameBall.color = BLACK;
		undraw_color = WHITE;
		reverseClearPlayScreen();
		drawMode(play_mode_string_2);
	}
	else
	{
		gameBall.color = WHITE;
		undraw_color = BLACK;
		clearPlayScreen();
		drawMode(play_mode_string_1);
	}
	reverseControllerInput();
}

