#include "./libavformat/avformat.h"

#if defined(CONFIG_WIN32)
#include <sys/types.h>
#include <sys/timeb.h>
#include <windows.h>
#else
#include <fcntl.h>
#include <sys/time.h>
#endif

#include <time.h>

#include <math.h>
#include <SDL.h>
#include <SDL_thread.h>

#ifdef CONFIG_WIN32
#undef main // We don't want SDL to override our main()
#endif

#pragma comment(lib, "SDL.lib")

/* 退出事件 */
#define FF_QUIT_EVENT   (SDL_USEREVENT + 2)

/* 视频队列的最长长度 */
#define MAX_VIDEOQ_SIZE (5 * 256 * 1024)
/* 音频队列的最大长度 */
#define MAX_AUDIOQ_SIZE (5 * 16 * 1024)

/* 图像队列的长度 */
#define VIDEO_PICTURE_QUEUE_SIZE 1

/* 数据包队列 */
typedef struct PacketQueue
{
	// 头指针和尾指针
    AVPacketList *first_pkt,  *last_pkt;
	// 队列长度
    int size;
	// 中断请求
    int abort_request;
	// 锁
    SDL_mutex *mutex;
	// 条件变量
    SDL_cond *cond;
} PacketQueue;

/* 视频图像 */
typedef struct VideoPicture
{
	// 显示层
    SDL_Overlay *bmp;
	// 宽高
    int width, height; // source height & width
} VideoPicture;

/* 视频的状态 */
typedef struct VideoState
{
	// 解析线程
    SDL_Thread *parse_tid;
	// 视频播放线程
    SDL_Thread *video_tid;

	// 中断请求
    int abort_request;

	// 表示一个输入上下文（或者表示一个文件上下文）
    AVFormatContext *ic;

    int audio_stream; 
    int video_stream;

	// 音频流
    AVStream *audio_st;
	// 视频流
    AVStream *video_st;

	// 音频包队列
    PacketQueue audioq;
	// 视频包队列
    PacketQueue videoq;

	// 播放队列
    VideoPicture pictq[VIDEO_PICTURE_QUEUE_SIZE];
    double frame_last_delay;

    uint8_t audio_buf[(AVCODEC_MAX_AUDIO_FRAME_SIZE *3) / 2];
    unsigned int audio_buf_size;
    int audio_buf_index;

	// 音频数据包
    AVPacket audio_pkt;

	// 音频数据和长度
    uint8_t *audio_pkt_data;
    int audio_pkt_size;

	// 视频解码器锁
    SDL_mutex *video_decoder_mutex;
	// 音频解码器锁
    SDL_mutex *audio_decoder_mutex;

	// 视频文件的名字
    char filename[240];

} VideoState;

static AVInputFormat *file_iformat;
static const char *input_filename;
static VideoState *cur_stream;

static SDL_Surface *screen;
/* 获取当前的时间 */
int64_t av_gettime(void)
{
#if defined(CONFIG_WINCE)
    return timeGetTime() *int64_t_C(1000);
#elif defined(CONFIG_WIN32)
    struct _timeb tb;
    _ftime(&tb);
    return ((int64_t)tb.time *int64_t_C(1000) + (int64_t)tb.millitm) *int64_t_C(1000);
#else
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (int64_t)tv.tv_sec *1000000+tv.tv_usec;
#endif
}

/* 数据包队列初始化 */
static void packet_queue_init(PacketQueue *q) // packet queue handling
{
    memset(q, 0, sizeof(PacketQueue));
	// 创建锁
    q->mutex = SDL_CreateMutex();
	// 创建条件变量
    q->cond = SDL_CreateCond();
}

/* 刷新数据包队列，就是释放所有数据包 */
static void packet_queue_flush(PacketQueue *q)
{
    AVPacketList *pkt,  *pkt1;

    SDL_LockMutex(q->mutex);
    for (pkt = q->first_pkt; pkt != NULL; pkt = pkt1)
    {
        pkt1 = pkt->next;
        av_free_packet(&pkt->pkt);
        av_freep(&pkt);
    }
    q->last_pkt = NULL;
    q->first_pkt = NULL;
    q->size = 0;
    SDL_UnlockMutex(q->mutex);
}

/* 销毁数据包队列  */
static void packet_queue_end(PacketQueue *q)
{
    packet_queue_flush(q);
    SDL_DestroyMutex(q->mutex);
    SDL_DestroyCond(q->cond);
}

/* 往数据包队列中压入一个数据包 */
static int packet_queue_put(PacketQueue *q, AVPacket *pkt)
{
    AVPacketList *pkt1;

    pkt1 = av_malloc(sizeof(AVPacketList));
    if (!pkt1)
        return  - 1;
    pkt1->pkt =  *pkt;
    pkt1->next = NULL;

    SDL_LockMutex(q->mutex);

    if (!q->last_pkt)
        q->first_pkt = pkt1;
    else
        q->last_pkt->next = pkt1;
    q->last_pkt = pkt1;
    q->size += pkt1->pkt.size;

    SDL_CondSignal(q->cond);

    SDL_UnlockMutex(q->mutex);
    return 0;
}

/* 请求退出 */
static void packet_queue_abort(PacketQueue *q)
{
    SDL_LockMutex(q->mutex);

    q->abort_request = 1;

    SDL_CondSignal(q->cond);

    SDL_UnlockMutex(q->mutex);
}

/* return < 0 if aborted, 0 if no packet and > 0 if packet.  */
/* 从队列中取出一个数据包 */
static int packet_queue_get(PacketQueue *q, AVPacket *pkt, int block)
{
    AVPacketList *pkt1;
    int ret;

    SDL_LockMutex(q->mutex);

    for (;;)
    {
        if (q->abort_request)
        {
            ret =  - 1; // 异常
            break;
        }

        pkt1 = q->first_pkt;
        if (pkt1)
        {
            q->first_pkt = pkt1->next;
            if (!q->first_pkt)
                q->last_pkt = NULL;
            q->size -= pkt1->pkt.size;
            *pkt = pkt1->pkt;
            av_free(pkt1);
            ret = 1;
            break;
        }
        else if (!block)// 阻塞标记，1(阻塞模式)，0(非阻塞模式)
        {
            ret = 0; // 非阻塞模式，没东西直接返回0
            break;
        }
        else
        {
            SDL_CondWait(q->cond, q->mutex);
        }
    }
    SDL_UnlockMutex(q->mutex);
    return ret;
}

/* 创建一个VideoPicture对象 */
static void alloc_picture(void *opaque)
{
    VideoState *is = opaque;
    VideoPicture *vp;

    vp = &is->pictq[0];

    if (vp->bmp)
        SDL_FreeYUVOverlay(vp->bmp);

    vp->bmp = SDL_CreateYUVOverlay(is->video_st->actx->width, 
                                   is->video_st->actx->height,
                                   SDL_YV12_OVERLAY,
                                   screen);

    vp->width = is->video_st->actx->width;
    vp->height = is->video_st->actx->height;
}

/* 显示图像 */
static int video_display(VideoState *is, AVFrame *src_frame, double pts)
{
    VideoPicture *vp;
    int dst_pix_fmt;
    AVPicture pict;

    if (is->videoq.abort_request)
        return  - 1;

    vp = &is->pictq[0];

    /* if the frame is not skipped, then display it */
    if (vp->bmp)
    {
        SDL_Rect rect;

        if (pts)
            Sleep((int)(is->frame_last_delay *1000));
#if 1
        /* get a pointer on the bitmap */
        SDL_LockYUVOverlay(vp->bmp);

        dst_pix_fmt = PIX_FMT_YUV420P;
        pict.data[0] = vp->bmp->pixels[0];
        pict.data[1] = vp->bmp->pixels[2];
        pict.data[2] = vp->bmp->pixels[1];

        pict.linesize[0] = vp->bmp->pitches[0];
        pict.linesize[1] = vp->bmp->pitches[2];
        pict.linesize[2] = vp->bmp->pitches[1];

        img_convert(&pict, 
                    dst_pix_fmt,
                    (AVPicture*)src_frame, 
                    is->video_st->actx->pix_fmt, 
                    is->video_st->actx->width,
                    is->video_st->actx->height);

        SDL_UnlockYUVOverlay(vp->bmp); /* update the bitmap content */

        rect.x = 0;
        rect.y = 0;
        rect.w = is->video_st->actx->width;
        rect.h = is->video_st->actx->height;
        SDL_DisplayYUVOverlay(vp->bmp, &rect);
#endif
    }
    return 0;
}

/* 视频处理线程 ：编码或者解码*/
static int video_thread(void *arg)
{
    VideoState *is = arg;
    AVPacket pkt1,  *pkt = &pkt1;
    int len1, got_picture;
    double pts = 0;

	// 分配一个帧 
    AVFrame *frame = av_malloc(sizeof(AVFrame));
    memset(frame, 0, sizeof(AVFrame));

    alloc_picture(is);

	// 循环
    for (;;)
    {
		// 取出一个数据包
        if (packet_queue_get(&is->videoq, pkt, 1) < 0)
            break;

		// 加锁，然后解码，然后解锁
        SDL_LockMutex(is->video_decoder_mutex);
        len1 = avcodec_decode_video(is->video_st->actx, frame, &got_picture, pkt->data, pkt->size);
        SDL_UnlockMutex(is->video_decoder_mutex);

		// 计算分数
        if (pkt->dts != AV_NOPTS_VALUE)
            pts = av_q2d(is->video_st->time_base) *pkt->dts;

		// 显示帧
        if (got_picture)
        {
            if (video_display(is, frame, pts) < 0)
                goto the_end;
        }
        av_free_packet(pkt);
    }

the_end: 
    av_free(frame);
    return 0;
}

/* decode one audio frame and returns its uncompressed size */
static int audio_decode_frame(VideoState *is, uint8_t *audio_buf, double *pts_ptr)
{
    AVPacket *pkt = &is->audio_pkt;
    int len1, data_size;
    
    for (;;)
    {
        /* NOTE: the audio packet can contain several frames */
        while (is->audio_pkt_size > 0)
        {
            SDL_LockMutex(is->audio_decoder_mutex);
            len1 = avcodec_decode_audio(is->audio_st->actx, (int16_t*)audio_buf,
                            &data_size, is->audio_pkt_data, is->audio_pkt_size);

            SDL_UnlockMutex(is->audio_decoder_mutex);
            if (len1 < 0)
            {
                /* if error, we skip the frame */
                is->audio_pkt_size = 0;
                break;
            }

            is->audio_pkt_data += len1;
            is->audio_pkt_size -= len1;
            if (data_size <= 0)
                continue;

            return data_size;
        }

        /* free the current packet */
        if (pkt->data)
            av_free_packet(pkt);

        /* read next packet */
        if (packet_queue_get(&is->audioq, pkt, 1) < 0)
            return  - 1;

        is->audio_pkt_data = pkt->data;
        is->audio_pkt_size = pkt->size;
    }
}

/* prepare a new audio buffer */
void sdl_audio_callback(void *opaque, Uint8 *stream, int len)
{
    VideoState *is = opaque;
    int audio_size, len1;
    double pts = 0;

    while (len > 0)
    {
        if (is->audio_buf_index >= is->audio_buf_size)
        {
            audio_size = audio_decode_frame(is, is->audio_buf, &pts);
            if (audio_size < 0)
            {
                /* if error, just output silence */
                is->audio_buf_size = 1024;
                memset(is->audio_buf, 0, is->audio_buf_size);
            }
            else
            {
//              audio_size = synchronize_audio(is, (int16_t*)is->audio_buf, audio_size, pts);
                is->audio_buf_size = audio_size;
            }
            is->audio_buf_index = 0;
        }
        len1 = is->audio_buf_size - is->audio_buf_index;
        if (len1 > len)
            len1 = len;
        memcpy(stream, (uint8_t*)is->audio_buf + is->audio_buf_index, len1);
        len -= len1;
        stream += len1;
        is->audio_buf_index += len1;
    }
}

/* open a given stream. Return 0 if OK */
/* 打开一个视频流或者音频流 ，并创建线程去编解码 */
static int stream_component_open(VideoState *is, int stream_index)
{
    AVFormatContext *ic = is->ic;
    AVCodecContext *enc;
    AVCodec *codec;
    SDL_AudioSpec wanted_spec, spec;

    if (stream_index < 0 || stream_index >= ic->nb_streams)
        return  - 1;

    enc = ic->streams[stream_index]->actx;

    /* prepare audio output */
	/* 音频 */
    if (enc->codec_type == CODEC_TYPE_AUDIO)
    {
        wanted_spec.freq = enc->sample_rate;
        wanted_spec.format = AUDIO_S16SYS;
        /* hack for AC3. XXX: suppress that */
        if (enc->channels > 2)
            enc->channels = 2;
        wanted_spec.channels = enc->channels;
        wanted_spec.silence = 0;
        wanted_spec.samples = 1024; //SDL_AUDIO_BUFFER_SIZE;
        wanted_spec.callback = sdl_audio_callback;
        wanted_spec.userdata = is;
        if (SDL_OpenAudio(&wanted_spec, &spec) < 0)
        {
            fprintf(stderr, "SDL_OpenAudio: %s\n", SDL_GetError());
            return  - 1;
        }
    }

	// 查找一个编解码器 
    codec = avcodec_find_decoder(enc->codec_id);

	// 打开编解码器
    if (!codec || avcodec_open(enc, codec) < 0)
        return  - 1;

    switch (enc->codec_type)
    {
    case CODEC_TYPE_AUDIO: // 音频
        is->audio_stream = stream_index;
        is->audio_st = ic->streams[stream_index];
        is->audio_buf_size = 0;
        is->audio_buf_index = 0;

        memset(&is->audio_pkt, 0, sizeof(is->audio_pkt));
        packet_queue_init(&is->audioq);
        SDL_PauseAudio(0);
        break;
    case CODEC_TYPE_VIDEO:
        is->video_stream = stream_index;
        is->video_st = ic->streams[stream_index];

        is->frame_last_delay = is->video_st->frame_last_delay;

        packet_queue_init(&is->videoq); // 队列初始化 
        is->video_tid = SDL_CreateThread(video_thread, is); // 创建视频线程去处理
        break;
    default:
        break;
    }
    return 0;
}

static void stream_component_close(VideoState *is, int stream_index)
{
    AVFormatContext *ic = is->ic;
    AVCodecContext *enc;

    if (stream_index < 0 || stream_index >= ic->nb_streams)
        return ;
    enc = ic->streams[stream_index]->actx;

    switch (enc->codec_type)
    {
    case CODEC_TYPE_AUDIO:
        packet_queue_abort(&is->audioq);
        SDL_CloseAudio();
        packet_queue_end(&is->audioq);
        break;
    case CODEC_TYPE_VIDEO:
        packet_queue_abort(&is->videoq);
        SDL_WaitThread(is->video_tid, NULL);
        packet_queue_end(&is->videoq);
        break;
    default:
        break;
    }

    avcodec_close(enc);
}

/* 解码线程 */
static int decode_thread(void *arg)
{
    VideoState *is = arg;
    AVFormatContext *ic;
    int err, i, ret, video_index, audio_index;
    AVPacket pkt1,  *pkt = &pkt1;
    AVFormatParameters params,  *ap = &params;
    
    int flags = SDL_HWSURFACE | SDL_ASYNCBLIT | SDL_HWACCEL | SDL_RESIZABLE;

    video_index =  - 1;
    audio_index =  - 1;

    is->video_stream =  - 1;
    is->audio_stream =  - 1;

    memset(ap, 0, sizeof(*ap));

	// 打开输入文件，并把信息填充到IO上下文对象中
    err = av_open_input_file(&ic, is->filename, NULL, 0, ap);
    if (err < 0)
    {
        ret =  - 1;
        goto fail;
    }
    is->ic = ic;

	// 比特流的数量
    for (i = 0; i < ic->nb_streams; i++)
    {
		// 编解码上下文
        AVCodecContext *enc = ic->streams[i]->actx;

		// 编解码类型
        switch (enc->codec_type)
        {
        case CODEC_TYPE_AUDIO: // 音频
            if (audio_index < 0)
               audio_index = i;
            break;
        case CODEC_TYPE_VIDEO: // 视频
            if (video_index < 0)
               video_index = i;

			// 设置视频模式，并得到屏幕
            screen = SDL_SetVideoMode(enc->width, enc->height, 0, flags);

            SDL_WM_SetCaption("FFplay", "FFplay"); // 修改是为了适配视频大小

//          schedule_refresh(is, 40);
            break;
        default:
            break;
        }
    }

	// 打开各个流的成分，并调用分量解码线程
    if (audio_index >= 0)
        stream_component_open(is, audio_index);

	// 打开流的各个成分，并调用分量解码线程
    if (video_index >= 0)
        stream_component_open(is, video_index);

    if (is->video_stream < 0 && is->audio_stream < 0)
    {
        fprintf(stderr, "%s: could not open codecs\n", is->filename);
        ret =  - 1;
        goto fail;
    }

	// 无限循环
    for (;;)
    {
        if (is->abort_request)
            break;

		// 如果队列已经满了，那么延迟去读
        if (is->audioq.size > MAX_AUDIOQ_SIZE || is->videoq.size > MAX_VIDEOQ_SIZE || url_feof(&ic->pb))
        {
            SDL_Delay(10); // if the queue are full, no need to read more,wait 10 ms
            continue;
        }

		// 读取一个数据包（表示一帧）
        ret = av_read_packet(ic, pkt); //av_read_frame(ic, pkt);
        if (ret < 0)
        {
            if (url_ferror(&ic->pb) == 0)
            {
                SDL_Delay(100); // wait for user event
                continue;
            }
            else
                break;
        }

		// 把数据包添加到队列中
        if (pkt->stream_index == is->audio_stream)
        {
            packet_queue_put(&is->audioq, pkt);
        }
        else if (pkt->stream_index == is->video_stream)
        {
            packet_queue_put(&is->videoq, pkt);
        }
        else
        {
            av_free_packet(pkt);
        }
    }

    while (!is->abort_request)   // wait until the end
    {
        SDL_Delay(100);
    }

    ret = 0;

fail: 
    if (is->audio_stream >= 0)
        stream_component_close(is, is->audio_stream);

    if (is->video_stream >= 0)
        stream_component_close(is, is->video_stream);

    if (is->ic)
    {
        av_close_input_file(is->ic);
        is->ic = NULL;
    }

    if (ret != 0)
    {
        SDL_Event event;

        event.type = FF_QUIT_EVENT;
        event.user.data1 = is;
        SDL_PushEvent(&event);
    }
    return 0;
}

/* 打开输入流（输入文件 ）*/
static VideoState *stream_open(const char *filename, AVInputFormat *iformat)
{
	// 视频状态
    VideoState *is;

    is = av_mallocz(sizeof(VideoState));
    if (!is)
        return NULL;
    pstrcpy(is->filename, sizeof(is->filename), filename);

	// 音频锁
    is->audio_decoder_mutex = SDL_CreateMutex();
	// 视频锁
    is->video_decoder_mutex = SDL_CreateMutex();

	// 创建解码线程
    is->parse_tid = SDL_CreateThread(decode_thread, is);
    if (!is->parse_tid)
    {
        av_free(is);
        return NULL;
    }
    return is;
}

static void stream_close(VideoState *is)
{
    VideoPicture *vp;
    int i;

    is->abort_request = 1;
    SDL_WaitThread(is->parse_tid, NULL);

    for (i = 0; i < VIDEO_PICTURE_QUEUE_SIZE; i++)
    {
        vp = &is->pictq[i];
        if (vp->bmp)
        {
            SDL_FreeYUVOverlay(vp->bmp);
            vp->bmp = NULL;
        }
    }

    SDL_DestroyMutex(is->audio_decoder_mutex);
    SDL_DestroyMutex(is->video_decoder_mutex);
}

void do_exit(void)
{
    if (cur_stream)
    {
        stream_close(cur_stream);
        cur_stream = NULL;
    }

    SDL_Quit();
    exit(0);
}

void event_loop(void) // handle an event sent by the GUI
{
    SDL_Event event;

    for (;;)
    {
        SDL_WaitEvent(&event);
        switch (event.type)
        {
        case SDL_KEYDOWN:
            switch (event.key.keysym.sym)
            {
            case SDLK_ESCAPE:
            case SDLK_q:
                do_exit();
                break;
            default:
                break;
            }
            break;
        case SDL_QUIT:
        case FF_QUIT_EVENT:
            do_exit();
            break;
        default:
            break;
        }
    }
}

int main(int argc, char **argv)
{
    int flags = SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER;

	// 注册所有支持的格式
    av_register_all();

	// 输入文件名
    input_filename = "clocktxt_320.avi";
//    input_filename = "d:/yuv/clocktxt.avi";

	// sdl初始化（sdl是一个跨平台多媒体开发库）
    if (SDL_Init(flags))
        exit(1);

	// 注册或者监听下列事件：激活事件、鼠标事件、windows的系统消息、用户事件
    SDL_EventState(SDL_ACTIVEEVENT, SDL_IGNORE);
    SDL_EventState(SDL_MOUSEMOTION, SDL_IGNORE);
    SDL_EventState(SDL_SYSWMEVENT, SDL_IGNORE);
    SDL_EventState(SDL_USEREVENT, SDL_IGNORE);

	// 打开输入流
    cur_stream = stream_open(input_filename, file_iformat);

    event_loop();

    return 0;
}
