/*
 * Highscore.h
 *
 *  Created on: 2013-10-08
 *      Author: Corsus
 */
 
#ifndef HIGHSCORE_H_
#define HIGHSCORE_H_

#include <stdio.h>
#include "altera_up_sd_card_avalon_interface.h"
#include "string.h"

#define FILE_NAME "HS.TXT"


bool writeHighscore(int);
int readHighscore(int);

#endif
