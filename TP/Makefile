CC = gcc

PAPIFLAGS = -lpapi -I/share/apps/papi/5.4.1/include -L/share/apps/papi/5.4.1/lib
CCFLAGS = -std=c99 -O2 -lm -g -Wall -Wextra -fopenmp -fno-omit-frame-pointer

# include directories
INCLUDES = -I.

default: papi main

papi: papi.c bucket.c bucketSortParallel.c bucketSort.c
	$(CC) $(CCFLAGS) $(PAPIFLAGS) $(INCLUDES) papi.c bucket.c bucketSortParallel.c bucketSort.c  -o papi.out

main: main.c bucket.c bucketSortParallel.c bucketSort.c
	$(CC) $(CCFLAGS) $(INCLUDES) main.c bucket.c bucketSortParallel.c bucketSort.c  -o main.out

clean:
	rm -f slurm* *.o .a *~ Makefile.bak *.out perf.data* perfreport
