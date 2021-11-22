#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
  
// size of array
#define n 100
  
int a[] = {86, 15, 5, 82, 18, 10, 48, 97, 4, 27, 44, 47, 76, 64, 2, 6, 99, 49, 6, 80, 9, 32, 45, 52, 67, 41, 95, 90, 65, 98, 97, 100, 54, 95, 84, 91, 43, 2, 100, 48, 24, 76, 100, 4, 24, 10, 13, 34, 50, 94, 69, 28, 40, 3, 15, 64, 7, 56, 42, 53, 40, 66, 77, 24, 4, 62, 92, 75, 6, 74, 4, 31, 16, 16, 47, 48, 39, 17, 87, 65, 24, 80, 12, 56, 2, 69, 66, 58, 12, 34, 9, 82, 25, 96, 8, 27, 48, 4, 58, 90};
  
// Temporary array for slave process
int a2[1000];
  
int main(int argc, char* argv[]) {
  
    int pid, np, elements_per_process, n_elements_recieved, name_len;
    // np -> no. of processes
    // pid -> process id
  
    MPI_Status status;
  
    // Creation of parallel processes
    MPI_Init(&argc, &argv);

    // Get the name of the processor
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    MPI_Get_processor_name(processor_name, &name_len);
  
    // find out process ID,
    // and how many processes were started
    MPI_Comm_rank(MPI_COMM_WORLD, &pid);
    MPI_Comm_size(MPI_COMM_WORLD, &np);
  
    // master process
    if (pid == 0) {
        int index, i;
        elements_per_process = n / np;
  
        // check if more than 1 processes are run
        if (np > 1) {
            // distributes the portion of array
            // to child processes to calculate
            // their partial sums
            for (i = 1; i < np - 1; i++) {
                index = i * elements_per_process;
  
                MPI_Send(&elements_per_process,
                         1, MPI_INT, i, 0,
                         MPI_COMM_WORLD);
                MPI_Send(&a[index],
                         elements_per_process,
                         MPI_INT, i, 0,
                         MPI_COMM_WORLD);
            }
  
            // last process adds remaining elements
            index = i * elements_per_process;
            int elements_left = n - index;
  
            MPI_Send(&elements_left,
                     1, MPI_INT,
                     i, 0,
                     MPI_COMM_WORLD);
            MPI_Send(&a[index],
                     elements_left,
                     MPI_INT, i, 0,
                     MPI_COMM_WORLD);
        }
  
        // master process add its own sub array
        int sum = 0;
        for (i = 0; i < elements_per_process; i++)
            sum += a[i];
        printf("\n-- Menjumlahkan element array dari nilai %d sampai nilai %d = %d pada komputasi ke-%d dari %d komputasi di CPU %s --\n", a[0], a[i-1], sum, pid+1, np, processor_name);
        // collects partial sums from other processes
        int tmp;
        for (i = 1; i < np; i++) {
            MPI_Recv(&tmp, 1, MPI_INT,
                     MPI_ANY_SOURCE, 0,
                     MPI_COMM_WORLD,
                     &status);
            int sender = status.MPI_SOURCE;
  
            sum += tmp;
        }
  
        // prints the final sum of array
        printf("####################################################\n");
        printf("Sum of array is : %d\n", sum);
    }
    // slave processes
    else {
        int i;
        MPI_Recv(&n_elements_recieved,
                 1, MPI_INT, 0, 0,
                 MPI_COMM_WORLD,
                 &status);
  
        // stores the received array segment
        // in local array a2
        MPI_Recv(&a2, n_elements_recieved,
                 MPI_INT, 0, 0,
                 MPI_COMM_WORLD,
                 &status);
  
        // calculates its partial sum
        int partial_sum = 0;
        for (i = 0; i < n_elements_recieved; i++)
            partial_sum += a2[i];
        printf("\n-- Menjumlahkan element array dari nilai %d sampai nilai %d = %d pada komputasi ke-%d dari %d komputasi di CPU %s --\n", a2[0], a2[i-1], partial_sum, pid+1, np, processor_name);
 
        // sends the partial sum to the root process
        MPI_Send(&partial_sum, 1, MPI_INT,
                 0, 0, MPI_COMM_WORLD);
    }
  
    // cleans up all MPI state before exit of process
    MPI_Finalize();
  
    return 0;
}
