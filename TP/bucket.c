#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "include/bucket.h"
#include <omp.h>

void FrontBackSplit(List_Bucket source, List_Bucket *frontRef, List_Bucket *backRef);


/* Insertion Sort List Version*/
List_Bucket InsertionSort_List(List_Bucket list) {
    List_Bucket k, BucketList;
    /* At least 2 values are needed */
    if (list == NULL || list->next == NULL) {
        return list;
    }

    BucketList = list;
    k = list->next;
    BucketList->next = NULL; /* primeiro bucket é uma nova lista */
    while (k != NULL) {
        List_Bucket ptr;
        /* verificar se insere antes do primeiro */
        if (BucketList->data > k->data) {
            List_Bucket tmp;
            tmp = k;
            k = k->next;
            tmp->next = BucketList;
            BucketList = tmp;
            continue;
        }

        // procura [i] > [i+1]
        for (ptr = BucketList; ptr->next != NULL; ptr = ptr->next) {
            if (ptr->next->data > k->data)
                break;
        }

        // se encontrar
        if (ptr->next != NULL) {
            List_Bucket tmp;
            tmp = k;
            k = k->next;
            tmp->next = ptr->next;
            ptr->next = tmp;
            continue;
        } else {
            ptr->next = k;
            k = k->next;
            ptr->next->next = NULL;
            continue;
        }
    }
    return BucketList;
}

List_Bucket SortedMerge_List(List_Bucket a, List_Bucket b) {
    List_Bucket result = NULL;
    
    /* Base cases */
    if (a == NULL)
        return (b);
    else if (b == NULL)
        return (a);

    /* Pick either a or b, and recur */
    if (a->data <= b->data) {
        result = a;
        result->next = SortedMerge_List(a->next, b);
    } else {
        result = b;
        result->next = SortedMerge_List(a, b->next);
    }
    return (result);
}

/* sorts the linked list by changing next pointers (not data) */
void MergeSort_List(List_Bucket *headRef) {
    List_Bucket head = *headRef;
    List_Bucket a;
    List_Bucket b;

    /* Base case -- length 0 or 1 */
    if ((head == NULL) || (head->next == NULL)) {
        return;
    }

    /* Split head into 'a' and 'b' sublists */
    FrontBackSplit(head, &a, &b);

    /* Recursively sort the sublists */
    MergeSort_List(&a);
    MergeSort_List(&b);

    /* answer = merge the two sorted lists together */
    *headRef = SortedMerge_List(a, b);
}

/*
    Sorts the given array into a bucket array

    Writes the resulting array on `output`
    Writes the bucket offsets in offsets

    Its slightly freindly to the systems cache
*/
void sort_into_bucket_array(int *input, int * output,int n ,int *offsets, int bucket_count, int max, int min) {
    int i, j, k;
    int range = get_range(max, min, bucket_count);
    int *counting = calloc(sizeof(int), bucket_count); // This arrays cause a lot of anoying dependences
    int *locations = malloc(sizeof(int) * n); // The purpose of this array is so that i don't have to repeat divisions

    // Counting Bucket Lengths
    for (i = 0; i < n; i++) {
        j = (input[i] - min) / range;
        locations[i] = j;
        counting[j]++;
    }

    // Calculating offsets
    offsets[0] = 0;
    for (i = 0, j = 0; i < bucket_count; i++) {
        k = counting[i] + j;
        offsets[i + 1] = k;
        j = k;
        counting[i] = 0; // Reset the value so it can be used as an index counter on the next cicle
    }

    // Assembling Buckets
    for (int i = 0; i < n; i++) {
        j = locations[i];
        output[offsets[j] + counting[j]++] = input[i];
    }

    free(counting);
    free(locations);
}

/*
    Sorts the given array into a bucket array

    Updates the original array with the result
    Writes the bucket offsets in offsets

    Its not freindly to the systems cache
*/
void sort_self_into_bucket_array(int *array, int n, int * offsets, int bucket_count, int max, int min) {
    int i, j, k;
    int range = get_range(max, min, bucket_count);
    int *counting = calloc(sizeof(int), bucket_count); // This array causes a lot of anoying dependences
    int *locations = malloc(sizeof(int) * n); // The purpose of this array is so that i don't have to repeat divisions

    // Counting Bucket Lengths
    for (i = 0; i < n; i++) {
        j = (array[i] - min) / range;
        locations[i] = j;
        counting[j]++; // Critical op
    }

    // Calculating offsets
    offsets[0] = 0;
    for (i = 0, j = 0; i < bucket_count; i++) {
        k = counting[i] + j;
        offsets[i + 1] = k;
        j = k;
        counting[i] = 0; // Reset the value so it can be used as an index counter on the next cicle
    }

    // Assembling Buckets
    int elem, t;
    for (i=0; i< n; i++){
        j = i;
        elem = array[j];
        k = locations[j];
        while (k >= 0){
            locations[j] = -1;
            j = offsets[k] + counting[k]++; 
            // Swap elem with array[j]
            t = array[j];
            array[j] = elem;
            elem = t;

            k = locations[j];
        }
    }

    free(counting);
    free(locations);
}


void InsertionSort_Array(int *array, int n) {
    int i, j, current;
    for (i = 1; i < n; i++) {
        current = array[i];
        j = i - 1;
        while (j >= 0 && array[j] > current) {
            array[j + 1] = array[j];
            j = j - 1;
        }
        array[j + 1] = current;
    }
}

void MergeSort_Array(int *array, int n) {
    if (n < 2)
        return;
    else if (n == 2) { //Slight Optimization
        int a = array[0], b = array[1];
        if (a > b) {
            array[0] = b;
            array[1] = a;
        }
        return;
    }

    // Recursive calls
    int middle = n / 2;
    int children[n];
    array_copy(array, children, n);
    
    MergeSort_Array(children, middle);  
    MergeSort_Array(children + middle, n - middle);

    // Sorted Merge
    int rd1 = 0, rd2 = middle, wr = 0;
    while (rd1 < middle && rd2 < n) {
        if (children[rd1] <= children[rd2])
            array[wr++] = children[rd1++];
        else
            array[wr++] = children[rd2++];
    }
    while (rd1 < middle)
        array[wr++] = children[rd1++];
    while (rd2 < n)
        array[wr++] = children[rd2++];
}


/* UTILITY FUNCTIONS */

/* Split the Buckets of the given list into front and back halves,
    and return the two lists using the reference parameters.
    If the length is odd, the extra Bucket should go in the front list.
    Uses the fast/slow pointer strategy. */
void FrontBackSplit(List_Bucket source, List_Bucket *frontRef, List_Bucket *backRef) {
    List_Bucket fast;
    List_Bucket slow;
    slow = source;
    fast = source->next;

    /* Advance 'fast' two Buckets, and advance 'slow' one Bucket */
    while (fast != NULL) {
        fast = fast->next;
        if (fast != NULL) {
            slow = slow->next;
            fast = fast->next;
        }
    }

    /* 'slow' is before the midpoint in the list, so split it in two
    at that point. */
    *frontRef = source;
    *backRef = slow->next;
    slow->next = NULL;
}

/* Function to insert a Bucket at the beginning of the linked list */
void push(List_Bucket *head_ref, int new_data) {
    /* allocate Bucket */
    List_Bucket new_Bucket = (List_Bucket) malloc(sizeof(struct BucketNode));

    /* put in the data */
    new_Bucket->data = new_data;

    /* link the old list off the new Bucket */
    new_Bucket->next = (*head_ref);

    /* move the head to point to the new Bucket */
    (*head_ref) = new_Bucket;
}

void max_min(int arr[], int n, int *max, int *min) {
    int cmax, cmin, current;
    cmax = cmin = arr[0];
    for (int i = 0; i < n; i++) {
        current = arr[i];
        if (current > cmax) {
            cmax = current;
        } else if (current < cmin) {
            cmin = current;
        }
    }
    *max = cmax;
    *min = cmin;
}

int get_range(int max, int min, int bucket_count) {
    int range, modulo;
    // This is basicly math.ceiling for ints 
    // The compiler should be smart enough to both these operations from the same division
    range = (max - min) / bucket_count;
    modulo = (max - min) % bucket_count;
    if (range && modulo)
        range++;
    return range;
}

void array_copy(int *src, int *dest, int n) {
    for (int i = 0; i < n; i++)
        dest[i] = src[i];
}

void array_copy_parallel(int *src, int *dest, int n) {
    #pragma omp parallel for num_threads(NUM_THREADS) schedule(guided)
    for (int i = 0; i < n; i++)
        dest[i] = src[i];
}


int * alloc_array(){
    return malloc(ARRAY_SIZE * sizeof(int));
}

void randomize_elements(int * array){
    srand(time(NULL));
    for (int i = 0; i < ARRAY_SIZE; i++)
        array[i] = rand();
}

void printArray(int ar[]) {
    printA("", ar, ARRAY_SIZE);
}

void printA(char * msg, int * array, int n){
    printf("%s [ ",msg);
    for(int i=0; i < n; i++)
        printf("%d ",array[i]);
    printf("]\n");
}

void printBuckets(List_Bucket list) {
    List_Bucket cur = list;
    while (cur) {
        printf("%d ", cur->data);
        cur = cur->next;
    }
}