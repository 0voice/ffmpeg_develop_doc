/************************************************************************/
/* �������                                                                     */
/************************************************************************/
#ifndef RATIONAL_H
#define RATIONAL_H

/* �����ṹ�� */
typedef struct AVRational
{
    int num; // numerator   // ����
    int den; // denominator // ��ĸ
} AVRational;

/* ������� */
static inline double av_q2d(AVRational a)
{
    return a.num / (double)a.den;
}

#endif
