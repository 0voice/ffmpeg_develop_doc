/************************************************************************
** 此文件实现了 URLProtocol 抽象层广义文件操作函数，
** 由于 URLProtocol 是底层其他具体文件(file,pipe 等)的简单封装，
** 这一层只是一个中转站，大部分函数都是简单中转到底层的具体实现函数                                                                     
/************************************************************************/

#include "../berrno.h"
#include "avformat.h"

/* ffmpeg可以支持多种协议，用链表把所有支持的协议串起来 */
URLProtocol *first_protocol = NULL;

/*  注册协议（所谓协议就是输入数据的载体：文件、套接字等形式） */
int register_protocol(URLProtocol *protocol)
{
    URLProtocol **p;
    p = &first_protocol;
    while (*p != NULL)
        p = &(*p)->next;
    *p = protocol;
    protocol->next = NULL;
    return 0;
}

/* 打开URL */
int url_open(URLContext **puc, const char *filename, int flags)
{
	// 上下文
    URLContext *uc;
	// 协议及操作
    URLProtocol *up;
    const char *p;
	// 协议字符串
    char proto_str[128],  *q;
    int err;

    p = filename;
    q = proto_str;

	// 确定协议的名字
    while (*p != '\0' &&  *p != ':')
    {
        if (!isalpha(*p))  // protocols can only contain alphabetic chars
            goto file_proto;
        if ((q - proto_str) < sizeof(proto_str) - 1)
            *q++ =  *p;
        p++;
    }
    // if the protocol has length 1, we consider it is a dos drive
    if (*p == '\0' || (q - proto_str) <= 1)
    {
file_proto: 
		strcpy(proto_str, "file");
    }
    else
    {
        *q = '\0';
    }

    up = first_protocol;

	// 寻找合适协议
    while (up != NULL)
    {
        if (!strcmp(proto_str, up->name))
            goto found;
        up = up->next;
    }
    err =  - ENOENT;
    goto fail;
found: 

	// URL上下文
	uc = av_malloc(sizeof(URLContext) + strlen(filename));
    if (!uc)
    {
        err =  - ENOMEM;
        goto fail;
    }
    strcpy(uc->filename, filename);
    uc->prot = up;
    uc->flags = flags;
    uc->max_packet_size = 0; // default: stream file

	// 打开URL
    err = up->url_open(uc, filename, flags);
    if (err < 0)
    {
        av_free(uc);
        *puc = NULL;
        return err;
    }
    *puc = uc;
    return 0;
fail:
	*puc = NULL;
    return err;
}

/* 读取数据 */
int url_read(URLContext *h, unsigned char *buf, int size)
{
    int ret;
    if (h->flags &URL_WRONLY)
        return AVERROR_IO;
    ret = h->prot->url_read(h, buf, size);
    return ret;
}

/* 在广义文件中跳转 */
offset_t url_seek(URLContext *h, offset_t pos, int whence)
{
    offset_t ret;

    if (!h->prot->url_seek)
        return  - EPIPE;
    ret = h->prot->url_seek(h, pos, whence);
    return ret;
}

/* 关闭URL */
int url_close(URLContext *h)
{
    int ret;

    ret = h->prot->url_close(h);
    av_free(h);
    return ret;
}

int url_get_max_packet_size(URLContext *h)
{
    return h->max_packet_size;
}
