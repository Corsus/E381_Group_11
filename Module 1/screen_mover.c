/*
 * screen_mover.c
 *
 *  Created on: 2013-09-30
 *      Author: EECE381 Group 11
 */
#include "screen_mover.h"

// lines on the screen
Line line1;
Line line2;

// line position counters
int screen_mover_counter;

// stat counters
int difficulty_counter;
int game_score;

/*
 * initialization of screen mover
 */
void initializeScreenMover()
{
	//initialize our background color
	undraw_color = BLACK;

	initializeScreenObjects();

	screen_mover_counter = 0;
	difficulty_counter = 1;
	game_score = 0;

	srand((unsigned) time(NULL));

	initialize_screen_irq();
}

/*
 * initialize screen objects (the two lines)
 */
void initializeScreenObjects()
{
	line1.y_pos = SCREEN_Y;
	line1.start_x1 = 0;
	line1.end_x1 = 0;
	line1.start_x2 = 0;
	line1.end_x2 = 0;
	line1.on_screen = 0;
	line1.lineColor = RED;
	line2.y_pos = SCREEN_Y;
	line2.start_x1 = 0;
	line2.end_x1 = 0;
	line2.start_x2 = 0;
	line2.end_x2 = 0;
	line2.on_screen = 0;
	line2.lineColor = GREEN;
}

/*
 * initialize the screen mover irq
 */
void initialize_screen_irq()
{
	printf("Initializing Screen Timer IRQ...\n");

	// setting up timer (Continuous, Interrupt enabled, Stopped)
	IOWR_ALTERA_AVALON_TIMER_CONTROL(SCREEN_TIMER_BASE, 0xb); //stop-start-cont-ito 1011

	// 1 250 000
	IOWR_ALTERA_AVALON_TIMER_PERIODL(SCREEN_TIMER_BASE, 0x12D0);
	IOWR_ALTERA_AVALON_TIMER_PERIODH(SCREEN_TIMER_BASE, 0x0013);

	// register isr
	alt_irq_register(SCREEN_TIMER_IRQ, 0x0, screen_isr);

	// enable interrupt
	alt_irq_enable(SCREEN_TIMER_IRQ);

	//start timer
	IOWR_ALTERA_AVALON_TIMER_CONTROL(SCREEN_TIMER_BASE, 0x7); //stop-start-cont-ito 0111

	printf("Screen timer started...\n");
}

/*
 * Handler for the interrupt
 */
void screen_isr(void* context, alt_u32 id)
{
	alt_u32 interruptible = alt_irq_interruptible(SCREEN_TIMER_IRQ);

	switch (screen_mover_counter)
	{
		case 0:
			generateRandomLine(&line1);
			break;
		case 120:
			generateRandomLine(&line2);
			break;
	}
	pushScreenUp();
	updateCounters();
	//clear timeout bit
	IOWR_ALTERA_AVALON_TIMER_STATUS(SCREEN_TIMER_BASE, 0x0);

	alt_irq_non_interruptible(interruptible);
}

/*
 * Method to update the line positions and animate the movement
 */
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

/*
 * Method to update the stat counters
 */
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
	drawInfoBarStats();
}

/*
 * Method to increase the difficulty over time
 */
void adjustDifficulty()
{
	switch(difficulty_counter)
	{
		case 10: // 900 000
			drawStatus("Owning!");
			IOWR_ALTERA_AVALON_TIMER_PERIODL(SCREEN_TIMER_BASE, 0xBBA0);
			IOWR_ALTERA_AVALON_TIMER_PERIODH(SCREEN_TIMER_BASE, 0x000D);
			IOWR_ALTERA_AVALON_TIMER_CONTROL(SCREEN_TIMER_BASE, 0x7);
			break;
		case 20: // 625 000
			drawStatus("Dominating!!");
			IOWR_ALTERA_AVALON_TIMER_PERIODL(SCREEN_TIMER_BASE, 0x8968);
			IOWR_ALTERA_AVALON_TIMER_PERIODH(SCREEN_TIMER_BASE, 0x0009);
			IOWR_ALTERA_AVALON_TIMER_CONTROL(SCREEN_TIMER_BASE, 0x7);
			break;
		case 40: // 500 000
			drawStatus("RAMPAGE!!!");
			IOWR_ALTERA_AVALON_TIMER_PERIODL(SCREEN_TIMER_BASE, 0xA120);
			IOWR_ALTERA_AVALON_TIMER_PERIODH(SCREEN_TIMER_BASE, 0x0007);
			IOWR_ALTERA_AVALON_TIMER_CONTROL(SCREEN_TIMER_BASE, 0x7);
			break;
		case 80: // 300 000
			drawStatus("G O D L I K E");
			IOWR_ALTERA_AVALON_TIMER_PERIODL(SCREEN_TIMER_BASE, 0x93E0);
			IOWR_ALTERA_AVALON_TIMER_PERIODH(SCREEN_TIMER_BASE, 0x0004);
			IOWR_ALTERA_AVALON_TIMER_CONTROL(SCREEN_TIMER_BASE, 0x7);
			break;
	}

	// change the playing mode every 15 levels
	if (difficulty_counter % 15 == 0)
	{
		switchPlayMode();
	}
}

/*
 * Method to randomize the gaps of the lines
 */
void generateRandomLine(Line *theLine)
{
	int startGap;
	int endGap;

	startGap = (rand() % (SCREEN_X_PLAY - WALL_GAP));
	endGap = startGap + WALL_GAP;

	theLine->y_pos = SCREEN_Y;	//start off the screen

	theLine->start_x1 = 0;
	theLine->end_x1 = startGap;

	theLine->start_x2 = endGap;
	theLine->end_x2 = SCREEN_X_PLAY - 1;

	theLine->on_screen = 1;
}
