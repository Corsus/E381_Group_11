/*
 * vga_display.c
 *
 *  Created on: 2013-10-02
 *      Author: htang
 */

#include "vga_display.h"

alt_up_pixel_buffer_dma_dev* pixel_buffer;
alt_up_char_buffer_dev *char_buffer;

const char* play = "PLAY";
const char* highscore = "HIGHSCORES";
char* score_string;
char* level_string;

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
	char_buffer = alt_up_char_buffer_open_dev("/dev/character_buffer");
	alt_up_char_buffer_init(char_buffer);

	// Clear the screen
	alt_up_pixel_buffer_dma_clear_screen(pixel_buffer, 0);
}

void initializeInfoBar()
{
	alt_up_char_buffer_clear(char_buffer);
	alt_up_pixel_buffer_dma_draw_box(pixel_buffer, SCREEN_X_PLAY, 0, SCREEN_X - 1, SCREEN_Y - 1, BLUE, 0);
	alt_up_char_buffer_clear(char_buffer);
	alt_up_char_buffer_string(char_buffer, "  GAME STATUS", 60, 5);
	alt_up_char_buffer_string(char_buffer, "---------------", 60, 6);
	alt_up_char_buffer_string(char_buffer, " SCORE", 60, 10);
	alt_up_char_buffer_string(char_buffer, " LEVEL", 60, 15);
}

void drawInfoBarStats()
{
	sprintf(score_string, "%lu", game_score);
	alt_up_char_buffer_string(char_buffer, score_string, 68, 10);
	sprintf(level_string, "%lu", difficulty_counter);
	alt_up_char_buffer_string(char_buffer, level_string, 68, 15);
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
}

void drawBall()
{
	alt_up_pixel_buffer_dma_draw_box(pixel_buffer,
			gameBall.nw_x, gameBall.nw_y,
			gameBall.se_x, gameBall.se_y,
			WHITE, 0);
}

void undrawBall()
{
	alt_up_pixel_buffer_dma_draw_box(pixel_buffer,
			gameBall.nw_x, gameBall.nw_y,
			gameBall.se_x, gameBall.se_y,
			BLACK, 0);
}


void drawMenu()
{
	//clear screen
	clearScreen();

	//draw menu title

	//F
	alt_up_pixel_buffer_dma_draw_vline(pixel_buffer, 50, 50, 130, WHITE, 0);
	alt_up_pixel_buffer_dma_draw_hline(pixel_buffer, 50, 75, 50, WHITE, 0);
	alt_up_pixel_buffer_dma_draw_hline(pixel_buffer, 50, 65, 80, WHITE, 0);

	//A
	alt_up_pixel_buffer_dma_draw_vline(pixel_buffer, 80, 80, 130, WHITE, 0);
	alt_up_pixel_buffer_dma_draw_hline(pixel_buffer, 80, 105, 80, WHITE, 0);
	alt_up_pixel_buffer_dma_draw_hline(pixel_buffer, 80, 105, 100, WHITE, 0);
	alt_up_pixel_buffer_dma_draw_vline(pixel_buffer, 105, 80, 130, WHITE, 0);

	//L
	alt_up_pixel_buffer_dma_draw_vline(pixel_buffer, 110, 80, 130, WHITE, 0);
	alt_up_pixel_buffer_dma_draw_hline(pixel_buffer, 110, 120, 130, WHITE, 0);

	//L
	alt_up_pixel_buffer_dma_draw_vline(pixel_buffer, 130, 80, 130, WHITE, 0);
	alt_up_pixel_buffer_dma_draw_hline(pixel_buffer, 130, 140, 130, WHITE, 0);

	alt_up_pixel_buffer_dma_draw_vline(pixel_buffer, 145, 50, 130, WHITE, 0);
	alt_up_pixel_buffer_dma_draw_hline(pixel_buffer, 145, 170, 50, WHITE, 0);
	alt_up_pixel_buffer_dma_draw_hline(pixel_buffer, 145, 170, 130, WHITE, 0);
	alt_up_pixel_buffer_dma_draw_vline(pixel_buffer, 170, 50, 130, WHITE, 0);

	alt_up_pixel_buffer_dma_draw_vline(pixel_buffer, 175, 80, 130, WHITE, 0);
	alt_up_pixel_buffer_dma_draw_hline(pixel_buffer, 175, 200, 80, WHITE, 0);
	alt_up_pixel_buffer_dma_draw_hline(pixel_buffer, 175, 200, 130, WHITE, 0);
	alt_up_pixel_buffer_dma_draw_vline(pixel_buffer, 200, 80, 130, WHITE, 0);

	alt_up_pixel_buffer_dma_draw_vline(pixel_buffer, 205, 80, 130, WHITE, 0);
	alt_up_pixel_buffer_dma_draw_vline(pixel_buffer, 215, 80, 130, WHITE, 0);
	alt_up_pixel_buffer_dma_draw_vline(pixel_buffer, 225, 80, 130, WHITE, 0);
	alt_up_pixel_buffer_dma_draw_hline(pixel_buffer, 205, 225, 130, WHITE, 0);

	alt_up_pixel_buffer_dma_draw_vline(pixel_buffer, 230, 80, 130, WHITE, 0);
	alt_up_pixel_buffer_dma_draw_vline(pixel_buffer, 255, 80, 130, WHITE, 0);
	alt_up_pixel_buffer_dma_draw_hline(pixel_buffer, 230, 255, 80, WHITE, 0);
	//finish drawing menu title

	alt_up_char_buffer_string(char_buffer, "Navigate the ball left and right to avoid the obstacles.", 10, 40);
	alt_up_char_buffer_string(char_buffer, "Game is over when the ball goes off the top.", 10, 41);
	alt_up_char_buffer_string(char_buffer, "The longer you survive, the higher your score!", 10, 42);
	alt_up_char_buffer_string(char_buffer, "Good luck and have fun! - EECE381 G11", 10, 44);

	// update menu selections
	update_menu_selection(2);
	alt_up_char_buffer_string(char_buffer, play, 20, 50);
	alt_up_char_buffer_string(char_buffer, highscore, 60, 50);
}

void update_menu_selection(int direction)
{
	if (direction == MENU_SCORE)
	{
		alt_up_char_buffer_draw(char_buffer, ' ', 15, 50);
		alt_up_char_buffer_draw(char_buffer, '>', 55, 50);
	}
	else if (direction == MENU_PLAY)
	{
		alt_up_char_buffer_draw(char_buffer, '>', 15, 50);
		alt_up_char_buffer_draw(char_buffer, ' ', 55, 50);
	}
}

void drawGameOverScreen()
{
	//clears the game screen
	clearScreen();

	//draws GAMEOVER screen
	alt_up_char_buffer_string(char_buffer, "   G   A   M   E        O   V   E   R   ", 19, 20);
	alt_up_char_buffer_string(char_buffer, "SCORE:", 30, 25);
	alt_up_char_buffer_string(char_buffer, "LEVEL:", 30, 30);
	sprintf(score_string, "%lu", game_score);
	alt_up_char_buffer_string(char_buffer, score_string, 45, 25);
	sprintf(level_string, "%lu", difficulty_counter);
	alt_up_char_buffer_string(char_buffer, level_string, 45, 30);
	alt_up_char_buffer_string(char_buffer, "PRESS START BUTTON TO CONTINUE", 25, 40);
	//completed
}

//function to clear pixel buffer and char buffer
void clearScreen()
{
	alt_up_pixel_buffer_dma_clear_screen(pixel_buffer, 0);
	alt_up_char_buffer_clear(char_buffer);
}
