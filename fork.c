#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/time.h>


double GetTime(void)
{
   struct  timeval time;
   double  Time;
   
   gettimeofday(&time, (struct timezone *) NULL);
   Time = ((double)time.tv_sec*1000000.0 + (double)time.tv_usec);
   return(Time);
}



int main(int argc, char **argv)
{
	int shmid1, shmid2, shmidout;
	int *matrix1, *matrix2, *matrixout;
	int rows1, columns1, rows2, columns2;
	int size, p_amount, elements_per_process;
	int i, k, j;
	int *pid;
	double t0, t1;
	char buffer[32];	
	int row=0, column=0, value=0, element;
	FILE* fd;

	if (argc != 2)
	{
		puts("Usage: ./fork <processes_amount>");
		return -1;
	}
	else
		if(0 >= (p_amount = atoi(argv[1])))
		{
			puts("Invalid entry.");
			return -1;
		}

	pid = malloc(sizeof(pid_t)*p_amount);


	fd = fopen("in1.txt", "r");
	fscanf(fd, "%s", buffer);
	fscanf(fd, "%s", buffer);
	fscanf(fd, "%s", buffer);
	rows1 = atoi(buffer);
	fscanf(fd, "%s", buffer);
	fscanf(fd, "%s", buffer);
	fscanf(fd, "%s", buffer);
	columns1 = atoi(buffer);
	
	fd = fopen("in2.txt", "r");
	fscanf(fd, "%s", buffer);
	fscanf(fd, "%s", buffer);
	fscanf(fd, "%s", buffer);
	rows2 = atoi(buffer);
	fscanf(fd, "%s", buffer);
	fscanf(fd, "%s", buffer);
	fscanf(fd, "%s", buffer);
	columns2 = atoi(buffer);
	
	if(columns1 != rows2)
	{
		puts("Invalid matrices dimensions. Aborted.");
		return -1;
	}	


	shmid1 = shmget(IPC_PRIVATE, sizeof(int)*rows1*columns1, 0666);
	shmid2 = shmget(IPC_PRIVATE, sizeof(int)*rows2*columns2, 0666);

	size = rows1*columns2;
	shmidout =  shmget(IPC_PRIVATE, sizeof(int)*size, 0666);
	
	if ((matrix1 = shmat(shmid1, NULL, 0)) == (int *) -1)
		{
			perror("shmat");
			puts("erro no shmat do matrix1");
			return -1;
		}
	if ((matrix2 = shmat(shmid2, NULL, 0)) == (int *) -1)
		{
			puts("erro no shmat do matrix2");
			return -1;
		}

	if ((matrixout = shmat(shmidout, NULL, 0)) == (int *) -1)
		{
			puts("erro no shmat do matrixout");
			return -1;
		}

	fd=fopen("in1.txt", "r");
	fscanf(fd, "%s", buffer); //gambiarra pra pular a informaçao de linha e coluna 
	fscanf(fd, "%s", buffer);
	fscanf(fd, "%s", buffer);
	fscanf(fd, "%s", buffer);
	fscanf(fd, "%s", buffer);
	fscanf(fd, "%s", buffer);

	for(k=0; k<rows1*columns1; k++)
	{
		fscanf(fd, "%s", buffer);
		matrix1[k] = atoi(buffer);
	}
	fclose(fd);
	
	fd=fopen("in2.txt", "r");
	fscanf(fd, "%s", buffer);
	fscanf(fd, "%s", buffer);
	fscanf(fd, "%s", buffer);
	fscanf(fd, "%s", buffer);
	fscanf(fd, "%s", buffer);
	fscanf(fd, "%s", buffer);
	for(k=0; k<rows2*columns2; k++)
	{
		fscanf(fd, "%s", buffer);
		matrix2[k] = atoi(buffer);
	}
	fclose(fd);
	



	elements_per_process = rows1*columns2/p_amount;

	for(i=0; i<p_amount; i++)
		pid[i] = 1;

	i = 0;

	t0 = GetTime();


 	do
	{
		pid[i] = fork();
		i++;
	}
	while(i<p_amount && pid[i-1] > 0);



	if (pid[i-1] == 0)
	{

		element = (i-1)*elements_per_process; //starting row
		for(k=0; k< elements_per_process; k++) //amount of elements each process should calculate
		{
			value = 0;
			row = (element+k)/columns2;
			column = (element+k)-row*columns2;
			for (j=0; j<columns1; j++)
			{

				value+= (*(matrix1+row*columns1+j)) * (*(matrix2+column+columns2*j));

			}

			*(matrixout+column+row*columns2) = value;

		}
		if(i==1)
		{
			element = rows1*columns2 - (rows1*columns2)%p_amount; //starting row
			for(k=0; k< (rows1*columns2)%p_amount; k++) 
			{
				value = 0;
				row = (element+k)/columns2;
				column = (element+k)-row*columns2;
				for (j=0; j<columns1; j++)
				{
					value+= (*(matrix1+row*columns1+j)) * (*(matrix2+column+columns2*j));
				}
				*(matrixout+column+row*columns2) = value;

			}
		}
	}
	else if (pid[i-1] == -1)
		puts ("Fork failed\n");
	else if (pid[i-1] >> 0)
	{
	
		for(i=0; i<p_amount; i++)
			waitpid(pid[i], NULL, NULL);

		t1 = GetTime();
		printf("tempo de execucao:%0.4fs\n", (float)(t1-t0)/1000000.0);

	
		fd = fopen("out.txt", "w");

        fprintf(fd, "LINHAS = ");
        sprintf(buffer, "%d", rows1);
        fprintf(fd, buffer);
        fprintf(fd, "\nCOLUNAS = ");
        sprintf(buffer, "%d", columns2);
        fprintf(fd, buffer);

        for(k=0; k<rows1; k++)
        {
                fprintf(fd, "\n");
                for(j=0; j<columns2; j++)
                {
                        sprintf(buffer, "%d ", *(matrixout+j+columns2*k));
                        fprintf(fd, buffer);
                }

        }

        fclose(fd);
        shmdt(matrix1);
        shmdt(matrix2);
        shmdt(matrixout);
        free(pid);





	}

	
}
