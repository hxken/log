/*
 * led_framework.c
 *
 *  Created on: 2016年8月4日
 *      Author: ken
 */
/*
 * 本代码基于LED驱动框架进行编写
 * 暂未采用platform 总线（由下一次实现）
 * 主要文件 <drivers/leds/led-class.c> <drivers/leds/led-core.c>
 * 头文件 <linux/leds.h>
 */
/*
 * 可以通过 echo 1 > /sys/class/leds/led_framework/brightness 来点亮led灯
 * echo 传入的是字符
 * 采用c函数读写时，也需要传入字符串才鞥使其正确工作。
 */


#include <linux/device.h>
#include <linux/fs.h>
#include <linux/module.h>
#include <linux/errno.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <asm/uaccess.h>
#include <asm/io.h>

#include <linux/types.h>
#include <linux/cdev.h>
#include <linux/ioport.h>  //request_mem_region();

#include <linux/ken_s5p4418_gpio.h>
#include <linux/leds.h>

#define DEV_MAJOR 0  //定义主设备号，为0表示不定义


struct __led_struct{
	struct _GPIO_struct_ * vGPIOC;
	struct led_classdev led_cdev;
	struct device dev;
}led_framework_struct;


static void led_frameworek_set(struct led_classdev*led_cdev, enum led_brightness value)
{
	if(value == LED_OFF)
	{
		led_framework_struct.vGPIOC->GPIOOUT &= ~(1 << 13);
		printk("LED off\n");
	}
	else
	{
		led_framework_struct.vGPIOC->GPIOOUT |= 1<< 13;
		printk("LED on\n");
	}
}

static int __init ken_led_init(void)
{
	int err = -1;
	led_framework_struct.led_cdev.name = "led_framework";
	led_framework_struct.led_cdev.brightness_set = led_frameworek_set;

	err = led_classdev_register(NULL, &led_framework_struct.led_cdev);
	if(err)
		printk("led_classdev_register err\n");

	err = (int)request_mem_region(GPIOCBASE, sizeof(unsigned int), "led_gpio");
	if(!err)
	{
		return -EBUSY;
	}
	led_framework_struct.vGPIOC = (struct _GPIO_struct_ *)ioremap(GPIOCBASE, sizeof(struct _GPIO_struct_));
	err = led_framework_struct.vGPIOC->GPIOOUTENB |= 1<<13 ;  //output mode

	led_framework_struct.vGPIOC->GPIOALTFN0 &= ~(0x3 << 26);
	led_framework_struct.vGPIOC->GPIOALTFN0 |= 0x01 << 26 ; //复用功能配置为Function1，即GPIO模式

	return 0;
}

static void __exit ken_led_exit(void)
{
	iounmap(led_framework_struct.vGPIOC);
	release_mem_region(GPIOCBASE, sizeof(unsigned int));
	led_classdev_unregister(&led_framework_struct.led_cdev);
	led_framework_struct.vGPIOC = NULL;

}

module_init(ken_led_init);
module_exit(ken_led_exit);

MODULE_AUTHOR("KEN");
MODULE_DESCRIPTION("For Nanopi2 Fire");
MODULE_LICENSE("GPL");
