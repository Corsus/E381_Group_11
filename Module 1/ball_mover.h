/*
 * ball_mover.h
 *
 *  Created on: 2013-09-30
 *      Author: htang
 */

#ifndef BALL_MOVER_H_
#define BALL_MOVER_H_

#include <math.h>
#include <stdlib.h>
#include <time.h>

#include "main.h"
#include "screen_mover.h"
#include "vga_display.h"

#include "altera_avalon_timer.h"
#include "altera_avalon_timer_regs.h"



void initializeBallMover();
void initialize_ball_irq();
void ball_isr(void* context);

void initialize_button_irq();
void pushbutton_isr(void* context);
void adjustPosition_isrHelper();

int detectCollision(int next_x, int next_y);


#endif /* BALL_MOVER_H_ */

