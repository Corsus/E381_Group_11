/*
 * Communication.h
 *
 *  Created on: 2013-11-13
 *      Author: htang
 */

#ifndef COMMUNICATION_H_
#define COMMUNICATION_H_

#include "main.h"
#include "altera_up_avalon_rs232.h"
#include <string.h>
#include <stdlib.h>


void initializeRS232();
void sendToClient(int clientID, char* msg);
void receiveFromClient();

void waitForClientsToSetup();
int isClientReady();

void handleFireCommandFromClient();

extern char* msgReceived;
extern int clientFromID;
extern int numReceived;

#endif /* COMMUNICATION_H_ */
