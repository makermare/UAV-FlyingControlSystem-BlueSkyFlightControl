/**********************************************************************************************************
                                天穹飞控 —— 致力于打造中国最好的多旋翼开源飞控
                                Github: github.com/loveuav/BlueSkyFlightControl
                                技术讨论：bbs.loveuav.com/forum-68-1.html
 * @文件     drv_usb.c
 * @说明     usb转串口驱动
 * @版本  	 V1.2
 * @作者     BlueSky
 * @网站     bbs.loveuav.com
 * @日期     2018.07
**********************************************************************************************************/
#include "drv_usb.h"
#include "usbd_cdc_if.h"

/**********************************************************************************************************
*函 数 名: Usb_Init
*功能说明: USB驱动初始化
*形    参: 无
*返 回 值: 无
**********************************************************************************************************/
void Usb_Init(void)
{
    // Has been inited in freertos.c
    //MX_USB_DEVICE_Init();
}

/**********************************************************************************************************
*函 数 名: Usb_Send
*功能说明: USB发送数据
*形    参: 发送数据缓冲区指针 数据长度
*返 回 值: 无
**********************************************************************************************************/
void Usb_Send(uint8_t *dataToSend, uint8_t length)
{
    CDC_Transmit_FS(dataToSend, length);
}
