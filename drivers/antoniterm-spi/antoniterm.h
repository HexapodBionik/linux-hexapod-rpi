#ifndef HHC_H_
#define HHC_H_

#include <linux/spi/spi.h>

#define DEVICE_VERSION	"1.0.0"
#define DEVICE_NAME	"ANTONITERM v" DEVICE_VERSION

struct antoniterm_device {
	struct spi_device *spi;
	struct device	  *dev;
};

#endif /* HHC_H_ */
