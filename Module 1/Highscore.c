/*
 * Highscore.c
 *
 *  Created on: 2013-10-08
 *      Author: EECE381 G11
 */
 
#include "Highscore.h"

//Writes into the highscore file
void writeHighscore (int score)
{
	int i,j,k,size;
	char temp[10];
	int Highscore[10] = {0,0,0,0,0,0,0,0,0,0};
	int HighscoreFinal[10] = {0,0,0,0,0,0,0,0,0,0};

	printf("Writing high score to storage device...\n");

	// read all the high scores
	// store them in array
	for (j = 0; j < 10; j++)
	{
		Highscore[j] = readHighscore(j+1);
		HighscoreFinal[j] = Highscore[j];
	}
	printf("Reading current high scores completed...\n");

	// check to see how many scores our current score is bigger
	for (k = 0; k < 10; k++)
	{
		if (score > Highscore[k])
		{
			break;
		}
	}

	// if k >= 10, this is not a high score, and we skip the entire process
	if (k < 10)
	{
		// copy over all values that are bigger than current score
		for (j = 0; j < k; j++)
		{
			HighscoreFinal[j] = Highscore[j];
		}
		// insert our new high score
		HighscoreFinal[k] = score;

		// copy over the remaining scores, while kicking out the last high score
		for (j = k+1; j < 10; j++)
		{
			HighscoreFinal[j] = Highscore[j-1];
		}
		printf("Updated highscores...\n");

		//open our score file
		int file_handle;
		file_handle = alt_up_sd_card_fopen(FILE_NAME, false);
		for(j = 0; j < 10; j++)
		{
			//write 6 digits + a space per high score = 7 bytes
			sprintf(temp, "%.6d ", HighscoreFinal[j]);

			size = strlen(temp);
			for(i = 0; i < size; i++)
			{
				alt_up_sd_card_write(file_handle, temp[i]);
			}
		}
		alt_up_sd_card_fclose(file_handle); //close the file

		printf("Write completed...\n");
	}
}

int readHighscore(int index) //fully working. Index of 1 means number 1 on highscoreboard
{
	int i, j, score;
	int file_handle = alt_up_sd_card_fopen(FILE_NAME, false);
	char temp[] = "000000";		//6 digits + 1 terminating char = 7 elements
	//char temp[10];

	// Highscores go from 1 to 10. We reject any other indices by returning -1
	if (index <= 0 || index > 10)
		return -1;

	if(file_handle == -1)
	{
		printf("File could not be opened.\n");
		alt_up_sd_card_fclose(file_handle); //close the file
		return -2;
	}
	else
	{
		//loop dedicated to skipping the first "index-1" high scores
		//because we are only interested in "index"'th high score
		for (i = 0; i < index - 1; i++)
		{
			//each score has 6 digits + 1 space = 7 bytes
			for (j = 0; j < 7; j++)
			{
				alt_up_sd_card_read(file_handle);
			}
		}

		//read and store the high score at position "index"
		for (i = 0; i < 6; i++)
		{
			temp[i] = alt_up_sd_card_read(file_handle);
		}

		// atoi ignores leading zeroes, so we are okay.
		score = atoi(temp);

		alt_up_sd_card_fclose(file_handle); //close the file
		return score;
	}
}
