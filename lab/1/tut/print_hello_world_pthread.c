#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#define NUM_THREADS 10

// user-defined functions
void * user_def_func(void *threadID){
    long TID;
    TID = (long) threadID;
    printf("Hello World! from thread #%ld\n", TID);
    pthread_exit(NULL);
}
int main(int argc, char *argv){
    pthread_t threads[NUM_THREADS];
    pthread_attr_t attr; // attribute of threads
    int creation_flag, join_flag;
    long i;
    void *status; // status of threads

    /* Initialize and set thread detached atribute */
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    for(i = 0; i < NUM_THREADS  ; i++){
        printf("In main: creating thread %ld\n", i);
        creation_flag = pthread_create(&threads[i], &attr, user_def_func, (void *)i);
        if(creation_flag) {
            printf("ERROR: return code from pthread_create() is %d\n", creation_flag);
            exit(-1);
        }
    }
    /* Free attribute and wait for the other threads */
    pthread_attr_destroy(&attr);

    for(i = 0; i < NUM_THREADS; i++){
        join_flag = pthread_join(threads[i], &status);
        if(join_flag){
            printf("ERROR: return code from pthread_join() is %d\n", join_flag);
            exit(-1);
        }
        printf("Main: completed join with thread %ld having a status of %ld\n", i, (long)status);
    }

    // free thread
    pthread_exit(NULL);
    return 0;
}

// How to build : gcc -pthread print_hello_world_pthread.c