// Thanks https://github.com/eschnou/arduino-px4flow-i2c

#include "main.h"

#ifndef PX4FLOW_H
#define PX4FLOW_H

// 7 Bit I2C Address of the Flow Module: Default 0x42 (user selectable bits 0,1,2)
#define PX4FLOW_ADDRESS 0x42

#define PX4FLOW_DATA_REG 0x00
#define PX4FLOW_INTEGRAL_DATA_REG 0x16

#define PX4FLOW_FLAME_LEN 22
#define PX4FLOW_INTEGRAL_FRAME_LEN 26

// As described in the documentation
// http://pixhawk.org/modules/px4flow
typedef struct {
    uint16_t frame_count;// counts created I2C frames
    int16_t pixel_flow_x_sum;// accumulated x flow in pixels*10 since last I2C frame
    int16_t pixel_flow_y_sum;// accumulated y flow in pixels*10 since last I2C frame
    int16_t flow_comp_m_x;// x velocity*1000 in meters / TimeStep
    int16_t flow_comp_m_y;// y velocity*1000 in meters / TimeStep
    int16_t quality;// Optical flow quality / confidence 0: bad, 255: maximum quality
    int16_t gyro_x_rate; //gyro x rate
    int16_t gyro_y_rate; //gyro y rate
    int16_t gyro_z_rate; //gyro z rate
    uint8_t gyro_range; // gyro range
    uint8_t sonar_timestamp;// TimeStep in milliseconds between I2C frames
    int16_t ground_distance;// Ground distance in meters*1000. Positive value: distance known. Negative value: Unknown distance
} px4flow_t;

typedef struct {
    uint16_t frame_count_since_last_readout;//number of flow measurements since last I2C readout [#frames]
    int16_t pixel_flow_x_integral;//accumulated flow in radians*10000 around x axis since last I2C readout [rad*10000]
    int16_t pixel_flow_y_integral;//accumulated flow in radians*10000 around y axis since last I2C readout [rad*10000]
    int16_t gyro_x_rate_integral;//accumulated gyro x rates in radians*10000 since last I2C readout [rad*10000]
    int16_t gyro_y_rate_integral;//accumulated gyro y rates in radians*10000 since last I2C readout [rad*10000]
    int16_t gyro_z_rate_integral;//accumulated gyro z rates in radians*10000 since last I2C readout [rad*10000]
    uint32_t integration_timespan;//accumulation timespan in microseconds since last I2C readout [microseconds]
    uint32_t sonar_timestamp;// time since last sonar update [microseconds]
    int16_t ground_distance;// Ground distance in meters*1000 [meters*1000]
    int16_t gyro_temperature;// Temperature * 100 in centi-degrees Celsius [degcelsius*100]
    uint8_t quality;// averaged quality of accumulated flow values [0:bad quality;255: max quality]
} px4flow_integral_t;

void PX4FLOW_Init(void);

void PX4FLOW_Update(void);

void PX4FLOW_Integral_Update(void);

uint16_t PX4FLOW_GetFrameCount(void);

int16_t PX4FLOW_GetPixelFlowX(void);

int16_t PX4FLOW_GetPixelFlowY(void);

int16_t PX4FLOW_GetPixelVelocityX(void);

int16_t PX4FLOW_GetPixelVelocityY(void);

int16_t PX4FLOW_GetQuality(void);

int16_t PX4FLOW_GetGyroX(void);

int16_t PX4FLOW_GetGyroY(void);

int16_t PX4FLOW_GetGyroZ(void);

uint8_t PX4FLOW_GETGyroRange(void);

uint8_t PX4FLOW_GetSonarTimestamp(void);

int16_t PX4FLOW_GetGroundDistance(void);

uint16_t FX4FLOW_GetFrameCount_Integral(void);

int16_t PX4FLOW_GetPixelFlowX_Integral(void);

int16_t PX4FLOW_GetPixelFlowY_Integral(void);

int16_t PX4FLOW_GetGyroX_Integral(void);

int16_t PX4FLOW_GetGyroY_Integral(void);

int16_t PX4FLOW_GetGyroZ_Integral(void);

uint32_t PX4FLOW_GetTimestamp_Integral(void);

uint32_t PX4FLOW_GetSonarTimestamp_Integral(void);

int16_t PX4FLOW_GetGroundDistance_Integral(void);

int16_t PX4FLOW_GetTemperature_Integral(void);

uint8_t PX4FLOW_GetQuality_Integral(void);

#endif