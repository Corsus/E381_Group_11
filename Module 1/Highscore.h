/*
 * Highscore.h
 *
 *  Created on: 2013-10-08
 *      Author: EECE381 G11
 */
 
#ifndef HIGHSCORE_H_
#define HIGHSCORE_H_

#include <stdio.h>
#include "altera_up_sd_card_avalon_interface.h"
#include "string.h"

#define FILE_NAME "HS.TXT"

void writeHighscore(int);
int readHighscore(int);

#endif
