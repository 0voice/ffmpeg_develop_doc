# 一、FFmpeg 播放视频的基本流程整理

播放流程: video.avi(Container) -> 打开得到 Video_Stream -> 读取Packet -> 解析到 Frame -> 显示Frame。

- Container：在音视频中的容器，一般指的是一种特定的文件格式(如 AVI/QT )，里面指明了所包含的音视频，字幕等相关信息。
- Stream：媒体流，指时间轴上的一段连续数据，如一段声音、视频或字幕数据。
- Packet：Stream中的Raw数据,包含了可以被解码成方便我们最后在应用程序中操作的帧的原始数据。
- Frame：Stream中的一个数据单元。
- Codec：编解码器(Code 和 Decode)，如 Divx和 MP3,以帧为单位实现压缩数据和原始数据之间的相互转换。

# 二、FFmpeg 各个结构体及相关方法流程整理

### 1. AVCodec 

AVCodec -- 编解码器，采用链表维护，每一个都有其对应的名字、类型、CodecID和对数据进行处理的编解码函数指针。

- avcodec_find_decoder/avcodec_find_encoder ：根据给定的codec id或解码器名称从系统中搜寻并返回一个AVCodec结构的指针
- avcodec_alloc_context3：根据 AVCodec 分配合适的 AVCodecContext
- avcodec_open/avcodec_open2/avcodec_close ：根据给定的 AVCodec 打开对应的Codec，并初始化 AVCodecContext/ 关闭Codec
- avcodec_alloc_frame：分配编解码需要的 AVFrame 结构
- avcodec_decode_video/avcodec_decode_video2 ：解码一个视频帧，输入数据在AVPacket结构中，输出数据在AVFrame结构中
- avcodec_decode_audio4：解码一个音频帧。输入数据在AVPacket结构中，输出数据在AVFrame结构中
- avcodec_encode_video/avcodec_encode_video2 ：编码一个视频帧，输入数据在AVFrame结构中，输出数据在AVPacket结构中 

### 2. AVCodecContext

AVCodecContext -- 和具体媒体数据相关的编解码器上下文，保存AVCodec指针和与codec相关的数据，包含了流中所使用的关于编解码器的所有信息

- codec_name[32]、codec_type(AVMediaType)、codec_id(CodecID)、codec_tag：编解码器的名字、类型(音频/视频/字幕等)、ID(H264/MPEG4等)、FOURC等信息
- hight/width,coded_width/coded_height： Video的高宽
- sample_fmt：音频的原始采样格式, 是 SampleFormat 枚举
- time_base：采用分数(den/num)保存了帧率的信息

### 3. AVFrame

- data/linesize：FFMpeg内部以平面的方式存储原始图像数据，即将图像像素分为多个平面（R/G/B或Y/U/V）数组
- data数组：其中的指针指向各个像素平面的起始位置，编码时需要用户设置数据
- linesize数组 ：存放各个存贮各个平面的缓冲区的行宽，编码时需要用户设置数据
- key_frame：该图像是否是关键帧，由 libavcodec 设置
- pict_type：该图像的编码类型：Intra(1)/Predicted(2)/Bi-dir(3) 等，默认值是 NONE(0)，其值由libavcodec设置
- pts：呈现时间，编码时由用户设置
- quality：从1(最好)到FF_LAMBDA_MAX(256*128-1,最差)，编码时用户设置，默认值是0
- nterlaced_frame：表明是否是隔行扫描的,编码时用户指定，默认0

### 4. AVFormatContext

AVFormatContext -- 格式转换过程中实现输入和输出功能、保存相关数据的主要结构，描述了一个媒体文件或媒体流的构成和基本信息

- nb_streams/streams ：AVStream结构指针数组, 包含了所有内嵌媒体流的描述，其内部有 AVInputFormat + AVOutputFormat 结构体，来表示输入输出的文件格式
- avformat_open_input：创建并初始化部分值，但其他一些值(如 mux_rate、key 等)需要手工设置初始值，否则可能出现异常
- avformat_alloc_output_context2：根据文件的输出格式、扩展名或文件名等分配合适的 AVFormatContext 结构

### 5. AVPacket

AVPacket -- 暂存解码之前的媒体数据（一个音/视频帧、一个字幕包等）及附加信息（解码时间戳、显示时间戳、时长等)，主要用于建立缓冲区并装载数据。

- data/size/pos： 数据缓冲区指针、长度和媒体流中的字节偏移量
- flags：标志域的组合，1(AV_PKT_FLAG_KEY)表示该数据是一个关键帧, 2(AV_PKT_FLAG_CORRUPT)表示该数据已经损坏
- destruct：释放数据缓冲区的函数指针，其值可为 [av_destruct_packet]/av_destruct_packet_nofree, 会被 av_free_packet 调用

### 6. AVStream

AVStream -- 描述一个媒体流，其大部分信息可通过 avformat_open_input 根据文件头信息确定，其他信息可通过 avformat_find_stream_info 获取，典型的有 视频流、中英文音频流、中英文字幕流(Subtitle)，可通过 av_new_stream、avformat_new_stream 等创建。

- index：在AVFormatContext中流的索引，其值自动生成(AVFormatContext::streams[index])
- nb_frames：流内的帧数目
- time_base：流的时间基准，是一个实数，该流中媒体数据的pts和dts都将以这个时间基准为粒度。通常，使用av_rescale/av_rescale_q可以实现不同时间基准的转换
- avformat_find_stream_info：获取必要的编解码器参数(如 AVMediaType、CodecID )，设置到 AVFormatContext::streams[i]::codec 中
- av_read_frame：从多媒体文件或多媒体流中读取媒体数据，获取的数据由 AVPacket 来存放
- av_seek_frame：改变媒体文件的读写指针来实现对媒体文件的随机访问，通常支持基于时间、文件偏移、帧号(AVSEEK_FLAG_FRAME)的随机访问方式

