/*
 * screen_mover.h
 *
 *  Created on: 2013-09-30
 *      Author: htang
 */

#ifndef SCREEN_MOVER_H_
#define SCREEN_MOVER_H_

#include <math.h>
#include <stdlib.h>
#include <time.h>

#include "altera_avalon_timer.h"
#include "altera_avalon_timer_regs.h"

#include "main.h"
#include "vga_display.h"

#define SCREEN_SPEED 2

void initializeScreenMover();
void initialize_screen_irq();
void screen_isr(void* context);
void pushScreenUp();
void updateCounters();
void generateRandomLine(int lineNumber);

void adjustDifficulty();

typedef struct
{
	int y_pos;

	int start_x1;
	int end_x1;
	int start_x2;
	int end_x2;

	int on_screen;
	unsigned long int lineColor;
}Line;

extern Line line1;
extern Line line2;

extern unsigned long int game_score;
extern unsigned long int difficulty_counter;

#endif /* SCREEN_MOVER_H_ */