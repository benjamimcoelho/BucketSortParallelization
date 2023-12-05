#ifndef BUCKET_H   /* Include guard */
#define BUCKET_H

#define ARRAY_SIZE 25000000    /* array size */
#define BUCKET_NUMBER 1000   /* number of buckets */
#define NUM_THREADS 128

struct BucketNode {
    int data;
    struct BucketNode *next;
};

typedef struct BucketNode *List_Bucket;

List_Bucket InsertionSort_List(List_Bucket list);

void MergeSort_List(List_Bucket *headRef);

// Uses more memory but it's very cache freindly
void sort_into_bucket_array(int *input, int * output, int n, int *offsets, int bucket_count, int max, int min);

// Uses less memory but is not cache freindly
void sort_self_into_bucket_array(int *array, int n, int * offsets, int bucket_count , int max, int min); 

void InsertionSort_Array(int * array, int n);

void MergeSort_Array(int *array, int n);

void max_min(int arr[], int n, int *max, int *min);

int get_range(int max, int min, int bucket_count);

void array_copy(int *src, int *dest, int n);

void array_copy_parallel(int *src, int *dest, int n);

int * alloc_array();

void randomize_elements(int * array);

void printArray(int arr[]);

void printA(char * msg, int * array, int n);

void printBuckets(List_Bucket list);

#endif