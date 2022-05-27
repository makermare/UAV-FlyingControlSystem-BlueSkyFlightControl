/**********************************************************************************************************
                                天穹飞控 —— 致力于打造中国最好的多旋翼开源飞控
                                Github: github.com/loveuav/BlueSkyFlightControl
                                技术讨论：bbs.loveuav.com/forum-68-1.html
 * @文件     matrix6.c
 * @说明     6维矩阵相关运算函数
 * @版本  	 V1.0
 * @作者     BlueSky
 * @网站     bbs.loveuav.com
 * @日期     2018.11
**********************************************************************************************************/
#include "matrix6.h"

//m[i][j]:i为行，j为列

/**********************************************************************************************************
*函 数 名: Matrix6_Add
*功能说明: 6维矩阵加法: c=a+b
*形    参: 矩阵a 矩阵b 矩阵c
*返 回 值: 无
**********************************************************************************************************/
void Matrix6_Add(float a[6][6], float b[6][6], float c[6][6])
{
    uint8_t i,j;
    for (i=0; i<6; i++)
    {
        for (j=0; j<6; j++)
        {
            c[i][j] = a[i][j] + b[i][j];
        }
    }
}

/**********************************************************************************************************
*函 数 名: Matrix6_Sub
*功能说明: 6维矩阵减法: c=a-b
*形    参: 矩阵a 矩阵b 矩阵c
*返 回 值: 无
**********************************************************************************************************/
void Matrix6_Sub(float a[6][6], float b[6][6], float c[6][6])
{
    uint8_t i,j;
    for (i=0; i<6; i++)
    {
        for (j=0; j<6; j++)
        {
            c[i][j] = a[i][j] - b[i][j];
        }
    }
}

/**********************************************************************************************************
*函 数 名: Matrix6_Mul
*功能说明: 3维矩阵乘法: c=a×b
*形    参: 矩阵a 矩阵b 矩阵c
*返 回 值: 无
**********************************************************************************************************/
void Matrix6_Mul(float a[6][6], float b[6][6], float c[6][6])
{
    uint8_t i,j;
    for (i=0; i<6; i++)
    {
        for (j=0; j<6; j++)
        {
            c[i][j] = a[i][0] * b[0][j] + a[i][1] * b[1][j] + a[i][2] * b[2][j] +
                      a[i][3] * b[3][j] + a[i][4] * b[4][j] + a[i][5] * b[5][j];
        }
    }
}

/**********************************************************************************************************
*函 数 名: Matrix6_Copy
*功能说明: 6维矩阵复制： b=a
*形    参: 矩阵a 矩阵b
*返 回 值: 无
**********************************************************************************************************/
void Matrix6_Copy(float a[6][6], float b[6][6])
{
    uint8_t i,j;
    for (i=0; i<6; i++)
    {
        for (j=0; j<6; j++)
        {
            b[i][j] = a[i][j];
        }
    }
}

/**********************************************************************************************************
*函 数 名: Matrix6_Tran
*功能说明: 6维矩阵求转置
*形    参: 矩阵a 矩阵b
*返 回 值: 无
**********************************************************************************************************/
void Matrix6_Tran(float a[6][6], float b[6][6])
{
    uint8_t i,j;
    for (i=0; i<6; i++)
    {
        for (j=0; j<6; j++)
        {
            b[i][j] = a[j][i];
        }
    }
}

/**********************************************************************************************************
*函 数 名: Matrix6_Det
*功能说明: 6维矩阵求逆 b= aˉ1，使用高斯消元法
*形    参: 矩阵a 矩阵b
*返 回 值: 无
**********************************************************************************************************/
bool Matrix6_Det(float a[6][6], float b[6][6])
{
    int i, j, k;
    float max, temp;
    float t[6][6];   //临时矩阵

    //将a矩阵存放在临时矩阵t中
    for (i = 0; i < 6; i++)
    {
        for (j = 0; j < 6; j++)
        {
            t[i][j] = a[i][j];
        }
    }

    //初始化b矩阵为单位阵
    for (i = 0; i < 6; i++)
    {
        for (j = 0; j < 6; j++)
        {
            b[i][j] = (i == j) ? (float)1 : 0;
        }
    }

    for (i = 0; i < 6; i++)
    {
        //寻找主元
        max = t[i][i];
        k = i;
        for (j = i + 1; j < 6; j++)
        {
            if (fabs(t[j][i]) > fabs(max))
            {
                max = t[j][i];
                k = j;
            }
        }

        //如果主元所在行不是第i行，进行行交换
        if (k != i)
        {
            for (j = 0; j < 6; j++)
            {
                temp = t[i][j];
                t[i][j] = t[k][j];
                t[k][j] = temp;
                //b伴随交换
                temp = b[i][j];
                b[i][j] = b[k][j];
                b[k][j] = temp;
            }
        }

        //判断主元是否为0, 若是, 则矩阵a不是满秩矩阵,不存在逆矩阵
        if (t[i][i] == 0)
        {
            return false;
        }

        //消去a的第i列除去i行以外的各行元素
        temp = t[i][i];
        for (j = 0; j < 6; j++)
        {
            t[i][j] = t[i][j] / temp;       //主对角线上的元素变为1
            b[i][j] = b[i][j] / temp;       //伴随计算
        }
        for (j = 0; j < 6; j++)             //第0行->第n行
        {
            if (j != i)                     //不是第i行
            {
                temp = t[j][i];
                for (k = 0; k < 6; k++)     //第j行元素 - i行元素*j列i行元素
                {
                    t[j][k] = t[j][k] - t[i][k] * temp;
                    b[j][k] = b[j][k] - b[i][k] * temp;
                }
            }
        }
    }

    return true;
}


/**********************************************************************************************************
*函 数 名: Vector3f_Add
*功能说明: 浮点型向量加法 v3 = v1 + v2
*形    参: 向量v1 向量v2  向量v3
*返 回 值: 无
**********************************************************************************************************/
void Vector6f_Add(float v1[6], float v2[6], float v3[6])
{
    for(uint8_t i=0; i<6; i++)
        v3[i] = v1[i] + v2[i];
}

/**********************************************************************************************************
*函 数 名: Vector3f_Sub
*功能说明: 浮点型向量减法 v3 = v1 - v2
*形    参: 向量v1 向量v2  向量v3
*返 回 值: 无
**********************************************************************************************************/
void Vector6f_Sub(float v1[6], float v2[6], float v3[6])
{
    for(uint8_t i=0; i<6; i++)
        v3[i] = v1[i] - v2[i];
}

/**********************************************************************************************************
*函 数 名: Matrix6MulVector6
*功能说明: 六维矩阵与六维向量相乘
*形    参: 六维矩阵 六维向量 计算结果
*返 回 值: 无
**********************************************************************************************************/
void Matrix6MulVector6(float m[6][6], float v[6], float result[6])
{    
    for(uint8_t i=0; i<6; i++)
    {
        result[i] = m[i][0] * v[0] + m[i][1] * v[1] + m[i][2] * v[2] +
                    m[i][3] * v[3] + m[i][4] * v[4] + m[i][5] * v[5];
    }
}
