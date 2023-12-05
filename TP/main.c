#include "include/bucketSort.h"
#include "include/bucketSortParallel.h"
#include "include/bucket.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define NUM_RUNS 5

int main(int argc, char const *argv[]) {
    if (argc < 3) {
        fprintf(stderr,"NOT ENOUGH ARGUMENTS!");
        return -1;
    }
    
    fprintf(stderr,"Setting Up run env...");
    int * array = alloc_array();
    double res = 0.0;
    fprintf(stderr, "done!\n");

    if (argv[1][0] && argv[1][1] && argv[2][0] && argv[2][1]){
            char ver = argv[1][1], imp = argv[2][1];
            fprintf(stderr, "Warming up...");
            randomize_elements(array);
            if (ver == 'S') bucketSortSequential(imp, array);
            else if (ver == 'P') bucketSortParallel(imp, array);
            else {
                fprintf(stderr,"Invalid Version, choose Sequencial (-S) or Parallel (-P)");
                return -2;
            }
            fprintf(stderr, "done!\n");

        for (int i = 0; i< NUM_RUNS; i++ ){
            randomize_elements(array);
            fprintf(stderr, "\nrun=%d - Ordering Array...", i);
            double init = omp_get_wtime();
            if (ver == 'S') bucketSortSequential(imp, array);
            else if (ver == 'P') bucketSortParallel(imp, array);
            res +=  omp_get_wtime() - init;
            fprintf(stderr, "done!\n");
            }
        }
    else fprintf(stderr,"Invalid Argument(s)");

    res /= (double) NUM_RUNS;
    res *= 1000000;

    free(array);
    printf("%d,%d,%d,%lf\n",ARRAY_SIZE,BUCKET_NUMBER,NUM_THREADS, res);
    
    return 0;
}
