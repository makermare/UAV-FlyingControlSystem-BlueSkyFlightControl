//
// Created by Liuyufanlyf on 2021/06/15.
//

#ifndef LC302_H
#define LC302_H

#include "board.h"

void LC302_init(void);

void LC302_Decode(uint8_t data);

int LC302_get_X_Integral(void);

int LC302_get_Y_Integral(void);

int LC302_getTimeSpan(void);

bool LC302_getAvaliable(void);

#endif //LC302_H
