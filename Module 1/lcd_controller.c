/*
 * lcd_controller.c
 *
 *  Created on: 2013-10-10
 *      Author: EECE381 Group 11
 */

#include "lcd_controller.h"

alt_up_character_lcd_dev* lcd;

/*
 * Initialize the LCD
 */
void initializeLCD()
{
	// open lcd
	lcd = alt_up_character_lcd_open_dev(CHARACTER_LCD_NAME);
	if (lcd == NULL)
	{
		printf("Error: could not open device\n");
	}
	else
	{
		printf("Opened LCD\n");
	}

	alt_up_character_lcd_init(lcd);
}

/*
 * Draw to the first row of the LCD
 * Takes in the string to draw
 */
void drawStatus(const char* string)
{
	clearRow(0);
	alt_up_character_lcd_set_cursor_pos(lcd, 0, 0);
	alt_up_character_lcd_string(lcd, string);
}

/*
 * Draw to the second row of the LCD
 * Takes in a string to draw
 */
void drawMode(const char* string)
{
	clearRow(1);
	alt_up_character_lcd_set_cursor_pos(lcd, 0, 1);
	alt_up_character_lcd_string(lcd, string);
}

/*
 * "Clears" a row of the LCD by writing spaces
 * Takes in the row to clear (0 or 1)
 */
void clearRow(int row)
{
	alt_up_character_lcd_set_cursor_pos(lcd, 0, row);
	alt_up_character_lcd_string(lcd, "                ");
}

