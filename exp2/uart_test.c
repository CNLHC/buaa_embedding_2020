#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>
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
int set_parity(int fd, int databits, int stopbits, int parity) {
    int ret;
    struct termios options;
    ret = tcgetattr(fd, &options);
    if (ret != 0) {
        printf("Setup Serial failed");
        return (1);
    }
    options.c_cflag &= ~CSIZE;
    switch (databits) {
        case 5:
            options.c_cflag |= CS5;
            break;

        case 6:
            options.c_cflag |= CS6;
            break;

        case 7:
            options.c_cflag |= CS7;
            break;

        case 8:
            options.c_cflag |= CS8;
            break;

        default:
            printf("Unsupported data size\n");
            return (2);
    }

    switch (parity) {

        case 'n':
        case 'N':
            options.c_cflag &= ~PARENB;
            options.c_iflag &= ~INPCK;
            break;

        case 'o':
        case 'O':
            options.c_cflag |= (PARODD | PARENB);
            options.c_iflag |= INPCK;
            break;

        case 'e':
        case 'E':
            options.c_cflag |= PARENB;
            options.c_cflag &= ~PARODD;
            options.c_iflag |= INPCK;
            break;

        default:
            printf("Unsupported parity\n");
            return (3);
    }

    switch (stopbits) { 

        case 1:
            options.c_cflag &= ~CSTOPB;
            break;

        case 2:
            options.c_cflag |= CSTOPB;
            break;

        default:
            printf("Unsupported stop bits\n");
            return (4);
    }

    tcflush(fd, TCIOFLUSH); 
    ret = tcsetattr(fd, TCSANOW, &options);
    if (ret != 0) {
        printf("Setup Serial failed");
        return (5);
    }
    return (0);
}

int main(int argc, char *argv[]) {
    int ret;
    int nread;
    int fd_tty;
    char buff[512];
    if (argc < 2) {
        printf("useage: %s dev\n", argv[0]);
        return (-1);
    }
    printf("start to open device!!!\n");
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
    while (1) {
        nread = read(fd_tty, buff, 255);
        write(fd_tty, buff, nread);
    }
    close(fd_tty);
    return (0);
}
