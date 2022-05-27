//
// Created by Liuyufanlyf on 2021/06/14.
//

#include "ToFaltimeter.h"

#include "board.h"
#include "flightStatus.h"
#include "faultDetect.h"
#include "tfminiplus.h"
#include "ahrs.h"

typedef struct {
    int32_t alt;
    int32_t lastAlt;
    float velocity;
    int32_t alt_offset;
} TOFALTIMETER_t;

TOFALTIMETER_t tofaltimeter;

static void ToFAltimeterDetectCheck(int32_t ToFAlt);

void ToFAltimeterDataTreat(void) {
    static uint64_t lastTime = 0;
    int32_t ToFAltTemp;

    float deltaT = (GetSysTimeUs() - lastTime) * 1e-6;
    lastTime = GetSysTimeUs();

    ToFAltTemp = TFminiPlus_GetDistance();

    // 是否需要角度补偿？
    // 若姿态角限制在 10° 以内，误差不超过 2%
    ToFAltTemp = ToFAltTemp * cosf(Radians(GetAngleMeasure().x)) * cosf(Radians(GetAngleMeasure().y));

    // 高度低通滤波
    // 激光测距是否准到能直接使用作为高度？
    tofaltimeter.alt = ToFAltTemp;

    // 速度低通滤波
    tofaltimeter.velocity =
            tofaltimeter.velocity * 0.65f + ((tofaltimeter.alt - tofaltimeter.lastAlt) / deltaT) * 0.35f;
    tofaltimeter.lastAlt = tofaltimeter.alt;

    ToFAltimeterDetectCheck(tofaltimeter.lastAlt);
}

int32_t ToFAltimeterGetAlt(void) {
    return tofaltimeter.alt;
}

float ToFAltimeterGetVelocity(void) {
    return tofaltimeter.velocity;
}

static void ToFAltimeterDetectCheck(int32_t ToFAlt) {
    static uint32_t cnt;
    static int32_t lastToFAlt = 0;

    if (ToFAlt == lastToFAlt) {
        cnt++;
        if (cnt > 100) {
            //未检测到ToF传感器
            FaultDetectSetError(TOF_UNDETECTED);
        }
    } else {
        cnt = 0;
        FaultDetectResetError(TOF_UNDETECTED);
    }

    lastToFAlt = ToFAlt;
}

