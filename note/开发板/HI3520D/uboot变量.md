# HI3520D Uboot变量

## 官方的原版
```
bootdelay=1
baudrate=115200
bootfile="uImage"
da=mw.b 0x82000000 ff 1000000;tftp 0x82000000 u-boot.bin.img;sf probe 0;flwrite
du=mw.b 0x82000000 ff 1000000;tftp 0x82000000 user-x.cramfs.img;sf probe 0;flwrite
dr=mw.b 0x82000000 ff 1000000;tftp 0x82000000 romfs-x.cramfs.img;sf probe 0;flwrite
dw=mw.b 0x82000000 ff 1000000;tftp 0x82000000 web-x.cramfs.img;sf probe 0;flwrite
dl=mw.b 0x82000000 ff 1000000;tftp 0x82000000 logo-x.cramfs.img;sf probe 0;flwrite
dc=mw.b 0x82000000 ff 1000000;tftp 0x82000000 custom-x.cramfs.img;sf probe 0;flwrite
up=mw.b 0x82000000 ff 1000000;tftp 0x82000000 update.img;sf probe 0;flwrite
tk=mw.b 0x82000000 ff 1000000;tftp 0x82000000 zImage.img; bootm 0x82000000
dd=mw.b 0x82000000 ff 1000000;tftp 0x82000000 mtd-x.jffs2.img;sf probe 0;flwrite
ethaddr=00:0b:3f:00:00:01
filesize=724A44
fileaddr=82000000
gatewayip=192.168.1.1
netmask=255.255.255.255
ipaddr=192.168.1.10
serverip=192.168.1.3
bootargs=mem=64M console=ttyAMA0,115200 root=/dev/mtdblock2 rootfstype=jffs2 mtdparts=hi_sfc:1M(boot),4M(kernel),11M(rootfs)
bootcmd=sf probe 0;sf read 0x82000000 0x100000 0x400000;bootm 0x82000000
stdin=serial
stdout=serial
stderr=serial
verify=n
ver=U-Boot 2010.06-svn109 (Jun 07 2013 - 09:54:25)

Environment size: 1221/262140 bytes
```

## 修改后，从hdd硬盘挂载根文件系统的 bootargs
```
mem=64M console=ttyAMA0,115200 root=/dev/sda1 rootfstype=ext4 init=linuxrc mtdparts=hi_sfc:1M(boot),4M(kernel),11M(rootfs)
```
