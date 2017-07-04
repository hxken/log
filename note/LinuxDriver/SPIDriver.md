# Linux SPI driver

## 驱动编译的Makefile
```Makefile
Linux_Dir=/home/ken/nanopi2/linux-3.4.y/

CC=arm-linux-gnueabihf-gcc

PWD := $(shell pwd)

AIM=spidriver.o

all:
	make -C $(Linux_Dir) M=$(PWD) modules
	@echo "------------- module sucess ----------------"

clean:
	make -C $(Linux_Dir) M=$(PWD) modules clean
	rm -rf modules.order

obj-m += $(AIM)
```

## 驱动的描述性宏
```c
MODULE_AUTHOR("KEN <hxstarken@163.com>");
MODULE_DESCRIPTION("spi test");
MODULE_LICENSE("GPL v2");
MODULE_ALIAS("spi:test");
```

## 设备驱动入口
```c
module_spi_driver(spi_test_driver);
```
***module_spi_driver*** 宏指明了驱动的入口。
```c
static struct spi_driver spi_test_driver = {
	.driver = {
		.name = "spiken",
		.owner = THIS_MODULE,
	},
	.probe = spi_test_probe,
	.remove = __devexit_p(spi_test_remove),
};
```
driver需要跟device配合才能正常被注册。其中 ***.name*** 与 ***spi_board_info*** 结构体中的 ***.modalias*** 相匹配，系统才调用 ***.probe*** 函数。

下面是在arch/arm/plat-s5p4418/nanopi2/device.c文件中添加的代码。
```c
static void spi0_cs(u32 chipselect)
{

	if (nxp_soc_gpio_get_io_func( CFG_SPI0_CS ) != nxp_soc_gpio_get_altnum( CFG_SPI0_CS ))
		nxp_soc_gpio_set_io_func( CFG_SPI0_CS, nxp_soc_gpio_get_altnum( CFG_SPI0_CS ));

	nxp_soc_gpio_set_io_dir( CFG_SPI0_CS, 1);
	nxp_soc_gpio_set_out_value( CFG_SPI0_CS , chipselect);

}

struct pl022_config_chip spi0_info = {
	/* available POLLING_TRANSFER, INTERRUPT_TRANSFER, DMA_TRANSFER */
	.com_mode = CFG_SPI0_COM_MODE,
	.iface = SSP_INTERFACE_MOTOROLA_SPI,
	/* We can only act as master but SSP_SLAVE is possible in theory */
	.hierarchy = SSP_MASTER,
	/* 0 = drive TX even as slave, 1 = do not drive TX as slave */
	.slave_tx_disable = 1,
	.rx_lev_trig = SSP_RX_4_OR_MORE_ELEM,
	.tx_lev_trig = SSP_TX_4_OR_MORE_EMPTY_LOC,
	.ctrl_len = SSP_BITS_8,
	.wait_state = SSP_MWIRE_WAIT_ZERO,
	.duplex = SSP_MICROWIRE_CHANNEL_FULL_DUPLEX,
	/*
	 * This is where you insert a call to a function to enable CS
	 * (usually GPIO) for a certain chip.
	 */

	.cs_control = spi0_cs,

	.clkdelay = SSP_FEEDBACK_CLK_DELAY_1T,
};

static struct spi_board_info spi_ken_board[] __initdata = {
	[0] = {
		.modalias        = "spiken",    /* fixup */
		.max_speed_hz    = 3125000,     /* max spi clock (SCK) speed in HZ */
		.bus_num         = 0,           /* Note> set bus num, must be smaller than ARRAY_SIZE(spi_plat_device) */
		.chip_select     = 0,           /* Note> set chip select num, must be smaller than spi cs_num */
		.controller_data = &spi0_info,
		.mode            = SPI_MODE_3 | SPI_CPOL | SPI_CPHA,
	},
};

spi_register_board_info(spi_ken_board, ARRAY_SIZE(spi_ken_board));
```  

## probe函数
```c
static int spi_test_probe(struct spi_device *spi)
{
	int err = 0;
	spi->max_speed_hz = 500000;

	err = spi_setup(spi);
	if (err<0)
	{
		return -EBUSY;
	}
	spidriver.spi = spi;
	//动态申请设备号
	err = alloc_chrdev_region(&spidriver.spi_dev_t, 0, 1, "spidriver");
	if (err<0)
	{
		return err;
	}

	spidriver.driver_class = class_create(THIS_MODULE, "spidriver");
	if (IS_ERR(spidriver.driver_class))
	{
		goto _err1;
	}

	spidriver.device = device_create(spidriver.driver_class, NULL, spidriver.spi_dev_t, NULL, "spidriver");
	if (IS_ERR(spidriver.device))
	{
		goto _err2;
	}

	spidriver.cdev = cdev_alloc();
	if (spidriver.cdev == NULL)
	{
		goto _err3;
	}
	spidriver.ops = kzalloc(sizeof(struct file_operations), GFP_KERNEL);
	if (spidriver.ops == NULL)
	{
		goto _err3;
	}
	spidriver.ops->owner = THIS_MODULE;
	spidriver.ops->open = spidriver_open;
	spidriver.ops->read = spidriver_read;
	spidriver.ops->write = spidriver_write;
	spidriver.ops->release = spidriver_release;

	cdev_init(spidriver.cdev, spidriver.ops);
	spidriver.cdev->owner = THIS_MODULE;
	err = cdev_add(spidriver.cdev, spidriver.spi_dev_t, 1);
	if (err < 0)
	{
		goto _err4;
	}

	return err;

	//##################################################################
	_err4:

	_err3:
	device_destroy(spidriver.driver_class, spidriver.spi_dev_t);
	_err2:
	class_destroy(spidriver.driver_class);
	_err1:
	unregister_chrdev_region(spidriver.spi_dev_t, 1);
	return err;
}
```

probe函数中 首先根据具体情况，修改 ***spi*** 结构体参数，然后调用 ***spi_setup(spi)*** 设置SPI硬件。

## SPI 读写函数
***spi_read()*** ***spi_write()*** 函数将spi_data指针指向的count字节的数据通过spi制定的硬件SPI进行读写。
```c
spi_read(spidriver.spi, spi_data, count);
spi_write(spidriver.spi, spi_data, len);
int spi_write_then_read(struct spi_device *spi,
		const void *txbuf, unsigned n_tx,
		void *rxbuf, unsigned n_rx);
```

下面通过 ***spi_write_then_read()*** 函数对spi读写进行分析：  
下面为 ***int spi_write_then_read()*** 的实现。

```c
int spi_write_then_read(struct spi_device *spi,
		const void *txbuf, unsigned n_tx,
		void *rxbuf, unsigned n_rx)
{
	static DEFINE_MUTEX(lock);

	int			status;
	struct spi_message	message;
	struct spi_transfer	x[2];
	u8			*local_buf;

	/* Use preallocated DMA-safe buffer.  We can't avoid copying here,
	 * (as a pure convenience thing), but we can keep heap costs
	 * out of the hot path ...
	 */
	if ((n_tx + n_rx) > SPI_BUFSIZ)
		return -EINVAL;

	spi_message_init(&message);
	memset(x, 0, sizeof x);
	if (n_tx) {
		x[0].len = n_tx;
		spi_message_add_tail(&x[0], &message);
	}
	if (n_rx) {
		x[1].len = n_rx;
		spi_message_add_tail(&x[1], &message);
	}

	/* ... unless someone else is using the pre-allocated buffer */
	if (!mutex_trylock(&lock)) {
		local_buf = kmalloc(SPI_BUFSIZ, GFP_KERNEL);
		if (!local_buf)
			return -ENOMEM;
	} else
		local_buf = buf;

	memcpy(local_buf, txbuf, n_tx);
	x[0].tx_buf = local_buf;
	x[1].rx_buf = local_buf + n_tx;

	/* do the i/o */
	status = spi_sync(spi, &message);
	if (status == 0)
		memcpy(rxbuf, x[1].rx_buf, n_rx);

	if (x[0].tx_buf == buf)
		mutex_unlock(&lock);
	else
		kfree(local_buf);

	return status;
}
```

spi数据的传输是通过 ***struct spi_message	message*** 来表示的。在一个 ***message*** 传输期间，SPI的CS将被一直拉低，知道完成后拉高。 两个 ***message*** 件的件的传输，则同样在一个 ***message*** 期间拉低，结束后拉高，再进行第二个 ***message*** 的输出传输。  

数据发送还是接收，是由 ***struct spi_transfer	x[2];*** 结构体极性标识的。
```c
x[0].len = n_tx;
x[1].len = n_rx;
x[0].tx_buf = local_buf;
x[1].rx_buf = local_buf + n_tx;
```
***.len***标明传输数据的船赌（字节为单位） 如果是接收数据，则填充 ***.rx_buf*** 发送则填充 ***.tx_buf***
调用 ***spi_message_add_tail)*** 将 ***struct spi_transfer*** 加入 ***struct spi_message***
```c
spi_message_add_tail(&x[0], &message);
spi_message_add_tail(&x[1], &message);
```

上述全部完后后，调用 ***spi_sync(spi, &message);*** 开始发送、接收数据。
