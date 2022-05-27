#include "main.h"
#include "icm20948.h"
#include "drv_spi.h"

#define MPU_A_2mg                ((float)0.00006103f)  //g/LSB
#define MPU_A_4mg                ((float)0.00012207f)  //g/LSB
#define MPU_A_8mg                ((float)0.00024414f)  //g/LSB

#define MPU_G_s250dps            ((float)0.0076296f)  //dps/LSB
#define MPU_G_s500dps            ((float)0.0152592f)  //dps/LSB
#define MPU_G_s1000dps           ((float)0.0305185f)  //dps/LSB
#define MPU_G_s2000dps           ((float)0.0610370f)  //dps/LSB

/*
 *
 * AUX I2C abstraction for magnetometer
 *
 */
void i2c_Mag_write(uint8_t reg, uint8_t value) {
    SPI_GyroSingleWrite(0x7F, 0x30);

    HAL_Delay(1);
    SPI_GyroSingleWrite(0x03, 0x0C);//mode: write

    HAL_Delay(1);
    SPI_GyroSingleWrite(0x04, reg);//set reg addr

    HAL_Delay(1);
    SPI_GyroSingleWrite(0x06, value);//send value

    HAL_Delay(1);
}

bool ICM20948_Detect(void) {
    //ICM20948_SelectBank(USER_BANK_0);
    uint8_t temp;
    SPI_GyroMultiRead(0x00, &temp, 1);
    if (temp == 0xEA) {
        return true;
    } else {
        return false;
    }
}

uint16_t ICM20948_Init(void) {
    ICM20948_SelectBank(USER_BANK_0);
    HAL_Delay(10);
    ICM20948_Disable_I2C();
    HAL_Delay(10);
    ICM20948_SetClock((uint8_t) CLK_BEST_AVAIL);
    HAL_Delay(10);
    ICM20948_AccelGyroOff();
    HAL_Delay(20);
    ICM20948_AccelGyroOn();
    HAL_Delay(10);

    ICM20948_SelectBank(USER_BANK_2);
    HAL_Delay(20);
    ICM20948_SetGyroRateLPF(GYRO_RATE_250, GYRO_LPF_17HZ);
    HAL_Delay(10);

    // Set gyroscope sample rate to 1.1khz (0x0A) in GYRO_SMPLRT_DIV register (0x00)
    SPI_GyroSingleWrite(0x00, 0x00);
    HAL_Delay(10);

    // Set gyroscope scale to +-2000dps (0x07) in GYRO_CONFIG_1 register (0x01)
    SPI_GyroSingleWrite(0x01, 0x07);
    HAL_Delay(10);

    // Set accelerometer low pass filter to 136hz (0x11) and the rate to 8G (0x04) in register ACCEL_CONFIG (0x14)
    SPI_GyroSingleWrite(0x14, (0x04 | 0x11));

    // Set accelerometer sample rate to 225hz (0x00) in ACCEL_SMPLRT_DIV_1 register (0x10)
    SPI_GyroSingleWrite(0x10, 0x00);
    HAL_Delay(10);

    // Set accelerometer sample rate to 1.125khz (0x00) in ACCEL_SMPLRT_DIV_2 register (0x11)
    SPI_GyroSingleWrite(0x11, 0x00);
    HAL_Delay(10);

    ICM20948_SelectBank(USER_BANK_2);
    HAL_Delay(20);

    // Configure AUX_I2C Magnetometer (onboard ICM-20948)
    SPI_GyroSingleWrite(0x7F, 0x00); // Select user bank 0
    SPI_GyroSingleWrite(0x0F, 0x30); // INT Pin / Bypass Enable Configuration
    SPI_GyroSingleWrite(0x03, 0x20); // I2C_MST_EN
    SPI_GyroSingleWrite(0x7F, 0x30); // Select user bank 3
    SPI_GyroSingleWrite(0x01, 0x4D); // I2C Master mode and Speed 400 kHz
    SPI_GyroSingleWrite(0x02, 0x01); // I2C_SLV0 _DLY_ enable
    SPI_GyroSingleWrite(0x05, 0x81); // enable IIC	and EXT_SENS_DATA==1 Byte

    // Initialize magnetometer
    i2c_Mag_write(0x32, 0x01); // Reset AK8963
    HAL_Delay(1000);
    i2c_Mag_write(0x31, 0x02); // use i2c to set AK8963 working on Continuous measurement mode1 & 16-bit output

    ICM20948_SelectBank(USER_BANK_0);
    HAL_Delay(20);
}

/*
 *
 * Read Accelerometer and Gyro data
 *
 */
void ICM20948_ReadAcc(Vector3f_t *acc) {
    Vector3i_t accRaw;
    uint8_t buffer[6];
    SPI_GyroMultiRead(0x2D, buffer, 6);

    accRaw.x = ((((int16_t)buffer[0]) << 8) | buffer[1]);
    accRaw.y = ((((int16_t)buffer[2]) << 8) | buffer[3]);
    accRaw.z = ((((int16_t)buffer[4]) << 8) | buffer[5]);

    //统一传感器坐标系（并非定义安装方向）
    accRaw.x = accRaw.x;
    accRaw.y = -accRaw.y;
    accRaw.z = accRaw.z;

    acc->x = (float)accRaw.x * MPU_A_8mg;
    acc->y = (float)accRaw.y * MPU_A_8mg;
    acc->z = (float)accRaw.z * MPU_A_8mg;
}

void ICM20948_ReadGyro(Vector3f_t *gyro) {
    Vector3i_t gyroRaw;
    uint8_t buffer[6];
    SPI_GyroMultiRead(0x33, buffer, 6);

    gyroRaw.x = ((((int16_t)buffer[0]) << 8) | buffer[1]);
    gyroRaw.y = ((((int16_t)buffer[2]) << 8) | buffer[3]);
    gyroRaw.z = ((((int16_t)buffer[4]) << 8) | buffer[5]);

    //统一传感器坐标系（并非定义安装方向）
    gyroRaw.x = gyroRaw.x;
    gyroRaw.y = -gyroRaw.y;
    gyroRaw.z = -gyroRaw.z;

    gyro->x = (float)gyroRaw.x * MPU_G_s2000dps;
    gyro->y = (float)gyroRaw.y * MPU_G_s2000dps;
    gyro->z = (float)gyroRaw.z * MPU_G_s2000dps;
}

void ICM20948_ReadTemp(float* temp){
    uint8_t buffer[2];
    static int16_t temperature_temp;

    SPI_GyroMultiRead(0x39, buffer, 2);
    temperature_temp = ((((int16_t)buffer[0]) << 8) | buffer[1]);
    *temp = 21 + (float)temperature_temp / 333.87f;
}

/*
 *
 * Read magnetometer
 *
 */
static uint8_t ICM20948_Mag_Read(uint8_t reg) {
    uint8_t Data;
    SPI_GyroSingleWrite(0x7F, 0x30);
    HAL_Delay(1);
    SPI_GyroSingleWrite(0x03, 0x0C | 0x80);
    HAL_Delay(1);
    SPI_GyroSingleWrite(0x04, reg);// set reg addr
    HAL_Delay(1);
    SPI_GyroSingleWrite(0x06, 0xff);//read
    HAL_Delay(1);
    SPI_GyroSingleWrite(0x7F, 0x00);
    SPI_GyroMultiRead(0x3B, &Data, 1);
    HAL_Delay(1);
    return Data;
}

void ICM20948_ReadMag(int16_t magn[3]) {
    uint8_t mag_buffer[10];

    mag_buffer[0] = ICM20948_Mag_Read(0x01);

    mag_buffer[1] = ICM20948_Mag_Read(0x11);
    mag_buffer[2] = ICM20948_Mag_Read(0x12);
    magn[0] = mag_buffer[1] | mag_buffer[2] << 8;
    mag_buffer[3] = ICM20948_Mag_Read(0x13);
    mag_buffer[4] = ICM20948_Mag_Read(0x14);
    magn[1] = mag_buffer[3] | mag_buffer[4] << 8;
    mag_buffer[5] = ICM20948_Mag_Read(0x15);
    mag_buffer[6] = ICM20948_Mag_Read(0x16);
    magn[2] = mag_buffer[5] | mag_buffer[6] << 8;

    i2c_Mag_write(0x31, 0x01);
}

void ICM20948_READ_MAG(int16_t magn[3]) {
    uint8_t mag_buffer[10];

    mag_buffer[0] = ICM20948_Mag_Read(0x01);

    mag_buffer[1] = ICM20948_Mag_Read(0x11);
    mag_buffer[2] = ICM20948_Mag_Read(0x12);
    magn[0] = mag_buffer[1] | mag_buffer[2] << 8;
    mag_buffer[3] = ICM20948_Mag_Read(0x13);
    mag_buffer[4] = ICM20948_Mag_Read(0x14);
    magn[1] = mag_buffer[3] | mag_buffer[4] << 8;
    mag_buffer[5] = ICM20948_Mag_Read(0x15);
    mag_buffer[6] = ICM20948_Mag_Read(0x16);
    magn[2] = mag_buffer[5] | mag_buffer[6] << 8;

    i2c_Mag_write(0x31, 0x01);
}

void ICM20948_SelectBank(uint8_t bank) {
    SPI_GyroSingleWrite(USER_BANK_SEL, bank);
}

void ICM20948_Disable_I2C(void) {
    SPI_GyroSingleWrite(0x03, 0x78);
}

void ICM20948_SetClock(uint8_t clk) {
    SPI_GyroSingleWrite(PWR_MGMT_1, clk);
}

void ICM20948_AccelGyroOff(void) {
    SPI_GyroSingleWrite(PWR_MGMT_2, (0x38 | 0x07));
}

void ICM20948_AccelGyroOn(void) {
    SPI_GyroSingleWrite(PWR_MGMT_2, (0x00 | 0x00));
}

void ICM20948_SetGyroRateLPF(uint8_t rate, uint8_t lpf) {
    SPI_GyroSingleWrite(GYRO_CONFIG_1, (rate | lpf));
}
