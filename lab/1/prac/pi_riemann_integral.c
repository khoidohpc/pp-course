// gcc ./prac/pi_riemann_integral.c -lm

#include <stdio.h>
#include <math.h>
#include <stdlib.h>

long n_rect = 10123456; // default number of rectangles
int threadct = 8; // default number of OpenMP threads to use

double f(double); // declare function that defines curve

int main(int argc, char** argv) {
    if (argc > 1)
        n_rect = strtol(argv[1], NULL, 10);
    if (argc > 2)
        threadct = strtol(argv[2], NULL, 10);
    if (n_rect <= 0 || threadct <= 0) {
        printf("Error in command-line argument(s)\n");
        return EXIT_FAILURE;  /* indicates error exit */
    }

    double a = -1.0, b = 1.0; // lower and upper interval endpoints
    double h = (b - a)/n_rect; // width of a rectangle subinterval
    double sum = 0.0; // accumulates areas all rectangles so far
    long i;
    
    #pragma omp parallel for num_threads(threadct) \
    shared(a, n_rect, h) private(i) reduction(+: sum)
    for (i = 0; i < n_rect; i++) sum += f(a + (i+0.5)*h) * h;

    printf("With n = %ld rectangles and %d threads, ", n_rect, threadct);
    printf("the estimate of pi is %.20g\n", 2*sum);

    return EXIT_SUCCESS;
}

double f(double x) { return sqrt(1-x*x); }