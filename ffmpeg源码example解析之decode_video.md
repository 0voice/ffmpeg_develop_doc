# 视频解码流程

## 源码文件

```
<ffmpeg>/doc/examples/decode_video.c
```

## 代码调用流程

[![image](https://xuleilx.github.io/images/decode_video.png)](https://xuleilx.github.io/images/decode_video.png)

该流程并不是一个正常的流程，它假设了该文件是mpeg1video的编码格式，并且没有封装容器。
通常情况下是需要解封装的，比如说拿到一个视频文件，并不知道是什么编码，这时候就需要解封装来了解容器里面数据流了。

首先我们先要了解ffmpeg的几个大类：

- AVFormat：封装、解封装、包含协议封装
- AVCodec：编解码
- AVFilter：音视频滤镜
- swscale：视频图像转换
- swresample：音频转换计算
- AVUtil :工具类

# 视频解封装，解码，图像转换流程

根据ffmpeg的几个大类，介绍解码视频并显示的一般流程和操作，序号为程序调用顺序

## AVFormat：封装、解封装、包含协议封装

### 解封装

```c
avformat_alloc_context 	#封装结构体分配内存 // 可以不调用，avformat_open_input会判断入参是否为NULL，自行分配
avformat_open_input     	#打开输入文件用于读取数据
avformat_find_stream_info#获取流信息
针对每个stream处理
    - pFormatContext->nb_streams
    - avcodec_find_decoder 	#根据流中的编码参数AVCodecParameters，查找是否支持该编码
    - 判断流的类型 pLocalCodecParameters->codec_type
    - 保存AVCodecParameters和AVCodec，用于后续处理
    
av_read_frame			#读取一包AVPacket数据包
```

## AVCodec：编解码

### 解码

```c
avcodec_alloc_context3 		#编解码结构体分配内存
avcodec_parameters_to_context#将解封装得到的编码参数AVCodecParameters赋值给编解码结构体
avcodec_open2 				#打开编码器
avcodec_send_packet 		#将解封装中得到的AVPacket数据包送给解码器
avcodec_receive_frame 		#读回一帧解码后的数据AVFrame
```

### AVPacket：压缩的数据包

```
av_packet_alloc 		#压缩的数据包分配内存
```

## swscale：视频图像转换

```
sws_getContext 		#给SwsContext结构体分配内存

sws_scale 			#视频图像转换
```

## AVUtil :工具类

### AVFrame：解码后的数据帧

```
av_frame_alloc 		#解码后的数据帧分配内存
```

### image

```
av_image_alloc 		#分配内存用于存放一张图片
```

# 项目

该项目用于学习ffmpeg编解码
使用qt主要是比较方便，后续采用glfw和sdl显示

## 功能描述

1. 使用FFMpeg解码
2. 将FFMpeg解码后的yuv数据转换成rgb
3. 使用QT的QLabel组件，通过QImage显示rgb数据
