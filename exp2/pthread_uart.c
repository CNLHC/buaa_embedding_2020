#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>

int ret;
int nread;
int fd_tty;
char buff[512];
sem_t gSem;

int set_parity(int fd, int databits, int stopbits, int parity) {
    int ret;
    struct termios options;
    ret = tcgetattr(fd, &options);
    if (ret != 0) {
        printf("Setup Serial failed");
        return (1);
    }
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;
    options.c_cflag &= ~PARENB;
    options.c_iflag &= ~INPCK;
    options.c_cflag &= ~CSTOPB;
    tcflush(fd, TCIOFLUSH);
    ret = tcsetattr(fd, TCSANOW, &options);
    if (ret != 0) {
        printf("Setup Serial failed");
        return (5);
    }
    return (0);
}

void *thread_read_function(void *aSem) {
    sem_t *sem = (sem_t *)aSem;

    while (1) {
        nread = read(fd_tty, buff, 255);
        sem_post(aSem);
    }

    pthread_exit(NULL);
    return (NULL);
}

void *thread_write_function(void *aSem) {
    sem_t *sem = (sem_t *)aSem;
    while (1) {
        sem_wait(sem);
        write(fd_tty, buff, nread);
    }
    pthread_exit(NULL);
    return (NULL);
}

int set_speed(int fd, int speed) {
    int ret;
    struct termios opt;
    ret = tcgetattr(fd, &opt);
    if (ret != 0) {
        printf("Get Serial speed failed");
        return (1);
    }
    tcflush(fd, TCIOFLUSH);
    cfsetispeed(&opt, speed);
    cfsetospeed(&opt, speed);
    ret = tcsetattr(fd, TCSANOW, &opt);
    if (ret != 0) {
        printf("Setup Serial speed failed");
        return (2);
    }
    return (0);
}

int main(int argc, char *argv[]) {
    int ret;
    int i;
    pthread_t tid[2];

    if (argc < 2) {
        printf("useage: %s dev\n", argv[0]);
        return (-1);
    }

    fd_tty = open(argv[1], O_RDWR);
    if (fd_tty < 0) {
        printf("open device %s faild\n", argv[1]);
        return (-1);
    }

    ret = set_speed(fd_tty, 9600);
    if (ret != 0) {
        close(fd_tty);
        return (-1);
    }
    ret = set_parity(fd_tty, 8, 1, 'n');
    if (ret != 0) {
        close(fd_tty);
        return (-1);
    }
    // init an semaphore shareable
    sem_init(&gSem, 1, 0);

    ret = pthread_create(&tid[0], NULL, thread_read_function, (void *)&gSem);
    if (ret != 0) {
        printf("create read thread failed\n");
        exit(-1);
    }
    ret = pthread_create(&tid[1], NULL, thread_write_function, (void *)&gSem);
    if (ret != 0) {
        printf("create write thread failed\n");
        exit(-1);
    }
    pthread_join(tid[0], NULL);
    pthread_join(tid[1], NULL);
    sem_destroy(&gSem);
    pthread_exit(NULL);
    return 0;
}

