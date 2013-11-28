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

char hit_signal[4];		//3 + 1 for null character
char miss_signal[4];	//3 + 1 for null character

char* ackMsg = "A";
char* lastMsgTo1 = NULL;
char* lastMsgTo2 = NULL;

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

	alt_irq_register(READ_TIMER_IRQ, 0x0, resend1_isr);

	alt_irq_enable(READ_TIMER_IRQ);

	//don't start it yet
	IOWR_ALTERA_AVALON_TIMER_CONTROL(READ_TIMER_BASE, 0xA);

	printf("Read timer initialization completed...\n");
}

void resend1_isr(void* context, alt_u32 id)
{
	int i;
	//resend the signal because client didn't not acknowledge
	alt_up_rs232_write_data(uart, (unsigned char) clientOneID);
	alt_up_rs232_write_data(uart, (unsigned char) strlen(lastMsgTo1));
	for (i = 0; i < strlen(lastMsgTo1); i++) {
		alt_up_rs232_write_data(uart, lastMsgTo1[i]);
	}
	printf("Resent message...\n");
	IOWR_ALTERA_AVALON_TIMER_STATUS(READ_TIMER_BASE, 0x0);
}

void resend2_isr(void* context, alt_u32 id)
{
	int i;
	//resend the signal because client didn't not acknowledge
	alt_up_rs232_write_data(uart, (unsigned char) clientTwoID);
	alt_up_rs232_write_data(uart, (unsigned char) strlen(lastMsgTo2));
	for (i = 0; i < strlen(lastMsgTo2); i++) {
		alt_up_rs232_write_data(uart, lastMsgTo2[i]);
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
	if (clientID % 2 == 1)
	{
		free(lastMsgTo1);
		lastMsgTo1 = (char*) malloc(sizeof(char) * (strlen(msg) + 1));
		strcpy(lastMsgTo1, msg);
	}
	else if (clientID % 2 == 0)
	{
		free(lastMsgTo2);
		lastMsgTo2 = (char*) malloc(sizeof(char) * (strlen(msg) + 1));
		strcpy(lastMsgTo2, msg);
	}

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
	if (clientFromID % 2 == 1)
	{
		clientOneID = clientFromID;
		printf("Client 1 reported in...\n");
	}
	else if (clientFromID % 2 == 0)
	{
		clientTwoID = clientFromID;
		printf("Client 2 reported in...\n");
	}


	do{
		receiveFromClient();
	}while (isClientReady() == -1);
	if (clientFromID % 2 == 1)
	{
		clientOneID = clientFromID;
		printf("Client 1 reported in...\n");
	}
	else if (clientFromID % 2 == 0)
	{
		clientTwoID = clientFromID;
		printf("Client 2 reported in...\n");
	}
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
	if (clientID % 2 == 1)
	{
		alt_irq_register(READ_TIMER_IRQ, 0x0, resend1_isr);
	}
	else if (clientID % 2 == 0)
	{
		alt_irq_register(READ_TIMER_IRQ, 0x0, resend2_isr);
	}
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
		if (clientFromID % 2 == 1 && msgReceived[0] == 'F')
		{
			x = ((int) msgReceived[1]) - 48;
			y = ((int) msgReceived[2]) - 48;
			if (playerTwoGrid[x][y] > 0)
			{
				//player one hit player one at (x,y)
				printf("P1 hit P2 at: %d %d \n", x, y);
				//decrement hp of corresponding ship
				playerTwoShips[playerTwoGrid[x][y] - 1]--;
				//check to see if hp fell to 0
				if (playerTwoShips[playerTwoGrid[x][y] - 1] == 0)
				{
					//tell client one that he sunk (S for sunk)
					sendToClient(clientOneID, "S");
					waitForAcknowledgement(clientOneID);
				}
				else
				{
					//tell client one that he hit (Y for yes)
					sendToClient(clientOneID, "Y");
					waitForAcknowledgement(clientOneID);
				}

				//tell client two he got hit at (X,Y)
				hit_signal[0] = 'H';
				hit_signal[1] = x + 48;
				hit_signal[2] = y + 48;
				hit_signal[3] = '\0';
				sendToClient(clientTwoID, hit_signal);
				waitForAcknowledgement(clientTwoID);

				//keep track of this hit
				playerTwoGrid[x][y] = -1;
			}
			else if (playerTwoGrid[x][y] == 0)
			{
				printf("P1 missed P2 at: %d %d \n", x, y);
				//player one missed on empty cell
				playerTwoGrid[x][y] = -1;
				//tell client one he missed (N for no)
				sendToClient(clientOneID, "N");
				waitForAcknowledgement(clientOneID);
				//tell client two the miss happened at (X,Y)
				miss_signal[0] = 'M';
				miss_signal[1] = x + 48;
				miss_signal[2] = y + 48;
				miss_signal[3] = '\0';
				sendToClient(clientTwoID, miss_signal);
				waitForAcknowledgement(clientTwoID);
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
		if (clientFromID % 2 == 0 && msgReceived[0] == 'F')
		{
			x = ((int) msgReceived[1]) - 48;
			y = ((int) msgReceived[2]) - 48;
			if (playerOneGrid[x][y] > 0)
			{
				//player two hit player one at (x,y)
				printf("P2 hit P1 at: %d %d \n", x, y);
				//decrement hp of corresponding ship
				playerOneShips[playerOneGrid[x][y] - 1]--;
				//check if hp fell to 0
				if (playerOneShips[playerOneGrid[x][y] - 1] == 0)
				{
					//tell client two that he sunk (S for sunk)
					sendToClient(clientTwoID, "S");
					waitForAcknowledgement(clientTwoID);
				}
				else
				{
					//tell client two that he hit (Y for yes)
					sendToClient(clientTwoID, "Y");
					waitForAcknowledgement(clientTwoID);
				}

				//tell client one he got hit at (X,Y)
				hit_signal[0] = 'H';
				hit_signal[1] = x + 48;
				hit_signal[2] = y + 48;
				hit_signal[3] = '\0';
				sendToClient(clientOneID, hit_signal);
				waitForAcknowledgement(clientOneID);

				playerOneGrid[x][y] = -1;
			}
			else if (playerOneGrid[x][y] == 0)
			{
				printf("P2 missed P1 at: %d %d \n", x, y);
				//player two missed on empty cell
				playerOneGrid[x][y] = -1;
				//tell client two he missed (N for no)
				sendToClient(clientTwoID, "N");
				waitForAcknowledgement(clientTwoID);
				//tell client one the miss happened at (X,Y)
				miss_signal[0] = 'M';
				miss_signal[1] = x + 48;
				miss_signal[2] = y + 48;
				miss_signal[3] = '\0';
				sendToClient(clientOneID, miss_signal);
				waitForAcknowledgement(clientOneID);
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
	if (clientFromID % 2 == 1)
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
	else if (clientFromID % 2 == 0)
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
