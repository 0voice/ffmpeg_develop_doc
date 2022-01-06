# 音频播放

## 源码文件

```
<ffmpeg>/doc/examples/decode_audio.c
<ffmpeg>/doc/examples/muxing.c
<ffmpeg>/doc/examples/resampling_audio.c
<ffmpeg>/doc/examples/transcode_aac.c
```

## 代码调用流程

代码流程跟《ffmpeg源码example解析之decode-video》基本类似，主要的区别是在播放上。对于播放器来说需要设置一套播放参数，比如：采样率，通道数，大小端，采样大小以及数据类型。理论上可以通过ffmpeg解析出来的的这些参数设置给播放器，但是ffmpeg的format的跟播放器的format不是同一个枚举，需要建立一个映射关系，所以觉得一般的播放器会统一设置成固定值，比如：
采样率：44100，通道数：2，大小端：LittleEndian，采样大小：16bit，数据类型：有符号
也就是说无论输入是什么配置参数，统一重采样成这套参数跟播放匹配。

# 项目

该项目用于学习ffmpeg编解码
使用qt主要是比较方便，后续采用glfw和sdl显示

## 功能描述

### 播放视频

1. 使用FFMpeg解码
2. 使用sws_scale将FFMpeg解码后的yuv数据转换成rgb
3. 使用QT的QLabel组件，通过QImage显示rgb数据

### 播放音频

1. 使用FFMpeg解码
2. 使用swr_convert将FFMpeg解码后的一帧数据转换成播放器指定的播放参数
3. 使用QT的QAudioOutput播放pcm数据
