/*
** 有缓存的广义文件 ByteIOContext 相关的文件操作
** ByteIOContext和URLContext的不同在于，ByteIOContext是带缓存的，即ByteIOContext的操作都是基于缓存的而不是基于文件描述符
** 可以这样理解URLContext读取出来的数据交给ByteIOContext管理，因此URLContext在ByteIOContext的下面，是ByteIOContext的底层
*/

#include "../berrno.h"
#include "avformat.h"
#include "avio.h"
#include <stdarg.h>

/* 最大的缓存长度 */
#define IO_BUFFER_SIZE 32768

/*
** 初始化 ByteIOContext 结构
*/
int init_put_byte(ByteIOContext *s, 
				  unsigned char *buffer, 
				  int buffer_size, 
				  int write_flag, 
				  void *opaque, 
				  int(*read_buf)(void *opaque, uint8_t *buf, int buf_size), 
				  int(*write_buf)(void *opaque, uint8_t *buf, int buf_size), 
				  offset_t(*seek)(void *opaque, offset_t offset, int whence))
{
    s->buffer = buffer;
    s->buffer_size = buffer_size;
    s->buf_ptr = buffer;
    s->write_flag = write_flag;
    if (!s->write_flag)
        s->buf_end = buffer;
    else
        s->buf_end = buffer + buffer_size;
    s->opaque = opaque;
    s->write_buf = write_buf;
    s->read_buf = read_buf;
    s->seek = seek;
    s->pos = 0;
    s->must_flush = 0;
    s->eof_reached = 0;
    s->error = 0;
    s->max_packet_size = 0;

    return 0;
}

/*
** 广义文件 ByteIOContext 的 seek 操作
*/
offset_t url_fseek(ByteIOContext *s, offset_t offset, int whence)
{
    offset_t offset1;

    if (whence != SEEK_CUR && whence != SEEK_SET)
        return  - EINVAL;

    if (whence == SEEK_CUR)
    {
        offset1 = s->pos - (s->buf_end - s->buffer) + (s->buf_ptr - s->buffer);
        if (offset == 0)
            return offset1;
        offset += offset1;
    }
    offset1 = offset - (s->pos - (s->buf_end - s->buffer));
    if (offset1 >= 0 && offset1 <= (s->buf_end - s->buffer))
    {
        s->buf_ptr = s->buffer + offset1; // can do the seek inside the buffer
    }
    else
    {
        if (!s->seek)
            return  - EPIPE;
        s->buf_ptr = s->buffer;
        s->buf_end = s->buffer;
        if (s->seek(s->opaque, offset, SEEK_SET) == (offset_t) - EPIPE)
            return  - EPIPE;
        s->pos = offset;
    }
    s->eof_reached = 0;

    return offset;
}

/* 跳转 */
void url_fskip(ByteIOContext *s, offset_t offset)
{
    url_fseek(s, offset, SEEK_CUR);
}

/* 获取长度 */
offset_t url_ftell(ByteIOContext *s)
{
    return url_fseek(s, 0, SEEK_CUR);
}

/* 文件大小 */
offset_t url_fsize(ByteIOContext *s)
{
    offset_t size;

    if (!s->seek)
        return  - EPIPE;
    size = s->seek(s->opaque,  - 1, SEEK_END) + 1;
    s->seek(s->opaque, s->pos, SEEK_SET);
    return size;
}

/* 是否到达文件尾 */
int url_feof(ByteIOContext *s)
{
    return s->eof_reached;
}

/* 错误码 */
int url_ferror(ByteIOContext *s)
{
    return s->error;
}

// Input stream
/* 填充buffer，也是一个读取文件的操作  */
static void fill_buffer(ByteIOContext *s)
{
    int len;

    if (s->eof_reached)
        return ;

    len = s->read_buf(s->opaque, s->buffer, s->buffer_size);
    if (len <= 0)
    {   // do not modify buffer if EOF reached so that a seek back can be done without rereading data
        s->eof_reached = 1;

        if (len < 0)
            s->error = len;
    }
    else
    {
        s->pos += len;
        s->buf_ptr = s->buffer;
        s->buf_end = s->buffer + len;
    }
}

/* 读取一个字节 */
int get_byte(ByteIOContext *s) // NOTE: return 0 if EOF, so you cannot use it if EOF handling is necessary
{
    if (s->buf_ptr < s->buf_end)
    {
        return  *s->buf_ptr++;
    }
    else
    {
		// 读取数据
        fill_buffer(s);
        if (s->buf_ptr < s->buf_end)
            return  *s->buf_ptr++;
        else
            return 0;
    }
}

/* 从广义文件 ByteIOContext 中以小端方式读取两个字节,实现代码充分复用 get_byte()函数 */
unsigned int get_le16(ByteIOContext *s)
{
    unsigned int val;
    val = get_byte(s);
    val |= get_byte(s) << 8;
    return val;
}

/*  从广义文件 ByteIOContext 中以小端方式读取四个字节,实现代码充分复用 get_le16()函数 */
unsigned int get_le32(ByteIOContext *s)
{
    unsigned int val;
    val = get_le16(s);
    val |= get_le16(s) << 16;
    return val;
}

#define url_write_buf NULL

/*  简单中转读操作函数 */
static int url_read_buf(void *opaque, uint8_t *buf, int buf_size)
{
    URLContext *h = opaque;
    return url_read(h, buf, buf_size);
}

/* 简单中转 seek 操作函数 */
static offset_t url_seek_buf(void *opaque, offset_t offset, int whence)
{
    URLContext *h = opaque;
    return url_seek(h, offset, whence);
}

/* 设置并分配广义文件 ByteIOContext 内部缓存的大小，更多的应用在修改内部缓存大小场合 */
int url_setbufsize(ByteIOContext *s, int buf_size) // must be called before any I/O
{
    uint8_t *buffer;
    buffer = av_malloc(buf_size);
    if (!buffer)
        return  - ENOMEM;

    av_free(s->buffer);
    s->buffer = buffer;
    s->buffer_size = buf_size;
    s->buf_ptr = buffer;
    if (!s->write_flag)
        s->buf_end = buffer;
    else
        s->buf_end = buffer + buf_size;
    return 0;
}

/*  打开广义文件 ByteIOContext  */
int url_fopen(ByteIOContext *s, const char *filename, int flags)
{
    URLContext *h;
	uint8_t *buffer;
    int buffer_size, max_packet_size;
    int err;

    err = url_open(&h, filename, flags);
    if (err < 0)
        return err;
   
    max_packet_size = url_get_max_packet_size(h);
    if (max_packet_size)
    {
        buffer_size = max_packet_size; // no need to bufferize more than one packet
    }
    else
    {
        buffer_size = IO_BUFFER_SIZE;
    }

    buffer = av_malloc(buffer_size);
    if (!buffer)
	{
        url_close(h);
        return  - ENOMEM;
	}

    if (init_put_byte(s,
					  buffer, 
					  buffer_size, 
					  (h->flags & URL_WRONLY || h->flags & URL_RDWR), 
					  h, 
					  url_read_buf, 
					  url_write_buf, 
					  url_seek_buf) < 0)
    {
        url_close(h);
        av_free(buffer);
        return AVERROR_IO;
    }

    s->max_packet_size = max_packet_size;

    return 0;
}

/* 关闭广义文件 ByteIOContext */
int url_fclose(ByteIOContext *s)
{
    URLContext *h = s->opaque;

    av_free(s->buffer);
    memset(s, 0, sizeof(ByteIOContext));
    return url_close(h);
}

/* 广义文件 ByteIOContext 读操作 */
int url_fread(ByteIOContext *s, unsigned char *buf, int size) // get_buffer
{
    int len, size1;

    size1 = size;
    while (size > 0)
    {
        len = s->buf_end - s->buf_ptr;
        if (len > size)
            len = size;
        if (len == 0)
        {
            if (size > s->buffer_size)
            {
                len = s->read_buf(s->opaque, buf, size);
                if (len <= 0)
                {
                    s->eof_reached = 1;
                    if (len < 0)
                        s->error = len;
                    break;
                }
                else
                {
                    s->pos += len;
                    size -= len;
                    buf += len;
                    s->buf_ptr = s->buffer;
                    s->buf_end = s->buffer /* + len*/;
                }
            }
            else
            {
                fill_buffer(s);
                len = s->buf_end - s->buf_ptr;
                if (len == 0)
                    break;
            }
        }
        else
        {
            memcpy(buf, s->buf_ptr, len);
            buf += len;
            s->buf_ptr += len;
            size -= len;
        }
    }
    return size1 - size;
}
