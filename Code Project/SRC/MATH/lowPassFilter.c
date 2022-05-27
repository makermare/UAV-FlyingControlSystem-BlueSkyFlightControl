/**********************************************************************************************************
                                天穹飞控 —— 致力于打造中国最好的多旋翼开源飞控
                                Github: github.com/loveuav/BlueSkyFlightControl
                                技术讨论：bbs.loveuav.com/forum-68-1.html
 * @文件     lowPassFilter.c
 * @说明     低通滤波器
 * @版本  	 V1.1
 * @作者     BlueSky
 * @网站     bbs.loveuav.com
 * @日期     2018.07
**********************************************************************************************************/
#include "lowPassFilter.h"

/**********************************************************************************************************
*函 数 名: LowPassFilter1st
*功能说明: 一阶低通滤波器
*形    参: 数据指针 新数据 滤波系数(新数据权重）
*返 回 值: 无
**********************************************************************************************************/
void LowPassFilter1st(Vector3f_t* data, Vector3f_t newData, float coff)
{
    data->x = data->x * (1 - coff) + newData.x * coff;
    data->y = data->y * (1 - coff) + newData.y * coff;
    data->z = data->z * (1 - coff) + newData.z * coff;
}

/**********************************************************************************************************
*函 数 名: LowPassFilter2ndFactorCal
*功能说明: 二阶IIR低通滤波器系数计算，该滤波器由二阶模拟低通滤波电路数字化推导而来
*形    参: 滤波器运行频率 截止频率 滤波器结构体指针
*返 回 值: 无
**********************************************************************************************************/
void LowPassFilter2ndFactorCal(float deltaT, float Fcut, LPF2ndData_t* lpf_data)
{
    float a = 1 / (2 * M_PI * Fcut * deltaT);
    lpf_data->b0 = 1 / (a*a + 3*a + 1);
    lpf_data->a1 = (2*a*a + 3*a) / (a*a + 3*a + 1);
    lpf_data->a2 = (a*a) / (a*a + 3*a + 1);
}

/**********************************************************************************************************
*函 数 名: LowPassFilter2nd
*功能说明: 二阶IIR低通滤波器过程实现
*形    参: 滤波器结构体指针 原始数据
*返 回 值: 经过滤波的数据
**********************************************************************************************************/
Vector3f_t LowPassFilter2nd(LPF2ndData_t* lpf_2nd, Vector3f_t rawData)
{
    Vector3f_t lpf_2nd_data;

    lpf_2nd_data.x = rawData.x * lpf_2nd->b0 + lpf_2nd->lastout.x * lpf_2nd->a1 - lpf_2nd->preout.x * lpf_2nd->a2;
    lpf_2nd_data.y = rawData.y * lpf_2nd->b0 + lpf_2nd->lastout.y * lpf_2nd->a1 - lpf_2nd->preout.y * lpf_2nd->a2;
    lpf_2nd_data.z = rawData.z * lpf_2nd->b0 + lpf_2nd->lastout.z * lpf_2nd->a1 - lpf_2nd->preout.z * lpf_2nd->a2;

    lpf_2nd->preout.x = lpf_2nd->lastout.x;
    lpf_2nd->preout.y = lpf_2nd->lastout.y;
    lpf_2nd->preout.z = lpf_2nd->lastout.z;

    lpf_2nd->lastout.x = lpf_2nd_data.x;
    lpf_2nd->lastout.y = lpf_2nd_data.y;
    lpf_2nd->lastout.z = lpf_2nd_data.z;

    return lpf_2nd_data;
}

/**********************************************************************************************************
*函 数 名: LinearComplementaryFilter
*功能说明: 光流速度和加速度计加速度的一阶后向差分线性互补滤波，用于求取地理坐标系xy平面的速度，使用前确保坐标系一致
*形    参: 时间常数 采样时间 xy轴加速度计的值（地理坐标系） xy轴光流速度（地理坐标系） xy轴速度（地理坐标系）
*返 回 值: 经过滤波的x，y速度
**********************************************************************************************************/
void LinearComplementaryFilter(float tao, float samplingtime, float acc_x, float acc_y, float v_opt_x, float v_opt_y,
                               float *v_x, float *v_y) {
    *v_x = tao / (tao + samplingtime) * (*v_x + samplingtime * acc_x) + samplingtime / (tao + samplingtime) * v_opt_x;
    *v_y = tao / (tao + samplingtime) * (*v_y + samplingtime * acc_y) + samplingtime / (tao + samplingtime) * v_opt_y;
}




