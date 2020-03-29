#include <math.h>
//# include <mpi.h>
#include </usr/include/mpi/mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int calculate_prime(int n, int, int step);

int main(int argc, char *argv[])
{
  int rank = 0, size = 0;
  int n = 1000000;

  int primeTotal = 0;
  int primeCount = 0;
  double wtime = 0;

  //Initialize MPI.
  MPI_Init(&argc, &argv);

  //Get the number of processes.
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  //Determine this processes's rank.
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  if (rank == 0)
    wtime = MPI_Wtime();

  MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

  primeCount = calculate_prime(n, rank, size);

  MPI_Reduce(&primeCount, &primeTotal, 1,
             MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

  if (rank == 0)
  {
    wtime = MPI_Wtime() - wtime;
    printf("The number of prime: %8d\n", primeTotal);
    printf("Total time:  %14f\n", wtime);
  }

  //Terminate MPI.
  MPI_Finalize();

  return 0;
}

int calculate_prime(int n, int start, int step)
{
  int i, j, count, total = 0;

  for (i = 2 + start; i <= n/2; i = i + step)
  {
    count = 1;
    for (j = 2; j < i; j++)
      if (i % j == 0) { count = 0; break; }
    total += count;
  }

  return total;
}