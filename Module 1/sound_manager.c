/*
 * sound_manager.c
 *
 *  Created on: 2013-10-08
 *      Author: htang
 */

#include "sound_manager.h"

alt_up_audio_dev * audio_dev;
volatile unsigned int* sound_data;	//store all sound data
volatile unsigned int* sound_buff;	//pointer to sound data for writing to FIFO

unsigned int byte_data[2];
unsigned int sound_data_counter = 0;

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
	int sample_counter;
	for (sample_counter = 0; sample_counter < SAMPLE_SIZE; sample_counter++)
	{
		// take 2 bytes at a time
		byte_data[0] = sound_data[sound_data_counter];
		byte_data[1] = sound_data[sound_data_counter+1];
		sound_data_counter += 2;

		// combine the two bytes and store into sample buffer
		sound_buff[sample_counter] = (byte_data[1] << 8) | byte_data[0];

		// if we finish reading our data buffer, then we loop back to start over
		if (sound_data_counter >= AUDIO_SIZE)
		{
			sound_data_counter = 0;
		}
	}
	// finally, we write this sample data to the FIFO
	alt_up_audio_write_fifo(audio_dev, sound_buff, SAMPLE_SIZE, ALT_UP_AUDIO_LEFT);
	alt_up_audio_write_fifo(audio_dev, sound_buff, SAMPLE_SIZE, ALT_UP_AUDIO_RIGHT);
}

// opens the sound file and loads it into memory
void load_sound_data()
{
	short int file_handle;
	int loop_counter;

	printf("Reading sound file...\n");
	file_handle = alt_up_sd_card_fopen("sample14.wav", false);

	// create large buffer to store all sound data
	sound_data = (unsigned int*) malloc(AUDIO_SIZE * sizeof(unsigned int));

	// skip header
	for (loop_counter = 0; loop_counter < HEADER_SIZE; loop_counter++)
	{
		alt_up_sd_card_read(file_handle);
	}

	// read and store sound data into memory
	for (loop_counter = 0; loop_counter < AUDIO_SIZE; loop_counter++)
	{
		sound_data[loop_counter] = alt_up_sd_card_read(file_handle);
	}

	printf("Read completed...\n");

	alt_up_sd_card_fclose(file_handle);

	// create buffer for storing samples from sound_data
	sound_buff = (unsigned int*) malloc(SAMPLE_SIZE * sizeof(unsigned int));
}
