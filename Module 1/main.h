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
#include "altera_up_avalon_audio.h"

#include "screen_mover.h"
#include "ball_mover.h"
#include "vga_display.h"

#include "sd_card_controller.h"

#define SCREEN_X 320
#define SCREEN_X_PLAY 240
#define SCREEN_Y 240
#define WALL_GAP 40

#define MENU_PLAY 2
#define MENU_SCORE 1

void initializeGame();
void initializeGameBall();

void menu_controller();
void selectMenu();
void stopGame();
void waitForInput();

void switchPlayMode();
void initialize_modeSwitch_IRQ();
void modeSwitch_isr(void* context, alt_u32 id);

typedef struct
{
	int nw_x, nw_y;
	int w_x, w_y;
	int sw_x, sw_y;

	int ne_x, ne_y;
	int e_x, e_y;
	int se_x, se_y;

	int color;
}Ball;

typedef enum
{
	MAIN_MENU,
	PLAYING,
	HIGH_SCORE,
	GAME_OVER
}GameStatus;

extern Ball gameBall;
extern int mode_switch_counter;

#endif /* MAIN_H_ */
