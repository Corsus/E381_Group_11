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
	initializeGame();
	printf("Starting position: %d\n", gameBall.x_pos);

	// draw menu
	// wait for user input

	//main loop
	while(1)
	{
	}
	return 0;
}

void initializeGame()
{
	initializeVgaDisplay();
	initializeGameBall();
	initializeBallMover();
	initializeScreenMover();
}

void initializeGameBall()
{
	gameBall.x_pos = (SCREEN_X / 2) - 1;
	gameBall.y_pos = 1;
}

