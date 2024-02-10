#include <linux/module.h>

#include "spi.h"
#include "antoniterm.h"
#include "fs.h"

static int antoniterm_probe(struct spi_device *spi);
static void antoniterm_remove(struct spi_device *spi);

static struct spi_driver antoniterm_spi_driver = {
	.driver = {
		.name = "antoniterm-spi",
	},
	.probe	  = antoniterm_probe,
	.remove	  = antoniterm_remove,
};

#define MAX_DEVICES 64
struct antoniterm_device *devices[MAX_DEVICES];
static size_t devices_cnt = 0;

static ssize_t new_antoniterm_device(struct spi_device *spi)
{
	struct antoniterm_device *antoniterm_dev;
	ssize_t device_num = -1;
	size_t i;

	/* Too many devices. */
	if (devices_cnt >= MAX_DEVICES) {
		return -1;
	}

	for (i = 0; i < MAX_DEVICES; i++) {
		if (devices[i] == 0) {
			device_num = i;
			break;
		}
	}

	if (device_num < 0) {
		printk(KERN_ERR "antoniterm-spi: new_antoniterm_device: "
				"Unreachable statement.");
		return -1;
	}

	antoniterm_dev = kzalloc(sizeof(struct antoniterm_device), GFP_KERNEL);
	if (!antoniterm_dev) {
		printk(KERN_ERR "antoniterm-spi: Out of memory.");
		return -1;
	}
	antoniterm_dev->spi = spi;

	devices[device_num] = antoniterm_dev;
	devices_cnt++;

	antoniterm_dev->dev = fs_create_device_and_config(device_num,
							  antoniterm_dev);
	if (!antoniterm_dev->dev) {
		kfree(antoniterm_dev);
		return -1;
	}

	return device_num;
}

static void destroy_antoniterm_device(size_t device_num)
{	
	if (!devices[device_num]) {
		printk(KERN_ERR "antoniterm-spi: destroy_antoniterm_device: "
				"Attempt to destroy nonexistent device.");
		return;
	}

	device_unregister(devices[device_num]->dev);
	kfree(devices[device_num]);
	devices[device_num] = 0;
	devices_cnt--;
}

static int antoniterm_probe(struct spi_device *spi)
{
	ssize_t dev_num;
	uint8_t magic;

	magic = spi_w8r8(spi, 0x81);
	if (magic != 0x63) {
		printk(KERN_ERR "antoniterm-spi: Probe failed: Bad magic");
		return -1;
	}

	dev_num = new_antoniterm_device(spi);
	if (dev_num < 0) {
		printk(KERN_ERR "antoniterm-spi: Could not setup sysfs config for device.");
		return -1;
	}

	dev_set_drvdata(&spi->dev, (void*) (dev_num+1));
	return 0;
}

static void antoniterm_remove(struct spi_device *spi)
{
	ssize_t dev_num;

	dev_num = (size_t) dev_get_drvdata(&spi->dev);
	if (dev_num == 0) {
		printk(KERN_WARNING "antoniterm-spi: Warning: Bad device.");
		return;
	}

	dev_num -= 1;
	destroy_antoniterm_device(dev_num);
}

int antoniterm_register(void)
{
	int err;

	err = fs_class_setup();
	if (err < 0) {
		return err;
	}

	err = spi_register_driver(&antoniterm_spi_driver);
	if (err < 0) {
		fs_class_destroy();
		return err;
	}

	return 0;
}

void antoniterm_deregister(void)
{
	spi_unregister_driver(&antoniterm_spi_driver);
	fs_class_destroy();
}

int16_t read_temperature(struct spi_device *spi)
{
	uint8_t th, tl;

	th = spi_w8r8(spi, 0x82);
	tl = spi_w8r8(spi, 0x83);

	return (int16_t) ((((uint16_t) th) << 8) | ((uint16_t) tl));
}
