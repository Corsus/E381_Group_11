#include <stdio.h>
#include "sys/alt_alarm.h"
#include "sys/alt_timestamp.h"

void multiply(int a[][100], int b[][100]);
void timer_test(void);
void test();

void timer_test(void)
{
	int freq;
	int cycles;
	float duration;
	int ticks_start;
	int ticks_end;
	int ticks_per_s;
	int ticks_duration;

	printf("Timers\n");

	printf(" Sys Clock Timer\n");
	ticks_per_s = alt_ticks_per_second();
	printf(" Tick Freq: %d\n", ticks_per_s);
	printf(" Recording starting ticks\n");
	ticks_start = alt_nticks();
	printf(" Sleeping for 5 seconds\n");
	usleep(5000000);
	printf(" Recording ending ticks\n");
	ticks_end = alt_nticks();
	ticks_duration = ticks_end - ticks_start;
	duration = (float) ticks_duration / (float) ticks_per_s;
	printf(" The program slept for %d ticks (%f seconds)\n\n", ticks_duration, duration);

	printf(" Timestamp Timer\n");
	freq = alt_timestamp_freq();
	printf(" CPU Freq: %d\n", freq);
	printf(" Resetting Timestamp timer\n");
	alt_timestamp_start();
	printf(" ...Timing the print of this statement...\n");
	cycles = alt_timestamp();
	duration = (float) cycles / (float) freq;
	printf(" It took %d cycles (%f seconds) to print the statement\n\n", cycles, duration);
}

void multiply(int a[][100], int b[][100])
{
	int i;
	int j;
	int k;
	int c[100][100];
	int sum;
	unsigned int start_time = 0;
	unsigned int end_time = 0;

	alt_timestamp_start();
	printf("Recording starting time: %u\n", start_time);
	start_time = (unsigned int) alt_timestamp();

	for (i = 0; i < 100; i++)
	{
		for (j = 0; j < 100; j++)
		{
			sum = 0;
			for (k = 0; k < 100; k++)
			{
				sum = sum + a[i][k] * b[k][j];
			}
			c[i][j] = sum;
		}
	}

	end_time = (unsigned int) alt_timestamp();
	printf("Recording ending time: %u\n", end_time);

	printf("Time taken: %u clock ticks\n", end_time - start_time);
	printf("            %f seconds\n", (float) (end_time - start_time) / (float)alt_timestamp_freq());
}

void test()
{
	int i;
	int j;

	int a[100][100];
	int b[100][100];

	for (i = 0; i < 100; i++)
	{
		for (j = 0; j < 100; j++)
		{
			a[i][j] = 2;
		}
	}

	for (i = 0; i < 100; i++)
	{
		for (j = 0; j < 100; j++)
		{
			b[i][j] = 3;
		}
	}
	multiply(a,b);
}

int main()
{
  printf("Hello from Nios II!\n");
  timer_test();
  test();
  return 0;
}
