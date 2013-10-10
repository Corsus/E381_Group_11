/*
 * sd_card_controller.c
 *
 *  Created on: 2013-10-08
 *      Author: htang
 */

#include "sd_card_controller.h"

alt_up_sd_card_dev* sd_card_reference;

void initializeSDCardController()
{
	sd_card_reference = alt_up_sd_card_open_dev(SD_CARD_NAME);
}
void connectToSDCard()
{
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
			}
			else
			{
				printf("Unknown file system.\n");
			}
		}
	}
}
