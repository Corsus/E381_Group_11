/*
 * Communication.c
 *
 *  Created on: 2013-11-13
 *      Author: htang
 */

#include "Communication.h"

alt_up_rs232_dev* uart;
unsigned char dataFromClient;
unsigned char parity;

char* msgReceived = NULL;
int clientFromID;
int numReceived;

char hit_signal[3];
char miss_signal[3];

void initializeRS232()
{
	printf("UART Initialization\n");
	uart = alt_up_rs232_open_dev(RS232_0_NAME);

	printf("Clearing read buffer to start\n");
	while (alt_up_rs232_get_used_space_in_read_FIFO(uart))
	{
		alt_up_rs232_read_data(uart, &dataFromClient, &parity);
	}
}

void sendToClient(int clientID, char* msg)
{
	int i;
	//printf("Sending the message to the Middleman\n");

	//send clientID
	alt_up_rs232_write_data(uart, (unsigned char) clientID);
	//send size of message
	alt_up_rs232_write_data(uart, (unsigned char) strlen(msg));
	//send the actual message
	for (i = 0; i < strlen(msg); i++) {
		alt_up_rs232_write_data(uart, msg[i]);
	}
}

void waitForClientsToSetup()
{
	do{
		receiveFromClient();
	}while (isClientReady() == -1);
	if (clientFromID == 1)
		printf("Client 1 reported in...\n");
	else if (clientFromID == 2)
		printf("Client 2 reported in...\n");
	do{
		receiveFromClient();
	}while (isClientReady() == -1);
	if (clientFromID == 1)
		printf("Client 1 reported in...\n");
	else if (clientFromID == 2)
		printf("Client 2 reported in...\n");
}

void receiveFromClient()
{
	int i;
	//get client ID
	while (alt_up_rs232_get_used_space_in_read_FIFO(uart) == 0);
	alt_up_rs232_read_data(uart, &dataFromClient, &parity);
	clientFromID = (int) dataFromClient;

	//get size of message
	while (alt_up_rs232_get_used_space_in_read_FIFO(uart) == 0);
	alt_up_rs232_read_data(uart, &dataFromClient, &parity);
	numReceived = (int) dataFromClient;

	//get message
	free(msgReceived);
	msgReceived = (char*) malloc(sizeof(char) * numReceived);

	for (i = 0; i < numReceived; i++)
	{
		while (alt_up_rs232_get_used_space_in_read_FIFO(uart) == 0);
		alt_up_rs232_read_data(uart, &dataFromClient, &parity);
		msgReceived[i] = dataFromClient;
	}
}

void handleFireCommandFromClient()
{
	int x;
	int y;
	receiveFromClient();
	if (clientFromID == 1)
	{
		if (msgReceived[0] == 'F')
		{
			x = ((int) msgReceived[1]) - 48;
			y = ((int) msgReceived[2]) - 48;
			if (playerTwoGrid[x][y] == SHIP)
			{
				printf("P1 hit P2 at: %d %d \n", x, y);
				//player one hit player one at (x,y)
				playerTwoGrid[x][y] = FIRE_HIT;
				//tell client one that he hit (Y for yes)
				sendToClient(1, "Y");
				usleep(3000000);
				//tell client two he got hit at (X,Y)
				hit_signal[0] = 'H';
				hit_signal[1] = msgReceived[1];
				hit_signal[2] = msgReceived[2];
				sendToClient(2, hit_signal);
			}
			else if (playerTwoGrid[x][y] == EMPTY)
			{
				printf("P1 missed P2 at: %d %d \n", x, y);
				//player one missed on empty cell
				playerTwoGrid[x][y] = FIRE_MISS;
				//tell client one he missed (N for no)
				sendToClient(1, "N");
				usleep(3000000);
				//tell client two the miss happened at (X,Y)
				miss_signal[0] = 'M';
				miss_signal[1] = msgReceived[1];
				miss_signal[2] = msgReceived[2];
				sendToClient(2, miss_signal);
			}
		}
	}
	else if (clientFromID == 2)
	{
		if (msgReceived[0] == 'F')
		{
			x = ((int) msgReceived[1]) - 48;
			y = ((int) msgReceived[2]) - 48;
			if (playerOneGrid[x][y] == SHIP)
			{
				printf("P2 hit P1 at: %d %d \n", x, y);
				//player two hit player one at (x,y)
				playerOneGrid[x][y] = FIRE_HIT;
				//tell client two that he hit (Y for yes)
				sendToClient(2, "Y");
				usleep(3000000);
				//tell client one he got hit at (X,Y)
				hit_signal[0] = 'H';
				hit_signal[1] = msgReceived[1];
				hit_signal[2] = msgReceived[2];
				sendToClient(1, hit_signal);
			}
			else if (playerOneGrid[x][y] == EMPTY)
			{
				printf("P2 missed P1 at: %d %d \n", x, y);
				//player two missed on empty cell
				playerOneGrid[x][y] = FIRE_MISS;
				//tell client two he missed (N for no)
				sendToClient(2, "N");
				usleep(3000000);
				//tell client one the miss happened at (X,Y)
				miss_signal[0] = 'M';
				miss_signal[1] = msgReceived[1];
				miss_signal[2] = msgReceived[2];
				sendToClient(1, miss_signal);
			}
		}
	}
}

int isClientReady()
{
	int isReady = -1;
	int i;
	char* shipInfo;
	if (clientFromID == 1)
	{
		if (msgReceived[0] == 'R')
		{
			//skip the R
			shipInfo =  msgReceived + 1;
			for (i = 0; i < 4; i++)
			{
				insertShipInGrid(1, shipInfo + 4*i);
			}
			isReady = 1;
		}
	}
	else if (clientFromID == 2)
	{
		if (msgReceived[0] == 'R')
		{
			//skip the R
			shipInfo =  msgReceived + 1;
			for (i = 0; i < 4; i++)
			{
				insertShipInGrid(2, shipInfo + 4*i);
			}
			isReady = 2;
		}
	}
	return isReady;
}

