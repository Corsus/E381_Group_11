/*
 * main.c
 *
 *  Created on: 2013-11-13
 *      Author: htang
 */
#include "main.h"

// 0 = empty, 1,2,3,4 = ship, -1 = fired at
int playerOneGrid[GRID_COLS][GRID_ROWS];
int playerTwoGrid[GRID_COLS][GRID_ROWS];

//keep track the health of the ships
int playerOneShips[4];
int playerTwoShips[4];

//clientID for client 1 and client 2
int clientOneID;
int clientTwoID;

char* ready_message = "R";
char* turn_message = "T";
char* win_message = "W";
char* lose_message = "L";

void initializeGridContents(int grid[GRID_COLS][GRID_ROWS])
{
	int i;
	int j;
	//initialize an empty board
	for (i = 0; i < GRID_ROWS; i++)
	{
		for (j = 0; j < GRID_COLS; j++)
		{
			grid[j][i] = 0;
		}
	}
	//also, initialize the health of all ships
	for (i = 0; i < 4; i++)
	{
		//start from smallest ship (1) to largest ship (4)
		playerOneShips[i] = i+1;
		playerTwoShips[i] = i+1;
	}
}

void testGridContents(int grid[GRID_COLS][GRID_ROWS])
{
	int i;
	int j;
	for (i = 0; i < GRID_ROWS; i++)
	{
		for (j = 0; j < GRID_COLS; j++)
		{
			printf("%d ", grid[j][i]);
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
				playerOneGrid[x + i][y] = size;
			}
		}
		else if (orientation == 'V')
		{
			for (i = 0; i < size; i++)
			{
				playerOneGrid[x][y + i] = size;
			}
		}
	}
	else if (player == 2)
	{
		if (orientation == 'H')
		{
			for (i = 0; i < size; i++)
			{
				playerTwoGrid[x + i][y] = size;
			}
		}
		else if (orientation == 'V')
		{
			for (i = 0; i < size; i++)
			{
				playerTwoGrid[x][y + i] = size;
			}
		}
	}
}

int isGameOver(int gridToCheck[GRID_COLS][GRID_ROWS])
{
	int i;
	int j;
	for (i = 0; i < GRID_ROWS; i++)
	{
		for (j = 0; j < GRID_COLS; j++)
		{
			if (gridToCheck[j][i] > 0)
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
	printf("Initializing RS232 Connection...\n");
	initializeRS232();
	initializeReadTimerInterrupt();

	while (1)
	{
		printf("Initializing Grid Models...\n");
		initializeGridContents(playerOneGrid);
		initializeGridContents(playerTwoGrid);

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
		sendToClient(clientOneID, ready_message);
		//wait for acknowledgement
		waitForAcknowledgement(clientOneID);

		//tell client 2 that loading is complete
		sendToClient(clientTwoID, ready_message);
		//wait for acknowledgement
		waitForAcknowledgement(clientTwoID);

		//game is playing loop
		while (1)
		{
			//tell client one to act "11T"
			printf("Client 1 turn to act...\n");
			sendToClient(clientOneID, turn_message);
			//wait for acknowledgement
			waitForAcknowledgement(clientOneID);
			//wait for client one response/action and handle it
			handleFireCommandFromClientOne();

			if (isGameOver(playerTwoGrid) == 1)
			{
				printf("Client 1 is the winner...\n");
				sendToClient(clientOneID, win_message);
				waitForAcknowledgement(clientOneID);
				sendToClient(clientTwoID, lose_message);
				waitForAcknowledgement(clientTwoID);
				break;
			}

			//tell client two to act "21T"
			printf("Client 2 turn to act...\n");
			sendToClient(clientTwoID, turn_message);
			//wait for acknowledgement
			waitForAcknowledgement(clientTwoID);
			//wait for client two response/action and handle it
			handleFireCommandFromClientTwo();

			if (isGameOver(playerOneGrid) == 1)
			{
				printf("Client 2 is the winner...\n");
				sendToClient(clientTwoID, win_message);
				waitForAcknowledgement(clientTwoID);
				sendToClient(clientOneID, lose_message);
				waitForAcknowledgement(clientOneID);
				break;
			}
		}
		printf("Game is over...\n");
		printf("Restarting server...\n");
	}
	return 0;
}
