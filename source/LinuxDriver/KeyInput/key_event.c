/*
 * input.c
 *
 *  Created on: 2016年8月21日
 *      Author: KEN
 */


#include <linux/input.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/module.h>
#include <linux/errno.h>
#include <linux/kernel.h>
#include <linux/input.h>
#include <linux/interrupt.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <asm/uaccess.h>
#include <linux/slab.h>

#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/timer.h>

static struct input_dev * button_dev;
static struct timer_list input_timer;//定义定时器

//定时器回调函数，在此函数中，将定时向内核报告有按键按下/松开
static void timer_function(unsigned long data)
{
	static int value = 0;
	static unsigned int i = 0;
	if(button_dev == NULL)
	{
		goto out;
	}
	if(i == 0)
	{
		i = 1;
		mod_timer(&input_timer, jiffies + 2 * HZ);
		return ;
	}
	value = ~value;

	input_report_key(button_dev, KEY_S, value);//向内核报告按键值
	input_sync(button_dev);
	if(i<4)
	{
		printk("clock %d\n", i);
		i++;
	}
out:
	mod_timer(&input_timer, jiffies + 4 * HZ);

}

static int __init input_init(void)
{
	int error;


	button_dev = input_allocate_device();//申请input结构体
	if (!button_dev) {
		printk(KERN_ERR "button.c: Not enough memory\n");
		error = -ENOMEM;
		return error;
	}

	//对button_dev结构体进行填充
	button_dev->name = "ken-key";
	//设置button_dev 支持的类型       按键    按键S
	input_set_capability(button_dev, EV_KEY, KEY_S);
	//注册button_dev 结构体
	error = input_register_device(button_dev);
	if (error) {
		printk(KERN_ERR "button.c: Failed to register device\n");
		goto err_free_dev;
	}

	//初始化定时器
	init_timer(&input_timer);
	//初始化定时器回调函数
	input_timer.function = timer_function;
	//将定时器添加到内核
	add_timer(&input_timer);

	printk("SUCCESS\n");
	return 0;


 err_free_dev:
	input_free_device(button_dev);

	return error;
}

static void __exit input_exit(void)
{
	del_timer(&input_timer);//删除定时器
	input_unregister_device(button_dev); //反注册button_dev
}


module_init(input_init);
module_exit(input_exit);

MODULE_AUTHOR("HX");
MODULE_LICENSE("Dual BSD/GPL");
