#include<stdio.h>
#include<stdlib.h>
#include<mpi.h>
#include <time.h>
#include <omp.h>

int main (int argc, char *argv[]) {
  
  int num_proccess = atoi(argv[0]); // numero de processos

  char output[4]; // sum (somente soma), time (somente tempo), all
  fgets(output, sizeof(output), stdin);
  
  int length = 0; // quantidade de números a serem somado
  scanf("%d", &length);

  float *numbers; // numeros do tipo float a serem somados
  numbers = malloc(length * sizeof(float));
  for (int i = 0; i < length; i++) {
    scanf("%f", &numbers[i]);
  }

  printf(num_proccess);
  printf(output);
  printf(length);
  printf(numbers);

	return 0;
}