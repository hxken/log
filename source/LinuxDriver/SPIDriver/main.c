/*
 * main.c
 *
 *  Created on: 2017年6月17日
 *      Author: ken
 */
/** 用于测试SPI Driver的应用层程序 运行时，需要输入设备节点如/dev/spidevice
*
*/
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define DATA_LEN 20

int main(int argc, char *argv[])
{
	int err = 0;
	int fd = 0;
	char buf[DATA_LEN];
	int i = 0;


	if (argc != 2)
	{
		printf("please input device\n");
		return -1;
	}
	fd = open(argv[1], O_RDWR);
	if (fd<0)
	{
		printf("open device error!!\n");
		return -1;
	}
	printf("open success!!\n");

	for (i=0; i<DATA_LEN; i++)
	{
		buf[i] = i;
	}
	err = write(fd, buf, DATA_LEN);
	printf("write %d data\n", err);

	close(fd);

	return 0;
}
