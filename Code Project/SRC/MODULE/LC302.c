//
// Created by liuyufanlyf on 2021/06/15.
//

#include "LC302.h"
#include "stdint.h"
#include "string.h"
#include "stdbool.h"
#include "drv_uart.h"

typedef struct {
    uint8_t frame_head;
    uint8_t frame_length;
    int16_t flow_x_integral;
    int16_t flow_y_integral;
    uint16_t integration_timespan;
    uint16_t ground_distance;
    uint8_t valid;
    uint8_t version;
    uint8_t check;
    uint8_t frame_tail;

    uint8_t raw[14];
    bool available
} lc302_t;

lc302_t lc302;

void LC302_init(void) {

}

void LC302_Decode(uint8_t data) {
    static uint32_t lastTime;
    static uint32_t dataCnt = 0;

    if (GetSysTimeMs() < 2000)
        return;

    uint32_t deltaT = GetSysTimeMs() - lastTime;
    lastTime = GetSysTimeMs();

    //数据间隔大于3ms则可以认为新的一帧开始了
    if (deltaT > 3) {
        dataCnt = 0;
    }

    lc302.raw[dataCnt++] = data;

    if (dataCnt == 14) {
        if (lc302.raw[0] == 0xFE && lc302.raw[13] == 0x55) {
            lc302.frame_length = lc302.raw[1];
            lc302.flow_x_integral = lc302.raw[2] + (lc302.raw[3] << 8);
            lc302.flow_y_integral = lc302.raw[4] + (lc302.raw[5] << 8);
            lc302.integration_timespan = lc302.raw[6] + (lc302.raw[7] << 8);
            lc302.ground_distance = lc302.raw[8] + (lc302.raw[9] << 8);
            lc302.valid = lc302.raw[10];
            lc302.version = lc302.raw[11];
            lc302.check = lc302.raw[12];

            //如果valid值为0xF5，则说明此帧有效
            if (lc302.valid != 0xF5) {
                lc302.flow_x_integral = 0;
                lc302.flow_y_integral = 0;
                lc302.available = false;
            } else {
                lc302.available = true;
            }
        } else {
            lc302.available = false;
        }
    }
}

int LC302_get_X_Integral(void) {
    return lc302.flow_x_integral;
}

int LC302_get_Y_Integral(void) {
    return lc302.flow_y_integral;
}

int LC302_getTimeSpan(void){
    return lc302.integration_timespan;
}

bool LC302_getAvaliable(void) {
    return lc302.available;
}


