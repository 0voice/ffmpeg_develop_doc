/************************************************************************/
/* 定义分数                                                                     */
/************************************************************************/
#ifndef RATIONAL_H
#define RATIONAL_H

/* 分数结构体 */
typedef struct AVRational
{
    int num; // numerator   // 分子
    int den; // denominator // 分母
} AVRational;

/* 计算分数 */
static inline double av_q2d(AVRational a)
{
    return a.num / (double)a.den;
}

#endif
