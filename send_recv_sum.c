/********************************************
 * Justin Rokisky (GK81776)
 * Parallel Computing CMSC 691
 * Homework 2
 ********************************************/

#include <mpi.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define MAXSIZE 65534.0

int main(int argc, char **argv)
{
	int myid, numprocs;
	int largest = INT_MIN;
	int i, j, k, low, tmp, tmp_largest, result=0;
	char fn[255];
	FILE *fp;
	double start_time, end_time;

	MPI_Init(&argc,&argv);
	MPI_Comm_size(MPI_COMM_WORLD,&numprocs);
	MPI_Comm_rank(MPI_COMM_WORLD,&myid);
	start_time = MPI_Wtime();

	// Compute the size of the chunk that all processes other than 0 will get.
	int chunk_size = (int) ceil(MAXSIZE / (double) numprocs);
	int data[chunk_size];
	int file_data[(int) MAXSIZE];

	if (myid == 0) {  /* Open input file and initialize data */
		strcpy(fn, "1_to_65534_shuffled.test");
		if ((fp = fopen(fn,"r")) == NULL) {
			printf("Cannot open the input file: %s\n\n", fn);
			exit(1);
		}

		// Write file into memory.
		for(i = 1; i < (int) MAXSIZE; i++) {
			fscanf(fp,"%d", &file_data[i]);
		}

		// Last chunk might be of a different size and will be computed
		// by process 0.
		for(i = 1; i < numprocs; i++) {
			low = (i-1) * chunk_size;
			for (j = 0; j < chunk_size; j++) {
				data[j] = file_data[low + j];
			}
			MPI_Send(&data, chunk_size, MPI_INT, i, 0, MPI_COMM_WORLD);
		}

		// Compute the sum of the last chunk_size.
		low = (numprocs-1) * chunk_size;
		int final_chunk_size = (int) MAXSIZE - low;
		for (k = 0; k < final_chunk_size; k++) {
			tmp = file_data[low + k];
			if (tmp > largest) {
				largest = tmp;	
			}
			result += tmp;
		}
		printf("[partial sum is: %d from process: %d]\n", result, myid);
		
		// Add results from other processes.
		for (i = 1; i < numprocs; i++) {
			tmp = 0;
			tmp_largest = 0;
			MPI_Recv(&tmp, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			MPI_Recv(&tmp_largest, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			if (tmp_largest > largest) {
				largest = tmp_largest;
			}
			result += tmp;
		}

		// Print results.
		printf("\n-----------------------------------------------\n");
		printf("The sum is: %d and the largest value is: %d.", result, largest);
		printf("\n-----------------------------------------------\n");
	} else {
		// Get our data chunk and compute largest value & sum.
		tmp = 0;
		MPI_Recv(&data, chunk_size, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		for (i = 0; i < chunk_size; i++) {
			if (data[i] > largest) {
				largest = data[i];
			}
			tmp += data[i];
		}

		// Send data to process 0.
		MPI_Send(&tmp, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
		MPI_Send(&largest, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
		printf("[partial sum is: %d from process: %d]\n", tmp, myid);
	}

	end_time = MPI_Wtime();
	printf("=== Process %d Run Time: %f\n", myid, end_time - start_time);
	MPI_Finalize();
	return 0;
}
