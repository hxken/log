# LED 驱动（采用系统框架）
暂时只实现了设置亮灭的功能
1. 实例化 led_dev
```c
struct led_classdev led_cdev;
2. 为结构体赋值
led_cdev.name = "led_framework";
led_cdev.brightness_set = led_frameworek_set;//用户编写的控制亮度的函数
```
led_frameworek_set() 函数为用户编写的，用户内核回调的控制LED亮度的函数。不仅仅只有亮灭，是能够根据传入的 ***enum len_brightness*** ,来设置亮度。我的实现如下(暂时只实现了亮灭)：
```c
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
```
3. 注册
```c
led_classdev_register(NULL, &led_cdev);
```
反注册
```c
led_classdev_unregister(&.led_cdev);
```
