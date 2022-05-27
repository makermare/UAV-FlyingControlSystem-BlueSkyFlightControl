//
// Created by Liuyufanlyf on 2021/05/14.
//

#ifndef QUADCOPTER_SPL06_007_H
#define QUADCOPTER_SPL06_007_H

// Using SPI interface
//#define HW_ADR 0x77 //SDO HIGH OR NC
//#define HW_ADR 0x76 //SDO LOW

#define CONTINUOUS_PRESSURE     1
#define CONTINUOUS_TEMPERATURE  2
#define CONTINUOUS_P_AND_T      3
#define PRESSURE_SENSOR     0
#define TEMPERATURE_SENSOR  1

struct spl06007_calib_param_t {
    int16_t c0;
    int16_t c1;
    int32_t c00;
    int32_t c10;
    int16_t c01;
    int16_t c11;
    int16_t c20;
    int16_t c21;
    int16_t c30;
};

struct spl06007_t {
    struct spl06007_calib_param_t calib_param;/**<calibration data*/
    uint8_t chip_id; /**<chip id*/
    int32_t i32rawPressure;
    int32_t i32rawTemperature;
    int32_t i32kP;
    int32_t i32kT;
};

extern float temperature, temperature2, baro_Offset;
extern unsigned char baro_start;

void spl06007_init(void);

void spl06007_rateset(uint8_t iSensor, uint8_t u8OverSmpl, uint8_t u8SmplRate);

void spl06007_start_temperature(void);

void spl06007_start_pressure(void);

void spl06007_start_continuous(uint8_t mode);

void spl06007_get_raw_temp(void);

void spl06007_get_raw_pressure(void);

float spl06007_get_temperature(void);

float spl06007_get_pressure(void);

float user_spl06007_get(void);

#endif
