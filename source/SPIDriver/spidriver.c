#include <linux/spi/spi.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <asm/uaccess.h>

static struct
{
	struct class *driver_class;
	dev_t spi_dev_t;
	struct device *device;
	struct cdev *cdev;
	struct file_operations *ops;
	struct spi_device *spi;
}spidriver;

static ssize_t spidriver_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
	int ret = 0, len = 0, missing = 0;
	uint8_t *spi_data = (uint8_t *)kzalloc(count, GFP_KERNEL);
	len = spi_read(spidriver.spi, spi_data, count);
	if (len<0)
	{
		printk("spi read err\n");
		kfree(spi_data);
		return -1;
	}
	missing = copy_to_user(buf, spi_data, len);
	kfree(spi_data);

	return len - missing;
}

static ssize_t spidriver_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
{
	int ret = 0, len = 0;
	uint8_t *spi_data = (uint8_t *)kzalloc(count, GFP_KERNEL);
	if (spi_data == NULL)
	{
		printk("malloc error\n");
		return -ENOMEM;
	}
//	printk("the count is %d\n", count);
	len = copy_from_user(spi_data, buf, count);
	if (len<0)
	{
		printk("copy from user error\n");
		return -EAGAIN;
	}
	len = count - len;
//	printk("copy %d data from user\n", count - len);
	ret = spi_write(spidriver.spi, spi_data, len);

	kfree(spi_data);
	if (ret<0)
	{
		printk("spi write err\n");
		return ret;
	}

	return len;
}

static int spidriver_open(struct inode *inode, struct file *filp)
{
	int err = 0;

	return err;
}

static int spidriver_release(struct inode *inode, struct file *filp)
{
	int err = 0;
	return err;
}

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

static int spi_test_remove(struct spi_device *spi)
{
	int err = 0;
	cdev_del(spidriver.cdev);
	device_destroy(spidriver.driver_class, spidriver.spi_dev_t);
	class_destroy(spidriver.driver_class);
	unregister_chrdev_region(spidriver.spi_dev_t, 1);
	return err;
}




static struct spi_driver spi_test_driver = {
	.driver = {
		.name = "spiken",
		.owner = THIS_MODULE,
	},
	.probe = spi_test_probe,
	.remove = __devexit_p(spi_test_remove),
};

module_spi_driver(spi_test_driver);

MODULE_AUTHOR("KEN <hxstarken@163.com>");
MODULE_DESCRIPTION("spi test");
MODULE_LICENSE("GPL v2");
MODULE_ALIAS("spi:test");
