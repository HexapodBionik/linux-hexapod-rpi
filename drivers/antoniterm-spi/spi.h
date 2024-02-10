#ifndef SPI_H_
#define SPI_H_

#include "antoniterm.h"

int antoniterm_register(void);
void antoniterm_deregister(void);

int16_t read_temperature(struct spi_device *spi);

#endif /* SPI_H_ */
