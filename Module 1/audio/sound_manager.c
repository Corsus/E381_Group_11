/*
 * sound_manager.c
 *
 *  Created on: 2013-10-08
 *      Author: htang
 */

#include "sound_manager.h"

alt_up_audio_dev * audio_dev;
int sound_data[20000];	//store all sound data
int* sound_buff;		//pointer to sound data for writing to FIFO

void av_config_setup()
{
	alt_up_av_config_dev * av_config = alt_up_av_config_open_dev(AUDIO_AND_VIDEO_CONFIG_NAME);
	while (!alt_up_av_config_read_ready(av_config)) {}
}

void initialize_audio()
{
	//initial setup for AV
	av_config_setup();

	// open the Audio port
	audio_dev = alt_up_audio_open_dev(AUDIO_NAME);
	if ( audio_dev == NULL)
		printf ("Error: could not open audio device \n");
	else
		printf ("Opened audio device \n");

	//connect to SD card
	connectToSDCard();
	//load sound data into our device
	load_sound_data();
	//initial write to FIFO (fill up FIFO)
	initialWriteToFIFO();
	//initialize audio interrupt
	initialize_audio_irq();
}


//initialize irq for audio write
void initialize_audio_irq()
{
	printf("Initializing Audio IRQ...\n");

	// register isr
	alt_irq_register(AUDIO_IRQ, 0x0, audio_isr);

	// enable interrupt
	alt_irq_enable(AUDIO_IRQ);

	alt_up_audio_disable_read_interrupt(audio_dev);
	alt_up_audio_enable_write_interrupt(audio_dev);

	printf("Audio IRQ initialized...\n");
}

void audio_isr(void* context, alt_u32 id)
{
	// FIFO is 75% empty, need to fill it up
	refillFIFO();
}

void load_sound_data()
{
	char byte_a;	//byte read from sd card
	char byte_b;	//byte read from sd card
	short combined_c;	//combined byte
	int result_d;		//resulting byte stored

	char* file_name;
	int file_handle;
	int header_counter = 0;		//loop counter to skip header
	int load_counter = 0;		//loop counter to load sound data

	printf("Reading sound file...\n");
	if (alt_up_sd_card_find_first("", file_name) == 0)
	{
		printf("%s\n", file_name);
		file_handle = alt_up_sd_card_fopen("sound.wav", false);

		// skip header
		while (header_counter < 44)
		{
			alt_up_sd_card_read(file_handle);
			header_counter++;
		}

		// read and store sound data into memory
		while (load_counter < 20000)
		{
			//read 2 bytes from file
			byte_a = alt_up_sd_card_read(file_handle);
			byte_b = alt_up_sd_card_read(file_handle);
			//combine the 2 bytes into 2 bytes (to perform arithmetic operations)
			combined_c = ((unsigned char) byte_a << 8) | (unsigned char) byte_b;
			combined_c = combined_c / 2;
			//store 2 bytes into 4 bytes (required by API)
			result_d = combined_c;
			printf("%d: %d\n", load_counter, result_d);
			//store the resulting 4 bytes into our Data Buffer
			sound_data[load_counter] = result_d;
			load_counter++;
		}
		sound_buff = &sound_data[0];
	}
	else
	{
		printf("Error: didn't read anything.\n");
	}
}

void initialWriteToFIFO()
{
	int fifospace = alt_up_audio_write_fifo_space(audio_dev, ALT_UP_AUDIO_RIGHT);
	if (fifospace == 0)
	{
		alt_up_audio_write_fifo(audio_dev, sound_buff, 128, ALT_UP_AUDIO_LEFT);
		alt_up_audio_write_fifo(audio_dev, sound_buff, 128, ALT_UP_AUDIO_RIGHT);
	}
}

void refillFIFO()
{
	if (sound_buff > &sound_data[19800])
	{
		sound_buff = &sound_data[0];
	}
	else
	{
		sound_buff = sound_buff + 90;
	}
	alt_up_audio_write_fifo(audio_dev, sound_buff, 90, ALT_UP_AUDIO_LEFT);
	alt_up_audio_write_fifo(audio_dev, sound_buff, 90, ALT_UP_AUDIO_RIGHT);
}

