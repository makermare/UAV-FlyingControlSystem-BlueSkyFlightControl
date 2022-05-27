/**********************************************************************************************************
                                天穹飞控 —— 致力于打造中国最好的多旋翼开源飞控
                                Github: github.com/loveuav/BlueSkyFlightControl
                                技术讨论：bbs.loveuav.com/forum-68-1.html
 * @文件     drv_adc.c
 * @说明     ADC驱动
 * @版本  	 V1.0
 * @作者     BlueSky
 * @网站     bbs.loveuav.com
 * @日期     2018.06
**********************************************************************************************************/
#include "drv_adc.h"

/**********************************************************************************************************
*函 数 名: Adc_Init
*功能说明: ADC初始化
*形    参: 无
*返 回 值: 无
**********************************************************************************************************/
void Adc_Init(void)
{
    MX_ADC1_Init();
    MX_ADC2_Init();

    if (HAL_ADCEx_Calibration_Start(&hadc1, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED) != HAL_OK) {
        Error_Handler();
    }

    if (HAL_ADCEx_Calibration_Start(&hadc2, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED) != HAL_OK) {
        Error_Handler();
    }

    if (HAL_ADC_Start(&hadc1) != HAL_OK) {
        Error_Handler();
    }

    if (HAL_ADC_Start(&hadc2) != HAL_OK) {
        Error_Handler();
    }
}

/**********************************************************************************************************
*函 数 名: GetVoltageAdcValue
*功能说明: 获取电压ADC采样值
*形    参: 无
*返 回 值: ADC采样值
**********************************************************************************************************/
uint16_t GetVoltageAdcValue(void)
{
    static uint16_t adcTemp;

    HAL_ADC_Start(&ADC_VOLTAGE);
    HAL_ADC_PollForConversion(&ADC_VOLTAGE, 1);
    adcTemp = HAL_ADC_GetValue(&ADC_VOLTAGE);

    return (adcTemp * 330 / 0xFFF);
}

/**********************************************************************************************************
*函 数 名: GetCurrentAdcValue
*功能说明: 获取电流ADC采样值
*形    参: 无
*返 回 值: ADC采样值
**********************************************************************************************************/
uint16_t GetCurrentAdcValue(void)
{
    static uint16_t adcTemp;

    HAL_ADC_Start(&ADC_CURRENT);
    HAL_ADC_PollForConversion(&ADC_CURRENT, 1);
    adcTemp = HAL_ADC_GetValue(&ADC_CURRENT);

    return (adcTemp * 330 / 0xFFF);
}







