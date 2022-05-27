#ifndef _COMMANDCONTROL_H_
#define _COMMANDCONTROL_H_

#include "board.h"

enum
{
    COMMAND_RECEIVED = 0,
    FINISH_ARM_FEEDBACK,
    START_TAKEOFF_FEEDBACK,
    FINISH_TAKEOFF_FEEDBACK,
    FINISH_POSITION_HOLD,
    FINISH_MOVE_X_FEEDBACK,
    FINISH_MOVE_Y_FEEDBACK,
    FINISH_MOVE_Z_FEEDBACK,
    START_LAND_FEEDBACK,
    FINISH_LAND_FEEDBACK,
    FINISH_DISARM_FEEDBACK,

    COMMAND_STARTUP_CHECK
};

typedef struct {
    float commandRollTarget;
    float commandPitchTarget;
    float commandYawVelocityTarget;

    float commandVelocityTargetX;
    float commandVelocityTargetY;
    float commandAltitudeTargetZ;
} FLIGHT_COMMAND_t;

typedef struct {
    uint8_t autoTakeOffCommand;
    uint8_t autoLandCommand;

    uint8_t armCommand;
    uint8_t disarmCommand;

    uint8_t flyAroundRod;
  
    uint8_t buzzerRing;
} MOTION_COMMAND_t;

typedef struct {
    int16_t servo1Value;
    int16_t servo2Value;
} SERVO_COMMAND_t;

extern FLIGHT_COMMAND_t flightCommand;
extern MOTION_COMMAND_t motionCommand;
extern SERVO_COMMAND_t servoCommand;

void CommandInit(void);
void CommandDataDecode(uint8_t *raw);

void CommandFeedback(uint8_t feedback);

#endif
