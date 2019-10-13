#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
#include <mpi.h>
#include <string.h>
#include <math.h>

int MAX_STRING = 100;

void readMetadata(int nProcess, int myRank, char outputType[5], int *length, double *numbersPerProccess) {
  if(myRank == 0) {
    int i;

    fgets(outputType, sizeof(char) * 5, stdin);  
    scanf("%d", length);

    *numbersPerProccess = ceil(*length / nProcess);
    //Sending to the other process how many numbers they should handle 
    for(i = 1; i < nProcess; i++) {
      // printf("Sending %f to %d\n", *numbersPerProccess, i);
      MPI_Send(numbersPerProccess, 1, MPI_DOUBLE, i, 0, MPI_COMM_WORLD);
    }
  } else {
    MPI_Recv(numbersPerProccess, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    // printf("thread %d receiving %f\n", myRank, *numbersPerProccess);
  }
}

int main (void) {
  int nProcess, myRank, length;
  double numbersPerProccess;
  char outputType[5];

  
  // printf("output: %s\n", output);
  

  // printf("length: %d\n", length);

  // float *numbers; // numeros do tipo float a serem somados
  // numbers = malloc(length * sizeof(float));
  // for (int i = 0; i < length; i++) {
  //   scanf("%f", &numbers[i]);
  // }
  
  // printf("numbers\n");
  // for (int i = 0; i < length; i++) {
  //   printf("%f ", numbers[i]);
    
  // }

  MPI_Init(NULL, NULL);
  MPI_Comm_size(MPI_COMM_WORLD, &nProcess);
  MPI_Comm_rank(MPI_COMM_WORLD, &myRank);

  readMetadata(nProcess, myRank, outputType, &length, &numbersPerProccess);
  // readArrayAndSplitData();

  MPI_Finalize();
	return 0;
}