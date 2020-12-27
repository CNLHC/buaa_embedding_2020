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

unsigned char minu_last_map[10] = {
	0xfd, 0x0d, 0xdb, 0xf3, 0x67, 0xb7, 0xbf, 0xe1,
    0xff, 0xf7
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
    int value[4] = {0};
    int loop_round = 100;
    int i = 0;

    // 循环100s, 之后自动关闭设备
    for(i=0;i<loop_round;i++){
        time(&now) ;
        tm_now = localtime(&now) ;
        printf("当前时间为 %2d:%2d\n", tm_now->tm_min, tm_now->tm_sec);
        // 换算成写入4个数码管寄存器的具体数值
        value[0] = hex_map[(tm_now->tm_min/10)];
        value[1] = minu_last_map[tm_now->tm_min%10];
        value[2] = hex_map[(tm_now->tm_sec/10)];
        value[3] = hex_map[tm_now->tm_sec%10];
        sleep(1);
    }

    // 4个数码管写入0, 恢复原始状态
    value[0] = 0; value[0] = 0; value[2] = 0; value[3] = 0; 
    
	//关闭数码管
    // fclose();
	return 0;
}

