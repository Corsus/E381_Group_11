/*
 * screen_mover.c
 *
 *  Created on: 2013-09-30
 *      Author: htang
 */
#include "screen_mover.h"

Line line1 = {SCREEN_Y,0,0,0,0,0,RED};
Line line2 = {SCREEN_Y,0,0,0,0,0,GREEN};

int screen_mover_counter = 0;
int difficulty_counter = 0;
unsigned long int game_score = 0;

void initializeScreenMover()
{
	srand((unsigned) time(NULL));

	initialize_screen_irq();
}

void initialize_screen_irq()
{
	printf("Initializing Screen Timer IRQ...\n");

	// setting up timer (Continuous, Interrupt enabled, Stopped)
	IOWR_ALTERA_AVALON_TIMER_CONTROL(SCREEN_TIMER_BASE, 0xb); //stop-start-cont-ito 1011

	IOWR_ALTERA_AVALON_TIMER_PERIODL(SCREEN_TIMER_BASE, 0x25A0);
	IOWR_ALTERA_AVALON_TIMER_PERIODH(SCREEN_TIMER_BASE, 0x0026);

	// register isr
	alt_ic_isr_register(SCREEN_TIMER_IRQ_INTERRUPT_CONTROLLER_ID, SCREEN_TIMER_IRQ,
			screen_isr, 0x0, 0x0);

	// enable interrupt
	alt_ic_irq_enable(SCREEN_TIMER_IRQ_INTERRUPT_CONTROLLER_ID, SCREEN_TIMER_IRQ);

	//start timer
	IOWR_ALTERA_AVALON_TIMER_CONTROL(SCREEN_TIMER_BASE, 0x7); //stop-start-cont-ito 0111

	printf("Screen timer started...\n");
}

void screen_isr(void* context)
{
	alt_u32 interruptible = alt_irq_interruptible(SCREEN_TIMER_IRQ);

	switch (screen_mover_counter)
	{
		case 0:
			generateRandomLine(1);
			break;
		case 120:
			generateRandomLine(2);
			break;
	}
	pushScreenUp();
	updateCounters();
	//clear timeout bit
	IOWR_ALTERA_AVALON_TIMER_STATUS(SCREEN_TIMER_BASE, 0x0);

	alt_irq_non_interruptible(interruptible);
}

void pushScreenUp()
{
	undrawLines();
	if (line1.on_screen == 1)
	{
		line1.y_pos -= SCREEN_SPEED;
		//printf("Line 1 position: %d\n", line1.y_pos);
	}

	if (line2.on_screen == 1)
	{
		line2.y_pos -= SCREEN_SPEED;
		//printf("Line 2 position: %d\n", line2.y_pos);
	}
	drawLines();
}

void updateCounters()
{
	if (screen_mover_counter < SCREEN_Y)
	{
		screen_mover_counter += SCREEN_SPEED;
	}
	else
	{
		screen_mover_counter = 0;
		difficulty_counter++;
		adjustDifficulty();
	}
	game_score++;
}

void adjustDifficulty()
{
	switch(difficulty_counter)
	{
		case 1: // 1 250 000
			IOWR_ALTERA_AVALON_TIMER_PERIODL(SCREEN_TIMER_BASE, 0x12D0);
			IOWR_ALTERA_AVALON_TIMER_PERIODH(SCREEN_TIMER_BASE, 0x0013);
			IOWR_ALTERA_AVALON_TIMER_CONTROL(SCREEN_TIMER_BASE, 0x7);
			break;
		case 5: // 900 000
			IOWR_ALTERA_AVALON_TIMER_PERIODL(SCREEN_TIMER_BASE, 0xBBA0);
			IOWR_ALTERA_AVALON_TIMER_PERIODH(SCREEN_TIMER_BASE, 0x000D);
			IOWR_ALTERA_AVALON_TIMER_CONTROL(SCREEN_TIMER_BASE, 0x7);
			break;
		case 25: // 625 000
			IOWR_ALTERA_AVALON_TIMER_PERIODL(SCREEN_TIMER_BASE, 0x8968);
			IOWR_ALTERA_AVALON_TIMER_PERIODH(SCREEN_TIMER_BASE, 0x0009);
			IOWR_ALTERA_AVALON_TIMER_CONTROL(SCREEN_TIMER_BASE, 0x7);
			break;
		case 125: // 500 000
			IOWR_ALTERA_AVALON_TIMER_PERIODL(SCREEN_TIMER_BASE, 0xA120);
			IOWR_ALTERA_AVALON_TIMER_PERIODH(SCREEN_TIMER_BASE, 0x0007);
			IOWR_ALTERA_AVALON_TIMER_CONTROL(SCREEN_TIMER_BASE, 0x7);
			break;
	}
}

void generateRandomLine(int lineNumber)
{
	Line* theLine;
	switch (lineNumber)
	{
		case 1:
			theLine = &line1;
			break;
		case 2:
			theLine = &line2;
			break;
	}

	int startGap;
	int endGap;

	startGap = (rand() % (SCREEN_X_PLAY - WALL_GAP));
	endGap = startGap + WALL_GAP;

	(*theLine).y_pos = SCREEN_Y;	//start off the screen

	(*theLine).start_x1 = 0;
	(*theLine).end_x1 = startGap;

	(*theLine).start_x2 = endGap;
	(*theLine).end_x2 = SCREEN_X_PLAY - 1;

	(*theLine).on_screen = 1;
}
