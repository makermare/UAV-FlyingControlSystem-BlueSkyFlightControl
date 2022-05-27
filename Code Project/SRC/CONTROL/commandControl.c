#include "commandControl.h"
#include "flightStatus.h"
#include "drv_uart.h"

FLIGHT_COMMAND_t flightCommand;
MOTION_COMMAND_t motionCommand;
SERVO_COMMAND_t servoCommand;

uint8_t CommandFeedbackBuffer[4];

void CommandInit(void) {
    CommandFeedbackBuffer[0] = 0xFE;
    CommandFeedbackBuffer[2] = 0xFE;
    CommandFeedbackBuffer[3] = 0xFE;

    CommandFeedback(COMMAND_STARTUP_CHECK);
}

void CommandDataDecode(uint8_t *raw) {
    //帧头帧尾校验
    if (raw[0] != 0xAA || raw[14] != 0xBB) {
        return;
    }

//    uint16_t checksum = 0;
//    for (uint8_t i = 0; i < 15; i++) {
//        checksum += raw[i];
//    }
//    checksum &= 0x00FF;
//
//    //和校验字节校验
//    if(checksum != raw[15]){
//        return;
//    }

    // 飞行控制命令，控制角度和飞行速度
    if(raw[1] == 0x41){
        flightCommand.commandRollTarget = (int16_t)(raw[2]|raw[3]<<8);
        flightCommand.commandPitchTarget = (int16_t)(raw[4]|raw[5]<<8);
        flightCommand.commandYawVelocityTarget = (int16_t)(raw[6]|raw[7]<<8);
        flightCommand.commandVelocityTargetX = (int16_t)(raw[8]|raw[9]<<8);
        flightCommand.commandVelocityTargetY = (int16_t)(raw[10]|raw[11]<<8);
        flightCommand.commandAltitudeTargetZ = (int16_t)(raw[12]|raw[13]<<8);
    }

    if(raw[1] == 0x51){
        //不能同时起飞和降落
        if(raw[2] == 1 && raw[3] == 1){
            return;
        }
        //不能同时解锁和上锁
        if(raw[4] == 1 && raw[5] == 1){
            return;
        }

        motionCommand.autoTakeOffCommand = raw[2];
        motionCommand.autoLandCommand = raw[3];
        motionCommand.armCommand = raw[4];
        motionCommand.disarmCommand = raw[5];
        motionCommand.flyAroundRod = raw[6];
        motionCommand.buzzerRing = raw[7];
    }

    if(raw[1] == 0x61){
        servoCommand.servo1Value = (int16_t)(raw[2]|raw[3]<<8);
        servoCommand.servo2Value = (int16_t)(raw[4]|raw[5]<<8);
    }
}

void CommandFeedback(uint8_t feedback) {
    CommandFeedbackBuffer[1] = feedback;
    //Uart_SendData(6, (uint8_t *) CommandFeedbackBuffer, 4);
}