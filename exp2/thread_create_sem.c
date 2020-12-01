#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>

sem_t gSem;


void *print_hello(void *arg) {
    int i = (int)arg;
    printf("SY2006106_Thread %d", i);
    sem_post(&gSem);
    pthread_exit(NULL);
    return (NULL);
}

int main(int argc, char *argv[]) {
    int ret;
    int i;
    int wait_counter = 0;
    sem_init(&gSem,1,0);
    pthread_t tid[5];
    for (i = 0; i < 5; i++) { 
        printf("in main: creating thread %d\n", i);
        ret = pthread_create(&tid[i], NULL, print_hello, (void *)i);
        if (ret != 0) {
            printf("create thread failed\n");
            exit(-1);
        }
    }
    for(wait_counter=0;wait_counter<5;++wait_counter)
        sem_wait(&gSem);

    printf("in main: exit!\n");
    sem_destroy(&gSem);
    pthread_exit(NULL);
    return (0);
}
