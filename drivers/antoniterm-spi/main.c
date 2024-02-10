#include <linux/init.h>
#include <linux/module.h>

#include "spi.h"

MODULE_AUTHOR("Antoni Przybylik");
MODULE_DESCRIPTION("v1.0.0");
MODULE_LICENSE("GPL");

static int __init antoniterm_init(void)
{
        int err;

        err = antoniterm_register();
	if (err) {
        	printk(KERN_ERR "antoniterm-spi: Error ocurred in init. "
				"Cleaning up.\n");
                return err;
	}

        return 0;
}

static void __exit antoniterm_exit(void)
{
	antoniterm_deregister();
}

module_init(antoniterm_init);
module_exit(antoniterm_exit);
