#include <mpi.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#define MAXSIZE 65534

int main(int argc, char **argv)
{
	int myid, numprocs;
	int data[MAXSIZE];
	int i, x, low, high, myresult=0, result=0;
	char fn[255];
	FILE *fp;
	double start_time, end_time;

	MPI_Init(&argc,&argv);
	MPI_Comm_size(MPI_COMM_WORLD,&numprocs);
	MPI_Comm_rank(MPI_COMM_WORLD,&myid);
	start_time = MPI_Wtime();

	if (myid == 0) {  /* Open input file and initialize data */
		strcpy(fn, "1_to_65534_shuffled.test");
		if ((fp = fopen(fn,"r")) == NULL) {
			printf("Cannot open the input file: %s\n\n", fn);
			exit(1);
		}

	for(i = 0; i < MAXSIZE; i++) fscanf(fp,"%d", &data[i]);
	}

MPI_Bcast(data, MAXSIZE, MPI_INT, 0, MPI_COMM_WORLD); /* broadcast data */ 

	x = MAXSIZE/numprocs; /* Add my portion of data */
	low = myid * x;
	high = low + x;
	for(i = low; i < high; i++) myresult += data[i];

	printf("partial sum %d from %d\n", myresult, myid); /* Compute global sum */

	MPI_Reduce(&myresult, &result, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
	if (myid == 0) printf("The sum is %d.\n", result);
	end_time = MPI_Wtime();
	printf("=== Process %d Run Time: %f\n", myid, end_time - start_time);
	MPI_Finalize();

return 0;
}


