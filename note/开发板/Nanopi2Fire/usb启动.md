# nanopi2 fire 从usb启动

bl2 加载地址：***0xFFFF0000***
uboot 加载地址： ***0x43C00000***

使用fatload 从 sd卡将内核加载到内存： ***fatload mmc 1 0x40080000 zImage***

bootz启动内核： ***bootz 0x40080000 - 0x4a000000***


```
0xFFFF0000
0x43C00000
fatls mmc 1
setenv bootargs console=ttyAMA0,115200n8
fatload mmc 1 0x40080000 zImage
fatload mmc 1 0x4a000000 s5p4418-artik530-raptor-rev03.dtb
bootz 0x40080000 - 0x4a000000

```
