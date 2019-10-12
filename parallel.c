#include<stdio.h>
#include<stdlib.h>
#include <time.h>
#include <omp.h>

int main (int argc, char *argv[]) {
  
  int num_proccess = atoi(argv[1]); // numero de processos
  printf("num_proccess: %d\n", num_proccess);

  char output[5]; // sum (somente soma), time (somente tempo), all
  fgets(output, sizeof(output), stdin);
  printf("output: %s\n", output);
  
  int length = 0; // quantidade de números a serem somado
  scanf("%d", &length);
  printf("length: %d\n", length);

  float *numbers; // numeros do tipo float a serem somados
  numbers = malloc(length * sizeof(float));
  for (int i = 0; i < length; i++) {
    scanf("%f", &numbers[i]);
  }
  
  printf("numbers\n");
  for (int i = 0; i < length; i++) {
    printf("%f ", numbers[i]);
    
  }
  
  (void)argc;
	return 0;
}