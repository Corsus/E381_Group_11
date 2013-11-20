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

char* ackMsg = "A";
int lastMsg_clientID;
char* lastMsg = NULL;

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

void initializeReadTimerInterrupt()
{
	printf("Initializing Read Timer Interrupt...\n");

	IOWR_ALTERA_AVALON_TIMER_CONTROL(READ_TIMER_BASE, 0xb);

	alt_irq_register(READ_TIMER_IRQ, 0x0, read_timer_isr);

	alt_irq_enable(READ_TIMER_IRQ);

	//don't start it yet
	IOWR_ALTERA_AVALON_TIMER_CONTROL(READ_TIMER_BASE, 0xA);

	printf("Read timer initialization completed...\n");
}

void read_timer_isr(void* context, alt_u32 id)
{
	int i;
	//resend the signal because client didn't not acknowledge
	alt_up_rs232_write_data(uart, (unsigned char) lastMsg_clientID);
	alt_up_rs232_write_data(uart, (unsigned char) strlen(lastMsg));
	for (i = 0; i < strlen(lastMsg); i++) {
		alt_up_rs232_write_data(uart, lastMsg[i]);
	}
	printf("Resent message...\n");
	IOWR_ALTERA_AVALON_TIMER_STATUS(READ_TIMER_BASE, 0x0);
}

void enableReadTimerInterrupt()
{
	IOWR_ALTERA_AVALON_TIMER_CONTROL(READ_TIMER_BASE, 0x7);
}
void disableReadTimerInterrupt()
{
	IOWR_ALTERA_AVALON_TIMER_CONTROL(READ_TIMER_BASE, 0xA);
}

void sendToClient(int clientID, char* msg)
{
	int i;
	//printf("Sending the message to the Middleman\n");

	//first keep track of this message
	lastMsg_clientID = clientID;
	free(lastMsg);
	lastMsg = (char*) malloc(sizeof(char) * (strlen(msg) + 1));
	strcpy(lastMsg, msg);

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
	// get client id
	while (alt_up_rs232_get_used_space_in_read_FIFO(uart) == 0);
	alt_up_rs232_read_data(uart, &dataFromClient, &parity);
	clientFromID = (int) dataFromClient;

	// get size of message
	while (alt_up_rs232_get_used_space_in_read_FIFO(uart) == 0);
	alt_up_rs232_read_data(uart, &dataFromClient, &parity);
	numReceived = (int) dataFromClient;

	// get message
	free(msgReceived);
	msgReceived = (char*) malloc(sizeof(char) * numReceived);

	for (i = 0; i < numReceived; i++)
	{
		while (alt_up_rs232_get_used_space_in_read_FIFO(uart) == 0);
		alt_up_rs232_read_data(uart, &dataFromClient, &parity);
		msgReceived[i] = dataFromClient;
	}
}

void waitForAcknowledgement(int clientID)
{
	printf("Waiting for acknowledgement from %d...\n", clientID);
	enableReadTimerInterrupt();
	do {
		receiveFromClient();
	}while (clientFromID != clientID && msgReceived[0] != 'A');
	disableReadTimerInterrupt();
	printf("Acknowledgement received from %d...\n", clientID);
}

void handleFireCommandFromClientOne()
{
	int x;
	int y;
	while (1)
	{
		//receive from client and send acknowledgement
		receiveFromClient();
		sendToClient(clientFromID, ackMsg);
		printf("Sent acknowledgement to %d...\n", clientFromID);

		//the idea here is to read all "useless" messages first
		//then when we read in the fire command, we process it

		//if message is fire command, we handle it
		//if message is not fire command, we loop back and wait for another message
		if (clientFromID == 1 && msgReceived[0] == 'F')
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
				waitForAcknowledgement(1);
				//tell client two he got hit at (X,Y)
				hit_signal[0] = 'H';
				hit_signal[1] = msgReceived[1];
				hit_signal[2] = msgReceived[2];
				sendToClient(2, hit_signal);
				waitForAcknowledgement(2);
			}
			else if (playerTwoGrid[x][y] == EMPTY)
			{
				printf("P1 missed P2 at: %d %d \n", x, y);
				//player one missed on empty cell
				playerTwoGrid[x][y] = FIRE_MISS;
				//tell client one he missed (N for no)
				sendToClient(1, "N");
				waitForAcknowledgement(1);
				//tell client two the miss happened at (X,Y)
				miss_signal[0] = 'M';
				miss_signal[1] = msgReceived[1];
				miss_signal[2] = msgReceived[2];
				sendToClient(2, miss_signal);
				waitForAcknowledgement(2);
			}
			// if fire command was handled, then we break out of this loop
			break;
		}
	}
}

void handleFireCommandFromClientTwo()
{
	int x;
	int y;
	while (1)
	{
		//receive message from client and send acknowledgement
		receiveFromClient();
		sendToClient(clientFromID, ackMsg);
		printf("Sent acknowledgement to %d...\n", clientFromID);

		//if this is a fire command, we handle it
		//otherwise, we loop back and wait for another message
		if (clientFromID == 2 && msgReceived[0] == 'F')
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
				waitForAcknowledgement(2);
				//tell client one he got hit at (X,Y)
				hit_signal[0] = 'H';
				hit_signal[1] = msgReceived[1];
				hit_signal[2] = msgReceived[2];
				sendToClient(1, hit_signal);
				waitForAcknowledgement(1);
			}
			else if (playerOneGrid[x][y] == EMPTY)
			{
				printf("P2 missed P1 at: %d %d \n", x, y);
				//player two missed on empty cell
				playerOneGrid[x][y] = FIRE_MISS;
				//tell client two he missed (N for no)
				sendToClient(2, "N");
				waitForAcknowledgement(2);
				//tell client one the miss happened at (X,Y)
				miss_signal[0] = 'M';
				miss_signal[1] = msgReceived[1];
				miss_signal[2] = msgReceived[2];
				sendToClient(1, miss_signal);
				waitForAcknowledgement(1);
			}
			//at this point, fire command has been handled, we can break out of the loop
			break;
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
