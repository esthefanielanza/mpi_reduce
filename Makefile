CC = mpicc
CFLAGS = -g -O0 -Wall -Wextra -Werror -std=c99 -fopenmp
LIBS = -lm

all: parallel clean

parallel: parallel.o
	$(CC) $(CFLAGS) parallel.o -o prog $(LIBS)

clean:
	rm *.o

