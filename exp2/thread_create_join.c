#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

void *print_hello(void *arg) {
    int i = (int)arg;
    printf("SY2006106_Thread %d", i);
    pthread_exit(NULL);
    return (NULL);
}

int main(int argc, char *argv[]) {
    int ret;
    size_t i;
    pthread_t tid[5];
    for (i = 0; i < 5; i++) { 
        printf("in main: creating thread %d\n", i);
        ret = pthread_create(&tid[i], NULL, print_hello, (void *)i);
        if (ret != 0) {
            printf("create thread failed\n");
            exit(-1);
        }
    }
    for (i = 0; i < 5; i++) {
        pthread_join(tid[i], NULL);
    }
    printf("in main: exit!\n");
    pthread_exit(NULL);
    return (0);
}
