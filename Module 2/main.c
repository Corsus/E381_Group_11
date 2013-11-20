/*
 * main.c
 *
 *  Created on: 2013-11-13
 *      Author: htang
 */
#include "main.h"

GridContent playerOneGrid[GRID_COLS][GRID_ROWS];
GridContent playerTwoGrid[GRID_COLS][GRID_ROWS];

char* ready_message = "R";
char* turn_message = "T";
char* win_message = "W";
char* lose_message = "L";

void initializeGridContents(GridContent grid[GRID_COLS][GRID_ROWS])
{
	int i;
	int j;
	for (i = 0; i < GRID_ROWS; i++)
	{
		for (j = 0; j < GRID_COLS; j++)
		{
			grid[j][i] = EMPTY;
		}
	}
}

void testGridContents(GridContent grid[GRID_COLS][GRID_ROWS])
{
	int i;
	int j;
	for (i = 0; i < GRID_ROWS; i++)
	{
		for (j = 0; j < GRID_COLS; j++)
		{
			if (grid[j][i] == EMPTY)
				printf("0 ");
			else if (grid[j][i] == SHIP)
				printf("1 ");
		}
		printf("\n");
	}
}

void insertShipInGrid(int player, char* shipInfo)
{
	int i;
	int size = ((int) shipInfo[0]) - 48;
	char orientation = shipInfo[1];
	int x = ((int) shipInfo[2]) - 48;
	int y = ((int) shipInfo[3]) - 48;
	if (player == 1)
	{
		if (orientation == 'H')
		{
			for (i = 0; i < size; i++)
			{
				playerOneGrid[x + i][y] = SHIP;
			}
		}
		else if (orientation == 'V')
		{
			for (i = 0; i < size; i++)
			{
				playerOneGrid[x][y + i] = SHIP;
			}
		}
	}
	else if (player == 2)
	{
		if (orientation == 'H')
		{
			for (i = 0; i < size; i++)
			{
				playerTwoGrid[x + i][y] = SHIP;
			}
		}
		else if (orientation == 'V')
		{
			for (i = 0; i < size; i++)
			{
				playerTwoGrid[x][y + i] = SHIP;
			}
		}
	}
}

int isGameOver(GridContent gridToCheck[GRID_COLS][GRID_ROWS])
{
	int i;
	int j;
	for (i = 0; i < GRID_ROWS; i++)
	{
		for (j = 0; j < GRID_COLS; j++)
		{
			if (gridToCheck[j][i] == SHIP)
			{
				//game is not over, this player is still alive
				return 0;
			}
		}
	}
	return 1; //game is over, this player is dead
}


int main()
{
	//main server loop
	while (1)
	{
		printf("Initializing Grid Models...\n");
		initializeGridContents(playerOneGrid);
		initializeGridContents(playerTwoGrid);

		printf("Initializing RS232 Connection...\n");
		initializeRS232();
		initializeReadTimerInterrupt();

		//wait for players to report ship positions (PRESS READY ON ANDROID)
		//store ship positions in grid model
		printf("Waiting for clients...\n");
		waitForClientsToSetup();
		printf("Setup is ready...\n");

		//testing purposes
		printf("Client 1 Board: \n");
		testGridContents(playerOneGrid);
		printf("Client 2 Board: \n");
		testGridContents(playerTwoGrid);

		//tell clients that loading is done
		printf("Game is starting. Informing clients...\n");
		//tell client 1 that loading is complete
		sendToClient(1, ready_message);
		//wait for acknowledgement
		waitForAcknowledgement(1);

		//tell client 2 that loading is complete
		sendToClient(2, ready_message);
		//wait for acknowledgement
		waitForAcknowledgement(2);

		//game is playing loop
		while (1)
		{
			//tell client one to act "11T"
			printf("Client 1 turn to act...\n");
			sendToClient(1, turn_message);
			//wait for acknowledgement
			waitForAcknowledgement(1);
			//wait for client one response/action and handle it
			handleFireCommandFromClientOne();

			if (isGameOver(playerTwoGrid) == 1)
			{
				printf("Client 1 is the winner...\n");
				sendToClient(1, win_message);
				waitForAcknowledgement(1);
				sendToClient(2, lose_message);
				waitForAcknowledgement(2);
				break;
			}

			//tell client two to act "21T"
			printf("Client 2 turn to act...\n");
			sendToClient(2, turn_message);
			//wait for acknowledgement
			waitForAcknowledgement(2);
			//wait for client two response/action and handle it
			handleFireCommandFromClientTwo();

			if (isGameOver(playerOneGrid) == 1)
			{
				printf("Client 2 is the winner...\n");
				sendToClient(2, win_message);
				waitForAcknowledgement(2);
				sendToClient(1, lose_message);
				waitForAcknowledgement(1);
				break;
			}
		}
		printf("Game is over...\n");
		printf("Restarting server...\n");
	}
	return 0;
}
