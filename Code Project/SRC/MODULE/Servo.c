#include "Servo.h"

//设置舵机的目标角度，范围-90°-90°
void ServoSetDeg(uint8_t num, int8_t value) {
    ServoControlPWMSet(num, value);
}