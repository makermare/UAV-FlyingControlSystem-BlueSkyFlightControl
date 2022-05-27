//
// Created by Liuyufanlyf on 2021/05/07.
//

#ifndef QUADCOPTER_DRV_I2C_H
#define QUADCOPTER_DRV_I2C_H

#include "board.h"

#include "i2c.h"

void I2C_Init();
bool I2C_Single_Write(I2C_HandleTypeDef* device, uint8_t SlaveAddress,uint8_t REG_Address,uint8_t REG_data);
uint8_t I2C_Single_Read(I2C_HandleTypeDef* device, uint8_t SlaveAddress,uint8_t REG_Address);
bool I2C_Multi_Read(I2C_HandleTypeDef* device, uint8_t SlaveAddress,uint8_t REG_Address,uint8_t * ptChar,uint8_t size);

#endif //QUADCOPTER_DRV_I2C_H
