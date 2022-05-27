#ifndef _MISSIONCONTROL_H_
#define _MISSIONCONTROL_H_

#include "mathTool.h"

enum
{
    RTH_STEP_START = 0,
    RTH_STEP_TURN,
    RTH_STEP_CLIMB,
    RTH_STEP_FLIGHT,
    RTH_STEP_TURN_BACK,
    RTH_STEP_LOITER
};

// 命令流程
enum
{
    WaitArm= 0,
    InTakeOff,
    FlightWithCommand,
    InLanding,
    WaitDisarm
};

void MissionControl(void);

#endif





