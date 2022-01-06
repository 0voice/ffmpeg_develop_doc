/************************************************************************/
/* 定义 dsp 优化限幅运算使用的查找表，实现其初始化函数          */
/************************************************************************/
#include "avcodec.h"
#include "dsputil.h"


uint8_t cropTbl[256+2 * MAX_NEG_CROP] = {0, };

/* dsp的静态初始化 ，实际作用是设置cropTbl数组的值 */
void dsputil_static_init(void)
{
    int i;

    for (i = 0; i < 256; i++)
        cropTbl[i + MAX_NEG_CROP] = i;

    for (i = 0; i < MAX_NEG_CROP; i++)
    {
        cropTbl[i] = 0;
        cropTbl[i + MAX_NEG_CROP + 256] = 255;
    }
}
