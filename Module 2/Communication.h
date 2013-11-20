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
#include "altera_avalon_timer.h"
#include "altera_avalon_timer_regs.h"
#include "priv/alt_legacy_irq.h"


void initializeRS232();
void sendToClient(int clientID, char* msg);
void receiveFromClient();

void waitForAcknowledgement(int clientID);
void initializeReadTimerInterrupt();
void enableReadTimerInterrupt();
void disableReadTimerInterrupt();
void read_timer_isr();

void waitForClientsToSetup();
int isClientReady();

void handleFireCommandFromClientOne();
void handleFireCommandFromClientTwo();

extern char* msgReceived;
extern int clientFromID;
extern int numReceived;

#endif /* COMMUNICATION_H_ */
