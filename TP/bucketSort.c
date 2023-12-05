#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "include/bucketSort.h"
#include "include/bucket.h"
#include <string.h>



void bucketSortSequential_Lists_aux(int arr[], int range, int min) {
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


void bucketSortSequential_Lists(int max, int min, int *array) {
    int range = get_range(max, min, BUCKET_NUMBER);
    bucketSortSequential_Lists_aux(array, range, min);
}

void bucketSortSequential_Array(int max, int min, int *array, int n) {
    int i, a, b;
    int * bucket_array = malloc(sizeof(int) * n);
    int * offsets = malloc(sizeof(int) * (BUCKET_NUMBER +1));
    
    sort_into_bucket_array(array, bucket_array, n, offsets, BUCKET_NUMBER, max, min);

    for (i = 1; i <= BUCKET_NUMBER; i++) {
        a = offsets[i-1];
        b = offsets[i];
        MergeSort_Array(bucket_array + a, b - a);
    }

    //Bucket array is now an ordered copy of the original array
    array_copy(bucket_array, array, n);
    free(bucket_array);
    free(offsets);
}

int bucketSortSequential(char version, int * array){
    int max, min;
    max_min(array, ARRAY_SIZE, &max, &min);

    if (version == 'L') bucketSortSequential_Lists(max, min, array);
    else if (version == 'A') bucketSortSequential_Array(max,min,array,ARRAY_SIZE);
        else {
         fprintf(stderr,"Invalid Implementation, choose List (-L) or Array (-A)");
        return -1;
    }
    return 0;
}