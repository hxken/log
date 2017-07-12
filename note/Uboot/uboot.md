# UBOOT
uboot 用于引导内核，对于内核的引导，其中 ***bootcmd*** ***bootargs*** 是关键的两个参数。
常见的一些环境变量：

|环境变量 | 描述|
|:-------:|:-----:|
|bootdelay |  执行自动启动的等候秒数|
|baudrate| 串口控制台的波特率|
|netmask|  以太网接口的掩码|
|ethaddr|  以太网卡的网卡物理地址|
|bootfile|  缺省的下载文件|
|bootargs| 传递给内核的启动参数|
|bootcmd| 自动启动时执行的命令|
|serverip| 服务器端的ip地址|
|ipaddr| 本地ip 地址|
|stdin| 标准输入设备|
|stdout|  标准输出设备|
|stderr|  标准出错设备|
## bootcmd
***bootcmd*** 是uboot 启动时，uboot自动执行的命令，执行完成后，才开始加载内核。
## bootargs
***bootargs*** 是uboot传递给Linux内核的参数，Linux内核会根据该参数进行相应的动作，如需挂载的文件系统的位置等。
1. 设置 bootargs  
在Uboot中，执行 ***set bootargs < args >*** 来对bootargs变量进行设置。如：
```
setenv bootargs root=/dev/nfs rw console=ttyAMA0,115200n8 init=/linuxrc mmc=2 nfsroot=192.168.1.200:/nfs ip=192.168.1.200:192.168.1.101:192.168.1.1:255.255.255.0::eth0:on
```
2. 保存  
***save*** 或者 ***saveenv***

## bootargs中的一些参数
1.  root  
***root*** 用来指定rootfs的位置， 如:
```
root=/dev/ram0 rw
root=/dev/sda1 rw
root=/dev/mmcblk0p3 rw
root=/dev/mtdblockx rw
```
其中 root=/dev/sda1 rw 表示根文件系统rootfs位于/dev/sda磁盘的分区1中。  
root=/dev/mmcblk0p3 rw 表示文件系统rootfs 位于/dev/mmcblk0的第三分区中。
```
root=/dev/nfs
```
在文件系统为基于nfs的文件系统的时候使用。当然指定root=/dev/nfs之后，还需要指定 ***nfsroot=serverip:nfs_dir*** ，即指明文件系统存在那个主机的那个目录下面.还需指定 ***ip=ip addr:server ip addr:gateway:netmask::which netcard:off***
2.  rootfstype  
这个选项需要跟root一起配合使用，指明文件系统的分区类型，如：
```
rootfstype=ext4
rootfstype=jffs2
```
3. console  
终端使用的串口,如：
```
console=ttyAMA0
```

4. mem  
mem=xxM 指定内存的大小，不是必须的。

5. ramdisk_size  
ramdisk_size=xxxxx  
可以告诉ramdisk 驱动，创建的ramdisk的size，默认情况下是4m(s390默认8M)，你可以查看Documentation/ramdisk.txt找到相关的描述。
6. initrd, noinitrd
当你没有使用ramdisk启动系统的时候，你可以使用noinitrd这个参数，但是如果使用了的话，就需要指定initrd=r_addr,size, r_addr表示initrd在内存中的位置，size表示initrd的大小。
7. init  
init指定的是内核启起来后，进入系统中运行的第一个脚本，如：
```
init=/linuxrc
```
来指定第一个启动脚本为文件系统根目录下的linuxrc文件。
8. mtdparts  
mtdparts=fc000000.nor_flash:1920k(linux),128k(fdt),20M(ramdisk),4M(jffs2),38272k(user),256k(env),384k(uboot)
要想这个参数起作用，内核中的mtd驱动必须要支持，即内核配置时需要选上Device Drivers  ---> Memory Technology Device (MTD) support  ---> Command line partition table parsing
mtdparts的格式如下：
```
mtdparts=<mtddef>[;<mtddef]
<mtddef>  := <mtd-id>:<partdef>[,<partdef>]
<partdef> := <size>[@offset][<name>][ro]
<mtd-id>  := unique id used in mapping driver/device
<size>    := standard linux memsize OR "-" to denote all remaining space
<name>    := (NAME)
```
因此你在使用的时候需要按照下面的格式来设置：
mtdparts=mtd-id:<size1>@<offset1>(<name1>),<size2>@<offset2>(<name2>)
如：
```
mtdparts=hi_sfc:1M(boot),4M(kernel),11M(rootfs)
```
这里面有几个必须要注意的：
a.  mtd-id 必须要跟你当前平台的flash的mtd-id一致，不然整个mtdparts会失效
b.  size在设置的时候可以为实际的size(xxM,xxk,xx)，也可以为'-'这表示剩余的所有空间。
举例：
假设flash 的mtd-id是sa1100，那么你可以使用下面的方式来设置：
mtdparts=sa1100:-     →  只有一个分区
mtdparts=sa1100:256k(ARMboot)ro,-(root)  →  有两个分区
可以查看drivers/mtd/cmdlinepart.c中的注释找到相关描述。
9. ip  
指定系统启动之后网卡的ip地址，如果你使用基于nfs的文件系统，那么必须要有这个参数，其他的情况下就看你自己的喜好了。设置ip有两种方法：
```
ip = ip addr
ip=ip addr:server ip addr:gateway:netmask::which netcard:off
```
这两种方法可以用，不过很明显第二种要详细很多，请注意第二种中which netcard 是指开发板上的网卡，而不是主机上的网卡。

## 常见的集中bootargs
1. 假设文件系统是ramdisk，且直接就在内存中，bootargs的设置应该如下：
```
setenv bootargs ‘initrd=0x32000000,0xa00000 root=/dev/ram0 console=ttySAC0 mem=64M init=/linuxrc’
```

2. 假设文件系统是ramdisk，且在flash中，bootargs的设置应该如下：
```
setenv bootargs ‘mem=32M console=ttyS0,115200 root=/dev/ram rw init=/linuxrc’
```
注意这种情况下你应该要在bootm命令中指定ramdisk在flash中的地址，如bootm kernel_addr ramdisk_addr (fdt_addr)

3. 假设文件系统是jffs2类型的，且在flash中，bootargs的设置应该如下
```
setenv bootargs ‘mem=32M console=ttyS0,115200 noinitrd root=/dev/mtdblock2 rw rootfstype=jffs2 init=/linuxrc’
```
4. 假设文件系统是基于nfs的，bootargs的设置应该如下
```
setenv bootargs ‘noinitrd mem=64M console=ttySAC0 root=/dev/nfs nfsroot=192.168.0.3:/nfs ip=192.168.0.5:192.168.0.3:192.168.0.3:255.255.255.0::eth0:off’
或者
setenv bootargs ‘noinitrd mem=64M console=ttySAC0 root=/dev/nfs nfsroot=192.168.0.3:/nfs ip=192.168.0.5’
```

上面就是我们经常使用的几种bootargs的组合，老实说，bootargs非常非常的灵活，所以设置的方法有很多中形式，具体的还应该根据你的平台具体的情况来设置

## 附录
```
bootargs=mem=64M console=ttyAMA0,115200 root=/dev/mtdblock2 rootfstype=jffs2 mtdparts=hi_sfc:1M(boot),4M(kernel),11M(rootfs)

mem=64M console=ttyAMA0,115200 root=/dev/sda1 rootfstype=ext4 init=linuxrc mtdparts=hi_sfc:1M(boot),4M(kernel),11M(rootfs)
```
