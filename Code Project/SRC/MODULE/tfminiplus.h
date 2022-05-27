//
// Created by Liuyufanlyf on 2021/06/05.
//

#ifndef TFMINIPLUS_H
#define TFMINIPLUS_H

#include "stdbool.h"

#include "usart.h"

typedef struct {
    int length;
    int singal_strength;
    uint8_t raw_data[9];
    bool avaliable;
} tfminiplus_data_t;

extern tfminiplus_data_t tf_data;

void TFminiPlus_Init(void);

void TFminiPlus_Decode(uint8_t data);

int TFminiPlus_GetDistance(void);

int TFminiPlus_GetSignalStrength(void);

bool TFminiPlus_GetAvaliable(void);

#endif //TFMINIPLUS_H
