#ifndef DSPUTIL_H
#define DSPUTIL_H

/************************************************************************/
/* 定义 dsp 优化限幅运算使用的查找表及其初始化函数。  */
/************************************************************************/
#define MAX_NEG_CROP 1024

extern uint8_t cropTbl[256+2 * MAX_NEG_CROP];

void dsputil_static_init(void);

#endif
