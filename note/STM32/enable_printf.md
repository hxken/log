# STM32中printf重定向
在STM32中，printf函数需要进行重定向后才能够正常使用，即指明从哪输出。
1. GCC 编译器中，对printf 的重定向需要定义 ***_write()*** 函数

```c
int _write (int file, char *pBuffer, int size)；

_read'`_lseek'`_isatty'`_fstat'`_write'`_sbrk'
```


2. KEIL5下  重定向printf函数 需要重载 ***fputc()***

```c
extern UART_HandleTypeDef huart1;

int fputc(int ch, FILE *f)
{
	HAL_UART_Transmit(&huart1, (unsigned char *)&ch, sizeof(unsigned char), 100);
	return ch;
}
```
