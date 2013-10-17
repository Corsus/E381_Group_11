/*
 * Highscore.c
 *
 *  Created on: 2013-10-08
 *      Author: Corsus
 */
 
#include "Highscore.h"

bool writeHighscore(int);
int readHighscore(int);
//Writes into the highscore file, returning a true if the process succeeded and false if it didn't
bool writeHighscore (int score)
{
	int i,j,k,size;
	char temp[10];
	int Highscore[70];
	int HighscoreFinal[70];

	j=0;
	while(readHighscore(j+1)>0)
	{
		Highscore[j] = readHighscore(j+1);
		j++;
	}

	k=0;
	while (score < Highscore[k]) // Find the point where score is bigger than Highscore[k]
	{
		k++;
	}

	for(j=0;j<k;j++)
	{
		HighscoreFinal[j]=Highscore[j];
	}
	HighscoreFinal[j]= score;

	j++;
	while(k<10)
	{
		HighscoreFinal[j] = Highscore[k];
		j++;
		k++;
	}

	int file_handle; //Have no idea why, but if the second parameter is true, this won't work.
	file_handle= alt_up_sd_card_fopen(FILE_NAME, false); //Have no idea why, but if the second parameter is true, this won't work.= alt_up_sd_card_fopen(FILE_NAME, false);
	for(j=0; j<10 ;j++)
	{
		sprintf(temp, "%.6d ", HighscoreFinal[j]);
		size = strlen(temp);

		for(i=0; i < size; i++)
		{
			alt_up_sd_card_write(file_handle, temp[ i ] );
		}
	}
	alt_up_sd_card_fclose(file_handle); //close the file
	return true;
}

int readHighscore(int index) //fully working. Index of 1 means number 1 on highscoreboard
{
	int i, j, score;
	int file_handle= alt_up_sd_card_fopen(FILE_NAME, false);
	char temp[10];

	if (index < 0)
		return -1;

	if(file_handle == -1)
	{
		printf("File could not be opened.");
		alt_up_sd_card_fclose(file_handle); //close the file
		return -2;
	}
	else if(file_handle == -2)
	{
		printf("File already opened.");
		alt_up_sd_card_fclose(file_handle); //close the file
		return -3;
	}
	else
	{
		for(j=0;j<index;j++)
		{
			i = 0;
			do
			{
				temp[i] = alt_up_sd_card_read(file_handle);

				if (temp[i] < 0)
				{
					alt_up_sd_card_fclose(file_handle); //close the file
					return -4;
				}
				i++;
			}while(temp[i-1] != ' ');
			temp[i-1]='\0';
		}
		score = atoi(temp);

		alt_up_sd_card_fclose(file_handle); //close the file
		return score;
	}
}