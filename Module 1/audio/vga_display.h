/*
 * vga_display.h
 *
 *  Created on: 2013-10-02
 *      Author: htang
 */

#ifndef VGA_DISPLAY_H_
#define VGA_DISPLAY_H_

#include <stdio.h>
#include <math.h>

#include "altera_up_avalon_video_pixel_buffer_dma.h"
#include "altera_up_avalon_video_character_buffer_with_dma.h"

#include "system.h"

#include "screen_mover.h"
#include "main.h"

#define WHITE 0xFFFF
#define RED 0xF800
#define GREEN 0x07E0
#define BLUE 0x001F
#define BLACK 0x0000

void initializeVgaDisplay();
void initializeInfoBar();
void drawInfoBarStats();
void drawLines();
void undrawLines();
void drawBall();
void undrawBall();
void clearPlayScreen();
void reverseClearPlayScreen();

void drawMenu();
void update_menu_selection(int direction);
void clearScreen();
void drawGameOverScreen();

extern int undraw_color;

#endif /* VGA_DISPLAY_H_ */
