#include <stdio.h>
#include <stdlib.h>
#include <time.h>

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
void merge_sort(int arr[], int l, int r) {
    if (l < r) {
        // Find the middle point m
        int m = l + (r - l) / 2;

        // Sort first and second halves
        merge_sort(arr, l, m);
        merge_sort(arr, m + 1, r);

        // Merge the sorted halves
        merge(arr, l, m, r);
    }
}

int main(int argc, char *argv[]) {
    srand(42); // Seed for reproducibility

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <size_exponent>\n", argv[0]);
        return EXIT_FAILURE;
    }
    int exp = atoi(argv[1]);
    int N = 1 << exp; // 2^exp
    printf("Array size: %d\n", N);

    int *arr = malloc(N * sizeof(int));
    // Initialize array with random integers
    for (int i = 0; i < N; i++)
        arr[i] = rand();

    int num_display = N;
    if (N > 20) {
        num_display = 10; // Limit output for large arrays
        printf("Displaying only first 10 elements of each array.\n");
    }

    printf("Original array:\n");
    for (int i = 0; i < num_display; i++)
        printf("%d ", arr[i]);
    printf("\n");

    // Measure sorting time
    clock_t start, end;
    start = clock(); // Start timing
    merge_sort(arr, 0, N - 1);
    end = clock(); // End timing

    printf("Sorted array:\n");
    for (int i = 0; i < num_display; i++)
        printf("%d ", arr[i]);
    printf("\n");

    printf("Time taken to sort: %lf seconds\n", ((double)(end - start)) / CLOCKS_PER_SEC);

    free(arr);
    return 0;
}
