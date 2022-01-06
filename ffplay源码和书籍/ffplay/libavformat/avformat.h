#ifndef AVFORMAT_H
#define AVFORMAT_H

/*************************************************
** 定义识别文件格式和媒体类型库使用的宏、数据结构和函数，
**通常这些宏、数据结构和函数在此模块内相对全局有效
*************************************************/
#ifdef __cplusplus

extern "C"
{
#endif

	/* 版本号相关 */
#define LIBAVFORMAT_VERSION_INT ((50<<16)+(4<<8)+0)
#define LIBAVFORMAT_VERSION     50.4.0
#define LIBAVFORMAT_BUILD       LIBAVFORMAT_VERSION_INT

#define LIBAVFORMAT_IDENT       "Lavf" AV_STRINGIFY(LIBAVFORMAT_VERSION)

	/* 编解码头文件 */
#include "../libavcodec/avcodec.h"

	/* io操作头文件 */
#include "avio.h"

	/* 错误码定义 */
#define AVERROR_UNKNOWN     (-1)	// unknown error
#define AVERROR_IO          (-2)	// i/o error
#define AVERROR_NUMEXPECTED (-3)	// number syntax expected in filename
#define AVERROR_INVALIDDATA (-4)	// invalid data found
#define AVERROR_NOMEM       (-5)	// not enough memory
#define AVERROR_NOFMT       (-6)	// unknown format
#define AVERROR_NOTSUPP     (-7)	// operation not supported

	/* 在文件中跳转 */
#define AVSEEK_FLAG_BACKWARD 1		// seek backward
#define AVSEEK_FLAG_BYTE     2		// seeking based on position in bytes
#define AVSEEK_FLAG_ANY      4		// seek to any frame, even non keyframes

#define AVFMT_NOFILE        0x0001	// no file should be opened

#define PKT_FLAG_KEY		0x0001

#define AVINDEX_KEYFRAME	0x0001

#define AVPROBE_SCORE_MAX	100

#define MAX_STREAMS 20

	/*
	** 数据包（视频包或者音频包，一般一个数据包包含一帧数据）
	** 它包含了未解码的帧数据
	*/
typedef struct AVPacket
{
    int64_t pts; // presentation time stamp in time_base units  表示时间，对视频是显示时间
    int64_t dts; // decompression time stamp in time_base units  解码时间，这个不是很重要
    int64_t pos; // byte position in stream, -1 if unknown
    uint8_t *data;								// 实际保存音视频数据缓存的首地址
    int size;											// 实际保存音视频数据缓存的大小
    int stream_index;							// 当前音视频数据包对应的流索引，在本例中用于区别音频还是视频
    int flags;										//数据包的一些标记，比如是否是关键帧等。
    void(*destruct)(struct AVPacket*); // 销毁函数
} AVPacket;

/* 视频包列表 */
typedef struct AVPacketList
{
    AVPacket pkt;
    struct AVPacketList *next;
} AVPacketList;

/* 释放数据包内的数据 */
static inline void av_destruct_packet(AVPacket *pkt)
{
    av_free(pkt->data);
    pkt->data = NULL;
    pkt->size = 0;
}

/* 释放一个视频包 */
static inline void av_free_packet(AVPacket *pkt)
{
    if (pkt && pkt->destruct)
        pkt->destruct(pkt);
}

/* 
** 获取一个完整的视频包
** 读文件往数据包中填数据，
** 注意程序跑到这里时，文件偏移量已确定，要读数据的大小也确定
** 但是数据包的缓存没有分配。分配好内存后，要初始化包的一些变量 
*/
static inline int av_get_packet(ByteIOContext *s, AVPacket *pkt, int size)
{
    int ret;
    unsigned char *data;
    if ((unsigned)size > (unsigned)size + FF_INPUT_BUFFER_PADDING_SIZE)
        return AVERROR_NOMEM;

	// 内存分配
    data = av_malloc(size + FF_INPUT_BUFFER_PADDING_SIZE);
    if (!data)
        return AVERROR_NOMEM;

    memset(data + size, 0, FF_INPUT_BUFFER_PADDING_SIZE);

    pkt->pts = AV_NOPTS_VALUE;
    pkt->dts = AV_NOPTS_VALUE;
    pkt->pos =  - 1;
    pkt->flags = 0;
    pkt->stream_index = 0;
    pkt->data = data;
    pkt->size = size;
    pkt->destruct = av_destruct_packet;

    pkt->pos = url_ftell(s);

	// 读取实际的数据
    ret = url_fread(s, pkt->data, size);
    if (ret <= 0)
        av_free_packet(pkt);
    else
        pkt->size = ret;

    return ret;
}

/* 
** 探测的数据
** 为识别文件格式，要读一部分文件头数据来分析匹配 ffplay 支持的文件格式文件特征
** 该结构定义了文件名，首地址和大小。此处的读独立于其他文件操作
*/
typedef struct AVProbeData
{
	// 文件名
    const char *filename;
	// 探测到的数据
    unsigned char *buf;
	// 数据长度 
    int buf_size;
} AVProbeData;

/* 
** 文件索引结构，flags 和 size 位定义是为了节省内存
*/
typedef struct AVIndexEntry
{
    int64_t pos;
    int64_t timestamp;
    int flags: 2;
    int size: 30; //yeah trying to keep the size of this small to reduce memory requirements (its 24 vs 32 byte due to possible 8byte align)
} AVIndexEntry;

/* 
** 流：视频流、音频流 
** AVStream 抽象的表示一个媒体流，定义了所有媒体一些通用的属性
** 具体可以再细分为视频流和音频流，视频流又可以分成AVI流和，mp4流等
*/
typedef struct AVStream
{
	// 解码器上下文
    AVCodecContext *actx;  // codec context, change from AVCodecContext *codec;

	// 具体的流
    void *priv_data;       // AVIStream 在本例中，关联到 AVIStream

	// 时间基准
    AVRational time_base; // 由 av_set_pts_info()函数初始化

	// 索引对象，假如不支持随机的查找的时候使用
    AVIndexEntry *index_entries; // only used if the format does not support seeking natively
    int nb_index_entries;
    int index_entries_allocated_size;

    double frame_last_delay; // 帧最后延迟
} AVStream;

/* 格式参数 */
typedef struct AVFormatParameters
{
    int dbg; //only for debug 只保留了一个调试标志
} AVFormatParameters;

/* 
** 的输入格式（读文件的结构和指针） 
** AVInputFormat 定义输入文件容器格式（注意不是AVI，而是AV Input），着重于功能函数
** 在程序运行时有多个实例
*/
typedef struct AVInputFormat
{
	// 文件名
    const char *name;

    int priv_data_size;

	// 探测函数
    int(*read_probe)(AVProbeData*);

	// 读取头部
    int(*read_header)(struct AVFormatContext *, AVFormatParameters *ap);

	// 读取一帧数据（一个数据包）
    int(*read_packet)(struct AVFormatContext *, AVPacket *pkt);

	// 关闭文件
    int(*read_close)(struct AVFormatContext*);

    const char *extensions;     // 文件扩展名

	// 下一个输入文件的格式
    struct AVInputFormat *next;

} AVInputFormat;

/* 
** AVFormatContext 结构表示程序运行的当前文件容器格式使用的上下文， 
** 着重于所有文件容器共有的属性，程序运行后仅一个实例
*/
typedef struct AVFormatContext  // format I/O context
{
	// 输入文件的格式，AVFormatContext是一个抽象的概念，而AVInputFormat则是一个具体的概念
    struct AVInputFormat *iformat; 

    void *priv_data;

	// 广义的输入文件
    ByteIOContext pb;

	// 流的数量
    int nb_streams;

	// 流（音频流、视频流）
    AVStream *streams[MAX_STREAMS];

} AVFormatContext;

int avidec_init(void);

void av_register_input_format(AVInputFormat *format);

void av_register_all(void);

AVInputFormat *av_probe_input_format(AVProbeData *pd, int is_opened);
int match_ext(const char *filename, const char *extensions);

int av_open_input_stream(AVFormatContext **ic_ptr, ByteIOContext *pb, const char *filename, 
						 AVInputFormat *fmt, AVFormatParameters *ap);

int av_open_input_file(AVFormatContext **ic_ptr, const char *filename, AVInputFormat *fmt, 
					   int buf_size, AVFormatParameters *ap);

int av_read_frame(AVFormatContext *s, AVPacket *pkt);
int av_read_packet(AVFormatContext *s, AVPacket *pkt);
void av_close_input_file(AVFormatContext *s);
AVStream *av_new_stream(AVFormatContext *s, int id);
void av_set_pts_info(AVStream *s, int pts_wrap_bits, int pts_num, int pts_den);

int av_index_search_timestamp(AVStream *st, int64_t timestamp, int flags);
int av_add_index_entry(AVStream *st, int64_t pos, int64_t timestamp, int size, int distance, int flags);

int strstart(const char *str, const char *val, const char **ptr);
void pstrcpy(char *buf, int buf_size, const char *str);

#ifdef __cplusplus
}

#endif

#endif
