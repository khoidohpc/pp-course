// CPP program to decompose a matrix using 
// Cholesky Decomposition 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define N 3

int threadct = 8; // default number of OpenMP threads to use

void Cholesky_Decomposition(int matrix[N][N], int n) 
{ 
    int lower[n][n]; 
    memset(lower, 0, sizeof(lower)); 
  
    // Decomposing a matrix into Lower Triangular
    #pragma omp parallel for num_threads(threadct) \
    shared(lower) private(i, j, k, sum)
    for (int i = 0; i < n; i++) { 
        for (int j = 0; j <= i; j++) { 
            int sum = 0; 
  
            if (j == i) // summation for diagnols 
            { 
                for (int k = 0; k < j; k++) 
                    sum += pow(lower[j][k], 2); 
                lower[j][j] = sqrt(matrix[j][j] - sum); 
            } else { 
  
                // Evaluating L(i, j) using L(j, j) 
                for (int k = 0; k < j; k++) 
                    sum += (lower[i][k] * lower[j][k]); 
                lower[i][j] = (matrix[i][j] - sum) / lower[j][j]; 
            } 
        } 
    } 
  
    // Displaying Lower Triangular and its Transpose 
    for (int i = 0; i < n; i++) { 
          
        // Lower Triangular 
        for (int j = 0; j < n; j++) 
        printf("%i\t", lower[i][j]); 
        printf("\t");
          
        // Transpose of Lower Triangular 
        for(int j = 0; j < n; j++) 
        printf("%i\t", lower[j][i]);
        printf("\n");
    } 
} 
  
// Driver Code 
int main() 
{ 
    int matrix[N][N] = {{  4,  12, -16}, 
                        {  2,  37, -43}, 
                        {-16, -43,  98}}; 
    Cholesky_Decomposition(matrix, N); 
    return 0;
} 

//  2       0       0               2       1       -8
//  1       6       0               0       6       -5
// -8      -5       3               0       0        3 