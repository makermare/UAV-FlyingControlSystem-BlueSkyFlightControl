//
// Created by Liuyufanlyf on 2021/05/14.
//
/**********************************************************************************************************
 * @文件     spl06-007.c
 * @说明     spl06气压传感器驱动
 * @版本  	 V1.0
 * @日期     2021.05
**********************************************************************************************************/
#include "drv_spi.h"
#include "spl06-007.h"

#define PRESSURE_REG 0X00
#define TEMP_REG 0X03
#define PRS_CFG 0x06
#define TMP_CFG 0x07
#define MEAS_CFG 0x08

#define SPL06_REST_VALUE 0x09
#define SPL06_REST_REG 0x0C

#define PRODUCT_ID 0X0D

static struct spl06007_t spl06007;
static struct spl06007_t *p_spl06007;

void spl06007_write(unsigned char regadr, unsigned char val) {
    Spi_BaroSingleWrite(regadr, val);
}

uint8_t spl06007_read(unsigned char regadr) {
    uint8_t reg_data;
    Spi_BaroMultiRead(regadr | 0x80, &reg_data, 1);
    return reg_data;
}

/*****************************************************************************
 函 数 名  : spl06007_get_calib_param
 功能描述  : 获取校准参数
 输入参数  : void
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :
*****************************************************************************/
void spl06007_get_calib_param(void) {
    uint32_t h;
    uint32_t m;
    uint32_t l;
    h = spl06007_read(0x10);
    l = spl06007_read(0x11);
    p_spl06007->calib_param.c0 = (int16_t) h << 4 | l >> 4;
    p_spl06007->calib_param.c0 = (p_spl06007->calib_param.c0 & 0x0800) ? (0xF000 | p_spl06007->calib_param.c0)
                                                                       : p_spl06007->calib_param.c0;
    h = spl06007_read(0x11);
    l = spl06007_read(0x12);
    p_spl06007->calib_param.c1 = (int16_t) (h & 0x0F) << 8 | l;
    p_spl06007->calib_param.c1 = (p_spl06007->calib_param.c1 & 0x0800) ? (0xF000 | p_spl06007->calib_param.c1)
                                                                       : p_spl06007->calib_param.c1;
    h = spl06007_read(0x13);
    m = spl06007_read(0x14);
    l = spl06007_read(0x15);
    p_spl06007->calib_param.c00 = (int32_t) h << 12 | (int32_t) m << 4 | (int32_t) l >> 4;
    p_spl06007->calib_param.c00 = (p_spl06007->calib_param.c00 & 0x080000) ? (0xFFF00000 | p_spl06007->calib_param.c00)
                                                                           : p_spl06007->calib_param.c00;
    h = spl06007_read(0x15);
    m = spl06007_read(0x16);
    l = spl06007_read(0x17);
    p_spl06007->calib_param.c10 = (int32_t) h << 16 | (int32_t) m << 8 | l;
    p_spl06007->calib_param.c10 = (p_spl06007->calib_param.c10 & 0x080000) ? (0xFFF00000 | p_spl06007->calib_param.c10)
                                                                           : p_spl06007->calib_param.c10;
    h = spl06007_read(0x18);
    l = spl06007_read(0x19);
    p_spl06007->calib_param.c01 = (int16_t) h << 8 | l;
    h = spl06007_read(0x1A);
    l = spl06007_read(0x1B);
    p_spl06007->calib_param.c11 = (int16_t) h << 8 | l;
    h = spl06007_read(0x1C);
    l = spl06007_read(0x1D);
    p_spl06007->calib_param.c20 = (int16_t) h << 8 | l;
    h = spl06007_read(0x1E);
    l = spl06007_read(0x1F);
    p_spl06007->calib_param.c21 = (int16_t) h << 8 | l;
    h = spl06007_read(0x20);
    l = spl06007_read(0x21);
    p_spl06007->calib_param.c30 = (int16_t) h << 8 | l;
}


void spl06007_init(void) {
    p_spl06007 = &spl06007; /* read Chip Id */
    p_spl06007->i32rawPressure = 0;
    p_spl06007->i32rawTemperature = 0;
    p_spl06007->chip_id = 0x34;
    spl06007_get_calib_param();

    spl06007_rateset(PRESSURE_SENSOR, 128, 32);
    // sampling rate = 1Hz; Temperature oversample = 1;
    spl06007_rateset(TEMPERATURE_SENSOR, 32, 8);

    //Start background measurement
    spl06007_start_continuous(CONTINUOUS_P_AND_T);
}


/*****************************************************************************
 函 数 名  : spl06007_rateset
 功能描述  :  设置温度传感器的每秒采样次数以及过采样率
 输入参数  : uint8 u8OverSmpl  过采样率         Maximal = 128
             uint8 u8SmplRate  每秒采样次数(Hz) Maximal = 128
             uint8 iSensor     0: Pressure; 1: Temperature
 输出参数  : 无
 返 回 值  : 无
*****************************************************************************/
void spl06007_rateset(uint8_t iSensor, uint8_t u8SmplRate, uint8_t u8OverSmpl) {
    uint8_t reg = 0;
    int32_t i32kPkT = 0;
    switch (u8SmplRate) {
        case 2:
            reg |= (1 << 5);
            break;
        case 4:
            reg |= (2 << 5);
            break;
        case 8:
            reg |= (3 << 5);
            break;
        case 16:
            reg |= (4 << 5);
            break;
        case 32:
            reg |= (5 << 5);
            break;
        case 64:
            reg |= (6 << 5);
            break;
        case 128:
            reg |= (7 << 5);
            break;
        case 1:
        default:
            break;
    }
    switch (u8OverSmpl) {
        case 2:
            reg |= 1;
            i32kPkT = 1572864;
            break;
        case 4:
            reg |= 2;
            i32kPkT = 3670016;
            break;
        case 8:
            reg |= 3;
            i32kPkT = 7864320;
            break;
        case 16:
            i32kPkT = 253952;
            reg |= 4;
            break;
        case 32:
            i32kPkT = 516096;
            reg |= 5;
            break;
        case 64:
            i32kPkT = 1040384;
            reg |= 6;
            break;
        case 128:
            i32kPkT = 2088960;
            reg |= 7;
            break;
        case 1:
        default:
            i32kPkT = 524288;
            break;
    }

    if (iSensor == 0) {
        p_spl06007->i32kP = i32kPkT;
        spl06007_write(0x06, reg);
        if (u8OverSmpl > 8) {
            reg = spl06007_read(0x09);
            spl06007_write(0x09, reg | 0x04);
        }
    }
    if (iSensor == 1) {
        p_spl06007->i32kT = i32kPkT;
        spl06007_write(0x07, reg | 0x80);  //Using mems temperature
        if (u8OverSmpl > 8) {
            reg = spl06007_read(0x09);
            spl06007_write(0x09, reg | 0x08);
        }
    }
}

/*****************************************************************************
 函 数 名  : spl06007_start_temperature
 功能描述  : 发起一次温度测量
 输入参数  : void
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :
*****************************************************************************/
void spl06007_start_temperature(void) {
    spl06007_write(0x08, 0x02);
}

/*****************************************************************************
 函 数 名  : spl06007_start_pressure
 功能描述  : 发起一次压力值测量
 输入参数  : void
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :
*****************************************************************************/
void spl06007_start_pressure(void) {
    spl06007_write(MEAS_CFG, 0x01);
}

/*****************************************************************************
 函 数 名  : spl06007_start_continuous
 功能描述  : Select node for the continuously measurement
 输入参数  : uint8 mode  1: pressure; 2: temperature; 3: pressure and temperature
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :
*****************************************************************************/
void spl06007_start_continuous(uint8_t mode) {
    spl06007_write(0x08, mode + 4);
}

/*****************************************************************************
 函 数 名  : spl06007_get_raw_temp
 功能描述  : 获取温度的原始值，并转换成32Bits整数
 输入参数  : void
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :
*****************************************************************************/
void spl06007_get_raw_temp(void) {
    uint8_t h[3] = {0};

    h[0] = spl06007_read(0x03);
    h[1] = spl06007_read(0x04);
    h[2] = spl06007_read(0x05);

    p_spl06007->i32rawTemperature = (int32_t) h[0] << 16 | (int32_t) h[1] << 8 | (int32_t) h[2];
    p_spl06007->i32rawTemperature = (p_spl06007->i32rawTemperature & 0x800000) ?
                                    (0xFF000000 | p_spl06007->i32rawTemperature) :
                                    p_spl06007->i32rawTemperature;
}

/*****************************************************************************
 函 数 名  : spl06007_get_raw_pressure
 功能描述  : 获取压力原始值，并转换成32bits整数
 输入参数  : void
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :
*****************************************************************************/
void spl06007_get_raw_pressure(void) {
    uint8_t h[3];

    h[0] = spl06007_read(0x00);
    h[1] = spl06007_read(0x01);
    h[2] = spl06007_read(0x02);

    p_spl06007->i32rawPressure = (int32_t) h[0] << 16 | (int32_t) h[1] << 8 | (int32_t) h[2];
    p_spl06007->i32rawPressure = (p_spl06007->i32rawPressure & 0x800000) ?
                                (0xFF000000 | p_spl06007->i32rawPressure) :
                                p_spl06007->i32rawPressure;
}


/*****************************************************************************
 函 数 名  : spl06007_get_temperature
 功能描述  : 在获取原始值的基础上，返回浮点校准后的温度值
 输入参数  : void
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :
*****************************************************************************/
float spl06007_get_temperature(void) {
    float fTCompensate;
    float fTsc;

    fTsc = p_spl06007->i32rawTemperature / (float) p_spl06007->i32kT;
    fTCompensate = p_spl06007->calib_param.c0 * 0.5 + p_spl06007->calib_param.c1 * fTsc;
    return fTCompensate;
}

/*****************************************************************************
 函 数 名  : spl06007_get_pressure
 功能描述  : 在获取原始值的基础上，返回浮点校准后的压力值
 输入参数  : void
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :
*****************************************************************************/
float spl06007_get_pressure(void) {
    float fTsc, fPsc;
    float qua2, qua3;
    float fPCompensate;

    fTsc = p_spl06007->i32rawTemperature / (float) p_spl06007->i32kT;
    fPsc = p_spl06007->i32rawPressure / (float) p_spl06007->i32kP;
    qua2 = p_spl06007->calib_param.c10 + fPsc * (p_spl06007->calib_param.c20 + fPsc * p_spl06007->calib_param.c30);
    qua3 = fTsc * fPsc * (p_spl06007->calib_param.c11 + fPsc * p_spl06007->calib_param.c21);
    //qua3 = 0.9f *fTsc * fPsc * (p_spl06007->calib_param.c11 + fPsc * p_spl06007->calib_param.c21);

    fPCompensate = p_spl06007->calib_param.c00 + fPsc * qua2 + fTsc * p_spl06007->calib_param.c01 + qua3;
    //fPCompensate = p_spl06007->calib_param.c00 + fPsc * qua2 + 0.9f *fTsc  * p_spl06007->calib_param.c01 + qua3;
    return fPCompensate;
}
