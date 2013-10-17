/*
 * sound_manager.h
 *
 *  Created on: 2013-10-08
 *      Author: EECE381 G11
 */

#ifndef SOUND_MANAGER_H_
#define SOUND_MANAGER_H_


#include "altera_up_avalon_audio.h"
#include "altera_up_avalon_audio_regs.h"
#include "altera_up_avalon_audio_and_video_config.h"
#include "altera_up_avalon_audio_and_video_config_regs.h"

#include "main.h"

#define HEADER_SIZE 44
#define SAMPLE_SIZE 96
#define GREET_SIZE 68442
#define LOSE_SIZE 72674
#define CONGRAT_SIZE 62476
#define BACKGROUND_SIZE 548236

void av_config_setup();
void initialize_audio();
void initialWriteToFIFO();

void initialize_audio_irq();
void audio_isr(void* context, alt_u32 id);
void load_sound_data();

void greet_isr(void* context, alt_u32 id);
void congrat_isr(void* context, alt_u32 id);
void lose_isr(void* context, alt_u32 id);
void background_isr(void* context, alt_u32 id);

void playGreet();
void playLose();
void playBackground();
void playCongrat();
void disableSound();


#endif /* SOUND_MANAGER_H_ */
