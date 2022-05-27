/**********************************************************************************************************
                                天穹飞控 —— 致力于打造中国最好的多旋翼开源飞控
                                Github: github.com/loveuav/BlueSkyFlightControl
                                技术讨论：bbs.loveuav.com/forum-68-1.html
 * @文件     rgb.c
 * @说明     RGB驱动
 * @版本  	 V1.0
 * @作者     BlueSky
 * @网站     bbs.loveuav.com
 * @日期     2018.11
**********************************************************************************************************/
#include "rgb.h"
#include "board.h"
#include "flightStatus.h"
#include "sensor.h"

/*
Blue  - PA6
Green - PC4
Red   - PC5
*/
#define LED_BLUE_GPIO     GPIOE
#define LED_BLUE_PIN      GPIO_PIN_6
#define LED_GREEN_GPIO    GPIOE
#define LED_GREEN_PIN     GPIO_PIN_5
#define LED_RED_GPIO      GPIOE
#define LED_RED_PIN       GPIO_PIN_4

void RGB_Green_On(void) {
    HAL_GPIO_WritePin(LED_GREEN_GPIO, LED_GREEN_PIN, 0);
}

void RGB_Green_Off(void) {
    HAL_GPIO_WritePin(LED_GREEN_GPIO, LED_GREEN_PIN, 1);
}

void RGB_Red_On(void) {
    HAL_GPIO_WritePin(LED_RED_GPIO, LED_RED_PIN, 0);
}

void RGB_Red_Off(void) {
    HAL_GPIO_WritePin(LED_RED_GPIO, LED_RED_PIN, 1);
}

void RGB_Blue_On(void) {
    HAL_GPIO_WritePin(LED_BLUE_GPIO, LED_BLUE_PIN, 0);
}

void RGB_Blue_Off(void) {
    HAL_GPIO_WritePin(LED_BLUE_GPIO, LED_BLUE_PIN, 1);
}

/**********************************************************************************************************
*函 数 名: RGB_Init
*功能说明: RGB初始化
*形    参: 无
*返 回 值: 无
**********************************************************************************************************/
void RGB_Init(void) {
    RGB_Green_Off();
    RGB_Red_Off();
    RGB_Blue_Off();
}

/**********************************************************************************************************
*函 数 名: RGB_Flash
*功能说明: RGB闪烁 运行频率200Hz
*形    参: 无
*返 回 值: 无
**********************************************************************************************************/
void RGB_Flash(void) {
    static uint32_t cnt = 0;
    static uint8_t initFlag = 0;

    switch (GetInitStatus()) {
        case HEATING:
            if (initFlag == 0) {
                if (cnt > 250) {
                    initFlag = 1;
                } else if (cnt > 150) {
                    RGB_Green_Off();
                    RGB_Red_Off();
                    RGB_Blue_Off();
                } else if (cnt > 100) {
                    RGB_Green_On();
                    RGB_Red_Off();
                    RGB_Blue_Off();
                } else if (cnt > 50) {
                    RGB_Green_Off();
                    RGB_Red_On();
                    RGB_Blue_Off();
                } else {
                    RGB_Green_Off();
                    RGB_Red_Off();
                    RGB_Blue_On();
                }
            } else {
                //传感器检测正常则红灯慢闪，不正常快闪
                if (SensorCheckStatus()) {
                    if (cnt % 100 == 0) {
                        RGB_Green_Off();
                        RGB_Red_On();
                        RGB_Blue_Off();
                    }
                    if (cnt % 200 == 0) {
                        RGB_Green_Off();
                        RGB_Red_Off();
                        RGB_Blue_Off();
                    }
                } else {
                    if (cnt % 10 == 0) {
                        RGB_Green_Off();
                        RGB_Red_On();
                        RGB_Blue_Off();
                    }
                    if (cnt % 20 == 0) {
                        RGB_Green_Off();
                        RGB_Red_Off();
                        RGB_Blue_Off();
                    }
                }
            }
            break;

        case HEAT_FINISH:
            if (cnt % 100 == 0) {
                RGB_Green_Off();
                RGB_Red_Off();
                RGB_Blue_On();
            }
            if (cnt % 200 == 0) {
                RGB_Green_Off();
                RGB_Red_Off();
                RGB_Blue_Off();
            }
            break;

        case INIT_FINISH:
            if (cnt % 10 == 0) {
                RGB_Green_Off();
                RGB_Red_Off();
                RGB_Blue_Off();
            }
            if (cnt % 300 == 0) {
                RGB_Green_On();
                RGB_Red_Off();
                RGB_Blue_Off();
            }
            break;

        default:
            break;
    }

    cnt++;
}
