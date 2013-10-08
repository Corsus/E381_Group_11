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
#include "altera_avalon_pio_regs.h"

#define VERTICAL_MOVE 0
#define HORIZONTAL_MOVE_LEFT 1
#define HORIZONTAL_MOVE_RIGHT 2
#define VERTICAL_SPEED 1
#define HORIZONTAL_SPEED 3

void initializeBallMover();
void initialize_ball_irq();
void ball_isr(void* context);
void enableTimerInterrupt();
void disableTimerInterrupt();

void handleControllerInput();

void detectCollision(int movement);
void handleCollision(int movement, int line_number);
void updateBallPosition(int movement);

#endif /* BALL_MOVER_H_ */

