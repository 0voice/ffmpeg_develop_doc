#ifndef AVCODEC_H
#define AVCODEC_H

/*
** 定义编解码器库使用的宏、数据结构和函数，通常这些宏、数据结构和函数在此模块内相对全局有效
*/
#ifdef __cplusplus
extern "C"
{
#endif

#include "../libavutil/avutil.h"

#define FFMPEG_VERSION_INT      0x000409
#define FFMPEG_VERSION          "CVS"

#define AV_STRINGIFY(s)         AV_TOSTRING(s)
#define AV_TOSTRING(s) #s

#define LIBAVCODEC_VERSION_INT  ((51<<16)+(8<<8)+0)
#define LIBAVCODEC_VERSION      51.8.0
#define LIBAVCODEC_BUILD        LIBAVCODEC_VERSION_INT

#define LIBAVCODEC_IDENT        "Lavc" AV_STRINGIFY(LIBAVCODEC_VERSION)

#define AV_NOPTS_VALUE          int64_t_C(0x8000000000000000)
#define AV_TIME_BASE            1000000

	/* 解码器ID （简化版的解码器只支持两种解码方式：truespeech和msrle）*/
enum CodecID
{
    CODEC_ID_TRUESPEECH,
    CODEC_ID_MSRLE,
	CODEC_ID_NONE
};

/* 解码器类型 */
enum CodecType
{
    CODEC_TYPE_UNKNOWN =  - 1,
	// 视频
	CODEC_TYPE_VIDEO, 
	// 音频
    CODEC_TYPE_AUDIO,
    CODEC_TYPE_DATA
};

#define AVCODEC_MAX_AUDIO_FRAME_SIZE 192000 // 1 second of 48khz 32bit audio

#define FF_INPUT_BUFFER_PADDING_SIZE 8

/*
AVPicture 和 AVFrame 主要表示解码过程中的使用缓存，通常帧缓存是 YUV 格式，输出格式有 YUV
也有 RGB 格式，所以定义了 4 个 data 指针来表示分量
*/

/* 图像 */
typedef struct AVPicture
{
    uint8_t *data[4];
    int linesize[4];
} AVPicture;

/* 帧 */
typedef struct AVFrame
{
    uint8_t *data[4];
    int linesize[4];
    uint8_t *base[4];
} AVFrame;

/*
** 程序运行时当前 Codec 使用的上下文， 着重于所有 Codec 共有的属性(并且是在程序运行时才能确定其
值)，codec 和 priv_data 关联其他结构的字段，便于在数据结构间跳转
*/

/* 程序运行的当前 Codec 使用的上下文 */
typedef struct AVCodecContext
{
    int bit_rate;
    int frame_number; // audio or video frame number

    unsigned char *extradata; // Codec的私有数据，对Audio是WAVEFORMATEX结构扩展字节。
    int extradata_size; // 对Video是BITMAPINFOHEADER后的扩展字节

    int width, height;

    enum PixelFormat pix_fmt;

    int sample_rate; // samples per sec  // audio only
    int channels;
    int bits_per_sample;
    int block_align;

	// 解码器
    struct AVCodec *codec;
    void *priv_data;

    enum CodecType codec_type; // see CODEC_TYPE_xxx
    enum CodecID codec_id; // see CODEC_ID_xxx

    int(*get_buffer)(struct AVCodecContext *c, AVFrame *pic);
    void(*release_buffer)(struct AVCodecContext *c, AVFrame *pic);
    int(*reget_buffer)(struct AVCodecContext *c, AVFrame *pic);

    int internal_buffer_count;
    void *internal_buffer;

	// 调色板控制
    struct AVPaletteControl *palctrl;
}AVCodecContext;

/* 编解码器（它就可以表示整个编码器、解码器）*/
/* 表示音视频编解码器， 着重于功能函数， 一种媒体类型对应一个 AVCodec
结构，在程序运行时有多个实例串联成链表便于查找*/
typedef struct AVCodec
{
    const char *name; // 名字
    enum CodecType type; // 类型，有 Video，Audio，Data 等类型
    enum CodecID id; // id
    int priv_data_size; // 对应的上下文的大小
    int(*init)(AVCodecContext*); // 初始化
    int(*encode)(AVCodecContext *, uint8_t *buf, int buf_size, void *data); // 编码函数
    int(*close)(AVCodecContext*); // 关闭
    int(*decode)(AVCodecContext *, void *outdata, int *outdata_size, uint8_t *buf, int buf_size); // 解码函数
    int capabilities; // 标示Codec 的能力

    struct AVCodec *next; // 用于把所有 Codec 串成一个链表，便于遍历
}AVCodec;

#define AVPALETTE_SIZE 1024
#define AVPALETTE_COUNT 256

/* 调色板大小和大小宏定义，每个调色板四字节(R,G,B,α)。有很多的视频图像颜色种类比较少，用索引
间接表示每个像素的颜色值，就可以用调色板和索引值实现简单的大约的 4:1 压缩比。 */
/* 调色板控制,调色板数据结构定义，保存调色板数据。 */
typedef struct AVPaletteControl
{
    // demuxer sets this to 1 to indicate the palette has changed; decoder resets to 0
    int palette_changed;

    /* 4-byte ARGB palette entries, stored in native byte order; note that
     * the individual palette components should be on a 8-bit scale; if
     * the palette data comes from a IBM VGA native format, the component
     * data is probably 6 bits in size and needs to be scaled */
    unsigned int palette[AVPALETTE_COUNT];

} AVPaletteControl;

int avpicture_alloc(AVPicture *picture, int pix_fmt, int width, int height);

void avpicture_free(AVPicture *picture);

int avpicture_fill(AVPicture *picture, uint8_t *ptr, int pix_fmt, int width, int height);
int avpicture_get_size(int pix_fmt, int width, int height);
void avcodec_get_chroma_sub_sample(int pix_fmt, int *h_shift, int *v_shift);

int img_convert(AVPicture *dst, int dst_pix_fmt, const AVPicture *src, int pix_fmt,
				int width, int height);

void avcodec_init(void);

void register_avcodec(AVCodec *format);
AVCodec *avcodec_find_decoder(enum CodecID id);

AVCodecContext *avcodec_alloc_context(void);

int avcodec_default_get_buffer(AVCodecContext *s, AVFrame *pic);
void avcodec_default_release_buffer(AVCodecContext *s, AVFrame *pic);
int avcodec_default_reget_buffer(AVCodecContext *s, AVFrame *pic);
void avcodec_align_dimensions(AVCodecContext *s, int *width, int *height);
int avcodec_check_dimensions(void *av_log_ctx, unsigned int w, unsigned int h);

int avcodec_open(AVCodecContext *avctx, AVCodec *codec);

int avcodec_decode_audio(AVCodecContext *avctx, int16_t *samples, int *frame_size_ptr,
						 uint8_t *buf, int buf_size);
int avcodec_decode_video(AVCodecContext *avctx, AVFrame *picture, int *got_picture_ptr,
						 uint8_t *buf, int buf_size);

int avcodec_close(AVCodecContext *avctx);

void avcodec_register_all(void);

void avcodec_default_free_buffers(AVCodecContext *s);

void *av_malloc(unsigned int size);
void *av_mallocz(unsigned int size);
void *av_realloc(void *ptr, unsigned int size);
void av_free(void *ptr);
void av_freep(void *ptr);
void *av_fast_realloc(void *ptr, unsigned int *size, unsigned int min_size);

void img_copy(AVPicture *dst, const AVPicture *src, int pix_fmt, int width, int height);

#ifdef __cplusplus
}

#endif

#endif
