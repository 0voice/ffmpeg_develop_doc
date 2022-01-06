#ifndef AVIO_H
#define AVIO_H

/*
** 文件读写模块定义的数据结构和函数声明
*/

#define URL_EOF (-1)

typedef int64_t offset_t;

/* 文件访问的权限的定义 */
#define URL_RDONLY 0
#define URL_WRONLY 1
#define URL_RDWR   2

/* 
** URL上下文：表示一个输入（文件或者套接字流 ）
** URLContext 结构表示程序运行的当前广义文件协议使用的上下文，着重于所有广义文件协议共有的
** 属性(并且是在程序运行时才能确定其值)和关联其他结构的字段 
*/
typedef struct URLContext
{
	// 具体的协议
    struct URLProtocol *prot;
    int flags;
    int max_packet_size; // if non zero, the stream is packetized with this max packet size
    void *priv_data;  // 对于file，则是一个文件句柄，如果是其他的协议，就可能是其他东西
    char filename[1]; // specified filename
} URLContext;

/* 
** URL协议（针对文件或者套接字流的操作）
** URLProtocol 定义广义的文件协议，着重于功能函数
** 一种广义的文件协议对应一个 URLProtocol 结构，
** 本例删掉了 pipe，udp，tcp等输入协议，仅保留一个 file 协议
*/
typedef struct URLProtocol
{
    const char *name;
    int(*url_open)(URLContext *h, const char *filename, int flags);
    int(*url_read)(URLContext *h, unsigned char *buf, int size);
    int(*url_write)(URLContext *h, unsigned char *buf, int size);
    offset_t(*url_seek)(URLContext *h, offset_t pos, int whence);
    int(*url_close)(URLContext *h);
    struct URLProtocol *next;
} URLProtocol;

/* 
** 广义上的，带缓冲的文件
*/
typedef struct ByteIOContext
{
	// 数据缓冲区
    unsigned char *buffer;
	// 缓冲区大小
    int buffer_size;
	// 有效的字节范围
    unsigned char *buf_ptr,  *buf_end;
	// 其他
    void *opaque;
	// 读取数据
    int (*read_buf)(void *opaque, uint8_t *buf, int buf_size);
	// 写入数据
    int (*write_buf)(void *opaque, uint8_t *buf, int buf_size);
	// 调整buffer指针
    offset_t(*seek)(void *opaque, offset_t offset, int whence);
	// 位置
    offset_t pos;    // position in the file of the current buffer
	// 是不是必须要刷新
    int must_flush;  // true if the next seek should flush
	// 是否到达文件尾部
    int eof_reached; // true if eof reached
	// 是否可写
    int write_flag;  // true if open for writing
	// 最大的数据包的大小
    int max_packet_size;
	// 错误码
    int error;       // contains the error code or 0 if no error happened
} ByteIOContext;

int url_open(URLContext **h, const char *filename, int flags);
int url_read(URLContext *h, unsigned char *buf, int size);
int url_write(URLContext *h, unsigned char *buf, int size);
offset_t url_seek(URLContext *h, offset_t pos, int whence);
int url_close(URLContext *h);
int url_get_max_packet_size(URLContext *h);

int register_protocol(URLProtocol *protocol);

int init_put_byte(ByteIOContext *s, 
				  unsigned char *buffer, 
				  int buffer_size, 
				  int write_flag, 
				  void *opaque, 
				  int(*read_buf)(void *opaque, uint8_t *buf, int buf_size),
				  int(*write_buf)(void *opaque, uint8_t *buf, int buf_size), 
				  offset_t(*seek)(void *opaque, offset_t offset, int whence));

offset_t url_fseek(ByteIOContext *s, offset_t offset, int whence);
void url_fskip(ByteIOContext *s, offset_t offset);
offset_t url_ftell(ByteIOContext *s);
offset_t url_fsize(ByteIOContext *s);
int url_feof(ByteIOContext *s);
int url_ferror(ByteIOContext *s);

int url_fread(ByteIOContext *s, unsigned char *buf, int size); // get_buffer
int get_byte(ByteIOContext *s);
unsigned int get_le32(ByteIOContext *s);
unsigned int get_le16(ByteIOContext *s);

int url_setbufsize(ByteIOContext *s, int buf_size);
int url_fopen(ByteIOContext *s, const char *filename, int flags);
int url_fclose(ByteIOContext *s);

int url_open_buf(ByteIOContext *s, uint8_t *buf, int buf_size, int flags);
int url_close_buf(ByteIOContext *s);

#endif
