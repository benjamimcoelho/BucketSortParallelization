/*
 * Algorithm : Bucket Sort
 * Time-Complexity : O(n)
 */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
#include <string.h>
#include "include/bucketSortParallel.h"
#include "include/bucket.h"

void BucketSortParallel_List_aux(int arr[], int range, int min) {
    int i, j;

    List_Bucket * buckets = (List_Bucket *) calloc(sizeof(List_Bucket), BUCKET_NUMBER);

    /* Scatter the array's elements through the buckets (allocates list nodes as needed) */
    for (i = 0; i < ARRAY_SIZE; ++i) {
        int pos = (arr[i] - min) / range;
        List_Bucket current = (List_Bucket) malloc(sizeof(struct BucketNode));
        current->data = arr[i];
        current->next = buckets[pos];
        buckets[pos] = current;
    }

    /* Order each bucket */
#pragma omp parallel for num_threads(NUM_THREADS) schedule(guided)
    for (i = 0; i < BUCKET_NUMBER; ++i) {
        MergeSort_List(&buckets[i]);
    }

    /* Regroup buckets into original array */
    for (i = 0, j = 0; i < BUCKET_NUMBER; ++i) {
        for(List_Bucket Bucket = buckets[i]; Bucket; Bucket = Bucket->next) {
            arr[j++] = Bucket->data;
        }
    }

    /* Release Memory */
    for (i = 0; i < BUCKET_NUMBER; ++i) {
        for(List_Bucket Bucket = buckets[i]; Bucket;){
            List_Bucket tmp = Bucket;
            Bucket = Bucket->next;
            free(tmp);
        }
    }

    free(buckets);
}


void bucketSortParallel_List(int max, int min, int *array) {
    int range = get_range(max, min, BUCKET_NUMBER);
    BucketSortParallel_List_aux(array, range, min);
}


void bucketSortParallel_Array(int max, int min, int *array, int n) {
    int i, a, b;
    int *bucket_array = malloc(sizeof(int) * n);
    int * offsets = malloc(sizeof(int) * (BUCKET_NUMBER +1));
    
    sort_into_bucket_array(array, bucket_array, n, offsets, BUCKET_NUMBER, max, min);

#pragma omp parallel for num_threads(NUM_THREADS) schedule(guided)
    for (i = 1; i <= BUCKET_NUMBER; i++) {
        a = offsets[i - 1];
        b = offsets[i];
        MergeSort_Array(bucket_array + a, b - a);
    }

    //Bucket array is now an ordered copy of the original array
    array_copy_parallel(bucket_array, array, n);
    free(bucket_array);
    free(offsets);
}

int bucketSortParallel(char version,int * array) {
    int max, min;
    max_min(array, ARRAY_SIZE, &max, &min);

    if (version == 'L') bucketSortParallel_List(max, min, array);
    else if (version == 'A') bucketSortParallel_Array(max,min,array,ARRAY_SIZE);
    else {
         fprintf(stderr,"Invalid Implementation, choose List (-L) or Array (-A)");
        return -1;
    }
    return 0;
}