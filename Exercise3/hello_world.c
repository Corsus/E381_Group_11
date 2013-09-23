/*
 * "Hello World" example.
 *
 * This example prints 'Hello from Nios II' to the STDOUT stream. It runs on
 * the Nios II 'standard', 'full_featured', 'fast', and 'low_cost' example
 * designs. It runs with or without the MicroC/OS-II RTOS and requires a STDOUT
 * device in your system's hardware.
 * The memory footprint of this hosted application is ~69 kbytes by default
 * using the standard reference design.
 *
 * For a reduced footprint version of this template, and an explanation of how
 * to reduce the memory footprint for a given application, see the
 * "small_hello_world" template.
 *
 */

#include <stdio.h>
#include "altera_up_sd_card_avalon_interface.h"

int main()
{
	alt_up_sd_card_dev* sd_card_reference = NULL;
	int connected = 0;
	char* file_name;

	//initialize sd card
	sd_card_reference = alt_up_sd_card_open_dev("/dev/sd_card");

	if (sd_card_reference != NULL)
	{
		// repeated check the connectivity of the SD CARD
		while (alt_up_sd_card_is_Present() == false);
		if (alt_up_sd_card_is_Present())
		{
			printf("Card connected.\n");
			if (alt_up_sd_card_is_FAT16())
			{
				printf("FAT16 file system detected.\n");
				if (alt_up_sd_card_find_first("", file_name) == 0)
				{
					printf("%s\n", file_name);
					while (alt_up_sd_card_find_next(file_name) == 0)
					{
						printf("%s\n", file_name);
					}
					printf("Success: read completed.\n");
				}
				else
				{
					printf("Error: didn't read anything.\n");
				}
			}
			else
			{
				printf("Unknown file system.\n");
			}
			printf("Terminating.\n");
		}
	}

	return 0;
}
