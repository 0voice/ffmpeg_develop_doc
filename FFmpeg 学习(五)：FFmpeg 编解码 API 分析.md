在上一篇文章 FFmpeg学习(四)：FFmpeg API 介绍与通用 API 分析 中，我们简单的讲解了一下FFmpeg 的API基本概念，并分析了一下通用API，本文我们将分析 FFmpeg 在编解码时使用的API。

## 一、FFmpeg 解码 API 分析



### 1. avformat_open_input 分析

函数 avformat_open_input 会根据所提供的文件路径判断文件的格式，其实就是通过这一步来决定到底是使用哪个Demuxer。

举个例子：如果是flv，那么Demuxer就会使用对应的ff_flv_demuxer，所以对应的关键生命周期的方法read_header、read_packet、read_seek、read_close都会使用该flv的Demuxer中函数指针指定的函数。read_header会将AVStream结构体构造好，以方便后续的步骤继续使用AVStream作为输入参数。



### 2. avformat_find_stream_info 分析

该方法的作用就是把所有的Stream的MetaData信息填充好。方法内部会先查找对于的解码器，然后打开对应的解码器，紧接着会利用Demuxer中的read_packet函数读取一段数据进行解码，当然，解码的数据越多，分析出来的流信息就越准确，如果是本地资源，那么很快就可以得到准确的信息了。但是对于网络资源来说，则会比较慢，因此该函数有几个参数可以控制读取数据的长度，一个是probe size，一个是max_analyze_duration, 还有一个就是fps_probe_size，这三个参数共同控制解码数据的长度，如果配置的这几个参数的数值越小，那么这个函数执行的时间就会越快，但会导致AVStream结构体里面的信息（视频的宽、高、fps、编码类型）不准确。



### 3. av_read_frame 分析

该方法读取出来的数据是AVPacket，在FFmpeg的早期版本中开发给开发者的函数其实就是av_read_packet，但是需要开发者自己来处理AVPacket中的数据不能被解码器处理完的情况，即需要把未处理完的压缩数据缓存起来的问题。所以在新版本的FFmpeg中，提供了该函数，用于处理此状况。 该函数的实现首先会委托到Demuxer的read_packet方法中，当然read_packet通过解服用层和协议层的处理后，会将数据返回到这里，在该函数中进行数据缓冲处理。

对于音频流，一个AVPacket可能会包含多个AVFrame，但是对于一个视频流，一个AVPacket只包含一个AVFrame，该函数最终只会返回一个AVPacket结构体。



### 4. avcodec_decode分析

该方法包含了两部分内容：一部分是解码视频，一部分是解码音频。在上面的函数分析中，我们知道，解码是会委托给对应的解码器来实施的，在打开解码器的时候就找到了对应的解码器的实现，比如对于解码H264来讲，会找到ff_h264_decoder，其中会有对应的生命周期函数的实现，最重要的就是init，decode，close三个方法，分别对应于打开解码器、解码及关闭解码器的操作，而解码过程就是调用decode方法。



### 5. avformat_close_input 分析

该函数负责释放对应的资源，首先会调用对应的Demuxer中的生命周期read_close方法，然后释放掉，AVFormatContext，最后关闭文件或者远程网络链接。 



## 二、FFmpeg 编码 API 分析



### 1. avformat_alloc_output_context2 分析

该函数内部需要调用方法avformat_alloc_context来分配一个AVFormatContext结构体，当然最关键的还是根据上一步注册的Muxer和Demuxer部分（也就是封装格式部分）去找对应的格式。有可能是flv格式、MP4格式、mov格式，甚至是MP3格式等，如果找不到对应的格式（应该是因为在configure选项中没有打开这个格式的开关），那么这里会返回找不到对于的格式的错误提示。在调用API的时候，可以使用av_err2str把返回的整数类型的错误代码转换为肉眼可读的字符串，这是个在调试中非常有用的工具函数。该函数最终会将找出来的格式赋值给AVFormatContext类型的oformat。



### 2. avio_open2 分析

首先会调用函数ffurl_open，构造出URLContext结构体，这个结构体中包含了URLProtocol（需要去第一步register_protocol中已经注册的协议链表）中去寻找；接着会调用avio_alloc_contex方法，分配出AVIOContext结构体，并将上一步构造出来的URLProtocol传递进来；然后把上一步分配出来的AVIOContext结构体赋值给AVFormatContext属性。

下面就是针对上面的描述总结的结构之间的构架图，各位可以参考此图进行进一步的理解：

![img](https://images2018.cnblogs.com/blog/682616/201807/682616-20180720100812049-1285283157.png)

avio_open2的过程也恰好是在上面我们分析avformat_open_input过程的一个逆过程。编码过程和解码过程从逻辑上来讲，也是一个逆过程，所以在FFmpeg实现的过程中，他们也互为逆过程。



### 3. 编码其他API（步骤）分析

 编码的其他步骤也是解码的一个逆过程，解码过程中的avformat_find_stream_info对应到编码就是avformat_new_stream和avformat_write_header。

- avformat_new_stream函数会将音频流或者视频流的信息填充好，分配出AVStream结构体，在音频流中分配声道、采样率、表示格式、编码器等信息，在视频中分配宽、高、帧率、表示格式、编码器等信息。
- avformat_write_header函数与解码过程中的read_header恰好是一个逆过程，这里就不多赘述了。

接下来就是编码阶段了：

1. 将手动封装好的AVFrame结构体，作为avcodec_encodec_video方法的输入，然后将其编码成为AVPacket，然后调用av_write_frame方法输出到媒体文件中。

2. av_write_frame 方法会将编码后的AVPacket结构体作为Muxer中的write_packet生命周期方法的输入，write_packet会加上自己封装格式的头信息，然后调用协议层，写到本地文件或者网络服务器上。

3. 最后一步就是av_write_trailer（该函数有一个非常大的坑，如果没执行write_header操作，就直接执行write_trailer操作，程序会直接Carsh掉，所以这两个函数必须成对出现），av_write_trailer会把没有输出的AVPacket全部丢给协议层去做输出，然后会调用Muxer的write_trailer生命周期方法（不同的格式，写出的尾部也不一样）。

 



## 三、FFmpeg 解码 API 超时设置

当视频流地址能打开，但是视频流中并没有流内容的时候，可能会导致整体执行流程阻塞在 avformat_open_input 或者 av_read_frame 方法上。

主要原因就是avformat_open_input 和av_read_frame 这两个方法是阻塞的。

av_read_frame() -> read_frame_internal() -> ff_read_packet() -> s->iformat->read_packet() -> read_from_url() -> ffurl_read() -> retry_transfer_wrapper() (此方法会堵塞)

虽然我们可以通过设置 ic->flags |= AVFMT_FLAG_NONBLOCK; 将操作设置为非阻塞，但这样设置是不推荐的，会导致后续的其他操作出现问题。

一般情况下，我们推荐另外两种机制进行设置：



### 1. 设置开流的超时时间

 在设置开流超时时间的时候，需要注意 不同的协议设置的方式是不一样的。

```
方法：timeout --> 单位：（http:ms udp:s)``方法：stimeout --> 单位:（rtsp us）　
```

设置udp、http 超时的示例代码如下：

```
AVDictionary* opts = NULL;
av_dict_set(&opts, "timeout", "3000000", 0);//单位 如果是http:ms  如果是udp:s
int ret = avformat_open_input(&ctx, url, NULL, &opts);
```

 

设置rtsp超时的示例代码如下：

```
AVDictionary* opts = NULL;
av_dict_set(&opts, "rtsp_transport", m_bTcp ? "tcp" : "udp", 0); //设置tcp or udp，默认一般优先tcp再尝试udp
av_dict_set(&opts, "stimeout", "3000000", 0);//单位us 也就是这里设置的是3s
ret = avformat_open_input(&ctx, url, NULL, &opts);
```



### 2. 设置interrupt_callback定义返回机制

设置回调，监控read超时情况，回调方法为：



```
int64_t lastReadPacktTime;
static int interrupt_cb(void *ctx)
{
    int timeout = 3;
    if (av_gettime() - lastReadPacktTime > timeout * 1000 * 1000)
    {
        return -1;
    }
    return 0;
}
```



回调函数中返回0则代表ffmpeg继续阻塞直到ffmpeg正常工作为止，否则就代表ffmpeg结束阻塞可以将操纵权交给用户线程并返回错误码。

对指定的 AVFormatContext 进行设置，并在需要调用的设置的时间之前，记录当前的时间，这样在回调的时候就能根据时间差，判断执行相应的逻辑：

avformat_open_input 设置方式：

```
inputContext = avformat_alloc_context();
lastReadPacktTime = av_gettime();
inputContext->interrupt_callback.callback = interrupt_cb;
int ret = avformat_open_input(&inputContext, inputUrl.c_str(), nullptr, nullptr);
```

 

av_read_frame 设置方式：

```
lastReadPacktTime = av_gettime();
ret = av_read_frame(inputContext, packet);
```

在实际开发中，只是设计这个机制，很容易出现超时，但如果超时时间设置过程，又容易阻塞线程。一般推荐的方案为：在超时的机制上增加连续读流的时长统计，当连续读流超时超过一定时间时就通知当前读流操作已失败。
