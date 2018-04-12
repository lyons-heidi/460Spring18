/**** C4.1.c file: compute matrix sum by threads ***/
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#define N 4
int A[N][N], sum[N];
void *func(void *arg) // threads function
{
    int j, row;
    pthread_t tid = pthread_self(); // get thread ID number
    
    // write code to do the following
    // 1. get row number from arg

    // 2. compute row sum; put row sum in sum[row]

    // 3. print thread ID and row sum
    
    return 0;
}
int main (int argc, char *argv[])
{
    pthread_t thread[N]; // thread IDs
    int i, j, r, total = 0;
    void *status;
    printf("Main: initialize A matrix\n");
    for (i=0; i<N; i++){
        sum[i] = 0;
        for (j=0; j<N; j++){
            A[i][j] = i*N + j + 1;
            printf("%4d ", A[i][j]);
        }
        printf("\n");
    }
    printf("Main: create %d threads\n", N);
    for(i=0; i<N; i++) {
        pthread_create(&thread[i], NULL, func, (void *)i);
    }
    printf("Main: try to join with threads\n");
    for(i=0; i<N; i++) {
        pthread_join(thread[i], &status);
        printf("Main: joined with %d [%lu]: status=%d\n",
        i, thread[i], (int)status);
    }
    printf("Main: compute and print total sum: ");
    for (i=0; i<N; i++)
        total += sum[i];
    printf("tatal = %d\n", total);
    pthread_exit(NULL);
}