# CSC 718 Final Project
## Authors: Cole Drumheller and Samuel Kampshoff
### MPI Build Instructions
- Compile command: mpicc -o [nameOfExecutable] [filename]
    - Example: mpicc -o mpiC mergeMPI_Collective.c
### MPI Run Instructions
- With Hostfile: mpirun -np [numberOfProcessors] -hostfile [hostfile] ./[executable] [exponent]
    - Example: mpirun -np 4 -hostfile hosts.txt ./mpiC 25
- Without Hostfile: mpirun -np [numberOfProcessors] ./[executable] [exponent] 
    - Example: mpirun -np 4 ./mpiC 25
### Pthread Build Instructions
- Compile command: gcc -pthread -o [nameOfExecutable] [filename]
    - Example: gcc -pthread -o pthreads pthread_merge_sort.c
### Sequential Build Instructions
- Compile command: gcc -o [nameOfExecutable] [filename]
    - Example: gcc -o seq seq_merge_sort.c
### Pthread/Sequential Run Instructions
- Run Command: ./[nameOfExecutable] [exponent]
    - Example: ./pthreads 25
### Arguments
- Exponent
    - This is the exponent used to set the array_size by doing 2^exponent
### Limitations
- Code assumes that the entered exponent is not too large that it exceeds the bounds of the 'int' data type 

