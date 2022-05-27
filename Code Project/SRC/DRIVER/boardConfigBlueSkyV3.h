/**********************************************************************************************************
                                天穹飞控 —— 致力于打造中国最好的多旋翼开源飞控
                                Github: github.com/loveuav/BlueSkyFlightControl
                                技术讨论：bbs.loveuav.com/forum-68-1.html
 * @文件     boardConfigBlueSkyV3.h
 * @说明     BlueSky V3 飞控硬件配置文件
 * @版本  	 V1.0
 * @作者     BlueSky
 * @网站     bbs.loveuav.com
 * @日期     2018.05
**********************************************************************************************************/

#ifndef __BOARDCONFIGBLUESKYV3_H__
#define __BOARDCONFIGBLUESKYV3_H__

/**********************************************************************************************************
*启用任务时间统计
**********************************************************************************************************/
//#define GET_RUNTIME_STATS

//define ENABLE_MAVLINK

//#define USE_LOG

// 遥控器协议选择
#define USE_SBUS
//#define USE_IBUS

/**********************************************************************************************************
*传感器安装方向
**********************************************************************************************************/
#define GYRO_ROTATION       ROTATION_ROLL_180_YAW_90
#define ACC_ROTATION        ROTATION_ROLL_180_YAW_90
#define MAG_ROTATION        ROTATION_YAW_180

/**********************************************************************************************************
*传感器配置
**********************************************************************************************************/
#define GYRO_TYPE            ICM20602        //陀螺仪型号
#define BARO_TYPE            SPL06          //气压计型号
#define BARO_COMMUNICATE_TYPE   BARO_I2C    //气压计通信接口
#define MAG_TYPE             IST8310        //罗盘型号
#define ToFALTIMETER_TYPE    TFMINIPLUS
#define OPTFLOW_TYPE         LC302

#define configUSE_SENSORHEAT 0              //是否使用传感器恒温

#define GYRO_SPI             hspi6              //陀螺仪SPI配置
#define GYRO_CS_GPIO         GPIOA
#define GYRO_CS_PIN          GPIO_PIN_4

#define BARO_I2C             hi2c1              //气压计I2C配置

#define MAG_I2C              hi2c2          //磁力计I2C配置

#define PX4FLOW_I2C          hi2c1          //PX4FLOW I2C配置

#define GPS_UART             3              //GPS串口配置
#define GPS_BAUDRATE         0              //波特率默认115200，可自动识别并对gps模块进行配置
#define DATA_UART            1              //数据链串口配置
#define DATA_BAUDRATE        921600
#define TOF_UART             0              //TOF模块串口配置
#define TOF_BAUDRATE         115200
#define SBUS_UART            8              //SBUS接收机串口配置
#define SBUS_BAUDRATE        100000

#ifdef USE_SBUS
#define SBUS_INV             1              //SBUS接收反向设置
#else
#define SBUS_INV             0
#endif

#define SBUS_INV_GPIO        GPIOB
#define SBUS_INV_PIN         GPIO_PIN_9

#define ESC_PROTOCOL         PWM            //电调输出信号协议选择

#define TEMP_TIM_FREQ        1000000        //传感器恒温PWM输出定时器配置
#define TEMP_TIM_PERIOD      2500
#define TEMP_TIM             htim1
#define TEMP_CH              TIM_CHANNEL_1

#define Beep_TIM_FREQ        2000           //蜂鸣器PWM输出频率
#define Beep_TIM             htim8
#define Beep_CH              TIM_CHANNEL_3

#define PWM_TIM_FREQ         480000       //无刷电机电调输出PWM输出定时器配置
#define PWM_TIM_PERIOD       1000
#define PWM1_TIM             htim2
#define PWM1_CH              TIM_CHANNEL_1
#define PWM2_TIM             htim2
#define PWM2_CH              TIM_CHANNEL_2
#define PWM3_TIM             htim2
#define PWM3_CH              TIM_CHANNEL_3
#define PWM4_TIM             htim2
#define PWM4_CH              TIM_CHANNEL_4

#define SERVO_PWM_TIM_NO1        htim3
#define SERVO_PWM_TIM_NO2        htim4
#define SERVO_PWM_TIM_FREQ         10000     // 50Hz Square Wave for Servo
#define SERVO_PWM_TIM_PERIOD       2000
#define SERVO_PWM1_TIM             htim3
#define SERVO_PWM1_CH              TIM_CHANNEL_1
#define SERVO_PWM2_TIM             htim3
#define SERVO_PWM2_CH              TIM_CHANNEL_2
#define SERVO_PWM3_TIM             htim4
#define SERVO_PWM3_CH              TIM_CHANNEL_3
#define SERVO_PWM4_TIM             htim4
#define SERVO_PWM4_CH              TIM_CHANNEL_4

#define ADC_VOLTAGE          hadc2           //电池电压采集ADC配置
#define ADC_VOLTAGE_CHAN     ADC_Channel_11
#define ADC_VOLTAGE_GPIO     GPIOC
#define ADC_VOLTAGE_PIN      GPIO_PIN_1
#define ADC_VOLTAGE_COEF     10.3f

#define ADC_CURRENT          hadc1           //电池电流采集ADC配置
#define ADC_CURRENT_CHAN     ADC_Channel_12
#define ADC_CURRENT_GPIO     GPIOC
#define ADC_CURRENT_PIN      GPIO_PIN_2
#define ADC_CURRENT_COEF     10.3f

/**********************************************************************************************************
*单片机Flash存储区域分配
**********************************************************************************************************/
//Flash扇区的基地址
#define FLASH_BASE_ADDR      (uint32_t)(0x08000000)
#define FLASH_END_ADDR       (uint32_t)(0x081FFFFF)

/* Base address of the Flash sectors Bank 1 */
#define ADDR_FLASH_SECTOR_0_BANK1     ((uint32_t)0x08000000) /* Base @ of Sector 0, 128 Kbytes */
#define ADDR_FLASH_SECTOR_1_BANK1     ((uint32_t)0x08020000) /* Base @ of Sector 1, 128 Kbytes */
#define ADDR_FLASH_SECTOR_2_BANK1     ((uint32_t)0x08040000) /* Base @ of Sector 2, 128 Kbytes */
#define ADDR_FLASH_SECTOR_3_BANK1     ((uint32_t)0x08060000) /* Base @ of Sector 3, 128 Kbytes */
#define ADDR_FLASH_SECTOR_4_BANK1     ((uint32_t)0x08080000) /* Base @ of Sector 4, 128 Kbytes */
#define ADDR_FLASH_SECTOR_5_BANK1     ((uint32_t)0x080A0000) /* Base @ of Sector 5, 128 Kbytes */
#define ADDR_FLASH_SECTOR_6_BANK1     ((uint32_t)0x080C0000) /* Base @ of Sector 6, 128 Kbytes */
#define ADDR_FLASH_SECTOR_7_BANK1     ((uint32_t)0x080E0000) /* Base @ of Sector 7, 128 Kbytes */

/* Base address of the Flash sectors Bank 2 */
#define ADDR_FLASH_SECTOR_0_BANK2     ((uint32_t)0x08100000) /* Base @ of Sector 0, 128 Kbytes */
#define ADDR_FLASH_SECTOR_1_BANK2     ((uint32_t)0x08120000) /* Base @ of Sector 1, 128 Kbytes */
#define ADDR_FLASH_SECTOR_2_BANK2     ((uint32_t)0x08140000) /* Base @ of Sector 2, 128 Kbytes */
#define ADDR_FLASH_SECTOR_3_BANK2     ((uint32_t)0x08160000) /* Base @ of Sector 3, 128 Kbytes */
#define ADDR_FLASH_SECTOR_4_BANK2     ((uint32_t)0x08180000) /* Base @ of Sector 4, 128 Kbytes */
#define ADDR_FLASH_SECTOR_5_BANK2     ((uint32_t)0x081A0000) /* Base @ of Sector 5, 128 Kbytes */
#define ADDR_FLASH_SECTOR_6_BANK2     ((uint32_t)0x081C0000) /* Base @ of Sector 6, 128 Kbytes */
#define ADDR_FLASH_SECTOR_7_BANK2     ((uint32_t)0x081E0000) /* Base @ of Sector 7, 128 Kbytes */

#define     FLASH_USER_PARA_START_ADDR      ADDR_FLASH_SECTOR_7_BANK1    //用户参数存储区

/**********************************************************************************************************
*串口配置
**********************************************************************************************************/
#define COM1 huart1
#define COM2 huart3
#define COM3 huart4
#define COM4 huart7
#define COM5 huart8
#define COM6 huart5

///**********************************************************************************************************
//*软件I2C引脚及参数配置
//**********************************************************************************************************/
//#define SOFT_I2C1_GPIO          GPIOB
//#define SOFT_I2C1_PIN_SCL       GPIO_Pin_6
//#define SOFT_I2C1_PIN_SDA       GPIO_Pin_7
//#define SOFT_I2C1_DELAY         0
//
//#define SOFT_I2C2_GPIO          GPIOA
//#define SOFT_I2C2_PIN_SCL       GPIO_Pin_7
//#define SOFT_I2C2_PIN_SDA       GPIO_Pin_7
//#define SOFT_I2C2_DELAY         0

#endif


