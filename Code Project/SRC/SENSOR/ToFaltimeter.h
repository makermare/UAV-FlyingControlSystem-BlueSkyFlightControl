//
// Created by Liuyufanlyf on 2021/06/14.
//

#ifndef TOF_ALTIMETER_H
#define TOF_ALTIMETER_H

#include "stdint.h"
#include "stdbool.h"

void ToFAltimeterDataTreat(void);

int32_t ToFAltimeterGetAlt(void);

float ToFAltimeterGetVelocity(void);


#endif //TOF_ALTIMETER_H
