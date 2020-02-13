#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>

#define A 1
#define B 2
#define THREAD_NUM 4

typedef int T;
typedef struct {
    T **a, **b, **c;
    size_t ra, ca;
    size_t rb, cb;   
} MatrixThreadInfo;

T **a, **b, **c;

T** allocateMatrix(size_t, size_t);
T** deallocateMatrix(T**, size_t);
void initializeMatrix(T**, size_t, size_t, T val);
bool validateMatrix(T**, size_t, size_t, size_t);
void multiplyMatrix(T**, size_t, size_t, T**, size_t, size_t, T**, int8_t);
void showMatrix(T**, size_t, size_t);

int step_i = 0;
pthread_mutex_t mutex;

int main(size_t argc, char** argv) {
    size_t N = 10;
    if(argc == 2) N = atoi(argv[1]);

    a = allocateMatrix(N, N); initializeMatrix(a, N, N, A);
    b = allocateMatrix(N, N); initializeMatrix(b, N, N, B);
    c = allocateMatrix(N, N); initializeMatrix(c, N, N, 0);

    multiplyMatrix(a, N, N, b, N, N, c, THREAD_NUM);

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

void *multiplyMatrixThread(void* arg) {
    MatrixThreadInfo *info = (MatrixThreadInfo*) arg;
    pthread_mutex_lock(&mutex);
    int8_t threadId = step_i++;
    pthread_mutex_unlock(&mutex);
    for(size_t i = threadId*info->ra/THREAD_NUM; i < (threadId + 1)*info->ra/THREAD_NUM; ++i)
    for(size_t j = 0; j < info->cb; ++j)
    {
        T sum = 0;
        for(size_t k = 0; k < info->ca; ++k)
            sum += info->a[i][k]*info->b[k][j];
        info->c[i][j] = sum;  
    }
}

void multiplyMatrix(T** a, size_t ra, size_t ca, T** b, size_t rb, size_t cb, T** c, int8_t num) {
    assert(ca == rb && "Not compatible matrix!");
    // Thread argument
    MatrixThreadInfo *info = (MatrixThreadInfo*) malloc(sizeof(MatrixThreadInfo));
    info->a = a; info->ra = ra; info->ca = ca;
    info->b = b; info->rb = rb; info->cb = cb;
    info->c = c;
    // Thread attribute
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    pthread_t threads[num];
    pthread_mutex_init(&mutex, NULL);
    
    // Call thread
    for(int8_t _ = 0; _ < num; ++_) {
        pthread_create(&threads[_], &attr, multiplyMatrixThread, (void *)info);
    }

    void *status;
    for(int i=0; i < THREAD_NUM; i++) pthread_join(threads[i], &status);

    pthread_mutex_destroy(&mutex);
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
