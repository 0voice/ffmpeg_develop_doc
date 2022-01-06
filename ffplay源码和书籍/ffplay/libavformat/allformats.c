/*
** 简单的注册/初始化函数，把相应的协议，文件格式，解码器等用相应的链表串起来便于查找
*/
#include "avformat.h"

extern URLProtocol file_protocol;

/* 注册所有支持的格式 */
void av_register_all(void)
{
    static int inited = 0;

	// 是否已经初始化完成
    if (inited != 0)
        return ;
    inited = 1;

	// ffplay 把 CPU 当做一个广义的 DSP。有些计算可以用 CPU 自带的加速指令来优化，ffplay 把这类函数
	// 独立出来放到 dsputil.h 和 dsputil.c 文件中， 用函数指针的方法映射到各个 CPU 具体的加速优化实现函数，
	//	此处初始化这些函数指针
    avcodec_init();

	// 注册所有的编解码器
    avcodec_register_all();

	// 注册支持的格式
    avidec_init();

	// 注册协议（文件协议、网络协议等）
    register_protocol(&file_protocol);
}
