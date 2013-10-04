/*
 * vga_display.c
 *
 *  Created on: 2013-10-02
 *      Author: htang
 */

#include "vga_display.h"

alt_up_pixel_buffer_dma_dev* pixel_buffer;
alt_up_char_buffer_dev *char_buffer;

void initializeVgaDisplay()
{
	//initialize pixel buffer
	pixel_buffer = alt_up_pixel_buffer_dma_open_dev(BUFFER_DMA_NAME);

	// Set the background buffer address
	alt_up_pixel_buffer_dma_change_back_buffer_address(pixel_buffer, PIXEL_BUFFER_BASE);

	// Swap background and foreground buffers
	alt_up_pixel_buffer_dma_swap_buffers(pixel_buffer);

	// Wait for the swap to complete
	while (alt_up_pixel_buffer_dma_check_swap_buffers_status(pixel_buffer));

	//initialize character buffer
	char_buffer = alt_up_char_buffer_open_dev(CHARACTER_LCD_NAME);
	alt_up_char_buffer_init(char_buffer);

	// Clear the screen
	alt_up_pixel_buffer_dma_clear_screen(pixel_buffer, 0);
}

void drawLines()
{
	//draw new lines
	if (line1.on_screen == 1)
	{
		alt_up_pixel_buffer_dma_draw_hline(pixel_buffer, line1.start_x1, line1.end_x1, line1.y_pos, line1.lineColor, 0);
		alt_up_pixel_buffer_dma_draw_hline(pixel_buffer, line1.start_x2, line1.end_x2, line1.y_pos, line1.lineColor, 0);
	}
	if (line2.on_screen == 1)
	{
		alt_up_pixel_buffer_dma_draw_hline(pixel_buffer, line2.start_x1, line2.end_x1, line2.y_pos, line2.lineColor, 0);
		alt_up_pixel_buffer_dma_draw_hline(pixel_buffer, line2.start_x2, line2.end_x2, line2.y_pos, line2.lineColor, 0);
	}
	if (line3.on_screen == 1)
	{
		alt_up_pixel_buffer_dma_draw_hline(pixel_buffer, line3.start_x1, line3.end_x1, line3.y_pos, line3.lineColor, 0);
		alt_up_pixel_buffer_dma_draw_hline(pixel_buffer, line3.start_x2, line3.end_x2, line3.y_pos, line3.lineColor, 0);
	}
}

void undrawLines()
{
	if (line1.on_screen == 1)
	{
		alt_up_pixel_buffer_dma_draw_hline(pixel_buffer, line1.start_x1, line1.end_x1, line1.y_pos, BLACK, 0);
		alt_up_pixel_buffer_dma_draw_hline(pixel_buffer, line1.start_x2, line1.end_x2, line1.y_pos, BLACK, 0);
	}
	if (line2.on_screen == 1)
	{
		alt_up_pixel_buffer_dma_draw_hline(pixel_buffer, line2.start_x1, line2.end_x1, line2.y_pos, BLACK, 0);
		alt_up_pixel_buffer_dma_draw_hline(pixel_buffer, line2.start_x2, line2.end_x2, line2.y_pos, BLACK, 0);
	}
	if (line3.on_screen == 1)
	{
		alt_up_pixel_buffer_dma_draw_hline(pixel_buffer, line3.start_x1, line3.end_x1, line3.y_pos, BLACK, 0);
		alt_up_pixel_buffer_dma_draw_hline(pixel_buffer, line3.start_x2, line3.end_x2, line3.y_pos, BLACK, 0);
	}
}

void drawBall()
{
	int x = gameBall.x_pos;
	int y = gameBall.y_pos;
	alt_up_pixel_buffer_dma_draw_box(pixel_buffer, x - 1, y - 1, x + 1, y + 1, WHITE, 0);
}

void undrawBall()
{
	int x = gameBall.x_pos;
	int y = gameBall.y_pos;
	alt_up_pixel_buffer_dma_draw_box(pixel_buffer, x - 1, y - 1, x + 1, y + 1, BLACK, 0);
}

void drawMenu()
{
	const char* play = "PLAY";
	const char* instructions = "INSTRUCTIONS";

	//clear screen
	alt_up_pixel_buffer_dma_clear_screen(pixel_buffer, 0);

	alt_up_char_buffer_string(char_buffer, play, 90, 160);
	alt_up_char_buffer_string(char_buffer, instructions, 250, 160);
}
