/*
 * vga_display.h
 *
 *  Created on: 2013-10-02
 *      Author: htang
 */

#ifndef VGA_DISPLAY_H_
#define VGA_DISPLAY_H_

#include "altera_up_avalon_video_pixel_buffer_dma.h"
#include "altera_up_avalon_video_character_buffer_with_dma.h"

#include "system.h"

#include "screen_mover.h"
#include "main.h"

#define WHITE 0xFFFF
#define RED 0xf800
#define YELLOW 0x07ff
#define BLUE 0x001f
#define BLACK 0x0000

void initializeVgaDisplay();
void drawLines();
void undrawLines();
void drawBall();
void undrawBall();

void drawMenu();


#endif /* VGA_DISPLAY_H_ */
