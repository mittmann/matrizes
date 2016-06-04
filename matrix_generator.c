#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define MAX_NUMBER 20

char in2 = 0;

int main (int argc, char** argv)
{
	int i, j;
	int* matrix;
	int rows, columns;
	int max = 0;
	char filepath1[10] = "in1.txt";
	char filepath2[10] = "in2.txt";
	char aux_string[10];
	FILE* fd;

	srand((unsigned)time(NULL));

	//TESTS ENTRY
	if (argc != 5 && argc != 6)
	{
		puts("Entry not valid. Try again.");
		return (-1);
	}
	for (i=1; i<5; i++)
	{
		if (!(atoi(argv[i]) >> 0))
		{
		puts("Entry not valid. Try again.");
		return (-1);
		}
	}

	if (argc == 5)
	{
		max = MAX_NUMBER;
	}
	else
		if(atoi(argv[5]) > 0)
			max = atoi(argv[5]);
		else
		{
			puts("Entry not valid. Try again.");
			return (-1);
		}
	

	while(in2 < 2)
	{

		if (in2 == 0)//CHECKS IF IT SHOULD GENERATE THE FIRST OR SECOND MATRIX
		{
			rows = atoi(argv[1]);
			columns = atoi(argv[2]);

			if(!(fd = fopen(filepath1, "w+")))
			{
				puts("Error creating file. Aborted");
				free(matrix);
				return -1;
			}

			in2++;
		}
		else
		{
			rows = atoi(argv[3]);
			columns = atoi(argv[4]);

			if(!(fd = fopen(filepath2, "w+")))
			{
				puts("Error creating file. Aborted");
				free(matrix);
				return -1;
			}
			in2++;

		}	

		

		matrix = malloc(sizeof(int)*rows*columns);



		//GENERATES THE MATRIX
		for(i=0; i<rows*columns; i++)
		{
			*(matrix+i) = rand()%max;
		}

		//PRINTS THE MATRIX ON THE SCREEN
		/*puts("Matrix generated: ");
		for(i=0; i<rows; i++)
		{
			printf("\n");
			for(j=0; j<columns; j++)
				printf("%4d ", *(matrix+j+columns*i));
		}*/

		
			//WRITES THE NUMBER OF ROWS AND COLUMNS AT THE TOP OF THE FILE
				fprintf(fd, "LINHAS = ");
				sprintf(aux_string, "%d", rows);
				fprintf(fd, aux_string);

				fprintf(fd, "\nCOLUNAS = ");
				sprintf(aux_string, "%d", columns);
				fprintf(fd, aux_string);
				

				for(i=0; i<rows; i++)
				{
					fprintf(fd, "\n");
					for(j=0; j<columns; j++)
					{
						sprintf(aux_string, "%d ", *(matrix+j+columns*i));
						fprintf(fd, aux_string);
					}
				}
			
		
		fclose(fd);
		free(matrix);
		

	}

	



}
