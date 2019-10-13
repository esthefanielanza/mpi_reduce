#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
#include <mpi.h>

int main (void) {
  int nProcess, myRank;

  MPI_Init(NULL, NULL);
  MPI_Comm_size(MPI_COMM_WORLD, &nProcess);
  MPI_Comm_rank(MPI_COMM_WORLD, &myRank);

  char output[5]; // sum (somente soma), time (somente tempo), all
  fgets(output, sizeof(output), stdin);
  // printf("output: %s\n", output);
  
  int length = 0; // quantidade de números a serem somado
  scanf("%d", &length);
  // printf("length: %d\n", length);

  float *numbers; // numeros do tipo float a serem somados
  numbers = malloc(length * sizeof(float));
  for (int i = 0; i < length; i++) {
    scanf("%f", &numbers[i]);
  }
  
  printf("numbers\n");
  for (int i = 0; i < length; i++) {
    printf("%f ", numbers[i]);
    
  }
  
  MPI_Finalize();
	return 0;
}