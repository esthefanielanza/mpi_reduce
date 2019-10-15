#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>
#include <string.h>
#include <math.h>

int MAX_STRING = 100;

void readMetadata(int nProcess, int myRank, char outputType[5], int *length, float *numbersPerProcess) {
  if(myRank == 0) {
    int i;

    fgets(outputType, sizeof(char) * 5, stdin);  
    scanf("%d", length);

    *numbersPerProcess = ceil(*length / (float)nProcess);
    // Sending to the other process how many numbers they should handle 
    for(i = 1; i < nProcess; i++) {
      MPI_Send(numbersPerProcess, 1, MPI_FLOAT, i, 0, MPI_COMM_WORLD);
    }
  } else {
    MPI_Recv(numbersPerProcess, 1, MPI_FLOAT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    // printf("thread %d receiving %f\n", myRank, *numbersPerProcess);
  }
}

void readArrayAndSplitData(int nProcess, int myRank, int length, float numbersPerProcess, float *partition) {
  int i;
  float currentNumber;

  if(myRank == 0) {
    int sendTo;

    for(i = 0; i < numbersPerProcess * nProcess; i++) {
      if(i < length) {
        scanf("%f", &currentNumber);
      } else {
        currentNumber = 0;
      }

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
    for(i = 0; i < numbersPerProcess; i++) {
      MPI_Recv(&currentNumber, 1, MPI_FLOAT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      partition[i] = currentNumber;
    }
  }
}

void sumOfElements(int nProcess, int myRank, float *partition, float *partitionSize, int *aliveProcess) {
  float numberToBeChanged;
  int lastIndex = *partitionSize - 1;

  if(*partitionSize == 1) {
    *aliveProcess = *aliveProcess - 1;
    return;
  }

  if(myRank == 0) {
    // printf("Sending %f to %d\n", partition[lastIndex], myRank + 1);
    MPI_Send(&partition[lastIndex], 1, MPI_FLOAT, nProcess - 1, 0, MPI_COMM_WORLD);
  } else {
    // printf("Sending %f to %d\n", partition[lastIndex], myRank - 1);
    MPI_Send(&partition[lastIndex], 1, MPI_FLOAT, myRank - 1, 0, MPI_COMM_WORLD);
  }

  if(myRank == nProcess - 1) {
    // printf("rank %d will receive from %d\n", myRank, myRank + 1);
    MPI_Recv(&numberToBeChanged, 1, MPI_FLOAT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  } else {
    // printf("rank %d will receive from %d\n", myRank, myRank - 1);
    MPI_Recv(&numberToBeChanged, 1, MPI_FLOAT, myRank + 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  }

  if(lastIndex - 1 >= 0) {
    partition[lastIndex  - 1] += numberToBeChanged;
    partition[lastIndex] = 0;
    *partitionSize = *partitionSize - 1;
  }
}


void reduce(int myRank, int nProcess, float *partition, float *partitionLength,  int jumpProcess) {
  float value = 0;

  if(partitionLength == 0) {
    return;
  }

  if(myRank % (jumpProcess * 2) == 0 && myRank + jumpProcess < nProcess) {
    MPI_Recv(&value, 1, MPI_FLOAT, myRank + jumpProcess, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    partition[0] += value;
    // printf("PASSO %d Processo %d tem valor %f\n", jumpProcess, myRank, partition[0]);
  } else if(myRank % (jumpProcess * 2) != 0 && myRank - jumpProcess >= 0) {
    value = partition[0];
    // printf("PASSO %d Sending %f to %d from %d\n", jumpProcess, value, myRank - jumpProcess, myRank);
    MPI_Send(&value, 1, MPI_FLOAT, myRank - jumpProcess, 0, MPI_COMM_WORLD);
    *partitionLength = *partitionLength - 1;
  }
}

void print(char outputType[5], int start, int end, float result) {
  if(outputType[0] == 's' || outputType[0] == 'a') {
    printf("%.2f\n", result);
  }
  
  if(outputType[0] == 't' || outputType[0] == 'a') {
    printf("%.6f\n", (start - end) * 1000.0);
  }
}

int main (void) {
  int nProcess, myRank, length;
  float numbersPerProcess;
  int aliveProcess, start, end;
  char outputType[5];

  // Initialize the MPI environment
  MPI_Init(NULL, NULL);
  // Get the number of processes
  MPI_Comm_size(MPI_COMM_WORLD, &nProcess);
  // Get the rank of the processes
  MPI_Comm_rank(MPI_COMM_WORLD, &myRank);

  if(myRank == 0) {
    start = MPI_Wtime();
  }

  // readMetadata: reads the input (stdin)
  readMetadata(nProcess, myRank, outputType, &length, &numbersPerProcess);
 
  // readArrayAndSplitData: read float array and splits between processes
  float *partition = (float *) calloc(numbersPerProcess, sizeof(float));
  readArrayAndSplitData(nProcess, myRank, length, numbersPerProcess, partition);  

  aliveProcess = nProcess;
  while(aliveProcess > 1) {
    sumOfElements(nProcess, myRank, partition, &numbersPerProcess, &aliveProcess);
  }
  
  // int i;
  // for(i = 0; i < numbersPerProcess; i++) {
  //   printf("partition: %d ", myRank);
  //   printf("%f ", partition[i]);
  // }
  // printf("\n");

  aliveProcess = nProcess;
  int jumpProcess = 1;
  while(jumpProcess <= nProcess) {
    reduce(myRank, nProcess, partition, &numbersPerProcess, jumpProcess);
    jumpProcess *= 2;
  }

  if(myRank == 0) {
    end = MPI_Wtime();
    print(outputType, start, end, partition[0]);
  }

  // Finalize MPI environment
  MPI_Finalize();
	return 0;
}