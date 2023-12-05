#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "include/bucketSortParallel.h"
#include "include/bucketSort.h"
#include "include/bucket.h"

#include "papi.h"

#define NUM_EVENTS 4
int Events[NUM_EVENTS] = {PAPI_TOT_CYC, PAPI_TOT_INS, PAPI_L1_DCM, PAPI_L2_DCM};

// PAPI counters' values
long long values[NUM_EVENTS], min_values[NUM_EVENTS];

// number of times the function is executed and measured
#define NUM_RUNS 5

int main(int argc, char *argv[]) {
    char version = NULL, imp = NULL;
    if (argc < 3) {
        printf("NOT ENOUGH ARGUMENTS!");
        return 0;
    }
    else if (argv[1][0] && argv[1][1] && argv[2][0] && argv[2][1]){
            version = argv[1][1];
            imp =  argv[2][1];
        }
    else printf("Invalid Argument(s)");

    long long start_usec, end_usec, elapsed_usec, min_usec = 0L;
    int i, run;
    int num_hwcntrs = 0;

    fprintf(stderr, "\nSetting up PAPI...");
    // Initialize PAPI
    PAPI_library_init(PAPI_VER_CURRENT);

    /* Get the number of hardware counters available */
    if ((num_hwcntrs = PAPI_num_counters()) <= PAPI_OK) {
        fprintf(stderr, "PAPI error getting number of available hardware counters!\n");
        return 0;
    }
    fprintf(stderr, "done!\nThis system has %d available counters.\n\n", num_hwcntrs);

    // We will be using at most NUM_EVENTS counters
    if (num_hwcntrs >= NUM_EVENTS) {
        num_hwcntrs = NUM_EVENTS;
    } else {
        fprintf(stderr, "Error: there aren't enough counters to monitor %d events!\n", NUM_EVENTS);
        return 0;
    }


    // warmup caches
    fprintf(stderr, "Warming up caches...");
    int * array = alloc_array();
    randomize_elements(array);
    if (version == 'S') bucketSortSequential(imp,array);
    else if (version == 'P') bucketSortParallel(imp,array);
    else {
          fprintf(stderr,"Invalid Version, choose Sequencial (-S) or Parallel (-P)");
         return -1;
    }

    fprintf(stderr, "done!\n");

    for (run = 0; run < NUM_RUNS; run++) {
        fprintf(stderr, "\nrun=%d - Ordering Array...", run);

        // use PAPI timer (usecs) - note that this is wall clock time
        // for process time running in user mode -> PAPI_get_virt_usec()
        // real and virtual clock cycles can also be read using the equivalent
        // PAPI_get[real|virt]_cyc()
        randomize_elements(array);
        start_usec = PAPI_get_real_usec();

        /* Start counting events */
        if (PAPI_start_counters(Events, num_hwcntrs) != PAPI_OK) {
            fprintf(stderr, "PAPI error starting counters!\n");
            return -3;
        }

    if (version == 'S') bucketSortSequential(imp,array);
    else if (version == 'P') bucketSortParallel(imp,array);

        /* Stop counting events */
        if (PAPI_stop_counters(values, NUM_EVENTS) != PAPI_OK) {
            fprintf(stderr, "PAPI error stoping counters!\n");
            return 0;
        }

        end_usec = PAPI_get_real_usec();
        fprintf(stderr, "done!\n");

        elapsed_usec = end_usec - start_usec;

        if ((run == 0) || (elapsed_usec < min_usec)) {
            min_usec = elapsed_usec;
            for (i = 0; i < NUM_EVENTS; i++) min_values[i] = values[i];
        }

    } // end runs
    free(array);
    fprintf(stderr,"\nWall clock time: %lld usecs\n", min_usec);

    // output PAPI counters' values
    for (i = 0; i < NUM_EVENTS; i++) {
        char EventCodeStr[PAPI_MAX_STR_LEN];

        if (PAPI_event_code_to_name(Events[i], EventCodeStr) == PAPI_OK) {
            fprintf(stderr, "%s = %lld\n", EventCodeStr, min_values[i]);
        } else {
            fprintf(stderr, "PAPI UNKNOWN EVENT = %lld\n", min_values[i]);
        }
    }

    float CPI = -1.0;
    // evaluate CPI and Texec here
    if ((Events[0] == PAPI_TOT_CYC) && (Events[1] == PAPI_TOT_INS)) {
        CPI = ((float) min_values[0]) / ((float) min_values[1]);
        fprintf(stderr, "CPI = %.2f\n", CPI);
    }
    
    fprintf(stderr,"Printing CSV row...");
    fprintf(stdout, "%d,%d,%d,%lld,",ARRAY_SIZE,BUCKET_NUMBER,NUM_THREADS,min_usec);
    for (i = 0; i < NUM_EVENTS; i++) 
        fprintf(stdout, "%lld,", min_values[i]);
    fprintf(stdout, "%.2f\n", CPI);
    fprintf(stderr,"done!\n");

    fprintf(stderr,"\nThat's all, folks\n");
    return 0;
}

