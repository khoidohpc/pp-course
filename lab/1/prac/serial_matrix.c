#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>

#define A 1
#define B 2

typedef int T;

T **a, **b, **c;

T** allocateMatrix(size_t, size_t);
T** deallocateMatrix(T**, size_t);
void initializeMatrix(T**, size_t, size_t, T val);
bool validateMatrix(T**, size_t, size_t, size_t);
void multiplyMatrix(T**, size_t, size_t, T**, size_t, size_t, T**);
void showMatrix(T**, size_t, size_t);


int main(size_t argc, char** argv) {
    size_t N = 10;
    if(argc == 2) N = atoi(argv[1]);

    a = allocateMatrix(N, N); initializeMatrix(a, N, N, A);
    b = allocateMatrix(N, N); initializeMatrix(b, N, N, B);
    c = allocateMatrix(N, N); initializeMatrix(c, N, N, 0);

    multiplyMatrix(a, N, N, b, N, N, c);

    // if(validateMatrix(c, N, N, N)) 
    //     printf("OK\n");
    // else 
    //     printf("BUG\n");

    // showMatrix(c, N, N);

    a = deallocateMatrix(a, N);
    b = deallocateMatrix(b, N);
    c = deallocateMatrix(c, N);

    return EXIT_SUCCESS;
}

T** allocateMatrix(size_t row, size_t col) {
    T **matrix = (T**) malloc(row * sizeof(T*));
    for(size_t _ = 0; _ < row; ++_) 
        matrix[_] = (T*) malloc(col * sizeof(T));

    return matrix;
}

T** deallocateMatrix(T** matrix, size_t row) {
    for(size_t _ = 0; _ < row; ++_) free(matrix[_]);
    free(matrix);
    return NULL;
}

void initializeMatrix(T** matrix, size_t row, size_t col, T val) {
    for(size_t i = 0; i < row; ++i)
    for(size_t j = 0; j < col; ++j)
        matrix[i][j] = val;
}

void multiplyMatrix(T** a, size_t ra, size_t ca, T** b, size_t rb, size_t cb, T** c) {
    assert(ca == rb && "Not compatible matrix!");

    for(size_t i = 0; i < ra; ++i)
    for(size_t j = 0; j < cb; ++j)
    {
        T sum = 0;
        for(size_t k = 0; k < ca; ++k)
            sum += a[i][k]*b[k][j];
        c[i][j] = sum;
    }
}

bool validateMatrix(T** matrix, size_t row, size_t col, size_t N) {
    T val = N*A*B;

    for(size_t i = 0; i < row; ++i)
    for(size_t j = 0; j < col; ++j)
        if(matrix[i][j] != val) return false;
    
    return true;
}

void showMatrix(T** matrix, size_t row, size_t col) {
    for(size_t i = 0; i < row; ++i) {
        for(size_t j = 0; j < col; ++j)
            printf("%i", matrix[i][j]);
        
        printf("\n");
    }
    
}