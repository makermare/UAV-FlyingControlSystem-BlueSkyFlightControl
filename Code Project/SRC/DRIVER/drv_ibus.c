//
// Created by 12484 on 2021/07/18.
//

#include "drv_ibus.h"
#include "rc.h"

uint8_t  ibus_raw[32];

RCDATA_t ibusData;

/**********************************************************************************************************
*函 数 名: Ibus_Init
*功能说明: ibus数据解析初始化
*形    参: 无
*返 回 值: 无
**********************************************************************************************************/
void Ibus_Init(void)
{
    if(SBUS_INV == 1) {
        HAL_GPIO_WritePin(SBUS_INV_GPIO, SBUS_INV_PIN, 1);
    }
    else
        HAL_GPIO_WritePin(SBUS_INV_GPIO, SBUS_INV_PIN, 0);
}

/**********************************************************************************************************
*函 数 名: Ibus_Decode
*功能说明: ibus协议解析
*形    参: 输入数据
*返 回 值: 无
**********************************************************************************************************/
void Ibus_Decode(uint8_t data)
{
    static uint32_t lastTime;
    static uint32_t dataCnt  = 0;
    static uint8_t  initFlag = 0;

    if(GetSysTimeMs() < 2000)
        return;

    uint32_t deltaT = GetSysTimeMs() - lastTime;
    lastTime = GetSysTimeMs();

    //发送频率200Hz，间隔时间约4.7ms
    //数据间隔大于3ms则可以认为新的一帧开始了
    if(deltaT > 3)
    {
        dataCnt = 0;
    }

    //接收数据
    ibus_raw[dataCnt++] = data;

    //每帧数据长度为25
    if(dataCnt == 32)
    {
        //判断帧头是否正确
        if(ibus_raw[0] != 0x20 || ibus_raw[1] != 0x40)
            return;

        //每个通道数据占据11个字节，15-18通道的解析较为麻烦，暂时不做
        //转换摇杆数据量程为[1000:2000]
        ibusData.roll     = ibus_raw[3] + ((ibus_raw[4]&0x0F)<<8);
        ibusData.pitch    = ibus_raw[5] + ((ibus_raw[6]&0x0F)<<8);
        ibusData.throttle = ibus_raw[7] + ((ibus_raw[8]&0x0F)<<8);
        ibusData.yaw      = ibus_raw[9] + ((ibus_raw[10]&0x0F)<<8);
        ibusData.aux1     = ibus_raw[11] + ((ibus_raw[12]&0x0F)<<8);
        ibusData.aux2     = ibus_raw[13] + ((ibus_raw[14]&0x0F)<<8);
        ibusData.aux3     = ibus_raw[15] + ((ibus_raw[16]&0x0F)<<8);
        ibusData.aux4     = ibus_raw[17] + ((ibus_raw[18]&0x0F)<<8);
        ibusData.aux5     = ibus_raw[19] + ((ibus_raw[20]&0x0F)<<8);
        ibusData.aux6     = ibus_raw[21] + ((ibus_raw[22]&0x0F)<<8);
        ibusData.aux7     = ibus_raw[23] + ((ibus_raw[23]&0x0F)<<8);
        ibusData.aux8     = ibus_raw[25] + ((ibus_raw[24]&0x0F)<<8);

        //一帧数据解析完成
        RcDataUpdate(ibusData);

        if(!initFlag)
        {
            initFlag = 1;
        }
    }
}