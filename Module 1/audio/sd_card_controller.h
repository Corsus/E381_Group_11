/*
 * sd_card_controller.h
 *
 *  Created on: 2013-10-08
 *      Author: htang
 */

#ifndef SD_CARD_CONTROLLER_H_
#define SD_CARD_CONTROLLER_H_


#include <stdio.h>

#include "altera_up_sd_card_avalon_interface.h"
#include "main.h"

void initializeSDCardController();
void connectToSDCard();

#endif /* SD_CARD_CONTROLLER_H_ */
