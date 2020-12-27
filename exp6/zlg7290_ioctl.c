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
	//自行设计
};

unsigned char bit_map[8] = {
	//自行设计
};

int main(int argc, const char *argv[])
{
	//首先打开设备文件，（自己在/dev/下新建）
	//调用ioctl(*, *, *)函数，实现数码管的显示
	//关闭数码管
	return 0;
}

