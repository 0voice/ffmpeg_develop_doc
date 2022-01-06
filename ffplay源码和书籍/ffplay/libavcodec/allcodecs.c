#include "avcodec.h"

/************************************************************************/
/* 简单的注册/初始化函数，把编解码器用相应的链表串起来便于查找识别 */
/************************************************************************/

/* 两个全局的编解码器 */
extern AVCodec truespeech_decoder;
extern AVCodec msrle_decoder;

/* 注册所有的编解码器 */
void avcodec_register_all(void)
{
    static int inited = 0;

    if (inited != 0)
        return ;

    inited = 1;

	// 这个简化版的ffmpeg只支持两种编解码方式：MSRLE、truespeech
    register_avcodec(&msrle_decoder);

    register_avcodec(&truespeech_decoder);
}
