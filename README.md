# 💯 2022年，最新 ffmpeg 资料整理，项目（调试可用），命令手册，文章，编解码论文，视频讲解，面试题全套资料

</br>

<p align="center">
<a> <img width="70%" height="70%" src="https://ahmadawais.com/wp-content/uploads/2021/05/FFmpeg.jpg"></a> 
</p>

</br>

本repo搜集整理全网ffmpeg学习资料。

所有数据来源于互联网。所谓取之于互联网，用之于互联网。

如果涉及版权侵犯，请邮件至 wchao_isvip@163.com ，我们将第一时间处理。

如果您对我们的项目表示赞同与支持，欢迎您 [lssues](https://github.com/0voice/ffmpeg_develop_doc/issues) 我们，或者邮件 wchao_isvip@163.com 我们，更加欢迎您 pull requests 加入我们。

感谢您的支持！

<p align="center">
  <a href="https://github.com/0voice/ffmpeg_develop_doc#%E5%85%B3%E6%B3%A8%E5%BE%AE%E4%BF%A1%E5%85%AC%E4%BC%97%E5%8F%B7%E5%90%8E%E5%8F%B0%E6%9C%8D%E5%8A%A1%E6%9E%B6%E6%9E%84%E5%B8%88%E8%81%94%E7%B3%BB%E6%88%91%E4%BB%AC%E5%85%8D%E8%B4%B9%E8%8E%B7%E5%8F%96%E6%9B%B4%E5%A4%9Affmepg%E5%AD%A6%E4%B9%A0%E8%B5%84%E6%96%99"><img src="https://img.shields.io/badge/微信公众号-green" alt=""></a>
  <a href="https://www.zhihu.com/people/xiao-zhai-nu-linux"><img src="https://img.shields.io/badge/知乎-blue" alt=""></a>
  <a href="https://space.bilibili.com/64514973"><img src="https://img.shields.io/badge/bilibili-red" alt=""></a>
</p>

- 目录
  - [@ 开源项目](https://github.com/0voice/ffmpeg_develop_doc#-%E5%BC%80%E6%BA%90%E9%A1%B9%E7%9B%AE)
  - [@ 典藏文档](https://github.com/0voice/ffmpeg_develop_doc#-%E5%85%B8%E8%97%8F%E6%96%87%E6%A1%A3)
  - [@ 系列文章](https://github.com/0voice/ffmpeg_develop_doc#-%E6%96%87%E7%AB%A0)
  - [@ 面试题](https://github.com/0voice/ffmpeg_develop_doc#-%E9%9D%A2%E8%AF%95%E9%A2%98)
  - [@ 教学视频](https://github.com/0voice/ffmpeg_develop_doc#-%E8%A7%86%E9%A2%91)
  - [@ 学术论文](https://github.com/0voice/ffmpeg_develop_doc#-%E8%AE%BA%E6%96%87)
  - [@ 资料下载](https://github.com/0voice/ffmpeg_develop_doc#%E8%81%94%E7%B3%BB%E4%B8%93%E6%A0%8F)


## 🏗 开源项目

- [bilibili/ijkplayer](https://github.com/bilibili/ijkplayer): 基于FFmpeg n3.4的Android/iOS视频播放器，支持MediaCodec, VideoToolbox。

- [befovy/fijkplayer](https://github.com/befovy/fijkplayer): ijkplayer for flutter. ijkplayer 的 flutter 封装。 Flutter video/audio player. Flutter media player plugin for android/iOS based on ijkplayer. fijkplayer 是基于 ijkplayer 封装的 flutter 媒体播放器，开箱即用，无需编译 ijkplayer

- [mpv-player/mpv](https://github.com/mpv-player/mpv): 命令行视频播放器

- [CarGuo/GSYVideoPlayer](https://github.com/CarGuo/GSYVideoPlayer): 视频播放器（IJKplayer、ExoPlayer、MediaPlayer），HTTPS，支持弹幕，外挂字幕，支持滤镜、水印、gif截图，片头广告、中间广告，多个同时播放，支持基本的拖动，声音、亮度调节，支持边播边缓存，支持视频自带rotation的旋转（90,270之类），重力旋转与手动旋转的同步支持，支持列表播放 ，列表全屏动画，视频加载速度，列表小窗口支持拖动，动画效果，调整比例，多分辨率切换，支持切换播放器，进度条小窗口预览，列表切换详情页面无缝播放，rtsp、concat、mpeg。

- [mpenkov/ffmpeg-tutorial](https://github.com/mpenkov/ffmpeg-tutorial): 教程，演示如何编写一个基于FFmpeg的视频播放器

- [imoreapps/ffmpeg-avplayer-for-ios-tvos](https://github.com/imoreapps/ffmpeg-avplayer-for-ios-tvos): 一个微小但强大的iOS和Apple TV OS的av播放器框架，是基于FFmpeg库。

- [unosquare/ffmediaelement](https://github.com/unosquare/ffmediaelement): FFME:高级WPF MediaElement(基于FFmpeg)

- [microshow/RxFFmpeg](https://github.com/microshow/RxFFmpeg)：RxFFmpeg 是基于 ( FFmpeg 4.0 + X264 + mp3lame + fdk-aac + opencore-amr + openssl ) 编译的适用于 Android 平台的音视频编辑、视频剪辑的快速处理框架，包含以下功能：视频拼接，转码，压缩，裁剪，片头片尾，分离音视频，变速，添加静态贴纸和gif动态贴纸，添加字幕，添加滤镜，添加背景音乐，加速减速视频，倒放音视频，音频裁剪，变声，混音，图片合成视频，视频解码图片，抖音首页，视频播放器及支持 OpenSSL https 等主流特色功能

- [wang-bin/QtAV](https://github.com/wang-bin/QtAV): 基于Qt和FFmpeg的跨平台多媒体框架,高性能。用户和开发人员友好。支持Android, iOS, Windows商店和桌面。基于Qt和FFmpeg的跨平台高性能音视频播放框架

- [xufuji456/FFmpegAndroid](https://github.com/xufuji456/FFmpegAndroid): android端基于FFmpeg实现音频剪切、拼接、转码、编解码；视频剪切、水印、截图、转码、编解码、转Gif动图；音视频合成与分离，配音；音视频解码、同步与播放；FFmpeg本地推流、H264与RTMP实时推流直播；FFmpeg滤镜：素描、色彩平衡、hue、lut、模糊、九宫格等；歌词解析与显示

- [Zhaoss/WeiXinRecordedDemo](https://github.com/Zhaoss/WeiXinRecordedDemo): 仿微信视频拍摄UI, 基于ffmpeg的视频录制编辑

- [yangjie10930/EpMedia](https://github.com/yangjie10930/EpMedia): Android上基于FFmpeg开发的视频处理框架，简单易用，体积小，帮助使用者快速实现视频处理功能。包含以下功能：剪辑，裁剪，旋转，镜像，合并，分离，变速，添加LOGO，添加滤镜，添加背景音乐，加速减速视频，倒放音视频

- [goldvideo/h265player](https://github.com/goldvideo/h265player): 一套完整的Web版H.265播放器解决方案，非常适合学习交流和实际应用。基于JS码流解封装、WebAssembly(FFmpeg)视频解码，利用Canvas画布投影、AudioContext播放音频。

- [wanliyang1990/wlmusic](https://github.com/wanliyang1990/wlmusic): 基于FFmpeg + OpenSL ES的音频播放SDK。可循环不间断播放短音频；播放raw和assets音频文件；可独立设置音量大小；可实时现在音量分贝大小（用于绘制波形图）；可改变音频播放速度和音调（变速不变调、变调不变速、变速又变调）；可设置播放声道（左声道、右声道和立体声）；可边播边录留住美好音乐；可裁剪指定时间段的音频，制作自己的彩铃；还可以从中获取音频原始PCM数据(可指定采样率)，方便二次开发等。

- [Jackarain/avplayer](https://github.com/Jackarain/avplayer): 一个基于FFmpeg、libtorrent的P2P播放器实现

- [tsingsee/EasyPlayerPro-Win](https://github.com/tsingsee/EasyPlayerPro-Win): EasyPlayerPro是一款免费的全功能流媒体播放器，支持RTSP、RTMP、HTTP、HLS、UDP、RTP、File等多种流媒体协议播放、支持本地文件播放，支持本地抓拍、本地录像、播放旋转、多屏播放、倍数播放等多种功能特性，核心基于ffmpeg，稳定、高效、可靠、可控，支持Windows、Android、iOS三个平台，目前在多家教育、安防、行业型公司，都得到的应用，广受好评！

- [yangfeng1994/FFmpeg-Android](https://github.com/yangfeng1994/FFmpeg-Android): FFmpeg-Android 是基于ffmpeg n4.0-39-gda39990编译运行在android平台的音视频的处理框架， 使用的是ProcessBuilder执行命令行操作， 可实现视频字幕添加、尺寸剪切、添加或去除水印、时长截取、转GIF动图、涂鸦、音频提取、拼接、质量压缩、加减速、涂鸦、 倒放、素描、色彩平衡、模糊、九宫格、添加贴纸、滤镜、分屏、图片合成视频等,音视频合成、截取、拼接，混音、音视频解码，视频特效等等音视频处理...

- [yangjie10930/EpMediaDemo](https://github.com/yangjie10930/EpMediaDemo): 基于FFmpeg开发的视频处理框架，简单易用，体积小，帮助使用者快速实现视频处理功能。包含以下功能：剪辑，裁剪，旋转，镜像，合并，分离，添加LOGO，添加滤镜，添加背景音乐，加速减速视频，倒放音视频。简单的Demo,后面逐渐完善各类功能的使用。

- [qingkouwei/oarplayer](https://github.com/qingkouwei/oarplayer): Android Rtmp播放器,基于MediaCodec与srs-librtmp,不依赖ffmpeg

- [goldvideo/decoder_wasm](https://github.com/goldvideo/decoder_wasm): 借助于WebAssembly技术，基于ffmpeg的H.265解码器。

- [HeZhang1994/video-audio-tools](https://github.com/HeZhang1994/video-audio-tools): To process/edit video and audio with Python+FFmpeg. [简单实用] 基于Python+FFmpeg的视频和音频的处理/剪辑。

- [jordiwang/web-capture](https://github.com/jordiwang/web-capture): 基于 ffmpeg + Webassembly 实现前端视频帧提取

- [ccj659/NDK-FFmpeg-master](https://github.com/ccj659/NDK-FFmpeg-master): Video and audio decoding based with FFmpeg 基于ffmpeg的 视频解码 音频解码.播放等

- [kolyvan/kxmovie](https://github.com/kolyvan/kxmovie):iOS电影播放器使用ffmpeg

- [CainKernel/CainCamera](https://github.com/CainKernel/CainCamera):一个关于美容相机、图像和短视频开发的Android项目

- [mifi/lossless-cut](https://github.com/mifi/lossless-cut): 一个基于FFmpeg的无损剪辑软件

## 📂 典藏文档

- [AAC解码算法原理详解](https://github.com/0voice/ffmpeg_develop_doc/blob/main/%E6%96%87%E6%A1%A3%E5%BA%93/AAC%E8%A7%A3%E7%A0%81%E7%AE%97%E6%B3%95%E5%8E%9F%E7%90%86%E8%AF%A6%E8%A7%A3.pdf)
- [FFMPEG教程完美排版](https://github.com/0voice/ffmpeg_develop_doc/blob/main/%E6%96%87%E6%A1%A3%E5%BA%93/FFMPEG%E6%95%99%E7%A8%8B%E5%AE%8C%E7%BE%8E%E6%8E%92%E7%89%88.pdf)
- [FFMpeg-SDK-开发手册](https://github.com/0voice/ffmpeg_develop_doc/blob/main/%E6%96%87%E6%A1%A3%E5%BA%93/FFMpeg-SDK-%E5%BC%80%E5%8F%91%E6%89%8B%E5%86%8C.pdf)
- [FFmpeg Basics](https://github.com/0voice/ffmpeg_develop_doc/blob/main/%E6%96%87%E6%A1%A3%E5%BA%93/FFmpeg%20Basics.pdf)
- [ffmpeg(libav)解码全解析(带书签)](https://github.com/0voice/ffmpeg_develop_doc/blob/main/%E6%96%87%E6%A1%A3%E5%BA%93/ffmpeg(libav)%E8%A7%A3%E7%A0%81%E5%85%A8%E8%A7%A3%E6%9E%90(%E5%B8%A6%E4%B9%A6%E7%AD%BE).pdf)
- [ffmpeg的tutorial中文版](https://github.com/0voice/ffmpeg_develop_doc/blob/main/%E6%96%87%E6%A1%A3%E5%BA%93/ffmpeg%E7%9A%84tutorial%E4%B8%AD%E6%96%87%E7%89%88.pdf)
- [ffmpeg中文文档](https://github.com/0voice/ffmpeg_develop_doc/blob/main/%E6%96%87%E6%A1%A3%E5%BA%93/ffmpeg%E7%9A%84%E4%B8%AD%E6%96%87%E6%96%87%E6%A1%A3.pdf)
- [详解FFMPEG API](https://github.com/0voice/ffmpeg_develop_doc/blob/main/%E6%96%87%E6%A1%A3%E5%BA%93/%E8%AF%A6%E8%A7%A3FFMPEG%20API.pdf)
- [ffmpeg常用命令参数详解](https://github.com/0voice/ffmpeg_develop_doc/blob/main/ffmpeg%E5%B8%B8%E7%94%A8%E5%91%BD%E4%BB%A4.md)
- [ffmepg整体分析](https://github.com/0voice/ffmpeg_develop_doc/blob/main/ffmepg%E6%95%B4%E4%BD%93%E5%88%86%E6%9E%90.pdf)

## 📃 文章

- [FFmpeg 学习(一)：FFmpeg 简介](https://github.com/0voice/ffmpeg_develop_doc/blob/main/FFmpeg%20%E5%AD%A6%E4%B9%A0(%E4%B8%80)%EF%BC%9AFFmpeg%20%E7%AE%80%E4%BB%8B%20.md)
- [FFmpeg 学习(二)：Mac下安装FFmpepg](https://github.com/0voice/ffmpeg_develop_doc/blob/main/FFmpeg%20%E5%AD%A6%E4%B9%A0(%E4%BA%8C)%EF%BC%9AMac%E4%B8%8B%E5%AE%89%E8%A3%85FFmpeg.md)
- [FFmpeg 学习(三)：将 FFmpeg 移植到 Android平台](https://github.com/0voice/ffmpeg_develop_doc/blob/main/FFmpeg%20%E5%AD%A6%E4%B9%A0(%E4%B8%89)%EF%BC%9A%E5%B0%86%20FFmpeg%20%E7%A7%BB%E6%A4%8D%E5%88%B0%20Android%E5%B9%B3%E5%8F%B0.md)
- [FFmpeg 学习(四)：FFmpeg API 介绍与通用 API 分析](https://github.com/0voice/ffmpeg_develop_doc/blob/main/FFmpeg%20%E5%AD%A6%E4%B9%A0(%E5%9B%9B)%EF%BC%9AFFmpeg%20API%20%E4%BB%8B%E7%BB%8D%E4%B8%8E%E9%80%9A%E7%94%A8%20API%20%E5%88%86%E6%9E%90.md)
- [FFmpeg 学习(五)：FFmpeg 编解码 API 分析](https://github.com/0voice/ffmpeg_develop_doc/blob/main/FFmpeg%20%E5%AD%A6%E4%B9%A0(%E4%BA%94)%EF%BC%9AFFmpeg%20%E7%BC%96%E8%A7%A3%E7%A0%81%20API%20%E5%88%86%E6%9E%90.md)
- [FFmpeg 学习(六)：FFmpeg 核心模块 libavformat 与 libavcodec 分析](https://github.com/0voice/ffmpeg_develop_doc/blob/main/FFmpeg%20%E5%AD%A6%E4%B9%A0(%E5%85%AD)%EF%BC%9AFFmpeg%20%E6%A0%B8%E5%BF%83%E6%A8%A1%E5%9D%97%20libavformat%20%E4%B8%8E%20libavcodec%20%E5%88%86%E6%9E%90.md)
- [FFmpeg 学习(七)：FFmpeg 学习整理总结](https://github.com/0voice/ffmpeg_develop_doc/blob/main/FFmpeg%20%E5%AD%A6%E4%B9%A0(%E4%B8%83)%EF%BC%9AFFmpeg%20%E5%AD%A6%E4%B9%A0%E6%95%B4%E7%90%86%E6%80%BB%E7%BB%93.md)

<br>

- [FFmpeg 结构体学习(一)： AVFormatContext 分析](https://github.com/0voice/ffmpeg_develop_doc/blob/main/FFmpeg%20%E7%BB%93%E6%9E%84%E4%BD%93%E5%AD%A6%E4%B9%A0(%E4%B8%80)%EF%BC%9A%20AVFormatContext%20%E5%88%86%E6%9E%90.md)
- [FFmpeg 结构体学习(二)： AVStream 分析](https://github.com/0voice/ffmpeg_develop_doc/blob/main/FFmpeg%20%E7%BB%93%E6%9E%84%E4%BD%93%E5%AD%A6%E4%B9%A0(%E4%BA%8C)%EF%BC%9A%20AVStream%20%E5%88%86%E6%9E%90.md)
- [FFmpeg 结构体学习(三)： AVPacket 分析](https://github.com/0voice/ffmpeg_develop_doc/blob/main/FFmpeg%20%E7%BB%93%E6%9E%84%E4%BD%93%E5%AD%A6%E4%B9%A0(%E4%B8%89)%EF%BC%9A%20AVPacket%20%E5%88%86%E6%9E%90.md)
- [FFmpeg 结构体学习(四)： AVFrame 分析](https://github.com/0voice/ffmpeg_develop_doc/blob/main/FFmpeg%20%E7%BB%93%E6%9E%84%E4%BD%93%E5%AD%A6%E4%B9%A0(%E5%9B%9B)%EF%BC%9A%20AVFrame%20%E5%88%86%E6%9E%90.md)
- [FFmpeg 结构体学习(五)： AVCodec 分析](https://github.com/0voice/ffmpeg_develop_doc/blob/main/FFmpeg%20%E7%BB%93%E6%9E%84%E4%BD%93%E5%AD%A6%E4%B9%A0(%E4%BA%94)%EF%BC%9A%20AVCodec%20%E5%88%86%E6%9E%90.md)
- [FFmpeg 结构体学习(六)： AVCodecContext 分析](https://github.com/0voice/ffmpeg_develop_doc/blob/main/FFmpeg%20%E7%BB%93%E6%9E%84%E4%BD%93%E5%AD%A6%E4%B9%A0(%E5%85%AD)%EF%BC%9A%20AVCodecContext%20%E5%88%86%E6%9E%90.md)
- [FFmpeg 结构体学习(七)： AVIOContext 分析](https://github.com/0voice/ffmpeg_develop_doc/blob/main/FFmpeg%20%E7%BB%93%E6%9E%84%E4%BD%93%E5%AD%A6%E4%B9%A0(%E4%B8%83)%EF%BC%9A%20AVIOContext%20%E5%88%86%E6%9E%90.md)
- [FFmpeg 结构体学习(八)：FFMPEG中重要结构体之间的关系](https://github.com/0voice/ffmpeg_develop_doc/blob/main/FFmpeg%20%E7%BB%93%E6%9E%84%E4%BD%93%E5%AD%A6%E4%B9%A0(%E5%85%AB)%EF%BC%9AFFMPEG%E4%B8%AD%E9%87%8D%E8%A6%81%E7%BB%93%E6%9E%84%E4%BD%93%E4%B9%8B%E9%97%B4%E7%9A%84%E5%85%B3%E7%B3%BB.md)

<br>

- [Linux上的ffmpeg完全使用指南](https://github.com/0voice/ffmpeg_develop_doc/blob/main/Linux%E4%B8%8A%E7%9A%84ffmpeg%E5%AE%8C%E5%85%A8%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97.md)
- [3个重点，20个函数分析，浅析FFmpeg转码过程](https://github.com/0voice/ffmpeg_develop_doc/blob/main/3%E4%B8%AA%E9%87%8D%E7%82%B9%EF%BC%8C20%E4%B8%AA%E5%87%BD%E6%95%B0%E5%88%86%E6%9E%90%EF%BC%8C%E6%B5%85%E6%9E%90FFmpeg%E8%BD%AC%E7%A0%81%E8%BF%87%E7%A8%8B.md)

## 🌅 面试题

##### [1. 为什么巨大的原始视频可以编码成很小的视频呢?这其中的技术是什么呢?](https://github.com/0voice/ffmpeg_develop_doc/blob/main/case_interview/001-README.md#subject_001)

##### [2. 怎么做到直播秒开优化？](https://github.com/0voice/ffmpeg_develop_doc/blob/main/case_interview/001-README.md#subject_002)

##### [3. 直方图在图像处理里面最重要的作用是什么？](https://github.com/0voice/ffmpeg_develop_doc/blob/main/case_interview/001-README.md#subject_003)

##### [4. 数字图像滤波有哪些方法？](https://github.com/0voice/ffmpeg_develop_doc/blob/main/case_interview/001-README.md#subject_004)

##### [5. 图像可以提取的特征有哪些？](https://github.com/0voice/ffmpeg_develop_doc/blob/main/case_interview/001-README.md#subject_005)

##### [6. 衡量图像重建好坏的标准有哪些？怎样计算？](https://github.com/0voice/ffmpeg_develop_doc/blob/main/case_interview/001-README.md#subject_006)

##### [7. AAC和PCM的区别？](https://github.com/0voice/ffmpeg_develop_doc/blob/main/case_interview/001-README.md#subject_007)

##### [8. H264存储的两个形态？](https://github.com/0voice/ffmpeg_develop_doc/blob/main/case_interview/001-README.md#subject_008)

##### [9. FFMPEG：图片如何合成视频？](https://github.com/0voice/ffmpeg_develop_doc/blob/main/case_interview/001-README.md#subject_009)

##### [10. 常见的音视频格式有哪些？](https://github.com/0voice/ffmpeg_develop_doc/blob/main/case_interview/001-README.md#subject_010)

##### [11. 请指出“1080p”的意义？](https://github.com/0voice/ffmpeg_develop_doc/blob/main/case_interview/001-README.md#subject_011)

##### [12. 请解释颜色的本质及其数字记录原理，并说出几个你所知道的色域。](https://github.com/0voice/ffmpeg_develop_doc/blob/main/case_interview/001-README.md#subject_012)

##### [13. 请解释“矢量图”和“位图”的区别？](https://github.com/0voice/ffmpeg_develop_doc/blob/main/case_interview/001-README.md#subject_013)

##### [14. 请从“光圈”“快门速度”“感光度”“白平衡”“景深”中任选2个进行叙述？](https://github.com/0voice/ffmpeg_develop_doc/blob/main/case_interview/001-README.md#subject_014)

##### [15. 视频分量YUV的意义及数字化格式？](https://github.com/0voice/ffmpeg_develop_doc/blob/main/case_interview/001-README.md#subject_015)

##### [16. 在MPEG标准中图像类型有哪些？](https://github.com/0voice/ffmpeg_develop_doc/blob/main/case_interview/001-README.md#subject_016)

##### [17. 列举一些音频编解码常用的实现方案？](https://github.com/0voice/ffmpeg_develop_doc/blob/main/case_interview/001-README.md#subject_017)

##### [18. 请叙述MPEG视频基本码流结构？](https://github.com/0voice/ffmpeg_develop_doc/blob/main/case_interview/001-README.md#subject_018)

##### [19. sps和pps的区别？](https://github.com/0voice/ffmpeg_develop_doc/blob/main/case_interview/001-README.md#subject_019)

##### [20. 请叙述AMR基本码流结构？](https://github.com/0voice/ffmpeg_develop_doc/blob/main/case_interview/001-README.md#subject_020)

##### [21. 预测编码的基本原理是什么？](https://github.com/0voice/ffmpeg_develop_doc/blob/main/case_interview/001-README.md#subject_021)

##### [22. 说一说ffmpeg的数据结构？](https://github.com/0voice/ffmpeg_develop_doc/blob/main/case_interview/001-README.md#subject_022)

##### [23. 说一说AVFormatContext 和 AVInputFormat之间的关系？](https://github.com/0voice/ffmpeg_develop_doc/blob/main/case_interview/001-README.md#subject_023)

##### [24. 说一说AVFormatContext, AVStream和AVCodecContext之间的关系？](https://github.com/0voice/ffmpeg_develop_doc/blob/main/case_interview/001-README.md#subject_024)

##### [25. 说一说视频拼接处理步骤？（细节处理，比如分辨率大小不一，时间处理等等）](https://github.com/0voice/ffmpeg_develop_doc/blob/main/case_interview/001-README.md#subject_025)

##### [26. NV21如何转换成I420？](https://github.com/0voice/ffmpeg_develop_doc/blob/main/case_interview/001-README.md#subject_026)

##### [27. DTS与PTS共同点？](https://github.com/0voice/ffmpeg_develop_doc/blob/main/case_interview/001-README.md#subject_027)

##### [28. 影响视频清晰度的指标有哪些？](https://github.com/0voice/ffmpeg_develop_doc/blob/main/case_interview/001-README.md#subject_028)

##### [29. 编解码处理时遇到什么困难？](https://github.com/0voice/ffmpeg_develop_doc/blob/main/case_interview/001-README.md#subject_029)

##### [30. 如何秒开视频？什么是秒开视频？](https://github.com/0voice/ffmpeg_develop_doc/blob/main/case_interview/001-README.md#subject_030)

##### [31. 如何降低延迟？如何保证流畅性？如何解决卡顿？解决网络抖动？](https://github.com/0voice/ffmpeg_develop_doc/blob/main/case_interview/001-README.md#subject_031)

##### [32. 需要把网络上一段视频存储下来（比如作为mp4 ), 请实现并说出方法（第一个视频需要翻墙才能进）？](https://github.com/0voice/ffmpeg_develop_doc/blob/main/case_interview/001-README.md#subject_032)

##### [33. 需要把网络上一段语音存储下来（比如作为mp3 ), 请实现并说出方法？](https://github.com/0voice/ffmpeg_develop_doc/blob/main/case_interview/001-README.md#subject_033)

##### [34. 为什么要有YUV这种数据出来？（YUV相比RGB来说的优点）](https://github.com/0voice/ffmpeg_develop_doc/blob/main/case_interview/001-README.md#subject_034)

##### [35. H264/H265有什么区别？](https://github.com/0voice/ffmpeg_develop_doc/blob/main/case_interview/001-README.md#subject_035)

##### [36. 视频或者音频传输，你会选择TCP协议还是UDP协议？为什么？](https://github.com/0voice/ffmpeg_develop_doc/blob/main/case_interview/001-README.md#subject_036)

##### [37. 平时说的软解和硬解，具体是什么？](https://github.com/0voice/ffmpeg_develop_doc/blob/main/case_interview/001-README.md#subject_037)

##### [38. 何为直播？何为点播？](https://github.com/0voice/ffmpeg_develop_doc/blob/main/case_interview/001-README.md#subject_038)

##### [39. 简述推流、拉流的工作流程？](https://github.com/0voice/ffmpeg_develop_doc/blob/main/case_interview/001-README.md#subject_039)

##### [40. 如何在直播中I帧间隔设置、与帧率分辨率选定？](https://github.com/0voice/ffmpeg_develop_doc/blob/main/case_interview/001-README.md#subject_040)

##### [41. 直播推流中推I帧与推非I帧区别是什么？](https://github.com/0voice/ffmpeg_develop_doc/blob/main/case_interview/001-README.md#subject_041)

##### [42. 常见的直播协议有哪些？之间有什么区别？](https://github.com/0voice/ffmpeg_develop_doc/blob/main/case_interview/001-README.md#subject_042)

##### [43. 点播中常见的数据传输协议主要有哪些？](https://github.com/0voice/ffmpeg_develop_doc/blob/main/case_interview/001-README.md#subject_043)

##### [44. RTMP、HLS协议各自的默认端口号是？](https://github.com/0voice/ffmpeg_develop_doc/blob/main/case_interview/001-README.md#subject_044)

##### [45. 简述RTMP协议，如何封装RTMP包？](https://github.com/0voice/ffmpeg_develop_doc/blob/main/case_interview/001-README.md#subject_045)

##### [46. m3u8构成是？直播中m3u8、ts如何实时更新？](https://github.com/0voice/ffmpeg_develop_doc/blob/main/case_interview/001-README.md#subject_046)

##### [47. 何为音视频同步，音视频同步是什么标准？](https://github.com/0voice/ffmpeg_develop_doc/blob/main/case_interview/001-README.md#subject_047)

##### [48. 播放器暂停、快进快退、seek、逐帧、变速怎么实现？](https://github.com/0voice/ffmpeg_develop_doc/blob/main/case_interview/001-README.md#subject_048)

##### [49. 说说你平时在播放过程中做的优化工作？](https://github.com/0voice/ffmpeg_develop_doc/blob/main/case_interview/001-README.md#subject_049)

##### [50. 你研究过哪些具体的流媒体服务器，是否做过二次开发？](https://github.com/0voice/ffmpeg_develop_doc/blob/main/case_interview/001-README.md#subject_050)

##### [51. 什么是GOP?](https://github.com/0voice/ffmpeg_develop_doc/blob/main/case_interview/002-README.md#subject_051)

##### [52. 音频测试的测试点,音频时延如何测试?](https://github.com/0voice/ffmpeg_develop_doc/blob/main/case_interview/002-README.md#subject_052)

##### [53. 美颜的实现原理，具体实现步骤?](https://github.com/0voice/ffmpeg_develop_doc/blob/main/case_interview/002-README.md#subject_053)

##### [54. 如何直播APP抓包过来的文件，如何过滤上行，下行，总码率？](https://github.com/0voice/ffmpeg_develop_doc/blob/main/case_interview/002-README.md#subject_054)

##### [55. 如何测试一个美颜挂件？](https://github.com/0voice/ffmpeg_develop_doc/blob/main/case_interview/002-README.md#subject_055)

##### [56. 为什么要用FLV？](https://github.com/0voice/ffmpeg_develop_doc/blob/main/case_interview/002-README.md#subject_056)

##### [57. 如何测试一个美颜挂件？](https://github.com/0voice/ffmpeg_develop_doc/blob/main/case_interview/002-README.md#subject_057)

##### [58. 平常的视频格式？](https://github.com/0voice/ffmpeg_develop_doc/blob/main/case_interview/002-README.md#subject_058)

##### [59. 何为homebrew？你用它安装过什么？常用命令有哪些？](https://github.com/0voice/ffmpeg_develop_doc/blob/main/case_interview/002-README.md#subject_059)

##### [60. RTMP、HLS协议各自的默认端口号是？](https://github.com/0voice/ffmpeg_develop_doc/blob/main/case_interview/002-README.md#subject_060)

## 🧿 视频

### 国外大神

No.|title
:------- | :---------------
1|[如何使用FFMPEG将MP4视频文件转换为GIF](https://www.0voice.com/uiwebsite/audio_video_streaming/video/001-如何使用FFMPEG将MP4视频文件转换为GIF.mp4)
2|[FFMPEG Introduction & Examples](https://www.0voice.com/uiwebsite/audio_video_streaming/video/002-FFMPEG%20Introduction%20%26%20Examples.mp4)
3|[Live Streaming with Nginx and FFmpeg](https://www.0voice.com/uiwebsite/audio_video_streaming/video/003-Live%20Streaming%20with%20Nginx%20and%20FFmpeg.mp4)
4|[Ep2 Ffmpeg Nginx & Nginx-Rtmp-Module Streaming to Server](https://www.0voice.com/uiwebsite/audio_video_streaming/video/004-Ep2%20Ffmpeg%20Nginx%20%26%20Nginx-Rtmp-Module%20Streaming%20to%20Server.mp4)
5|[Streaming an IP Camera to a Web Browser using FFmpeg](https://www.0voice.com/uiwebsite/audio_video_streaming/video/005-Streaming%20an%20IP%20Camera%20to%20a%20Web%20Browser%20using%20FFmpeg.mp4)
6|[Easy Screencasting and Webcamming with ffmpeg in Linux](https://www.0voice.com/uiwebsite/audio_video_streaming/video/006-Easy%20Screencasting%20and%20Webcamming%20with%20ffmpeg%20in%20Linux.mp4)
7|[Streaming an IP Camera to a Web Browser using FFmpeg](https://www.0voice.com/uiwebsite/audio_video_streaming/video/007-Streaming%20an%20IP%20Camera%20to%20a%20Web%20Browser%20using%20FFmpeg.mp4)
8|[FFMPEG Advanced Techniques Pt2 - Filtergraphs & Timeline](https://www.0voice.com/uiwebsite/audio_video_streaming/video/008-FFMPEG%20Advanced%20Techniques%20Pt2%20-%20Filtergraphs%20%26%20Timeline.mp4)
9|[Convert HEVCh265 mkv video to AVCh264 mp4 with ffmpeg](https://www.0voice.com/uiwebsite/audio_video_streaming/video/009-Convert%20HEVCh265%20mkv%20video%20to%20AVCh264%20mp4%20with%20ffmpeg.mp4)
10|[How to add soft subtitles( srt subrip) to mp4 video using ffmpeg](https://www.0voice.com/uiwebsite/audio_video_streaming/video/010-How%20to%20add%20soft%20subtitles(%20srt%20subrip)%20to%20mp4%20video%20using%20ffmpeg.mp4)
11|[FFmpeg Processing multiple video files by using.bat file](https://www.0voice.com/uiwebsite/audio_video_streaming/video/011-FFmpeg%20Processing%20multiple%20video%20files%20by%20using.bat%20file.mp4)
12|[Opensource Multimedia Framework -- FFmpeg](https://www.0voice.com/uiwebsite/audio_video_streaming/video/012-Opensource%20Multimedia%20Framework%20--%20FFmpeg.mp4)
13|[rtsp streaming node js ip camera jsmpeg](https://www.0voice.com/uiwebsite/audio_video_streaming/video/013-rtsp%20streaming%20node%20js%20ip%20camera%20jsmpeg.mp4)
14|[H.265 RTSP Streaming to VLC + NewTek NDI Integration](https://www.0voice.com/uiwebsite/audio_video_streaming/video/014-H.265%20RTSP%20Streaming%20to%20VLC%20+%20NewTek%20NDI%20Integration.mp4)
15|[IP camera stream using RTSP and openCV python](https://www.0voice.com/uiwebsite/audio_video_streaming/video/015-IP%20camera%20stream%20using%20RTSP%20and%20openCV%20python.mp4)
16|[NAT Traversal & RTSP](https://www.0voice.com/uiwebsite/audio_video_streaming/video/016-NAT%20Traversal%20%26%20RTSP.mp4)
17|[Simple client et serveur de Streaming RTSP MJPEG(JAVA SE)](https://www.0voice.com/uiwebsite/audio_video_streaming/video/017-Simple%20client%20et%20serveur%20de%20Streaming%20RTSP%20MJPEG(JAVA%20SE).mp4)
18|[Build Your First WebRTC Video Chat App](https://www.0voice.com/uiwebsite/audio_video_streaming/video/018-Build%20Your%20First%20WebRTC%20Video%20Chat%20App.mp4)
19|[P2P Video Chat with JavaScript/WebRTC](https://www.0voice.com/uiwebsite/audio_video_streaming/video/019-P2P%20Video%20Chat%20with%20JavaScript%20WebRTC.mp4)
20|[Building a WebRTC app - LIVE](https://www.0voice.com/uiwebsite/audio_video_streaming/video/020-Building%20a%20WebRTC%20app%20-%20LIVE.mp4)
21|[Zoom vs WebRTC](https://www.0voice.com/uiwebsite/audio_video_streaming/video/021-Zoom%20vs%20WebRTC.mp4)
22|[Architectures for a kickass WebRTC application](https://www.0voice.com/uiwebsite/audio_video_streaming/video/022-Architectures%20for%20a%20kickass%20WebRTC%20application.mp4)
23|[(REACT NATIVE) - integrate webRTC](https://www.0voice.com/uiwebsite/audio_video_streaming/video/023-(REACT%20NATIVE)%20-%20integrate%20webRTC.mp4)
24|[How to build Serverless Video Chat App using Firebase and WebRTC in React](https://www.0voice.com/uiwebsite/audio_video_streaming/video/024-How%20to%20build%20Serverless%20Video%20Chat%20App%20using%20Firebase%20and%20WebRTC%20in%20React.mp4)
25|[Implementation Lessons using WebRTC in Asterisk](https://www.0voice.com/uiwebsite/audio_video_streaming/video/025-Implementation%20Lessons%20using%20WebRTC%20in%20Asterisk.mp4)

### 国内大佬

No.|title | 地址
:------- | :---------------| :---------------
26|windows ffmpeg命令行环境搭建|[百度网盘](https://pan.baidu.com/s/1eCQ7o3gcuU06k6-ZcXUASQ)  提取码：i3f2
27|FFMPEG如何查询命令帮助文档|[百度网盘](https://pan.baidu.com/s/1oA2OErmfZZpEEY_wRQrl_A)  提取码：9mqk
28|ffmpeg音视频处理流程|[百度网盘](https://pan.baidu.com/s/1jSIop6IUtxOwkse7xnCI7Q)  提取码：azx3
29|ffmpeg命令分类查询|[百度网盘](https://pan.baidu.com/s/1VGwop_lOJozEh_gYpKYkrw)  提取码：odhc
30|ffplay播放控制|[百度网盘](https://pan.baidu.com/s/1BbKQvJdokQrazoNtYjhA2Q)  提取码：e51s
31|ffplay命令选项(上)|[百度网盘](https://pan.baidu.com/s/1upOGZQdmXyiZbWO1LBcTCQ)  提取码：n1zx
32|ffplay命令选项(下)|[百度网盘](https://pan.baidu.com/s/1d55H9PyK1CU9Nfu37NIBhw)  提取码：rtn0
33|ffplay命令播放媒体|[百度网盘](https://pan.baidu.com/s/1FjJnW8eBZxsKIIdvbh0f-A)  提取码：bs9s
34|ffplay简单过滤器|[百度网盘](https://pan.baidu.com/s/1YlkCGIMH62Wj0-OTRLxDkA)  提取码：r4rk
35|ffmpeg命令参数说明|[百度网盘](https://pan.baidu.com/s/1aOL7vXnspVAh-iNYsz_5xA)  提取码：5q18
36|ffmpeg命令提取音视频数据|[百度网盘](https://pan.baidu.com/s/1Zlv_6a-O9Fj9HFpt9S6Z5g)  提取码：v807
37|ffmpeg命令提取像素格式和PCM数据|[百度网盘](https://pan.baidu.com/s/1Z1cdwVexIvAiyCQNPA0k3A)  提取码：az9x
38|ffmpeg命令转封装|[百度网盘](https://pan.baidu.com/s/1TxZpe2RicrGWgZPhi81E2g)  提取码：s7ez
39|fmpeg命令裁剪和合并视频|[百度网盘](https://pan.baidu.com/s/1W8b_krHc3PzAfoRXneS2Wg)  提取码：6g0g
40|fmpeg命令图片与视频互转|[百度网盘](https://pan.baidu.com/s/1nHhhA3y8dHneFVfNoY_fHg)  提取码：a3p5
41|ffmpeg命令视频录制|[百度网盘](https://pan.baidu.com/s/1zGz_P34GHKE5KVt_b8bT3w)  提取码：em7b
42|ffmpeg命令直播(上)|[百度网盘](https://pan.baidu.com/s/1rtCfJWWaanK6Syk2254h2g)  提取码：ilxz
43|ffmpeg命令直播(下)|[百度网盘](https://pan.baidu.com/s/1mo7vo4d_ghqrue7gzE0M1g)  提取码：akyr
44|ffmpeg过滤器-裁剪|[百度网盘](https://pan.baidu.com/s/1vuQLx_ff8ZnlStxX2aOeXA)  提取码：toii
45|ffmpeg过滤器-文字水印|[百度网盘](https://pan.baidu.com/s/1YilCkZg99xhwEQBwjenWKQ)  提取码：unuu
46|ffmpeg过滤器-图片水印|[百度网盘](https://pan.baidu.com/s/11VFsXn-c8e9GZ3Wy4M8hAA)  提取码：mw4v
47|ffmpeg过滤器-画中画|[百度网盘](https://pan.baidu.com/s/1TFiR47qhPTHAzbSQhatEBA)  提取码：c6fc
48|ffmpeg过滤器-多宫格|[百度网盘](https://pan.baidu.com/s/1Ib73MtuqgaFoECuSrzOApQ)  提取码：aioi
49|SRS流媒体服务器实战(上)|[百度网盘](https://pan.baidu.com/s/1kZTa5-0kfCcdMiObpJdOfQ)  提取码：4134
50|SRS流媒体服务器实战(下)|[百度网盘](https://pan.baidu.com/s/1goy3g9rmHc-JmO9VpsCKvg)  提取码：g4be
51|音视频开发-ffplay.iikplayer、vlc的播放器设计实现|[百度网盘](https://pan.baidu.com/s/1NTT_fzfkWIYy2DX90joAoA)  提取码：1img
52|音视频成长之路-进阶三部曲|[百度网盘](https://pan.baidu.com/s/1XUTn60ZHTBt63CmQe2vObw)  提取码：4nw3
53|为什么直播领域也要搞WebRTC-srs4.0|[百度网盘](https://pan.baidu.com/s/1c9dexc7-QglR-0hkvqnUEQ)  提取码：m47a
54|腾讯课堂直播如何做到低延迟|[百度网盘](https://pan.baidu.com/s/1oRuwvWRyw7YjDAqzMPnZyQ)  提取码：jruh
55|rtmp2webrtc提出问题-灵魂拷问|[百度网盘](https://pan.baidu.com/s/1cyf0qCYUYKNyfSchyY6aWQ)  提取码：pupp

## 📰 论文

[分布式视频处理系统设计与实现](https://github.com/0voice/ffmpeg_develop_doc/blob/main/%E5%88%86%E5%B8%83%E5%BC%8F%E8%A7%86%E9%A2%91%E5%A4%84%E7%90%86%E7%B3%BB%E7%BB%9F%E8%AE%BE%E8%AE%A1%E4%B8%8E%E5%AE%9E%E7%8E%B0.pdf)

[基于Android的H.264_AVC解码器的设计与实现](https://github.com/0voice/ffmpeg_develop_doc/blob/main/%E5%9F%BA%E4%BA%8EAndroid%E7%9A%84H.264_AVC%E8%A7%A3%E7%A0%81%E5%99%A8%E7%9A%84%E8%AE%BE%E8%AE%A1%E4%B8%8E%E5%AE%9E%E7%8E%B0.pdf)

[基于FFMPEG的视频转换系统](https://github.com/0voice/ffmpeg_develop_doc/blob/main/%E5%9F%BA%E4%BA%8EFFMPEG%E7%9A%84%E8%A7%86%E9%A2%91%E8%BD%AC%E6%8D%A2%E7%B3%BB%E7%BB%9F.pdf)

[基于FFMPEG的跨平台视频编解码研究](https://github.com/0voice/ffmpeg_develop_doc/blob/main/%E5%9F%BA%E4%BA%8EFFMPEG%E7%9A%84%E8%B7%A8%E5%B9%B3%E5%8F%B0%E8%A7%86%E9%A2%91%E7%BC%96%E8%A7%A3%E7%A0%81%E7%A0%94%E7%A9%B6.pdf)

[基于FFMPEG解码的音视频同步实现](https://github.com/0voice/ffmpeg_develop_doc/blob/main/%E5%9F%BA%E4%BA%8EFFMPEG%E8%A7%A3%E7%A0%81%E7%9A%84%E9%9F%B3%E8%A7%86%E9%A2%91%E5%90%8C%E6%AD%A5%E5%AE%9E%E7%8E%B0.pdf)

[基于FFMpeg的稳定应用层组播流媒体直播系统研究](https://github.com/0voice/ffmpeg_develop_doc/blob/main/%E5%9F%BA%E4%BA%8EFFMpeg%E7%9A%84%E7%A8%B3%E5%AE%9A%E5%BA%94%E7%94%A8%E5%B1%82%E7%BB%84%E6%92%AD%E6%B5%81%E5%AA%92%E4%BD%93%E7%9B%B4%E6%92%AD%E7%B3%BB%E7%BB%9F%E7%A0%94%E7%A9%B6.pdf)

[基于FFmpeg和SDL的智能录屏及播放系统](https://github.com/0voice/ffmpeg_develop_doc/blob/main/%E5%9F%BA%E4%BA%8EFFmpeg%E5%92%8CSDL%E7%9A%84%E6%99%BA%E8%83%BD%E5%BD%95%E5%B1%8F%E5%8F%8A%E6%92%AD%E6%94%BE%E7%B3%BB%E7%BB%9F.pdf)

[基于FFmpeg和SDL的视频流播放存储研究综述](https://github.com/0voice/ffmpeg_develop_doc/blob/main/%E5%9F%BA%E4%BA%8EFFmpeg%E5%92%8CSDL%E7%9A%84%E8%A7%86%E9%A2%91%E6%B5%81%E6%92%AD%E6%94%BE%E5%AD%98%E5%82%A8%E7%A0%94%E7%A9%B6%E7%BB%BC%E8%BF%B0.pdf)

[基于FFmpeg的H.264解码器实现](https://github.com/0voice/ffmpeg_develop_doc/blob/main/%E5%9F%BA%E4%BA%8EFFmpeg%E7%9A%84H.264%E8%A7%A3%E7%A0%81%E5%99%A8%E5%AE%9E%E7%8E%B0.pdf)

[基于FFmpeg的网络视频监控系统的设计与实现](https://github.com/0voice/ffmpeg_develop_doc/blob/main/%E5%9F%BA%E4%BA%8EFFmpeg%E7%9A%84%E7%BD%91%E7%BB%9C%E8%A7%86%E9%A2%91%E7%9B%91%E6%8E%A7%E7%B3%BB%E7%BB%9F%E7%9A%84%E8%AE%BE%E8%AE%A1%E4%B8%8E%E5%AE%9E%E7%8E%B0.pdf)

[基于FFmpeg的视频转码与保护系统的设计与实现](https://github.com/0voice/ffmpeg_develop_doc/blob/main/%E5%9F%BA%E4%BA%8EFFmpeg%E7%9A%84%E8%A7%86%E9%A2%91%E8%BD%AC%E7%A0%81%E4%B8%8E%E4%BF%9D%E6%8A%A4%E7%B3%BB%E7%BB%9F%E7%9A%84%E8%AE%BE%E8%AE%A1%E4%B8%8E%E5%AE%9E%E7%8E%B0.pdf)

[基于FFmpeg的高清实时直播系统设计与实现](https://github.com/0voice/ffmpeg_develop_doc/blob/main/%E5%9F%BA%E4%BA%8EFFmpeg%E7%9A%84%E9%AB%98%E6%B8%85%E5%AE%9E%E6%97%B6%E7%9B%B4%E6%92%AD%E7%B3%BB%E7%BB%9F%E8%AE%BE%E8%AE%A1%E4%B8%8E%E5%AE%9E%E7%8E%B0.pdf)

[基于H.264与H.265的低延时视频监控系统的设计与实现](https://github.com/0voice/ffmpeg_develop_doc/blob/main/%E5%9F%BA%E4%BA%8EH.264%E4%B8%8EH.265%E7%9A%84%E4%BD%8E%E5%BB%B6%E6%97%B6%E8%A7%86%E9%A2%91%E7%9B%91%E6%8E%A7%E7%B3%BB%E7%BB%9F%E7%9A%84%E8%AE%BE%E8%AE%A1%E4%B8%8E%E5%AE%9E%E7%8E%B0.pdf)

[基于H.265的无线视频监控系统设计与实现](https://github.com/0voice/ffmpeg_develop_doc/blob/main/%E5%9F%BA%E4%BA%8EH.265%E7%9A%84%E6%97%A0%E7%BA%BF%E8%A7%86%E9%A2%91%E7%9B%91%E6%8E%A7%E7%B3%BB%E7%BB%9F%E8%AE%BE%E8%AE%A1%E4%B8%8E%E5%AE%9E%E7%8E%B0.pdf)

[基于H.265的视频教育系统的设计与实现](https://github.com/0voice/ffmpeg_develop_doc/blob/main/%E5%9F%BA%E4%BA%8EH.265%E7%9A%84%E8%A7%86%E9%A2%91%E6%95%99%E8%82%B2%E7%B3%BB%E7%BB%9F%E7%9A%84%E8%AE%BE%E8%AE%A1%E4%B8%8E%E5%AE%9E%E7%8E%B0.pdf)

[基于Hadoop的视频转码优化的研究](https://github.com/0voice/ffmpeg_develop_doc/blob/main/%E5%9F%BA%E4%BA%8EHadoop%E7%9A%84%E8%A7%86%E9%A2%91%E8%BD%AC%E7%A0%81%E4%BC%98%E5%8C%96%E7%9A%84%E7%A0%94%E7%A9%B6.pdf)

[基于RTMP协议的流媒体系统的设计实现](https://github.com/0voice/ffmpeg_develop_doc/blob/main/%E5%9F%BA%E4%BA%8ERTMP%E5%8D%8F%E8%AE%AE%E7%9A%84%E6%B5%81%E5%AA%92%E4%BD%93%E7%B3%BB%E7%BB%9F%E7%9A%84%E8%AE%BE%E8%AE%A1%E5%AE%9E%E7%8E%B0.pdf)

[基于RTMP的高清流媒体直播点播封装技术的研究与实现](https://github.com/0voice/ffmpeg_develop_doc/blob/main/%E5%9F%BA%E4%BA%8ERTMP%E7%9A%84%E9%AB%98%E6%B8%85%E6%B5%81%E5%AA%92%E4%BD%93%E7%9B%B4%E6%92%AD%E7%82%B9%E6%92%AD%E5%B0%81%E8%A3%85%E6%8A%80%E6%9C%AF%E7%9A%84%E7%A0%94%E7%A9%B6%E4%B8%8E%E5%AE%9E%E7%8E%B0.caj)

[基于RTSP协议的iOS视频播放器的设计与实现](https://github.com/0voice/ffmpeg_develop_doc/blob/main/%E5%9F%BA%E4%BA%8ERTSP%E5%8D%8F%E8%AE%AE%E7%9A%84iOS%E8%A7%86%E9%A2%91%E6%92%AD%E6%94%BE%E5%99%A8%E7%9A%84%E8%AE%BE%E8%AE%A1%E4%B8%8E%E5%AE%9E%E7%8E%B0.pdf)

[基于RTSP协议的多源视音频实时直播系统的设计与实现](https://github.com/0voice/ffmpeg_develop_doc/blob/main/%E5%9F%BA%E4%BA%8ERTSP%E5%8D%8F%E8%AE%AE%E7%9A%84%E5%A4%9A%E6%BA%90%E8%A7%86%E9%9F%B3%E9%A2%91%E5%AE%9E%E6%97%B6%E7%9B%B4%E6%92%AD%E7%B3%BB%E7%BB%9F%E7%9A%84%E8%AE%BE%E8%AE%A1%E4%B8%8E%E5%AE%9E%E7%8E%B0.pdf)

[基于RTSP的H.264实时流媒体传输方案的研究与实现](https://github.com/0voice/ffmpeg_develop_doc/blob/main/%E5%9F%BA%E4%BA%8ERTSP%E7%9A%84H.264%E5%AE%9E%E6%97%B6%E6%B5%81%E5%AA%92%E4%BD%93%E4%BC%A0%E8%BE%93%E6%96%B9%E6%A1%88%E7%9A%84%E7%A0%94%E7%A9%B6%E4%B8%8E%E5%AE%9E%E7%8E%B0.pdf)

[基于RTSP的音视频传输系统研究与实现](https://github.com/0voice/ffmpeg_develop_doc/blob/main/%E5%9F%BA%E4%BA%8ERTSP%E7%9A%84%E9%9F%B3%E8%A7%86%E9%A2%91%E4%BC%A0%E8%BE%93%E7%B3%BB%E7%BB%9F%E7%A0%94%E7%A9%B6%E4%B8%8E%E5%AE%9E%E7%8E%B0.pdf)

[基于TCP传输的嵌入式流媒体播放系统](https://github.com/0voice/ffmpeg_develop_doc/blob/main/%E5%9F%BA%E4%BA%8ETCP%E4%BC%A0%E8%BE%93%E7%9A%84%E5%B5%8C%E5%85%A5%E5%BC%8F%E6%B5%81%E5%AA%92%E4%BD%93%E6%92%AD%E6%94%BE%E7%B3%BB%E7%BB%9F.pdf)

[基于ffmpeg的高性能高清流媒体播放器软件设计](https://github.com/0voice/ffmpeg_develop_doc/blob/main/%E5%9F%BA%E4%BA%8Effmpeg%E7%9A%84%E9%AB%98%E6%80%A7%E8%83%BD%E9%AB%98%E6%B8%85%E6%B5%81%E5%AA%92%E4%BD%93%E6%92%AD%E6%94%BE%E5%99%A8%E8%BD%AF%E4%BB%B6%E8%AE%BE%E8%AE%A1.pdf)

[基于流媒体技术的移动视频直播系统的设计与实现](https://github.com/0voice/ffmpeg_develop_doc/blob/main/%E5%9F%BA%E4%BA%8E%E6%B5%81%E5%AA%92%E4%BD%93%E6%8A%80%E6%9C%AF%E7%9A%84%E7%A7%BB%E5%8A%A8%E8%A7%86%E9%A2%91%E7%9B%B4%E6%92%AD%E7%B3%BB%E7%BB%9F%E7%9A%84%E8%AE%BE%E8%AE%A1%E4%B8%8E%E5%AE%9E%E7%8E%B0.pdf)

[直播聚合平台的设计与实现](https://github.com/0voice/ffmpeg_develop_doc/blob/main/%E7%9B%B4%E6%92%AD%E8%81%9A%E5%90%88%E5%B9%B3%E5%8F%B0%E7%9A%84%E8%AE%BE%E8%AE%A1%E4%B8%8E%E5%AE%9E%E7%8E%B0.pdf)

[音视频信号采集压缩及传输系统的设计与实现](https://github.com/0voice/ffmpeg_develop_doc/blob/main/%E9%9F%B3%E8%A7%86%E9%A2%91%E4%BF%A1%E5%8F%B7%E9%87%87%E9%9B%86%E5%8E%8B%E7%BC%A9%E5%8F%8A%E4%BC%A0%E8%BE%93%E7%B3%BB%E7%BB%9F%E7%9A%84%E8%AE%BE%E8%AE%A1%E4%B8%8E%E5%AE%9E%E7%8E%B0.pdf)


## 联系专栏

#### [【免费】FFmpeg/WebRTC/RTMP/NDK/Android音视频流媒体高级开发](https://ke.qq.com/course/3202131?flowToken=1035185)

#### 音视频学习资料包：

<img width="65%" height="65%" src="https://user-images.githubusercontent.com/87457873/148544835-6355a58e-73ff-4d99-905f-6a22db167c3c.jpg"/>

#### 关注微信公众号【后台服务架构师】——【联系我们】，免费获取更多FFmepg学习资料！

<img width="65%" height="65%" src="https://user-images.githubusercontent.com/87457873/130796999-03af3f54-3719-47b4-8e41-2e762ab1c68b.png"/>

