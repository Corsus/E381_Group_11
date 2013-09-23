#include <stdio.h>
#include "altera_up_avalon_character_lcd.h"
#include "lcd.h"

#define keys (volatile char*) 0x00002000

int main()
{
	alt_up_character_lcd_dev* lcd;

	// open lcd
	lcd = alt_up_character_lcd_open_dev("/dev/character_lcd");
	if (lcd == NULL)
	{
	  alt_printf("Error: could not open device\n");
	}
	else
	{
	  alt_printf("Opened the device\n");
	}

	//initialize
	alt_up_character_lcd_init(lcd);

	while(1)
	{
		if (!((*keys) & 0x1))
		{
			alt_up_character_lcd_set_cursor_pos(lcd, 0, 0);
			alt_up_character_lcd_string(lcd, "0");
		}
		else
		{
			alt_up_character_lcd_set_cursor_pos(lcd, 0, 0);
			alt_up_character_lcd_string(lcd, " ");
		}

		if (!((*keys) & 0x2))
		{
			alt_up_character_lcd_set_cursor_pos(lcd, 0, 1);
			alt_up_character_lcd_string(lcd, "1");
		}
		else
		{
			alt_up_character_lcd_set_cursor_pos(lcd, 0, 1);
			alt_up_character_lcd_string(lcd, " ");
		}

		if (!((*keys) & 0x4))
		{
			alt_up_character_lcd_set_cursor_pos(lcd, 15, 0);
			alt_up_character_lcd_string(lcd, "2");
		}
		else
		{
			alt_up_character_lcd_set_cursor_pos(lcd, 15, 0);
			alt_up_character_lcd_string(lcd, " ");
		}

		if (!((*keys) & 0x8))
		{
			alt_up_character_lcd_set_cursor_pos(lcd, 15, 1);
			alt_up_character_lcd_string(lcd, "3");
		}
		else
		{
			alt_up_character_lcd_set_cursor_pos(lcd, 15, 1);
			alt_up_character_lcd_string(lcd, " ");
		}
	}

	return 0;
}
