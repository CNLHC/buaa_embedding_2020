#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <time.h>
#include <string.h>

#define WRITE_DPRAM _IO('Z', 0)

unsigned char hex_map[16] = {
	0xfc, 0x0c, 0xda, 0xf2, 0x66, 0xb6, 0xbe, 0xe0,//自行设计
    0xfe, 0xf6, 0xee, 0x3e, 0x9c, 0x7a, 0x9e, 0x8e
};

unsigned char bit_map[8] = {
	0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01//自行设计
};

int main(int argc, const char *argv[])
{
	//首先打开设备文件，（自己在/dev/下新建）
    // fd = fopen();
    
	//调用ioctl(*, *, *)函数，实现数码管的显示
    //ioctl(,,)
    time_t now ;
    struct tm *tm_now ;
    while(1){
        time(&now) ;
        tm_now = localtime(&now) ;
        printf("当前时间为 %2d:%2d\n", tm_now->tm_min, tm_now->tm_sec);
        sleep(1);
    }

	//关闭数码管
    // fclose();
	return 0;
}

