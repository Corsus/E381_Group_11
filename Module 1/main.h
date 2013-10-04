/*
 * main.h
 *
 *  Created on: 2013-10-01
 *      Author: htang
 */

#ifndef MAIN_H_
#define MAIN_H_

#include <stdio.h>
#include <unistd.h>

#include "system.h"
#include "sys/alt_irq.h"
#include "priv/alt_legacy_irq.h"
#include "altera_up_sd_card_avalon_interface.h"
#include "altera_avalon_pio_regs.h"

#include "screen_mover.h"
#include "ball_mover.h"
#include "vga_display.h"

#define SCREEN_X 320
#define SCREEN_Y 240
#define WALL_GAP 20

void initializeGame();
void initializeGameBall();

typedef struct
{
	int x_pos;
	int y_pos;
}Ball;

extern Ball gameBall;

#endif /* MAIN_H_ */
