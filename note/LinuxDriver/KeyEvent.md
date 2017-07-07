# 按键输入

KeyEvent 很简单，主要过程就是：
1. 实例化 input_dev
```c
 static struct input_dev * button_dev = input_allocate_device();
```
2.  填充结构体
```c
button_dev->name = "ken-key";
//设置button_dev 支持的类型       按键     按键S
input_set_capability(button_dev, EV_KEY, KEY_S);
```
3. 注册 input_dev
```c
input_register_device(button_dev);
```
反注册
```c
input_free_device(button_dev);
```
4. 当有按键事件发生时，向内核汇报
```c
input_report_key(button_dev, KEY_S, value);//向内核报告按键值
input_sync(button_dev);
```
