#include <math.h>
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

    if (rank == 0)
    {
        wtime = MPI_Wtime();
        printf("Computes the value of Pi using Monte Carlo simulation with one billion points:\n");
    }

    uint64_t processCount = (uint64_t)(range / size);
    pointCount = calculate_pi(processCount);

    MPI_Reduce(&pointCount, &count, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0)
    {
        wtime = MPI_Wtime() - wtime;
        printf("The total of points in square: %8ld\n", range);
        printf("The total of points in circle: %8ld\n", count);
        printf("Times:  %8f\n", wtime);
        pi = 4.0 * (double)count / (double)range;
        printf("Value of PI: %.3lf\n", pi);
    }

    //Terminate MPI.
    MPI_Finalize();

    return EXIT_SUCCESS;
}

uint64_t calculate_pi(uint64_t range)
{
    // Initialize the pseudo number
    // Monte Carlo technique
    uint64_t pcount = 0;
    unsigned int seed = rand() % 30000;

    for (uint64_t i = 0; i < range; ++i)
    {
        // Create point
        double x = (double)rand_r(&seed) / (double)RAND_MAX;
        double y = (double)rand_r(&seed) / (double)RAND_MAX;
        // Calculator radius
        double r = sqrt(x * x + y * y);
        // Is it in a circle
        if (r <= 1)
            pcount = pcount + 1;
    }
    return pcount;
}