# MAC/iOS利用FFmpeg解析音视频数据流

## 1.简易流程

**使用流程**

- 初始化解析类: `- (instancetype)initWithPath:(NSString *)path;`
- 开始解析: `startParseWithCompletionHandler`
- 获取解析后的数据: 从上一步中`startParseWithCompletionHandler`方法中的Block获取解析后的音视频数据.

**FFmpeg parse流程**

- 创建format context: `avformat_alloc_context`
- 打开文件流: `avformat_open_input`
- 寻找流信息: `avformat_find_stream_info`
- 获取音视频流的索引值: `formatContext->streams[i]->codecpar->codec_type == (isVideoStream ? AVMEDIA_TYPE_VIDEO : AVMEDIA_TYPE_AUDIO)`
- 获取音视频流: `m_formatContext->streams[m_audioStreamIndex]`
- 解析音视频数据帧: `av_read_frame`
- 获取extra data: `av_bitstream_filter_filter`

具体步骤

\1. 将FFmpeg框架导入项目中
下面的链接中包含搭建iOS需要的FFmpeg环境的详细步骤,需要的可以提前阅读.
[iOS编译FFmpeg](https://zhuanlan.zhihu.com/p/533700525)
导入FFmpeg框架后,首先需要将用到FFmpeg的文件改名为`.mm`, 因为涉及C,C++混编,所以需要更改文件名
然后在头文件中导入FFmpeg头文件.

```text
// FFmpeg Header File
#ifdef __cplusplus
extern "C" {
#endif
    
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include "libavutil/avutil.h"
#include "libswscale/swscale.h"
#include "libswresample/swresample.h"
#include "libavutil/opt.h"
    
#ifdef __cplusplus
};
#endif
```

注意: FFmpeg是一个广为流传的框架,其结构复杂,一般导入都按照如上格式,以文件夹名为根目录进行导入,具体设置,请参考上文链接.

## 2. 初始化

### **2.1. 注册FFmpeg**

- `void av_register_all(void);` 初始化libavformat并注册所有muxers，demuxers与协议。如果不调用此功能，则可以选择一个特定想要支持的格式。

一般在程序中的main函数或是主程序启动的代理方法`- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions`中初始化FFmpeg,执行一次即可.

```text
av_register_all();
```

### **2.2. 利用视频文件生成格式上下文对象**

- `avformat_alloc_context()`: 初始化avformat上下文对象.

- `int avformat_open_input(AVFormatContext **ps, const char *url, AVInputFormat *fmt, AVDictionary **options)`函数

- - `fmt`: 如果非空表示强制指定一个输入流的格式, 设置为空会自动选择.

- `int avformat_find_stream_info(AVFormatContext *ic, AVDictionary **options);` :读取媒体文件的数据包以获取流信息

```text
- (AVFormatContext *)createFormatContextbyFilePath:(NSString *)filePath {
    if (filePath == nil) {
        log4cplus_error(kModuleName, "%s: file path is NULL",__func__);
        return NULL;
    }
    
    AVFormatContext  *formatContext = NULL;
    AVDictionary     *opts          = NULL;
    
    av_dict_set(&opts, "timeout", "1000000", 0);//设置超时1秒
    
    formatContext = avformat_alloc_context();
    BOOL isSuccess = avformat_open_input(&formatContext, [filePath cStringUsingEncoding:NSUTF8StringEncoding], NULL, &opts) < 0 ? NO : YES;
    av_dict_free(&opts);
    if (!isSuccess) {
        if (formatContext) {
            avformat_free_context(formatContext);
        }
        return NULL;
    }
    
    if (avformat_find_stream_info(formatContext, NULL) < 0) {
        avformat_close_input(&formatContext);
        return NULL;
    }
    
    return formatContext;
}
```

### **2.3. 获取Audio / Video流的索引值.**

通过遍历format context对象可以从`nb_streams`数组中找到音频或视频流索引,以便后续使用

注意: 后面代码中仅需要知道音频,视频的索引就可以快速读取到format context对象中对应流的信息.

```text
- (int)getAVStreamIndexWithFormatContext:(AVFormatContext *)formatContext isVideoStream:(BOOL)isVideoStream {
    int avStreamIndex = -1;
    for (int i = 0; i < formatContext->nb_streams; i++) {
        if ((isVideoStream ? AVMEDIA_TYPE_VIDEO : AVMEDIA_TYPE_AUDIO) == formatContext->streams[i]->codecpar->codec_type) {
            avStreamIndex = i;
        }
    }
    
    if (avStreamIndex == -1) {
        log4cplus_error(kModuleName, "%s: Not find video stream",__func__);
        return NULL;
    }else {
        return avStreamIndex;
    }
}
```

### **2.4. 是否支持音视频流**

目前视频仅支持H264, H265编码的格式.实际过程中,解码得到视频的旋转角度可能是不同的,以及不同机型可以支持的解码文件格式也是不同的,所以可以用这个方法手动过滤一些不支持的情况.具体请下载代码观看,这里仅列出实战中测试出支持的列表.

```text
        /*
         各机型支持的最高分辨率和FPS组合:
         
         iPhone 6S: 60fps -> 720P
         30fps -> 4K
         
         iPhone 7P: 60fps -> 1080p
         30fps -> 4K
         
         iPhone 8: 60fps -> 1080p
         30fps -> 4K
         
         iPhone 8P: 60fps -> 1080p
         30fps -> 4K
         
         iPhone X: 60fps -> 1080p
         30fps -> 4K
         
         iPhone XS: 60fps -> 1080p
         30fps -> 4K
         */
```

音频本例中仅支持AAC格式.其他格式可根据需求自行更改.

## 3. 开始解析

- 初始化AVPacket以存放解析后的数据

使用AVPacket这个结构体来存储压缩数据.对于视频而言, 它通常包含一个压缩帧,对音频而言,可能包含多个压缩帧,该结构体类型通过`av_malloc()`函数分配内存,通过`av_packet_ref()`函数拷贝,通过`av_packet_unref().`函数释放内存.

```text
AVPacket    packet;
av_init_packet(&packet);
```

解析数据
`int av_read_frame(AVFormatContext *s, AVPacket *pkt);` : 此函数返回存储在文件中的内容，并且不验证解码器的有效帧是什么。它会将存储在文件中的内容分成帧，并为每次调用返回一个。它不会在有效帧之间省略无效数据，以便为解码器提供解码时可能的最大信息。

```text
            int size = av_read_frame(formatContext, &packet);
            if (size < 0 || packet.size < 0) {
                handler(YES, YES, NULL, NULL);
                log4cplus_error(kModuleName, "%s: Parse finish",__func__);
                break;
            }
```

获取sps, pps等NALU Header信息
通过调用`av_bitstream_filter_filter`可以从码流中过滤得到sps, pps等NALU Header信息.
av_bitstream_filter_init: 通过给定的比特流过滤器名词创建并初始化一个比特流过滤器上下文.
av_bitstream_filter_filter: 此函数通过过滤`buf`参数中的数据,将过滤后的数据放在`poutbuf`参数中.输出的buffer必须被调用者释放.
此函数使用buf_size大小过滤缓冲区buf，并将过滤后的缓冲区放在poutbuf指向的缓冲区中。

```text
attribute_deprecated int av_bitstream_filter_filter	(	AVBitStreamFilterContext * 	bsfc,   
AVCodecContext * 	avctx,
const char * 	args,   // filter 配置参数
uint8_t ** 	poutbuf,    // 过滤后的数据
int * 	poutbuf_size,   // 过滤后的数据大小
const uint8_t * 	buf,// 提供给过滤器的原始数据
int 	buf_size,       // 提供给过滤器的原始数据大小
int 	keyframe        // 如果要过滤的buffer对应于关键帧分组数据，则设置为非零
)	
```

注意: 下面使用`new_packet`是为了解决`av_bitstream_filter_filter`会产生内存泄漏的问题.每次使用完后将用`new_packet`释放即可.

```text
if (packet.stream_index == videoStreamIndex) {
    static char filter_name[32];
    if (formatContext->streams[videoStreamIndex]->codecpar->codec_id == AV_CODEC_ID_H264) {
        strncpy(filter_name, "h264_mp4toannexb", 32);
        videoInfo.videoFormat = XDXH264EncodeFormat;
    } else if (formatContext->streams[videoStreamIndex]->codecpar->codec_id == AV_CODEC_ID_HEVC) {
        strncpy(filter_name, "hevc_mp4toannexb", 32);
        videoInfo.videoFormat = XDXH265EncodeFormat;
    } else {
        break;
    }
    
    AVPacket new_packet = packet;
    if (self->m_bitFilterContext == NULL) {
        self->m_bitFilterContext = av_bitstream_filter_init(filter_name);
    }
    av_bitstream_filter_filter(self->m_bitFilterContext, formatContext->streams[videoStreamIndex]->codec, NULL, &new_packet.data, &new_packet.size, packet.data, packet.size, 0);
    
}
```

- 根据特定规则生成时间戳

可以根据自己的需求自定义时间戳生成规则.这里使用当前系统时间戳加上数据包中的自带的pts/dts生成了时间戳.

```text
    CMSampleTimingInfo timingInfo;
    CMTime presentationTimeStamp     = kCMTimeInvalid;
    presentationTimeStamp            = CMTimeMakeWithSeconds(current_timestamp + packet.pts * av_q2d(formatContext->streams[videoStreamIndex]->time_base), fps);
    timingInfo.presentationTimeStamp = presentationTimeStamp;
    timingInfo.decodeTimeStamp       = CMTimeMakeWithSeconds(current_timestamp + av_rescale_q(packet.dts, formatContext->streams[videoStreamIndex]->time_base, input_base), fps);
```

- 获取parse到的数据

本例将获取到的数据放在自定义的结构体中,然后通过block回调传给方法的调用者,调用者可以在回调函数中处理parse到的视频数据.

```text
struct XDXParseVideoDataInfo {
    uint8_t                 *data;
    int                     dataSize;
    uint8_t                 *extraData;
    int                     extraDataSize;
    Float64                 pts;
    Float64                 time_base;
    int                     videoRotate;
    int                     fps;
    CMSampleTimingInfo      timingInfo;
    XDXVideoEncodeFormat    videoFormat;
};

...

    videoInfo.data          = video_data;
    videoInfo.dataSize      = video_size;
    videoInfo.extraDataSize = formatContext->streams[videoStreamIndex]->codec->extradata_size;
    videoInfo.extraData     = (uint8_t *)malloc(videoInfo.extraDataSize);
    videoInfo.timingInfo    = timingInfo;
    videoInfo.pts           = packet.pts * av_q2d(formatContext->streams[videoStreamIndex]->time_base);
    videoInfo.fps           = fps;
    
    memcpy(videoInfo.extraData, formatContext->streams[videoStreamIndex]->codec->extradata, videoInfo.extraDataSize);
    av_free(new_packet.data);
    
    // send videoInfo
    if (handler) {
        handler(YES, NO, &videoInfo, NULL);
    }
    
    free(videoInfo.extraData);
    free(videoInfo.data);
```

获取parse到的音频数据

```text
struct XDXParseAudioDataInfo {
    uint8_t     *data;
    int         dataSize;
    int         channel;
    int         sampleRate;
    Float64     pts;
};

...

    if (packet.stream_index == audioStreamIndex) {
        XDXParseAudioDataInfo audioInfo = {0};
        audioInfo.data = (uint8_t *)malloc(packet.size);
        memcpy(audioInfo.data, packet.data, packet.size);
        audioInfo.dataSize = packet.size;
        audioInfo.channel = formatContext->streams[audioStreamIndex]->codecpar->channels;
        audioInfo.sampleRate = formatContext->streams[audioStreamIndex]->codecpar->sample_rate;
        audioInfo.pts = packet.pts * av_q2d(formatContext->streams[audioStreamIndex]->time_base);
        
        // send audio info
        if (handler) {
            handler(NO, NO, NULL, &audioInfo);
        }
        
        free(audioInfo.data);
    }
```

- 释放packet

因为我们已经将packet中的关键数据拷贝到自定义的结构体中,所以使用完后需要释放packet.

```text
av_packet_unref(&packet);
```

parse完成后释放相关资源

```text
- (void)freeAllResources {
    if (m_formatContext) {
        avformat_close_input(&m_formatContext);
        m_formatContext = NULL;
    }
    
    if (m_bitFilterContext) {
        av_bitstream_filter_close(m_bitFilterContext);
        m_bitFilterContext = NULL;
    }
}
```

注意: 如果使用FFmpeg硬解,则仅仅需要获取到AVPacket数据结构即可.不需要再将数据封装到自定义的结构体中

## 4. 外部调用

上面操作执行完后,即可通过如下block获取解析后的数据,一般需要继续对音视频进行解码操作.后面文章会讲到,请持续关注.

```text
    XDXAVParseHandler *parseHandler = [[XDXAVParseHandler alloc] initWithPath:path];
    [parseHandler startParseGetAVPackeWithCompletionHandler:^(BOOL isVideoFrame, BOOL isFinish, AVPacket packet) {
        if (isFinish) {
            // parse finish
            ...
            return;
        }
        
        if (isVideoFrame) {
            // decode video
            ...
        }else {
            // decode audio
            ...
        }
    }];
```

原文https://zhuanlan.zhihu.com/p/533710513