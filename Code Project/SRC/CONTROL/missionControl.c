/**********************************************************************************************************
                                天穹飞控 —— 致力于打造中国最好的多旋翼开源飞控
                                Github: github.com/loveuav/BlueSkyFlightControl
                                技术讨论：bbs.loveuav.com/forum-68-1.html
 * @文件     missionControl.c
 * @说明     自主控制功能，如自动下降、自动起飞、自动返航、自动航线等
 * @版本  	 V1.1
 * @作者     BlueSky
 * @网站     bbs.loveuav.com
 * @日期     2018.07
**********************************************************************************************************/
#include "missionControl.h"
#include "waypointControl.h"
#include "flightStatus.h"
#include "flightControl.h"
#include "commandControl.h"
#include "board.h"
#include "navigation.h"
#include "ahrs.h"
#include "gps.h"
#include "buzzer.h"
#include "Servo.h"

static Vector3f_t posCtlTarget;
static float yawHold;

uint8_t rthStep;
uint8_t commandStep;
static float rthHeight = 3000;  //预设返航高度 单位：cm
static float rthWaitTime = 2000;  //返航回到Home点后的悬停等待时间 单位：ms

void AutoLand(void);

void AutoTakeOff(void);

void CommandFlight();

void ReturnToHome(void);

/**********************************************************************************************************
*函 数 名: MissionControl
*功能说明: 自主控制任务
*形    参: 无
*返 回 值: 无
**********************************************************************************************************/
void MissionControl(void)
{
    uint8_t flightMode;

    //获取当前飞行模式
    flightMode = GetFlightMode();

    //判断当前飞行模式并选择执行对应的飞行任务
    if (flightMode == AUTOLAND)
    {
        //自动降落
        AutoLand();

        //锁定状态下转为自动模式
        if (GetArmedStatus() == DISARMED)
            SetFlightMode(AUTO);
    }
    else if (flightMode == AUTOTAKEOFF)
    {
        AutoTakeOff();
    }
    else if (flightMode == COMMAND)
    {
        CommandFlight();
    }
    else if (flightMode == RETURNTOHOME)
    {
        //自动返航
        ReturnToHome();

        //未起飞时转为自动降落
        if (GetFlightStatus() < IN_AIR)
            SetFlightMode(AUTOLAND);
    }
    else if (flightMode == AUTOPILOT)
    {
        //自动航线
        WaypointControl();
    }
    else
    {
        //在非任务控制模式下，更新当前位置目标
        posCtlTarget = GetCopterPosition();
        yawHold = GetCopterAngle().z;

        //命令流程复位
        commandStep = 0;
        //返航步骤标志位复位
        rthStep = 0;
    }
}

// 命令模式使用流程管理，
// 流程命令分为五种：解锁、起飞、命令控制、降落和上锁，对应五段流程：待解锁阶段、起飞阶段、命令控制阶段、降落阶段、待上锁阶段
void CommandFlight()
{
    Vector3f_t posNow = GetCopterPosition();

    // 待机模式刷新目标位置
    if (GetFlightStatus() == STANDBY)
    {
        posCtlTarget.x = posNow.x;
        posCtlTarget.y = posNow.y;
        //使能位置控制
        SetPosCtlStatusX(ENABLE);
        SetPosCtlStatusY(ENABLE);
        SetPosControlStatus(POS_HOLD);
        SetPosOuterCtlTarget(posCtlTarget);

        yawHold = GetCopterAngle().z;
        //设置航向锁定目标角度，使能航向锁定
        SetYawCtlTarget(yawHold);
        SetYawHoldStatus(ENABLE);
    }

    switch (commandStep)
    {
    case WaitArm:
    {
        if (motionCommand.armCommand == 1)
        {
            SetArmedStatus(ARMED);
            //向上位机报告解锁完成
            CommandFeedback(FINISH_ARM_FEEDBACK);
            commandStep = InTakeOff;
        }
        break;
    }
    case InTakeOff:
    {
        if (motionCommand.autoTakeOffCommand == 1)
        {
            //交由自动起飞模式控制起飞
            SetFlightMode(AUTOTAKEOFF);
            //向上位机报告开始起飞
            CommandFeedback(START_TAKEOFF_FEEDBACK);
            commandStep = FlightWithCommand;
        }
        break;
    }
    case FlightWithCommand:
    {
        if (motionCommand.autoLandCommand == 1)
        {
            //以当前位置定点降落
            posCtlTarget = posNow;
            SetPosOuterCtlTarget(posCtlTarget);
            SetPosControlStatus(POS_HOLD);
            SetPosCtlStatusX(ENABLE);
            SetPosCtlStatusY(ENABLE);
            //收到降落指令后跳转降落阶段
            commandStep = InLanding;
            break;
        }

        //因为xy速度不为0时飞机默认一定会进行控制，所以设置此模式必须保证xy的速度目标为0且高度适当
        if (motionCommand.flyAroundRod == 1)
        {
            SetPosControlStatus(POS_CHANGED);
            SetPosCtlStatusX(DISABLE);
            SetPosCtlStatusY(DISABLE);
            //运动过程中不断刷新位置外环控制目标
            SetPosOuterCtlTargetX(posNow.x);
            SetPosOuterCtlTargetY(posNow.y);

            //航向锁定失能，此时以flightCommand的yaw角速度来控制航向
            SetYawHoldStatus(DISABLE);
            //运动过程中不断刷新航向角控制目标
            yawHold = GetCopterAngle().z;
            SetYawCtlTarget(yawHold);
        }
        else
        {
            SetPosControlStatus(POS_HOLD);
            //使能航向锁定，定点控制
            SetYawHoldStatus(ENABLE);
            SetPosCtlStatusX(ENABLE);
            SetPosCtlStatusY(ENABLE);
        }

        if (flightCommand.commandVelocityTargetX != 0)
        {
            SetPosControlStatus(POS_CHANGED);
            SetPosCtlStatusX(DISABLE);
            SetPosInnerCtlTargetX(flightCommand.commandVelocityTargetX);
            //运动过程中不断刷新外环控制目标
            SetPosOuterCtlTargetX(posNow.x);
        }
        else
        {
            SetPosControlStatus(POS_HOLD);
            SetPosCtlStatusX(ENABLE);
        }

        if (flightCommand.commandVelocityTargetY != 0)
        {
            SetPosControlStatus(POS_CHANGED);
            SetPosCtlStatusY(DISABLE);
            SetPosInnerCtlTargetY(flightCommand.commandVelocityTargetY);
            //运动过程中不断刷新外环控制目标
            SetPosOuterCtlTargetY(posNow.y);
        }
        else
        {
            SetPosControlStatus(POS_HOLD);
            SetPosCtlStatusY(ENABLE);
        }

        //高度控制采用直接控制高度外环的方式，高度目标为0时表示保持当前高度
        if(flightCommand.commandAltitudeTargetZ != 0)
        {
            SetAltControlStatus(ALT_CHANGED);
            SetAltCtlStatus(ENABLE);
            SetAltOuterCtlTarget(flightCommand.commandAltitudeTargetZ);
        }
        else
        {
            SetAltControlStatus(ALT_HOLD);
            SetAltCtlStatus(ENABLE);
        }
        break;
    }
    case InLanding:
    {
        //降落命令的执行与其他模式稍有不同，当飞控处于命令飞行阶段时，上位机下达降落命令后才前进到降落阶段，因此直接执行
        //交由自动降落模式降落
        SetFlightMode(AUTOLAND);
        //向上位机报告开始降落
        CommandFeedback(START_LAND_FEEDBACK);
        commandStep = WaitDisarm;
        break;
    }
    case WaitDisarm:
    {
        if (motionCommand.disarmCommand == 1)
        {
            SetArmedStatus(DISARMED);
            //向上位机报告降落完成
            CommandFeedback(FINISH_DISARM_FEEDBACK);
            //上锁后跳转回初始步骤
            if(GetArmedStatus() == DISARMED)
            {
                commandStep = WaitArm;
            }
        }
        break;
    }
    default:
    {

    }
    }

    if(motionCommand.buzzerRing == 1)
    {
        buzzerRing();
    }
    else
    {
        buzzerStop();
    }

    //设置舵机旋转角度
    ServoSetDeg(1, servoCommand.servo1Value);
    ServoSetDeg(2, servoCommand.servo2Value);
}

/**********************************************************************************************************
*函 数 名: AutoLand
*功能说明: 自动降落
*形    参: 无
*返 回 值: 无
**********************************************************************************************************/
void AutoLand(void)
{
    static float velCtlTarget = 0;
    float altitude = GetCopterPosition().z;

    //使能航向锁定
    SetYawHoldStatus(ENABLE);

    //原作者在此处根据GPS状态判断是否使能位置控制，先去掉
    //使能位置控制
    SetPosCtlStatusX(ENABLE);
    SetPosCtlStatusY(ENABLE);

    //更新位置控制目标
    SetPosOuterCtlTarget(posCtlTarget);

    //直接控制速度，禁用高度控制
    SetAltCtlStatus(DISABLE);

    //更新高度控制状态
    SetAltControlStatus(ALT_CHANGED);

    //由于地面效应的存在，飞机将接地时升力显著上升，减速降落的目标速度不应过低，以免低高度下位置发生漂移
    if (altitude < 10)
    {
        velCtlTarget = -20.f;
    }
    else if (altitude < 60)
    {
        velCtlTarget = -1.f * altitude - 10.f;
    }
    else if (altitude < 500)
    {
        velCtlTarget = velCtlTarget * 0.99f - 70.0f * 0.01f;
    }
    else if (altitude < 1000)
    {
        velCtlTarget = velCtlTarget * 0.99f - 80.0f * 0.01f;
    }
    else if (altitude < 5000)
    {
        velCtlTarget = velCtlTarget * 0.99f - 200.0f * 0.01f;
    }
    else
    {
        velCtlTarget = velCtlTarget * 0.99f - 250.0f * 0.01f;
    }

    //更新高度内环控制目标，速度限幅提高观感和安全性
    velCtlTarget = ConstrainFloat(velCtlTarget, -60, 60);
    SetAltInnerCtlTarget(velCtlTarget);

    if (GetCopterVelocity().z == 0)
    {
        //通知上位机降落完成
        //CommandFeedback(FINISH_LAND_FEEDBACK);
        //将控制权交还命令模式
        SetFlightMode(COMMAND);
    }
}

/**********************************************************************************************************
*函 数 名: AutoTakeOff
*功能说明: 自动起飞
*形    参: 无
*返 回 值: 无
**********************************************************************************************************/
void AutoTakeOff(void)
{
    // 高度控制目标
    static float AltCtlTarget = 100;
    float altitude = GetCopterPosition().z;

    static bool FinishTakeOff;
    static uint32_t FinishTakeOffTime;

    // 待机模式刷新目标位置
    if (GetFlightStatus() == STANDBY)
    {
        posCtlTarget.x = GetCopterPosition().x;
        posCtlTarget.y = GetCopterPosition().y;

        yawHold = GetCopterAngle().z;
    }

    //设置航向锁定目标角度，使能航向锁定
    SetYawCtlTarget(yawHold);
    SetYawHoldStatus(ENABLE);

    //使能位置控制
    SetPosCtlStatusX(ENABLE);
    SetPosCtlStatusY(ENABLE);
    SetPosControlStatus(POS_HOLD);

    //更新位置控制目标
    SetPosOuterCtlTarget(posCtlTarget);

    //使能高度控制
    SetAltCtlStatus(ENABLE);

    //更新高度控制状态
    SetAltControlStatus(ALT_CHANGED);

    //更新高度外环控制目标
    SetAltOuterCtlTarget(AltCtlTarget);

    float err = (altitude > AltCtlTarget) ? (altitude - AltCtlTarget) : (AltCtlTarget - altitude);

    if (err < 1)
    {
        SetAltControlStatus(ALT_HOLD);
        //通知上位机起飞完成
        //CommandFeedback(FINISH_TAKEOFF_FEEDBACK);
        //将控制权交还命令模式
        SetFlightMode(COMMAND);
    }
}

/**********************************************************************************************************
*函 数 名: ReturnToHome
*功能说明: 自动返航
*形    参: 无
*返 回 值: 无
**********************************************************************************************************/
void ReturnToHome(void)
{
    /********************************************自动返航控制逻辑**********************************************

    1.先判断当前Home点距离，若小于一定值，则直接转入步骤4
    2.记录当前飞机航向，然后更改航向目标为Home方向（机头朝Home点）
    3.判断当前高度，若小于返航高度则上升，反之进入下一步
    4.设定位置控制目标为Home点，开始返航，根据Home点距离调节飞行速度
    5.到达Home点上方，将航向目标变为初始记录值，并等待一定时间（预设值），转入自动降落模式，返航完毕

    全程检测GPS定位状态，若失去定位则转入自动降落模式
    **********************************************************************************************************/

    static uint32_t timeRecord[8];
    static float originYaw;
    static float distanceToHome;
    static float directionToHome;
    Vector3f_t position;
    static float velCtlTargert;
    Vector3f_t syncRatio;
    Vector3f_t posCtlError;
    Vector3f_t homePos;

    //获取当前位置
    position = GetCopterPosition();
    //计算Home点距离
    distanceToHome = GetDistanceToHome(position);
    //计算Home点方向
    directionToHome = GetDirectionToHome(position);

    switch (rthStep)
    {
    case RTH_STEP_START:
        //记录起始航向
        originYaw = GetCopterAngle().z;

        //初始化飞行速度
        velCtlTargert = 0;

        //使能位置控制
        SetPosCtlStatusX(ENABLE);
        SetPosCtlStatusY(ENABLE);
        //设置位置控制目标
        SetPosOuterCtlTarget(posCtlTarget);
        //更新位置控制状态
        SetPosControlStatus(POS_HOLD);

        //判断当前Home点距离，小于一定值则转入步骤4
        if (distanceToHome < 1000)
        {
            //更新位置控制目标
            posCtlTarget = GetCopterPosition();
            //转入步骤4
            rthStep = RTH_STEP_FLIGHT;
            break;
        }

        rthStep = RTH_STEP_TURN;
        break;

    case RTH_STEP_TURN:
        //设置Home方向为航向目标
        SetYawCtlTarget(directionToHome);

        if (abs(directionToHome - GetCopterAngle().z) < 3)
        {
            rthStep = RTH_STEP_CLIMB;
            posCtlTarget.z = GetCopterPosition().z;
        }

        timeRecord[RTH_STEP_TURN] = GetSysTimeMs();
        break;

    case RTH_STEP_CLIMB:
        //等待1秒
        if (GetSysTimeMs() - timeRecord[RTH_STEP_TURN] < 1000)
            break;

        //若当前高度小于返航高度，则更新高度控制目标，反之保持当前高度
        if (position.z < rthHeight)
        {
            //使能高度控制
            SetAltCtlStatus(ENABLE);

            //设置高度目标为返航高度
            SetAltOuterCtlTarget(rthHeight);
        }

        //到达目标高度后进入下一步骤
        if (position.z - rthHeight > -50)
        {
            rthStep = RTH_STEP_FLIGHT;
        }

        timeRecord[RTH_STEP_CLIMB] = GetSysTimeMs();
        break;

    case RTH_STEP_FLIGHT:
        //等待1秒
        if (GetSysTimeMs() - timeRecord[RTH_STEP_CLIMB] < 1000)
            break;

        homePos = GetHomePosition();
        posCtlError.x = posCtlTarget.x - homePos.x;
        posCtlError.y = posCtlTarget.y - homePos.y;

        //设置位置控制环最大输出
        SetMaxPosOuterCtl(800);

        //计算XY轴位置控制同步速率
        if (abs(posCtlError.x) > abs(posCtlError.y))
        {
            syncRatio.x = 1;
            syncRatio.y = abs(posCtlError.y / posCtlError.x);
        }
        else
        {
            syncRatio.x = abs(posCtlError.x / posCtlError.y);
            syncRatio.y = 1;
        }

        //根据Home点距离调整返航飞行速度
        if (distanceToHome < 1500)
            velCtlTargert = velCtlTargert * 0.999f + (100.0f / 500) * 0.001f;   //1m/s
        else if (distanceToHome < 2000)
            velCtlTargert = velCtlTargert * 0.999f + (200.0f / 500) * 0.001f;   //2m/s
        else if (distanceToHome < 3000)
            velCtlTargert = velCtlTargert * 0.999f + (300.0f / 500) * 0.001f;   //3m/s
        else if (distanceToHome < 8000)
            velCtlTargert = velCtlTargert * 0.999f + (500.0f / 500) * 0.001f;   //5m/s
        else
            velCtlTargert = velCtlTargert * 0.999f + (800.0f / 500) * 0.001f;   //8m/s

        //计算位置控制目标，目标值以一定速率匀速变化
        if (abs(posCtlError.x) > 1)
            posCtlTarget.x -= velCtlTargert * syncRatio.x * (posCtlError.x / abs(posCtlError.x));
        if (abs(posCtlError.y) > 1)
            posCtlTarget.y -= velCtlTargert * syncRatio.y * (posCtlError.y / abs(posCtlError.y));

        if (distanceToHome > 1000)
        {
            //不断更新航向目标为Home方向
            SetYawCtlTarget(directionToHome);
        }

        //Home点距离小于50cm则进入下一步
        if (distanceToHome < 50)
        {
            rthStep = RTH_STEP_TURN_BACK;
            posCtlTarget = homePos;
        }

        //设置位置控制目标
        SetPosOuterCtlTarget(posCtlTarget);

        timeRecord[RTH_STEP_FLIGHT] = GetSysTimeMs();
        break;

    case RTH_STEP_TURN_BACK:
        //等待1秒
        if (GetSysTimeMs() - timeRecord[RTH_STEP_FLIGHT] < 1000)
            break;

        //将航向控制目标设为初始记录值
        SetYawCtlTarget(originYaw);

        if (abs(originYaw - GetCopterAngle().z) < 3)
        {
            rthStep = RTH_STEP_LOITER;
        }

        timeRecord[RTH_STEP_TURN_BACK] = GetSysTimeMs();
        break;

    case RTH_STEP_LOITER:
        //等待预定的时间
        if (GetSysTimeMs() - timeRecord[RTH_STEP_TURN_BACK] < rthWaitTime)
            break;

        //返航完毕，转入自动降落模式
        SetFlightMode(AUTOLAND);
        //重置返航步骤标志位
        rthStep = RTH_STEP_START;
        break;

    default:
        break;
    }

    //使能高度控制
    SetAltCtlStatus(ENABLE);
    //使能航向锁定
    SetYawHoldStatus(ENABLE);

    //GPS不可用时直接转入自动降落模式
    if (GpsGetFixStatus() == false)
    {
        SetFlightMode(AUTOLAND);
    }
}

