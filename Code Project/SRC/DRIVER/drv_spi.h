#ifndef __DRV_SPI_H__
#define __DRV_SPI_H__

#include "board.h"

#include "spi.h"

void SPI_Init(void);

void SPI_GyroSingleWrite(uint8_t reg, uint8_t value);
void SPI_GyroMultiRead(uint8_t reg,uint8_t *data, uint8_t length);
void Spi_BaroSingleWrite(uint8_t reg, uint8_t value);
void Spi_BaroMultiRead(uint8_t reg,uint8_t *data, uint8_t length);

#endif










