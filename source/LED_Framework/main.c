/*len驱动 应用层测试程序*/

#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

/*
* 写入时，必须是字符串，否则工作不正确。
* echo 传入的就是字符串
*/

int main(int argc, char *argv[] )
{
	int fd ;
	int err = -1;
	unsigned char value[4];
	if(argc != 2)
	{
		printf("should input led status after process\n");
		return -1;
	}

	if( atoi(argv[1]))
	{
		strcpy(value, "1");
	}
	else
	{

		strcpy(value, "0");
	}

//	fd = open("/dev/led_auto", O_RDWR);
	fd = open("/sys/class/leds/led_framework/brightness", O_RDWR);
	if(fd < 0)
	{
		printf("open file err\n");
		exit(-1);
	}

	err = write(fd, value, strlen(value));

	close(fd);
	return 0;
}
