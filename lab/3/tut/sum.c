#include <stdint.h>
//# include <mpi.h>
#include </usr/include/mpi/mpi.h>
#include <stdio.h>
#include <stdlib.h>

uint64_t calculate_pi(uint64_t range);

MPI_Status status;

double main(int argc, char **argv)
{
    uint64_t count = 0, range = 100000000, pointCount = 0;
    int rank, size;
    double pi, wtime;

    //Initialize MPI.
    MPI_Init(&argc, &argv);

    //Get the number of processes.
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    //Determine this processes's rank.
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    rank = rank + 1;

    if (rank == 1)
    {
        wtime = MPI_Wtime();
        printf("Sum 1 to p.\n");
    }

    uint64_t processCount = (uint64_t)(range / size);

    MPI_Reduce(&rank, &count, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 1)
    {
        wtime = MPI_Wtime() - wtime;
        printf("Sum: %ld\n", count);
        printf("Times:  %8f\n", wtime);
    }

    //Terminate MPI.
    MPI_Finalize();

    return EXIT_SUCCESS;
}
