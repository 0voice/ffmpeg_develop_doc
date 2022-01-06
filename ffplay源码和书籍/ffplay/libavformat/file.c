/*
** ffplay 把 file 当做类似于 rtsp，rtp，tcp 等协议的一种协议，用 file:前缀标示 file 协议。
** URLContext 结构抽象统一表示这些广义上的协议，对外提供统一的抽象接口。
** 各具体的广义协议实现文件实现 URLContext 接口。此文件实现了 file 广义协议的 URLContext 接口
*/

#include "../berrno.h"

#include "avformat.h"
#include <fcntl.h>

#ifndef CONFIG_WIN32
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#else
#include <io.h>
#define open(fname,oflag,pmode) _open(fname,oflag,pmode)
#endif

/*
** 打开文件 
*/
static int file_open(URLContext *h, const char *filename, int flags)
{
    int access;
    int fd;

	// 搜索file协议
    strstart(filename, "file:", &filename);

	// 访问标志
    if (flags &URL_RDWR)
        access = O_CREAT | O_TRUNC | O_RDWR;
    else if (flags &URL_WRONLY)
        access = O_CREAT | O_TRUNC | O_WRONLY;
    else
        access = O_RDONLY;
#if defined(CONFIG_WIN32) || defined(CONFIG_OS2) || defined(__CYGWIN__)
    access |= O_BINARY;
#endif
	// 打开文件
    fd = open(filename, access, 0666);
    if (fd < 0)
        return  - ENOENT;

	// 文件描述符赋给URLContext的priv_data
    h->priv_data = (void*)(size_t)fd;
    return 0;
}

/* 读取数据 */
static int file_read(URLContext *h, unsigned char *buf, int size)
{
    int fd = (size_t)h->priv_data;
    return read(fd, buf, size);
}

/* 写入数据 */
static int file_write(URLContext *h, unsigned char *buf, int size)
{
    int fd = (size_t)h->priv_data;
    return write(fd, buf, size);
}

/* 随机跳转 */
static offset_t file_seek(URLContext *h, offset_t pos, int whence)
{
    int fd = (size_t)h->priv_data;
    return lseek(fd, pos, whence);
}

/* 关闭文件 */
static int file_close(URLContext *h)
{
    int fd = (size_t)h->priv_data;
    return close(fd);
}

/* FILE类型的protocol */
URLProtocol file_protocol =
{
    "file",
	file_open,
	file_read,
	file_write,
	file_seek,
	file_close,
};
