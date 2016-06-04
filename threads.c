#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/time.h>

///////////////////
	int *matrix1, *matrix2, *matrix_out;	
	int rows1 = 0, rows2 = 0, columns1 = 0, columns2 = 0;
	int done = 0, size = 0, running = 0;
	FILE* fd1, *fd2;
	sem_t mutex, join;
	double t0, t1;
///////////////////////

double GetTime(void)
{
   struct  timeval time;
   double  Time;
   
   gettimeofday(&time, (struct timezone *) NULL);
   Time = ((double)time.tv_sec*1000000.0 + (double)time.tv_usec);
   return(Time);
}

void *load1(void *arg)
{
	int k;
	char buffer[32];
	fd1 = fopen("in1.txt", "r");
	fscanf(fd1, "%s", buffer);
	fscanf(fd1, "%s", buffer);
	fscanf(fd1, "%s", buffer);
	rows1 = atoi(buffer);
	fscanf(fd1, "%s", buffer);
	fscanf(fd1, "%s", buffer);
	fscanf(fd1, "%s", buffer);
	columns1 = atoi(buffer);
	matrix1 = malloc(sizeof(int)*rows1*columns1);

	for (k=0; k<rows1*columns1; k++)
	{
		fscanf(fd1, "%s", buffer);
		*(matrix1+k) = atoi(buffer);

	}	
	fclose(fd1);
}

void *load2(void *arg)
{
	int k;
	char buffer[32];
	fd2 = fopen("in2.txt", "r");
	fscanf(fd2, "%s", buffer);
	fscanf(fd2, "%s", buffer);
	fscanf(fd2, "%s", buffer);
	rows2 = atoi(buffer);
	fscanf(fd2, "%s", buffer);
	fscanf(fd2, "%s", buffer);
	fscanf(fd2, "%s", buffer);
	columns2 = atoi(buffer);
	matrix2 = malloc(sizeof(int)*rows2*columns2);	

	for (k=0; k<rows2*columns2; k++)
	{
		fscanf(fd2, "%s", buffer);
		*(matrix2+k) = atoi(buffer);
	}
	fclose(fd2);
}


void *mult(void *arg)
{
	int position = 0, row =0, column = 0, value = 0;
	int k;

	do
	{
		value = 0;
		sem_wait(&mutex); //beginning of the critical section
		position = done;
		done++;
		sem_post(&mutex); //ending of the critical section

		if(done > size)
			pthread_exit(-1);

		row = position/columns2;
		column = position-row*columns2;

		for (k = 0; k<columns1; k++)	
			value += (*(matrix1+row*columns1+k))  * (*(matrix2+column+columns2*k));

		*(matrix_out+column+row*columns2) = value;
	}while(done < size);
	

}

int main(int argc, char** argv)
{
	int i, j, thamount;
	char buffer[32];
	pthread_t th1, th2;
	pthread_t *threads;
	if (argc != 2)
	{
		puts("Error! Invalid entry");
		return -1;
	}
	if(!(atoi(argv[1]) >> 0))
	{
		puts("Error! Invalid entry");
		return -1;
	}

	thamount = atoi(argv[1]);
		
	pthread_create(&th1, NULL, load1, 0); //loads the matrices, with one thread for each
	pthread_create(&th2, NULL, load2, 0);

	pthread_join(th1, NULL);
	pthread_join(th2, NULL);




	if(columns1 != rows2)
	{
		puts("Error! Invalid matrix dimensions. Aborted.");
		return -1;
	}

	sem_init(&mutex, 0, 1); 
	

	size = rows1*columns2; //set the amount of elements to calculate

	matrix_out = malloc(sizeof(int)*rows1*columns2);

	t0 = GetTime();

	threads = malloc(sizeof(pthread_t)*thamount);

	for(i=0; i<thamount; i++)
	{
		pthread_create((threads+i), NULL, mult, 0);
	}


	for(i=0; i<thamount; i++)
	{
		pthread_join(*(threads+i), NULL);
		
	}

	t1 = GetTime();
	printf("tempo de execucao:%0.4fs\n", (t1-t0)/1000000.0);
	
	fd1 = fopen("out.txt", "w");

	fprintf(fd1, "LINHAS = ");
	sprintf(buffer, "%d", rows1);
	fprintf(fd1, buffer);
	fprintf(fd1, "\nCOLUNAS = ");
	sprintf(buffer, "%d", columns2);
	fprintf(fd1, buffer);

	for(i=0; i<rows1; i++)
	{
		fprintf(fd1, "\n");
		for(j=0; j<columns2; j++)
		{
			sprintf(buffer, "%d ", *(matrix_out+j+columns2*i));
			fprintf(fd1, buffer);
		}

	}
	
	

	fclose(fd1);
	free(threads);
	free(matrix1);			
	free(matrix2);
	free(matrix_out);





}


