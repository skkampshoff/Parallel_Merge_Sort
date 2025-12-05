#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void merge(int *arr, int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;

    int *L = malloc(n1 * sizeof(int));
    int *R = malloc(n2 * sizeof(int));

    for (int i = 0; i < n1; i++) L[i] = arr[left + i];
    for (int i = 0; i < n2; i++) R[i] = arr[mid + 1 + i];

    int i = 0, j = 0, k = left;
    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) arr[k++] = L[i++];
        else arr[k++] = R[j++];
    }

    while (i < n1) arr[k++] = L[i++];
    while (j < n2) arr[k++] = R[j++];

    free(L);
    free(R);
}

void merge_sort(int *arr, int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        merge_sort(arr, left, mid);
        merge_sort(arr, mid + 1, right);
        merge(arr, left, mid, right);
    }
}

void master(int n, int size) {
    char hname[256];
    gethostname(hname, sizeof(hname));
    printf("Rank 0 running on node %s\n", hname);
    int *arr = malloc(n * sizeof(int));

    srand(42);
    for (int i = 0; i < n; i++) 
        arr[i] = rand();

    int num_display = n;
    if (n > 20) {
        num_display = 10; // Limit output for large arrays
        printf("Displaying only first 10 elements of each array.\n");
    }

    printf("Original array:\n");
    for (int i = 0; i < num_display; i++)
        printf("%d ", arr[i]);
    printf("\n");

    int local_n = n / size;
    int *local_arr = malloc(local_n * sizeof(int));
    double start_time = MPI_Wtime();
    // Scatter pieces of the array
    MPI_Scatter(arr, local_n, MPI_INT, local_arr, local_n, MPI_INT, 0, MPI_COMM_WORLD);

    // Sort our own chunk
    merge_sort(local_arr, 0, local_n - 1);

    // Gather back sorted chunks
    MPI_Gather(local_arr, local_n, MPI_INT, arr, local_n, MPI_INT, 0, MPI_COMM_WORLD);

    // Merge all chunks on master
    int step = local_n;
    while (step < n) {
        for (int i = 0; i + step < n; i += 2 * step) {
            int left = i;
            int mid = i + step - 1;
            int right = (i + 2 * step - 1 < n) ? (i + 2 * step - 1) : (n - 1);
            merge(arr, left, mid, right);
        }
        step *= 2;
    }

    double end_time = MPI_Wtime();
    

    printf("Sorted array:\n");
    for (int i = 0; i < num_display; i++)
        printf("%d ", arr[i]);
    printf("\n");

    printf("Time taken for sorting: %f seconds\n", end_time - start_time);

    free(arr);
    free(local_arr);
}

void worker(int rank, int n) {
    char hname[256];
    gethostname(hname, sizeof(hname));
    printf("Rank %d running on node %s\n",rank, hname);
    int size;
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int local_n = n / size;
    int *local_arr = malloc(local_n * sizeof(int));

    // Receive local chunk from scatter
    MPI_Scatter(NULL, 0, MPI_INT, local_arr, local_n, MPI_INT, 0, MPI_COMM_WORLD);

    // Sort chunk
    merge_sort(local_arr, 0, local_n - 1);

    // Send back to master
    MPI_Gather(local_arr, local_n, MPI_INT, NULL, 0, MPI_INT, 0, MPI_COMM_WORLD);

    free(local_arr);
}

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (argc != 2) {
        if (rank == 0)
            printf("Usage: mpirun -np <procs> ./prog <power>\n");
        MPI_Finalize();
        return 0;
    }

    int exp = atoi(argv[1]);
    int n = 1 << exp; // 2^exp
    
    if (n % size != 0) {
        if (rank == 0)
            printf("Error: array size must be divisible by number of processes\n");
        MPI_Finalize();
        return 0;
    }

    if (rank == 0){
        printf("Array size: 2^%d (%d)\n", exp, n);
        master(n, size);
    }
    else
        worker(rank, n);

    MPI_Finalize();
    return 0;
}
