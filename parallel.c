#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
#include <mpi.h>
#include <string.h>
#include <math.h>

int MAX_STRING = 100;

void readMetadata(int nProcess, int myRank, char outputType[5], int *length, int *numbersPerProccess) {
  if(myRank == 0) {
    int i;

    fgets(outputType, sizeof(char) * 5, stdin);  
    scanf("%d", length);

    *numbersPerProccess = *length / nProcess;
    // Sending to the other process how many numbers they should handle 
    for(i = 1; i < nProcess; i++) {
      if(i != nProcess - 1) {
        MPI_Send(numbersPerProccess, 1, MPI_FLOAT, i, 0, MPI_COMM_WORLD);
      } else {
        *numbersPerProccess = *length - (*numbersPerProccess * i);
        MPI_Send(numbersPerProccess, 1, MPI_FLOAT, i, 0, MPI_COMM_WORLD);
      }
    }
  } else {
    MPI_Recv(numbersPerProccess, 1, MPI_FLOAT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    // printf("thread %d receiving %f\n", myRank, *numbersPerProccess);
  }
}

void readArrayAndSplitData(int nProcess, int myRank, int length, int numbersPerProccess, float *partition) {
  int i;
  float currentNumber;

  if(myRank == 0) {
    int sendTo;

    for(i = 0; i < length; i++) {
      scanf("%f", &currentNumber);
      sendTo = i % nProcess;

      if(sendTo == 0) {
        partition[i / nProcess] = currentNumber;
      } else {
        // address of sent buffer; number of elements in sent buffer; datetype; 
        // rank of destination;message tag; communicator
        MPI_Send(&currentNumber, 1, MPI_FLOAT, sendTo, 0, MPI_COMM_WORLD);
      }
    }
  } else {    
    for(i = 0; i < numbersPerProccess; i++) {
      MPI_Recv(&currentNumber, 1, MPI_FLOAT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      partition[i] = currentNumber;
    }
  }
}

// sumOfElements(float *partition) {
//   float sum = 0.0;

  
// }

int main (void) {
  int i;
  int nProcess, myRank, length;
  int numbersPerProccess;
  char outputType[5];

  // Initialize the MPI environment
  MPI_Init(NULL, NULL);
  // Get the number of processes
  MPI_Comm_size(MPI_COMM_WORLD, &nProcess);
  // Get the rank of the processes
  MPI_Comm_rank(MPI_COMM_WORLD, &myRank);

  // readMetadata: reads the input (stdin)
  readMetadata(nProcess, myRank, outputType, &length, &numbersPerProccess);
 
  // readArrayAndSplitData: read float array and splits between processes
  float *partition = (float *) calloc(numbersPerProccess, sizeof(float));
  readArrayAndSplitData(nProcess, myRank, length, numbersPerProccess, partition);  

  printf("partition %d\n", myRank);
  for(i = 0; i < numbersPerProccess; i++) {
    printf("%f ", partition[i]);
  }
  printf("\n");

  // Finalize MPI environment
  MPI_Finalize();
	return 0;
}