# C语言

1. 使用宏封装自己的debug
```c
#define dbg(msg, arg...) printf(msg, ##arg)
```
