#define _POSIX_C_SOURCE 199309L // Required for clock_gettime
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

// Merge two sorted halves: arr[l..m] and arr[m+1..r]
void merge(int arr[], int l, int m, int r) {
    int left_size = m - l + 1;
    int right_size = r - m;

    int *L = malloc(left_size * sizeof(int));
    int *R = malloc(right_size * sizeof(int));

    // Copy to temporary arrays
    for (int i = 0; i < left_size; i++)
        L[i] = arr[l + i];
    for (int i = 0; i < right_size; i++)
        R[i] = arr[m + 1 + i];

    // Merge temp arrays back into arr[l..r]
    int i = 0, j = 0, k = l;
    while (i < left_size && j < right_size) {
        if (L[i] <= R[j]) {
            arr[k] = L[i];
            i++;
        }
        else {
            arr[k] = R[j];
            j++;
        }
        k++;
    }

    // Copy remaining elements of L[], if any
    while (i < left_size) {
        arr[k] = L[i];
        i++;
        k++;
    }

    // Copy remaining elements of R[], if any
    while (j < right_size) {
        arr[k] = R[j];
        j++;
        k++;
    }

    free(L);
    free(R);
}

// Recursive merge sort: sort arr[l..r]
// l is the left index and r is the right index of the sub-array to be sorted
void merge_sort_seq(int arr[], int l, int r) {
    if (l < r) {
        // Find the middle point m
        int m = l + (r - l) / 2;

        // Sort first and second halves
        merge_sort_seq(arr, l, m);
        merge_sort_seq(arr, m + 1, r);

        // Merge the sorted halves
        merge(arr, l, m, r);
    }
}

typedef struct {
    int *arr;
    int l;
    int r;
    int threads_available;
} merge_sort_args;


void *merge_sort_pthread(void *arg) {
    merge_sort_args *args = (merge_sort_args *)arg;
    int *arr = args->arr;
    int l = args->l;
    int r = args->r;
    int threads = args->threads_available;

    if (l < r) {
        int m = l + (r - l) / 2;

        if (threads <= 1) {
            merge_sort_seq(arr, l, m);
            merge_sort_seq(arr, m + 1, r);
        } else {
            // FIX: Divide threads between children
            pthread_t left_thread, right_thread;
            merge_sort_args *left_args = malloc(sizeof(merge_sort_args));
            merge_sort_args *right_args = malloc(sizeof(merge_sort_args));

            // Give half threads to left, half to right
            left_args->arr = arr;
            left_args->l = l;
            left_args->r = m;
            left_args->threads_available = threads / 2;

            right_args->arr = arr;
            right_args->l = m + 1;
            right_args->r = r;
            right_args->threads_available = threads / 2;

            pthread_create(&left_thread, NULL, merge_sort_pthread, left_args);
            pthread_create(&right_thread, NULL, merge_sort_pthread, right_args);

            pthread_join(left_thread, NULL);
            pthread_join(right_thread, NULL);

            free(left_args);
            free(right_args);
        }
        // Always merge after sorting halves
        merge(arr, l, m, r);
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    srand(42); // Seed for reproducibility

    if (argc != 3) {
        fprintf(stderr, "Usage: %s <size_exponent> <num_threads>\n", argv[0]);
        return EXIT_FAILURE;
    }
    int exp = atoi(argv[1]);
    int N = 1 << exp; // 2^exp
    printf("Array size: 2^%d (%d)\n", exp, N);
    int num_threads = atoi(argv[2]);
    printf("Number of threads: %d\n", num_threads);

    int *arr = malloc(N * sizeof(int));
    // Initialize array with random integers
    for (int i = 0; i < N; i++)
        arr[i] = rand(); // Random integers between 0 and 999

    // Limit output for large arrays
    int num_display = N;
    if (N > 20) {
        num_display = 10; 
        printf("Displaying only first 10 elements of each array.\n");
    }

    printf("Original array:\n");
    for (int i = 0; i < num_display; i++)
        printf("%d ", arr[i]);
    printf("\n");

    // Measure sorting time
    struct timespec start, end;

    // Initialize arguments for the initial call
    merge_sort_args init_args = {arr, 0, N - 1, num_threads};

    clock_gettime(CLOCK_REALTIME, &start);
    merge_sort_pthread(&init_args);
    clock_gettime(CLOCK_REALTIME, &end);

    printf("Sorted array:\n");
    for (int i = 0; i < num_display; i++)
        printf("%d ", arr[i]);
    printf("\n");

    // Calculate time taken
    double time_taken = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("Time taken to sort: %lf seconds\n", time_taken);

    free(arr);
    return 0;
}
