#include "vector3.h"
#include "stdbool.h"

#define USER_BANK_SEL	(0x7F)
#define USER_BANK_0		(0x00)
#define USER_BANK_1		(0x10)
#define USER_BANK_2		(0x20)
#define USER_BANK_3		(0x30)

#define PWR_MGMT_1 		(0x06)
#define PWR_MGMT_2		(0x07)
#define GYRO_CONFIG_1	(0x01)

#define CLK_BEST_AVAIL	(0x01)
#define GYRO_RATE_250	(0x00)
#define GYRO_LPF_17HZ 	(0x29)

void ICM20489_PowerOn();
bool ICM20948_Detect(void);
uint16_t ICM20948_Init(void);

void ICM20948_ReadAcc(Vector3f_t* acc);
void ICM20948_ReadGyro(Vector3f_t* gyro);
void ICM20948_ReadMag(int16_t magn[3]);
void ICM20948_ReadTemp(float* temp);

void ICM20948_SelectBank(uint8_t bank);
void ICM20948_Disable_I2C(void);
void ICM20948_CSHigh(void);
void ICM20948_CSLow(void);
void ICM20948_SetClock(uint8_t clk);
void ICM20948_AccelGyroOff(void);
void ICM20948_AccelGyroOn(void);
void ICM20948_SetGyroRateLPF(uint8_t rate, uint8_t lpf);
void ICM20948_SetGyroLPF(uint8_t lpf);
