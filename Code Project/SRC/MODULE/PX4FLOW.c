#include "PX4FLOW.h"
#include "drv_i2c.h"

px4flow_t frame;
px4flow_integral_t integral_frame;

uint8_t raw[32];

bool PX4FLOW_Detect() {

}

void PX4FLOW_Init() {

}

void PX4FLOW_Update() {
    HAL_I2C_Mem_Read(&PX4FLOW_I2C, PX4FLOW_ADDRESS << 1, PX4FLOW_DATA_REG, I2C_ADDRESSINGMODE_7BIT, raw,
                     PX4FLOW_FLAME_LEN,
                     100);
    frame.frame_count = (raw[1] << 8) + raw[0];
    frame.pixel_flow_x_sum = (raw[3] << 8) + raw[2];
    frame.pixel_flow_y_sum = (raw[5] << 8) + raw[4];
    frame.flow_comp_m_x = (raw[7] << 8) + raw[6];
    frame.flow_comp_m_y = (raw[9] << 8) + raw[8];
    frame.quality = (raw[11] << 8) + raw[10];
    frame.gyro_x_rate = (raw[13] << 8) + raw[12];
    frame.gyro_y_rate = (raw[15] << 8) + raw[14];
    frame.gyro_z_rate = (raw[17] << 8) + raw[16];
    frame.gyro_range = raw[18];
    frame.sonar_timestamp = raw[19];
    frame.ground_distance = (raw[21] << 8) + raw[20];
}

uint16_t PX4FLOW_GetFrameCount(void) {
    return frame.frame_count;
}

int16_t PX4FLOW_GetPixelFlowX(void) {
    return frame.pixel_flow_x_sum;
}

int16_t PX4FLOW_GetPixelFlowY(void) {
    return frame.pixel_flow_y_sum;
}

int16_t PX4FLOW_GetPixelVelocityX(void) {
    return frame.flow_comp_m_x;
}

int16_t PX4FLOW_GetPixelVelocityY(void) {
    return frame.flow_comp_m_y;
}

int16_t PX4FLOW_GetQuality(void) {
    return frame.quality;
}

int16_t PX4FLOW_GetGyroX(void) {
    return frame.gyro_x_rate;
}

int16_t PX4FLOW_GetGyroY(void) {
    return frame.gyro_y_rate;
}

int16_t PX4FLOW_GetGyroZ(void) {
    return frame.gyro_z_rate;
}

uint8_t PX4FLOW_GETGyroRange(void) {
    return frame.gyro_range;
}

uint8_t PX4FLOW_GetSonarTimestamp(void) {
    return frame.sonar_timestamp;
}

int16_t PX4FLOW_GetGroundDistance(void) {
    return frame.ground_distance;
}

void PX4FLOW_Integral_Update() {
    HAL_I2C_Mem_Read(&PX4FLOW_I2C, PX4FLOW_ADDRESS << 1, PX4FLOW_INTEGRAL_DATA_REG, I2C_ADDRESSINGMODE_7BIT, raw,
                     PX4FLOW_INTEGRAL_FRAME_LEN, 100);
    integral_frame.frame_count_since_last_readout = (raw[1] << 8) + raw[0];
    integral_frame.pixel_flow_x_integral = (raw[3] << 8) + raw[2];
    integral_frame.pixel_flow_y_integral = (raw[5] << 8) + raw[4];
    integral_frame.gyro_x_rate_integral = (raw[7] << 8) + raw[6];
    integral_frame.gyro_y_rate_integral = (raw[9] << 8) + raw[8];
    integral_frame.gyro_z_rate_integral = (raw[11] << 8) + raw[10];
    integral_frame.integration_timespan = (raw[15] << 24) + (raw[14] << 16) + (raw[13] << 8) + raw[12];
    integral_frame.sonar_timestamp = (raw[19] << 24) + (raw[18] << 16) + (raw[17] << 8) + raw[16];
    integral_frame.ground_distance = (raw[21] << 8) + raw[20];
    integral_frame.gyro_temperature = (raw[23] << 8) + raw[22];
    integral_frame.quality = raw[24];
}

uint16_t FX4FLOW_GetFrameCount_Integral(void) {
    return integral_frame.frame_count_since_last_readout;
}

int16_t PX4FLOW_GetPixelFlowX_Integral(void) {
    return integral_frame.pixel_flow_x_integral;
}

int16_t PX4FLOW_GetPixelFlowY_Integral(void) {
    return integral_frame.pixel_flow_y_integral;
}

int16_t PX4FLOW_GetGyroX_Integral(void) {
    return integral_frame.gyro_x_rate_integral;
}

int16_t PX4FLOW_GetGyroY_Integral(void) {
    return integral_frame.gyro_y_rate_integral;
}

int16_t PX4FLOW_GetGyroZ_Integral(void) {
    return integral_frame.gyro_z_rate_integral;
}

uint32_t PX4FLOW_GetTimestamp_Integral(void) {
    return integral_frame.integration_timespan;
}

uint32_t PX4FLOW_GetSonarTimestamp_Integral(void) {
    return integral_frame.sonar_timestamp;
}

int16_t PX4FLOW_GetGroundDistance_Integral(void) {
    return integral_frame.ground_distance;
}

int16_t PX4FLOW_GetTemperature_Integral(void) {
    return integral_frame.gyro_temperature;
}

uint8_t PX4FLOW_GetQuality_Integral(void) {
    return integral_frame.quality;
}