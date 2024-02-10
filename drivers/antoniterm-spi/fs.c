#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/slab.h>
#include <linux/module.h>

#include "antoniterm.h"
#include "spi.h"

/*
 * Name Attribute
 */
static ssize_t name_show(struct device *dev,
		         struct device_attribute *attr, char *buf)
{
	return sprintf(buf, DEVICE_NAME "\n");
}
static DEVICE_ATTR_RO(name);

/*
 * Show temperature attribute method
 */
static ssize_t temperature_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	struct device *device;
	struct antoniterm_device *antoniterm_dev;

	device = container_of(kobj, struct device, kobj);
	antoniterm_dev = dev_get_drvdata(device);

	return sprintf(buf, "%hd\n",\
		       read_temperature(antoniterm_dev->spi));
}

static struct kobj_attribute temperature_attr = __ATTR_RO(temperature);

/*
 * ANTONITERM Device Attributes
 */
static struct attribute *antoniterm_device_attrs[] = {
	&dev_attr_name.attr,
	&temperature_attr.attr,
	NULL,
};
ATTRIBUTE_GROUPS(antoniterm_device);

/*
 * Hexapod Hardware Controller Device Class
 */
static struct class antoniterm_class = {
	.name = "antoniterm",
	.dev_groups = antoniterm_device_groups,
};

/*
 * Sets up /sys/class/antoniterm/
 */
int fs_class_setup(void)
{
	int err;

	err = class_register(&antoniterm_class);
	if (err) {
		printk(KERN_ERR "antoniterm-spi: Failed to register device class.");
		return -1;
	}

	return 0;
}

/*
 * Destroys antoniterm device class
 */
void fs_class_destroy(void)
{
	class_unregister(&antoniterm_class);
}

static void empty_release(struct device *device)
{
}

/*
 * Creates new device.
 * (and sets up /sys/class/antoniterm/antoniterm<antoniterm_num>/)
 */
struct device *fs_create_device_and_config(int8_t antoniterm_num,
					   struct antoniterm_device *antoniterm_dev)
{
	char name[20];
	struct device *device;

	device = kzalloc(sizeof(struct device), GFP_KERNEL);
	if (!device) {
		printk(KERN_ERR "antoniterm-spi: Out of memory.");
		return 0;
	}
	device->class = &antoniterm_class;

	snprintf(name, 20, "antoniterm%d", (int32_t) antoniterm_num);
	device->kobj.name = kzalloc(strlen(name)+1, GFP_KERNEL);
	strcpy((char*) device->kobj.name, name);
	device->release = empty_release;
	dev_set_drvdata(device, antoniterm_dev);
	if (device_register(device) < 0) {
		printk(KERN_ERR "antoniterm-spi: Could not register device.");
		return 0;
	}

	return device;
}
