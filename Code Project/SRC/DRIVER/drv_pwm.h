#ifndef __DRV_PWM_H__
#define __DRV_PWM_H__

#include "board.h"

#include "tim.h"

void PWM_Init(void);
void TempControlPWMSet(int32_t pwmValue);
void ServoControlPWMSet(uint8_t motor, int16_t pwmValue);
void MotorPWMSet(uint8_t motor, uint16_t pwmValue);
void BuzzerPWMSet(uint16_t pwmValue);

#endif

