//
// Created by Liuyufanlyf on 2021/05/07.
//

#include "drv_i2c.h"

void I2C_Init() {
    MX_I2C1_Init();
    MX_I2C2_Init();
    MX_I2C4_Init();
}

bool I2C_Single_Write(I2C_HandleTypeDef *device, uint8_t SlaveAddress, uint8_t REG_Address, uint8_t REG_data) {
    HAL_I2C_Mem_Write(device, SlaveAddress<<1, REG_Address, I2C_MEMADD_SIZE_8BIT, &REG_data, 1, 1);
};

uint8_t I2C_Single_Read(I2C_HandleTypeDef *device, uint8_t SlaveAddress, uint8_t REG_Address) {
    uint8_t temp;
    HAL_I2C_Mem_Read(device, SlaveAddress<<1, REG_Address, I2C_MEMADD_SIZE_8BIT, &temp, 1, 1);
    return temp;
};

bool I2C_Multi_Read(I2C_HandleTypeDef *device, uint8_t SlaveAddress, uint8_t REG_Address, uint8_t *ptChar, uint8_t size) {
    HAL_I2C_Mem_Read(device, SlaveAddress<<1, REG_Address, I2C_MEMADD_SIZE_8BIT, ptChar, size, 1);
};
