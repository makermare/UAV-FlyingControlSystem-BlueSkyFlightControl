//
// Created by Liuyufanlyf on 2021/06/05.
//

#include "string.h"
#include "tfminiplus.h"
#include "board.h"

tfminiplus_data_t tf_data;

void TFminiPlus_Init(void) {
    tf_data.length = 0;
    tf_data.singal_strength = 0;
    tf_data.avaliable = false;
}

void TFminiPlus_Decode(uint8_t data) {
    uint8_t raw[16];

    static uint32_t lastTime;
    static uint32_t dataCnt = 0;

    uint32_t deltaT = GetSysTimeMs() - lastTime;
    lastTime = GetSysTimeMs();

    //数据间隔大于3ms则可以认为新的一帧开始了
    if (deltaT > 3) {
        dataCnt = 0;
    }

    raw[dataCnt++] = data;

    if (dataCnt == 9) {

        if (raw[0] == 0x59 && raw[1] == 0x59) {
            // Calculate the checksum
            uint16_t checksum = 0;
            for (uint8_t i = 0; i < 8; i++) {
                checksum += raw[i];
            }
            checksum &= 0xFF;

            if (checksum == raw[8]) {
                tf_data.length = raw[2] + (raw[3] << 8);
                tf_data.singal_strength = raw[4] + (raw[5] << 8);
                if (tf_data.singal_strength > 100 && tf_data.singal_strength < 65535) {
                    tf_data.avaliable = true;
                    memset(raw, 0, 16);
                } else {
                    tf_data.avaliable = false;
                    memset(raw, 0, 16);
                    return;
                }
            } else {
                tf_data.avaliable = false;
                memset(raw, 0, 16);
                return;
            }
        } else {
            tf_data.avaliable = false;
            memset(raw, 0, 16);
            return;
        }
    }
}

int TFminiPlus_GetDistance(void) {
    return tf_data.length;
}

int TFminiPlus_GetSignalStrength(void) {
    return tf_data.singal_strength;
}

bool TFminiPlus_GetAvaliable(void) {
    bool temp = tf_data.avaliable;
    tf_data.avaliable = false;
    return temp;
}