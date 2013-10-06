/*
 * main.c
 *
 *  Created on: 2013-10-01
 *      Author: htang
 */

#include "main.h"

// Position of the Ball
Ball gameBall;

int main()
{
	printf("Hello. Welcome.\n");
	initializeVgaDisplay();
	initializeGame();

	// draw menu
	// wait for user input

	//main loop
	while(1)
	{
		handleControllerInput();
	}
	return 0;
}

void initializeGame()
{
	initializeGameBall();
	initializeBallMover();
	initializeScreenMover();
	initializeInfoBar();
}

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

