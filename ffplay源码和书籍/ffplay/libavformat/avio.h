#ifndef AVIO_H
#define AVIO_H

/*
** �ļ���дģ�鶨������ݽṹ�ͺ�������
*/

#define URL_EOF (-1)

typedef int64_t offset_t;

/* �ļ����ʵ�Ȩ�޵Ķ��� */
#define URL_RDONLY 0
#define URL_WRONLY 1
#define URL_RDWR   2

/* 
** URL�����ģ���ʾһ�����루�ļ������׽����� ��
** URLContext �ṹ��ʾ�������еĵ�ǰ�����ļ�Э��ʹ�õ������ģ����������й����ļ�Э�鹲�е�
** ����(�������ڳ�������ʱ����ȷ����ֵ)�͹��������ṹ���ֶ� 
*/
typedef struct URLContext
{
	// �����Э��
    struct URLProtocol *prot;
    int flags;
    int max_packet_size; // if non zero, the stream is packetized with this max packet size
    void *priv_data;  // ����file������һ���ļ�����������������Э�飬�Ϳ�������������
    char filename[1]; // specified filename
} URLContext;

/* 
** URLЭ�飨����ļ������׽������Ĳ�����
** URLProtocol ���������ļ�Э�飬�����ڹ��ܺ���
** һ�ֹ�����ļ�Э���Ӧһ�� URLProtocol �ṹ��
** ����ɾ���� pipe��udp��tcp������Э�飬������һ�� file Э��
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
** �����ϵģ���������ļ�
*/
typedef struct ByteIOContext
{
	// ���ݻ�����
    unsigned char *buffer;
	// ��������С
    int buffer_size;
	// ��Ч���ֽڷ�Χ
    unsigned char *buf_ptr,  *buf_end;
	// ����
    void *opaque;
	// ��ȡ����
    int (*read_buf)(void *opaque, uint8_t *buf, int buf_size);
	// д������
    int (*write_buf)(void *opaque, uint8_t *buf, int buf_size);
	// ����bufferָ��
    offset_t(*seek)(void *opaque, offset_t offset, int whence);
	// λ��
    offset_t pos;    // position in the file of the current buffer
	// �ǲ��Ǳ���Ҫˢ��
    int must_flush;  // true if the next seek should flush
	// �Ƿ񵽴��ļ�β��
    int eof_reached; // true if eof reached
	// �Ƿ��д
    int write_flag;  // true if open for writing
	// �������ݰ��Ĵ�С
    int max_packet_size;
	// ������
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
