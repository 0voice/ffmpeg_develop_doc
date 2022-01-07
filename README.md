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
  <a href="https://github.com/0voice/kernel_memory_management#%E8%81%94%E7%B3%BB%E4%B8%93%E6%A0%8F"><img src="https://img.shields.io/badge/微信公众号-green" alt=""></a>
  <a href="https://www.zhihu.com/people/xiao-zhai-nu-linux"><img src="https://img.shields.io/badge/知乎-blue" alt=""></a>
  <a href="https://space.bilibili.com/64514973"><img src="https://img.shields.io/badge/bilibili-red" alt=""></a>
</p>

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

## 🌅 面试题

##### [1. 为什么巨大的原始视频可以编码成很小的视频呢?这其中的技术是什么呢?](https://github.com/0voice/audio_video_streaming/blob/main/case_interview/001-README.md#subject_001)
##### [2. 怎么做到直播秒开优化？](https://github.com/0voice/audio_video_streaming/blob/main/case_interview/001-README.md#subject_002)
##### [3. 直方图在图像处理里面最重要的作用是什么？](https://github.com/0voice/audio_video_streaming/blob/main/case_interview/001-README.md#subject_003)
##### [4. 数字图像滤波有哪些方法？](https://github.com/0voice/audio_video_streaming/blob/main/case_interview/001-README.md#subject_004)
##### [5. 图像可以提取的特征有哪些？](https://github.com/0voice/audio_video_streaming/blob/main/case_interview/001-README.md#subject_005)
##### [6. 衡量图像重建好坏的标准有哪些？怎样计算？](https://github.com/0voice/audio_video_streaming/blob/main/case_interview/001-README.md#subject_006)
##### [7. AAC和PCM的区别？](https://github.com/0voice/audio_video_streaming/blob/main/case_interview/001-README.md#subject_007)
##### [8. H264存储的两个形态？](https://github.com/0voice/audio_video_streaming/blob/main/case_interview/001-README.md#subject_008)
##### [9. FFMPEG：图片如何合成视频？](https://github.com/0voice/audio_video_streaming/blob/main/case_interview/001-README.md#subject_009)
##### [10. 常见的音视频格式有哪些？](https://github.com/0voice/audio_video_streaming/blob/main/case_interview/001-README.md#subject_010)
##### [11. 请指出“1080p”的意义？](https://github.com/0voice/audio_video_streaming/blob/main/case_interview/001-README.md#subject_011)
##### [12. 请解释颜色的本质及其数字记录原理，并说出几个你所知道的色域。](https://github.com/0voice/audio_video_streaming/blob/main/case_interview/001-README.md#subject_012)
##### [13. 请解释“矢量图”和“位图”的区别？](https://github.com/0voice/audio_video_streaming/blob/main/case_interview/001-README.md#subject_013)
##### [14. 请从“光圈”“快门速度”“感光度”“白平衡”“景深”中任选2个进行叙述？](https://github.com/0voice/audio_video_streaming/blob/main/case_interview/001-README.md#subject_014)
##### [15. 视频分量YUV的意义及数字化格式？](https://github.com/0voice/audio_video_streaming/blob/main/case_interview/001-README.md#subject_015)
##### [16. 在MPEG标准中图像类型有哪些？](https://github.com/0voice/audio_video_streaming/blob/main/case_interview/001-README.md#subject_016)
##### [17. 列举一些音频编解码常用的实现方案？](https://github.com/0voice/audio_video_streaming/blob/main/case_interview/001-README.md#subject_017)
##### [18. 请叙述MPEG视频基本码流结构？](https://github.com/0voice/audio_video_streaming/blob/main/case_interview/001-README.md#subject_018)
##### [19. sps和pps的区别？](https://github.com/0voice/audio_video_streaming/blob/main/case_interview/001-README.md#subject_019)
##### [20. 请叙述AMR基本码流结构？](https://github.com/0voice/audio_video_streaming/blob/main/case_interview/001-README.md#subject_020)
##### [21. 预测编码的基本原理是什么？](https://github.com/0voice/audio_video_streaming/blob/main/case_interview/001-README.md#subject_021)
##### [22. 说一说ffmpeg的数据结构？](https://github.com/0voice/audio_video_streaming/blob/main/case_interview/001-README.md#subject_022)
##### [23. 说一说AVFormatContext 和 AVInputFormat之间的关系？](https://github.com/0voice/audio_video_streaming/blob/main/case_interview/001-README.md#subject_023)
##### [24. 说一说AVFormatContext, AVStream和AVCodecContext之间的关系？](https://github.com/0voice/audio_video_streaming/blob/main/case_interview/001-README.md#subject_024)
##### [25. 说一说视频拼接处理步骤？（细节处理，比如分辨率大小不一，时间处理等等）](https://github.com/0voice/audio_video_streaming/blob/main/case_interview/001-README.md#subject_025)
##### [26. NV21如何转换成I420？](https://github.com/0voice/audio_video_streaming/blob/main/case_interview/001-README.md#subject_026)
##### [27. DTS与PTS共同点？](https://github.com/0voice/audio_video_streaming/blob/main/case_interview/001-README.md#subject_027)
##### [28. 影响视频清晰度的指标有哪些？](https://github.com/0voice/audio_video_streaming/blob/main/case_interview/001-README.md#subject_028)
##### [29. 编解码处理时遇到什么困难？](https://github.com/0voice/audio_video_streaming/blob/main/case_interview/001-README.md#subject_029)
##### [30. 如何秒开视频？什么是秒开视频？](https://github.com/0voice/audio_video_streaming/blob/main/case_interview/001-README.md#subject_030)
##### [31. 如何降低延迟？如何保证流畅性？如何解决卡顿？解决网络抖动？](https://github.com/0voice/audio_video_streaming/blob/main/case_interview/001-README.md#subject_031)
##### [32. 需要把网络上一段视频存储下来（比如作为mp4 ), 请实现并说出方法（第一个视频需要翻墙才能进）？](https://github.com/0voice/audio_video_streaming/blob/main/case_interview/001-README.md#subject_032)
##### [33. 需要把网络上一段语音存储下来（比如作为mp3 ), 请实现并说出方法？](https://github.com/0voice/audio_video_streaming/blob/main/case_interview/001-README.md#subject_033)
##### [34. 为什么要有YUV这种数据出来？（YUV相比RGB来说的优点）](https://github.com/0voice/audio_video_streaming/blob/main/case_interview/001-README.md#subject_034)
##### [35. H264/H265有什么区别？](https://github.com/0voice/audio_video_streaming/blob/main/case_interview/001-README.md#subject_035)
##### [36. 视频或者音频传输，你会选择TCP协议还是UDP协议？为什么？](https://github.com/0voice/audio_video_streaming/blob/main/case_interview/001-README.md#subject_036)
##### [37. 平时说的软解和硬解，具体是什么？](https://github.com/0voice/audio_video_streaming/blob/main/case_interview/001-README.md#subject_037)
##### [38. 何为直播？何为点播？](https://github.com/0voice/audio_video_streaming/blob/main/case_interview/001-README.md#subject_038)
##### [39. 简述推流、拉流的工作流程？](https://github.com/0voice/audio_video_streaming/blob/main/case_interview/001-README.md#subject_039)
##### [40. 如何在直播中I帧间隔设置、与帧率分辨率选定？](https://github.com/0voice/audio_video_streaming/blob/main/case_interview/001-README.md#subject_040)
##### [41. 直播推流中推I帧与推非I帧区别是什么？](https://github.com/0voice/audio_video_streaming/blob/main/case_interview/001-README.md#subject_041)
##### [42. 常见的直播协议有哪些？之间有什么区别？](https://github.com/0voice/audio_video_streaming/blob/main/case_interview/001-README.md#subject_042)
##### [43. 点播中常见的数据传输协议主要有哪些？](https://github.com/0voice/audio_video_streaming/blob/main/case_interview/001-README.md#subject_043)
##### [44. RTMP、HLS协议各自的默认端口号是？](https://github.com/0voice/audio_video_streaming/blob/main/case_interview/001-README.md#subject_044)
##### [45. 简述RTMP协议，如何封装RTMP包？](https://github.com/0voice/audio_video_streaming/blob/main/case_interview/001-README.md#subject_045)
##### [46. m3u8构成是？直播中m3u8、ts如何实时更新？](https://github.com/0voice/audio_video_streaming/blob/main/case_interview/001-README.md#subject_046)
##### [47. 何为音视频同步，音视频同步是什么标准？](https://github.com/0voice/audio_video_streaming/blob/main/case_interview/001-README.md#subject_047)
##### [48. 播放器暂停、快进快退、seek、逐帧、变速怎么实现？](https://github.com/0voice/audio_video_streaming/blob/main/case_interview/001-README.md#subject_048)
##### [49. 说说你平时在播放过程中做的优化工作？](https://github.com/0voice/audio_video_streaming/blob/main/case_interview/001-README.md#subject_049)
##### [50. 你研究过哪些具体的流媒体服务器，是否做过二次开发？](https://github.com/0voice/audio_video_streaming/blob/main/case_interview/001-README.md#subject_050)
##### [51. 什么是GOP?](https://github.com/0voice/audio_video_streaming/blob/main/case_interview/002-README.md#subject_051)
##### [52. 音频测试的测试点,音频时延如何测试?](https://github.com/0voice/audio_video_streaming/blob/main/case_interview/002-README.md#subject_052)
##### [53. 美颜的实现原理，具体实现步骤?](https://github.com/0voice/audio_video_streaming/blob/main/case_interview/002-README.md#subject_053)
##### [54. 如何直播APP抓包过来的文件，如何过滤上行，下行，总码率？](https://github.com/0voice/audio_video_streaming/blob/main/case_interview/002-README.md#subject_054)
##### [55. 如何测试一个美颜挂件？](https://github.com/0voice/audio_video_streaming/blob/main/case_interview/002-README.md#subject_055)
##### [56. 为什么要用FLV？](https://github.com/0voice/audio_video_streaming/blob/main/case_interview/002-README.md#subject_056)
##### [57. 如何测试一个美颜挂件？](https://github.com/0voice/audio_video_streaming/blob/main/case_interview/002-README.md#subject_057)
##### [58. 平常的视频格式？](https://github.com/0voice/audio_video_streaming/blob/main/case_interview/002-README.md#subject_058)
##### [59. 何为homebrew？你用它安装过什么？常用命令有哪些？](https://github.com/0voice/audio_video_streaming/blob/main/case_interview/002-README.md#subject_059)
##### [60. RTMP、HLS协议各自的默认端口号是？](https://github.com/0voice/audio_video_streaming/blob/main/case_interview/002-README.md#subject_060)

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


## 🖥️ ffmpeg常用命令

### `ffmpeg --help`大概分为6个部分，具体如下：

- ffmpeg信息查询部分
- 公共操作参数部分
- 文件主要操作参数部分
- 视频操作参数部分
- 音频操作参数部分
- 字母操作参数部分

### 查看支持的容器格式

```
# 封装和解封装
ffmpeg -formats
# 解封装
ffmpeg -demuxers
# 封装
ffmpeg -muxers
# 查看FLV封装器的参数支持
ffmpeg -h muxer=flv
# 查看FLV解封装器的参数支持
ffmpeg -h demuxer=flv
```

### 查看支持的编解码格式

```
# 编解码
ffmpeg -codecs
# 解码
ffmpeg -decoders
# 编码
ffmpeg -encoders
# 查看H.264(AVC)的编码参数支持
ffmpeg -h encoder=h264
# 查看H.264(AVC)的解码参数支持
ffmpeg -h decoder=h264
```

### 查看支持的滤镜

```
# 滤镜
ffmpeg -filters
# 查看colorkey滤镜的参数支持
ffmpeg -h filter=colorkey
```

### 转码

```
ffmpeg -i WMV9_1280x720.wmv -vcodec mpeg4 -b:v 200 -r 15 -an output.mp4 
# -i 文件 （后缀名）封装格式
# -vcodec 视频编码格式
# -b:v 视频码率
# -r 视频帧率
# -an 不包括音频
```

## ffprobe常用命令

### `-show_packets` 查看多媒体数据包信息

| 字段          | 说明                                         |
| :------------ | :------------------------------------------- |
| codec_type    | 多媒体类型，如视频包、音频包等               |
| stream_index  | 多媒体的stream索引                           |
| pts           | 多媒体的显示时间值                           |
| pts_time      | 根据不同格式计算过后的多媒体的显示时间       |
| dts           | 多媒体解码时间值                             |
| dts_time      | 根据不同格式计算过后的多媒体的解码时间       |
| duration      | 多媒体包占用的时间值                         |
| duration_time | 根据不同格式计算过后的多媒体包所占用的时间值 |
| size          | 多媒体包的大小                               |
| pos           | 多媒体包所在的文件偏移位置                   |
| flags         | 多媒体包标记，如关键包与非关键包的标记       |

### `-show_format` 查看多媒体的封装格式

| 字段             | 说明                 |
| :--------------- | :------------------- |
| filename         | 文件名               |
| nb_streams       | 媒体中包含的流的个数 |
| nb_programs      | 节目数               |
| format_name      | 使用的封装模块的名称 |
| format_long_name | 封装的完整名称       |
| start_time       | 媒体文件的起始时间   |
| duration         | 媒体文件的总时间长度 |
| size             | 媒体文件的大小       |
| bit_rate         | 媒体文件的码率       |

### `-show_frames` 查看视频文件中的帧信息

| 属性              | 说明                           | 值       |
| :---------------- | :----------------------------- | :------- |
| media_type        | 帧的类型（视频、音频、字幕等） | video    |
| stream_index      | 帧所在的索引区域               | 0        |
| key_frame         | 是否为关键帧                   | 1        |
| pkt_pts           | Frame包的pts                   | 0        |
| pkt_pts_time      | Frame包的pts的时间显示         | 0.080000 |
| pkt_dts           | Frame包的dts                   | 80       |
| pkt_dts_time      | Frame包的dts的时间显示         | 0.080000 |
| pkt_duration      | Frame包的时长                  | N/A      |
| pkt_duration_time | Frame包的时长时间显示          | N/A      |
| pkt_pos           | Frame包所在文件的偏移位置      | 344      |
| width             | 帧显示的宽度                   | 1280     |
| height            | 帧显示的高度                   | 714      |
| pix_fmt           | 帧的图像色彩格式               | yuv420p  |
| pict_type         | 帧类型                         | I        |

### `-show_streams` 查看多媒体文件中的流信息

| 属性             | 说明                              | 值             |
| :--------------- | :-------------------------------- | :------------- |
| index            | 流所在的索引区域                  | 0              |
| codec_name       | 编码名                            | h264           |
| codec_long_name  | 编码全名                          | MPEG-4 part 10 |
| profile          | 编码的profile                     | High           |
| level            | 编码的level                       | 31             |
| has_b_frames     | 包含B帧信息                       | 2              |
| codec_type       | 编码类型                          | video          |
| codec_time_base  | 编码的时间戳计算基础单位          | 1/50           |
| pix_fmt          | 图像显示的色彩格式                | yuv420p        |
| coded_width      | 图像的宽度                        | 1280           |
| coded_height     | 图像的高度                        | 714            |
| codec_tag_string | 编码的标签数据                    | [0][0][0][0]   |
| r_frame_rate     | 实际帧率                          | 25/1           |
| avg_frame_rate   | 平均帧率                          | 25/1           |
| time_base        | 时间基数（用来进行timestamp计算） | 1/1000         |
| bit_rate         | 码率                              | 200000         |
| max_bit_rate     | 最大码率                          | N/A            |
| nb_frames        | 帧数                              | N/A            |

### `-printf_format`或`-of` 格式化输出支持XML、INI、JSON、CSV、FLAT等

## ffplay

### 可视化

Visualize information exported by some codecs.

http://ffmpeg.org/ffmpeg-all.html#codecview

https://trac.ffmpeg.org/wiki/Debug/MacroblocksAndMotionVectors

```
ffmpeg -h filter=codecview
```

- Visualize forward predicted MVs of all frames using

  ```
  ffplay -flags2 +export_mvs input.mp4 -vf codecview=mv_type=fp
  ```

- Visualize multi-directionals MVs of P and B-Frames using

  ```
  ffplay -flags2 +export_mvs input.mp4 -vf codecview=mv=pf+bf+bb
  ```

## ffmpeg转封装格式

- 需要知道 **源容器** 和 **目标容器** 的可容纳的编码格式

- 编码格式如果相互兼容，可以用`-c copy`拷贝原有的stream

  `ffmpeg -i input.mp4 -c copy -f flv output.flv`

- 编码格式如果不兼容，需要转化成目标文件支持的编码

  `ffmpeg -i input_ac3.mp4 -vcodec copy -acodec aac -f flv output.flv`

### HLS

1. FFmpeg转HLS举例

   常规的从文件转换HLS直播时：

   ```
   ffmpeg -re -i input.mp4 -c copy -f hls -bsf:v h264_mp4toannexb output.m3u8
   # -bsf:v h264_mp4toannexb 作用是将MP4中的H.264数据转换成H.264 AnnexB标准编码，AnnexB标准的编码常见于实时传输流中
   ```

如果源文件为FLV、TS等可以作为直播传输流的视频，则不需要这个参数。

1. ffmpeg推流上传HLS相关的M3U8以及TS文件

   Nginx配置webdav模块

   ```
   ffmpeg -re -i input.mp4 -c copy -f hls -hls_time 3 -hls_list_size 0 -method PUT -t 30 http://127.0.0.1/test/output.m3u8
   ```

### 音视频文件音视频流抽取

1. FFmpeg抽取音视频文件中的AAC音频流
   `ffmpeg -i input.mp4 -vn -acodec copy output.aac`

2. FFmpeg抽取音视频文件中的H.264视频流
   `ffmpeg -i input.mp4 -vcodec copy -an output.h264`

3. FFmpeg抽取音视频文件中的H.265视频流(前提文件视频编码格式为hevc)

   ```
   ffmpeg -i input.mp4 -vcodec copy -an -bsf hevc_mp4toannexb -f hevc output.hevc
   ```

   ## ffmpeg转码

### h264转h265(HEVC)

```
ffmpeg -i input.mp4 -c:v libx265 -vtag hvc1 h265_output.mp4
```

### aac转MP3(需要安装libmp3lame)

```
ffmpeg -i AVC_high_1280x720_2013.mp4 -vn -acodec libmp3lame -f mp3 out.mp3
```

### x264

#### 安装

```
$git clone git://git.videolan.org/x264.git
$cd x264
$./configure –enable-shared 
$make
$sudo make install
```

#### 查看

```
x264 --full help
```

#### 设置编码参数

1. 编码器预设参数设置preset
   通常通过preset来设置编码的速度，影响清晰度
   `ffmpeg -i input.mp4 -vcodec libx264 -preset ultrafast -b:v 2000k output.mp4`

2. H.264编码优化参数tune
   在使用ffmpeg与x264进行H.264直播编码并进行推流时，只用tune参数的zerolatency将会提升效率，因为其降低了因编码导致的延迟。

3. H.264的profile与level设置
   baseline profile编码的H.264视频不会包含B Slice，而使用main profile、high profile编码出来的视频，均可以包含B Slice
   `ffmpeg -i input.mp4 -vcodec libx264 -profile:v baseline -level 3.1 -s 352x288 -an -y -t 10 output_baseline.ts`
   `ffmpeg -i input.mp4 -vcodec libx264 -profile:v high -level 3.1 -s 352x288 -an -y -t 10 output_high.ts`

   查看包含B帧的情况：

   `ffprobe -v quiet -show_frames -select_streams v output_baseline.ts | grep "pict_type=B" | wc -l`

   当进行实时流媒体直播时，采用baseline编码相对main或high的profile会更可靠些。

4. 控制场景切花关键帧插入参数 sc_threshold
   ffmpeg通过-g参数设置以帧数间隔为GOP的长度，但是当遇到场景切换时，例如从一个画面突然变成另一个画面时，会强行插入一个关键帧，这是GOP的间隔将会重新开始，可以通过使用sc_threshold参数进行设定以决定是否在场景切换时插入关键帧。
   ffmpeg命令控制编码时的GOP大小
   `ffmpeg -i AVC_high_1280x720_2013.mp4 -c:v libx264 -g 50 -t 60 output.mp4`
   为了使得GOP的插入更加均匀，使用参数 sc_threshold
   `ffmpeg -i AVC_high_1280x720_2013.mp4 -c:v libx264 -g 50 -sc_threshold 0 -t 60 -y output.mp4`

5. 设置x264内部参数x264opts
   去掉B帧
   `ffmpeg -i input.mp4 -c:v libx264 -x264opts "bframes=0" -g 50 -sc_threshold 0 output.mp4`
   控制I帧、P帧、B帧的频率与规律
   例如设置GOP中，每2个P帧之间存放3个B帧：
   `ffmpeg -i input.mp4 -c:v libx264 -x264opts "bframes=3:b-adapt=0" -g 50 -sc_threshold 0 output.mp4`

6. CBR 恒定码率设置参数 nal-hrd （固定码率好处，可能是网络传输）

   VBR：可变码率

   CBR：恒定码率

   ABR：平均码率。VBR和CBR混合产物。

   ```
   ffmpeg -i input.mp4 -c:v libx264 -x264opts "bframes=10:b-adapt=0" -b:v 1000k -maxrate 1000k -minrate 1000k -bufsize 50k -nal-hrd cbr -g 50 -sc_threshold 0 output.ts
   # 设置B帧的个数，并且是每2个P帧之间包含10个B帧
   # 设置视频码率为 1000 kbit/s
   # 设置最大码率为 1000 kbit/s
   # 设置最小码率为 1000 kbit/s
   # 设置编码的buffer大小为 50KB
   # 设置 H.264 的编码HRD信号形式为 CBR
   # 设置每50帧一个GOP
   # 设置场景切换不强行插入关键帧
   ```

#### MP3/AAC

1. MP3转码
   `ffmpeg -i INPUT -acodec libmp3lame output.mp3`

2. 参数控制

   ```
   # -q 控制码率(0~9) 高->低
   ffmpeg -i input.mp3 -acodec libmp3lame -q:a 8 output.mp3
   # -b 设置为CBR
   ffmpeg -i input.mp3 -acodec libmp3lame -b:a 64k output.mp3
   # -abr 设置为abr编码
   ffmpeg -i input.mp3 -acodec libmp3lame -b:a 64k -abr 1 output.mp3
   ```

## ffmpeg流媒体

### ffmpeg发布与录制RTMP流

FFmpeg操作RTMP的参数

| 参数           | 类型   | 说明                                                         |
| :------------- | :----- | :----------------------------------------------------------- |
| rtmp_app       | 字符串 | RTMP流发布点，又称为APP                                      |
| rtmp_buffer    | 整数   | 客户端buffer大小（单位：毫秒），默认为3秒                    |
| rtmp_conn      | 字符串 | 在RTMP的Connect命令中增加自定义AMF数据                       |
| rtmp_flashver  | 字符串 | 设置模拟的flashplugin的版本号                                |
| rtmp_live      | 整数   | 指定RTMP流媒体播放类型，具体如下： - any：直播或点播 - live：直播 - recorded：点播 |
| rtmp_pageurl   | 字符串 | RTMP在Connect命令中设置的PageURL字段，其为播放时所在的Web页面URL |
| rtmp_playpath  | 字符串 | RTMP流播放的Stream地址，或者成为**秘钥**，或者成为发布流     |
| rtmp_subscribe | 字符串 | 直播名称，默认设置为rtmp_playpath的值                        |
| rtmp_swfhash   | 二进制 | 解压swf文件后的SHA256的hash值                                |
| rtmp_swfsize   | 整数   | swf文件解压后的大小，用于swf认证                             |
| rtmp_swfurl    | 字符串 | RTMP的Connect命令中设置的swfURL播放器的URL                   |
| rtmp_swfverify | 字符串 | 设置swf认证时swf文件的URL地址                                |
| rtmp_tcurl     | 字符串 | RTMP的Connect命令中设置的tcURL目标发布点地址，一般形如 rtmp://xxx.xxx.xxx/app |
| rtmp_listen    | 整数   | 开启RTMP服务时所监听的端口                                   |
| listen         | 整数   | 与rtmp_listen相同                                            |
| timeout        | 整数   | 监听rtmp端口时设置的超时时间，以秒为单位                     |

1. rtmp_app、rtmp_playpath 参数

   通过rtmp_app、rtmp_playpath参数设置rtmp的推流发布点

   `ffmpeg -re -i AVC_high_1280x720_2013.mp4 -c copy -f flv -rtmp_app live -rtmp_playpath play rtmp://127.0.0.1`

   等价于

   `ffmpeg -re -i AVC_high_1280x720_2013.mp4 -c copy -f flv rtmp://127.0.0.1/live/play`

   ### ffmpeg录制RTSP流

FFmpeg操作RTSP的参数

| 参数                | 类型   | 说明                                                         |
| :------------------ | :----- | :----------------------------------------------------------- |
| initial_pause       | 布尔   | 建立连接后暂停播放                                           |
| rtsp_transport      | 标记   | 设置RTSP传输协议，具体如下： - udp：UDP - tcp：TCP -udp_multicast：UDP多播协议 - http：HTTP隧道 |
| rtsp_flags          | 标记   | RTSP使用标记，具体如下： - filter_src：只接收指定IP的流 - listen：设置为被动接收模式 - prefer_tcp：TCP亲和模式，如果TCP可用则首选TCP传输 |
| allowed_media_types | 标记   | 设置允许接收的数据模式（默认全部开启），具体如下： - video：只接收视频 - audio：只接收音频 - data：只接收数据 - subtitle：只接收字幕 |
| min_port            | 整数   | 设置最小本地UDP端口，默认为5000                              |
| max_port            | 整数   | 设置最大本地UDP端口，默认为65000                             |
| timeout             | 整数   | 设置监听端口超时时间                                         |
| reorder_queue_size  | 整数   | 设置录制数据Buffer的大小                                     |
| buffer_size         | 整数   | 设置底层传输包Buffer的大小                                   |
| user-agent          | 字符串 | 用户客户端标识                                               |

1. TCP方式录制RTSP直播流

   ffmpeg默认使用的rtsp拉流方式为UDP，为了避免丢包导致的花屏、绿屏、灰屏、马赛克等问题，将UDP改为TCP传输：

```
ffmpeg -rtsp_transport tcp -i rtsp://127.0.0.1/test.mkv -c copy -f mp4 output.mp4
```

1. User-Agent设置参数

   ```
   ffmpeg -user-agent "Alex-Player" -i rtsp://input:554/live/1/stream.sdp -c copy -f mp4 -u output.mp4
   ```

   ### FFmpeg录制HTTP流

FFmpeg操作HTTP的参数

| 参数              | 类型   | 说明                             |
| :---------------- | :----- | :------------------------------- |
| seekable          | 布尔   | 设置HTTP连接为可seek操作         |
| chunked_post      | 布尔   | 使用Chunked模式post数据          |
| http_proxy        | 字符串 | 设置HTTP代理传输数据             |
| headers           | 字符串 | 自定义HTTP Header数据            |
| content_type      | 字符串 | 设置POST的内容类型               |
| user_agent        | 字符串 | 设置HTTP请求客户端信息           |
| multiple_requests | 布尔   | HTTP长连接开启                   |
| post_data         | 二进制 | 设置将要POST的数据               |
| cookies           | 字符串 | 设置HTTP请求时写代码的Cookies    |
| icy               | 布尔   | 请求ICY源数据：默认开关          |
| auth_type         | 整数   | HTTP验证类型设置                 |
| offset            | 整数   | 初始化HTTP请求时的偏移位置       |
| method            | 字符串 | 发起HTTP请求时使用的HTTP的方法   |
| reconnect         | 布尔   | 在EOF之前断开发起重连            |
| reconnect_at_eof  | 布尔   | 在得到EOF时发起重连              |
| reply_code        | 整数   | 作为HTTP服务时向客户端反馈状态码 |

### FFmpeg录制和发布TCP与UDP流

略

### FFmpeg推多路流

1. 推流（tee协议输出多路流）
   `ffmpeg -re -i AVC_high_1280x720_2013.mp4 -vcodec libx264 -acodec aac -map 0 -f flv "tee:rtmp://127.0.0.1/live/p1|rtmp://127.0.0.1/live/p2"`
2. 验证
   `ffmpeg -i rtmp://127.0.0.1/live/p1 -i rtmp://127.0.0.1/live/p2`

## ffmpeg滤镜使用

http://ffmpeg.org/ffmpeg-filters.html

### FFmpeg滤镜Filter描述格式

1. FFmpeg滤镜Filter的参数排列方式
   [输入流或标记]滤镜参数[临时标记名];[输入流或标记]滤镜参数[临时标记名]…
   输入两个文件，一个视频，一个图片，将logo进行缩放，然后放在视频的左上角：

   ```
   ffmpeg -i input.mp4 -i input.jpg -filter_complex " [1:v] scale=176:144[logo];[0:v][logo]overlay=x=0:y=0" output.mp4
   # [0:v]/[1:v]代表第几个输入的视频
   ```

2. FFmpeg为视频加水印

- `drawtext`滤镜

  ```
  ffmpeg -h filter=drawtext
  # 文字水印
  ffmpeg -i input.mp4 -ss 50 -vf "drawtext=fontsize=100:fontfile=/usr/share/fonts/truetype/freefont/FreeSerif.ttf :text='Hello World':fontcolor='yellow':x=20:y=20" output.mp4
  # 动态日期
  ffmpeg -i input.mp4 -ss 50 -vf "drawtext=fontsize=100:fontfile=/usr/share/fonts/truetype/freefont/FreeSerif.ttf :text='%{localtime\:%Y\-%m\-%d %H-%M-%S}':fontcolor='yellow':x=20:y=20" output.mp4
  # 闪烁
  ffmpeg -i input.mp4 -ss 50 -vf "drawtext=fontsize=100:fontfile=/usr/share/fonts/truetype/freefont/FreeSerif.ttf :text='%{localtime\:%Y\-%m\-%d %H-%M-%S}':fontcolor='yellow':x=20:y=20:enable=lt(mod(t\,3)\,1)" output.mp4
  ```

- `movie`滤镜

  ```
  # 图片水印
  ffmpeg -i input.mp4 -vf "movie=logo.png[wm];[in][wm]overlay=30:10[out]" output.mp4
  # colorkey 半透明
  ffmpeg -i input.mp4 -ss 55 -vf "movie=../picture/3d_data.png,colorkey=black:1.0:0.1[wm];[in][wm]overlay=30:10[out]" output.mp4
  ```

- `overlay`滤镜

  ```
  # 画中画
  ffmpeg -re -i input.mp4 -vf "movie=sub.mp4,scale=480x320[test];[in][test]overlay[out]" -vcodec libx264 output.flv
  # 跑马灯
  ffmpeg -re -i input.mp4 -vf "movie=sub.wmv,scale=480x320[test];[in][test]overlay=x='if(gte(t,2), -w+(t-2)*50, NAN)':y=0[out]" -vcodec libx264 output.flv
  # 视频多宫格处理
  ffmpeg -i input1.mp4 -i input2.mp4 -i input3.mp4 -i input4.mp4 -filter_complex "
  nullsrc=size=1280x720 [background];
  [0:v] setpts=PTS-STARTPTS, scale=640x360 [upleft];
  [1:v] setpts=PTS-STARTPTS, scale=640x360 [upright];
  [2:v] setpts=PTS-STARTPTS, scale=640x360 [downleft];
  [3:v] setpts=PTS-STARTPTS, scale=640x360 [downright];
  [background][upleft] overlay=shortest=1 [background+upleft];
  [background+upleft][upright] overlay=shortest=1:x=640 [background+up];
  [background+up][downleft] overlay=shortest=1:y=360 [background+up+downleft];
  [background+up+downleft][downright] overlay=shortest=1:x=640:y=360
  " output.mp4
  ```

### FFmpeg音频流滤镜操作

1. 双声道合并单声道
   `fmpeg -i input.mp3 -ac 1 output.mp3`

2. 双声道提取

   - map_channel
     `ffmpeg -i input.mp3 -map_channel 0.0.0 left.mp3 -map_channel 0.0.1 right.mp3`

   - pan
     `ffmpeg -i input.mp3 -filter_complex "[0:0]pan=1c|c0=c0[left];[0:0]pan=1c|c0=c1[right]" -map "[left]" left.mp3 -map "[right]" right.mp3`

1. 双声道转双音频流

   ```
   ffmpeg -i input.mp4 -filter_complex channelsplit=channel_layout=stereo output.mka
   ffprobe output.mka
   # 可以看到有两个stream
   ```

不常用，大多数播放器也只会播放第一个流

1. 单声道转双声道
   `ffmpeg -i left.aac -ac 2 output.m4a`
   这样的双声道并不是真正的双声道，而是单声道处理成的多声道，效果不会比原来多声道效果好

2. 两个音频源合并双声道
   `ffmpeg -i left.mp3 -i right.mp3 -filter_complex "[0:a][1:a]amerge=inputs=2[aout]" -map "[aout]" output.mka`

3. 多个音频合并为多声道

   ```
   ffmpeg -i front_left.wav -i front_right.wav -i front_center.wav -i lfe.wav -i back_left.wav -i back_right.wav -filter_complex "[0:a][1:a][2:a][3:a][4:a][5:a]amerge=inputs=6[aout]" -map "[aout]" output.wav
   ```

   ### FFmpeg音频音量探测

4. 音频音量获得
   `ffmpeg -i input.wav -filter_complex volumedetect -f null -`

5. 绘制音频波形

   ```
   ffmpeg -i input.wav -filter_complex "showwavespic=s=640x120" -frames:v 1 output.png
   # 不通声道的波形图
   ffmpeg -i input.wav -filter_complex "showwavespic=s=640x120:split_channels=1" -frames:v 1 output.png
   ```

### FFmpeg为视频加字母

1. ASS字母流写入视频流
   `ffmpeg -i input.mp4 -vf ass=t1.ass -f mp4 output.mp4`

2. ASS字母流写入封装容器

   ```
   ffmpeg -i input.mp4 -vf ass=t1.ass -acodec copy -vcodec copy -scodec copy output.mp4
   # 输入的视频文件汇总原本同样带有字幕流，希望使用t1.ass字幕流，通过map写入
   # 下面命令会分别将第一个输入文件的第一个流和第二个流与第二个输入文件的第一个流写入output.mkv
   ffmpeg -i input.mp4 -i t1.ass -map 0:0 -map 0:1 -map 1:0 -acodec copy -vcodec copy -scodec copy output.mkv
   ```

### FFmpeg视频抠图合并

1. chromakey 抠图和背景视频合并的操作

   ```
   # 查询颜色支持
   ffmpeg -colors
   # chromakey滤镜将绿色背景中的人物抠出来，贴到input.mp4为背景的视频中
   ffmpeg -i input.mp4 -i input_green.mp4 -filter_complex "[1:v]chromakey=Green:0.1:0.2[ckout];[0:v][ckout]overlay[out]" -map "[out]" output.mp4
   # FFmpeg中除了有chromakey滤镜外，还有colorkey参数，chromakey滤镜主要用于YUV数据，所以一般来说做绿幕处理更有优势；而colorkey处理纯色均可以，因为colorkey主要用于RGB数据。
   ```

### FFmpeg 3D视频处理

- ```
  stereo3d
  ```

  滤镜

  ```
  # 黄蓝
  ffplay -vf "stereo3d=sbsl:aybd" AVC_high_1280x720_2013.mp4
  # 红蓝
  ffplay -vf "stereo3d=sbsl:aybg" AVC_high_1280x720_2013.mp4
  ```

### FFmpeg定时视频截图

- `vframe`参数截取一张图片
  `ffmpeg -i input.flv -ss 00:00:7.435 -vframes 1 output.png`

- ```
  fps
  ```

  滤镜定时获得图片

  ```
  # 每隔1秒钟生成一张PNG图片
  ffmpeg -i input.flv -vf fps=1 out%d.png
  # 每隔一封中生成一张jpg图片
  ffmpeg -i input.flv -vf fps=1/60 out%d.jpg
  # select 按照关键帧截取图片
  ffmpeg -i input.flv -vf "select='eq(pict_type,PICT_TYPE_I)'" -vsync vfr thumb%04d.png
  ```

### FFmpeg 生成测试源数据

1. 音频测试流
   lavfi 模拟音频源的abuffer、aevalsrc、anullsrc、flite、anoisesrc、sine滤镜生成音频流

   ```
   # 白噪声
   ffmpeg -re -f lavfi -i aevalsrc="-2+random(0)" -t 5 output.mp3
   # 正弦波
   ffmpeg -re -f lavfi -i "sine" -t 5 output.mp3
   ```

2. 视频测试流
   通过FFmpeg模拟多种视频源：allrgb、allyuv、color、haldclutsrc、nullsrc、rgbtestsrc、smptebars、smptehdbars、testsrc、testsrc2、yuvtestsrc

   ```
   # 生成长度为5.3秒、图像大小为QCIF分辨率、帧率为25fps的视频图像数据，并编码成H.264
   ffmpeg -re -f lavfi -i testsrc=duration=5.3:size=qcif:rate=25 -vcodec libx264 -r:v 25 output.mp4
   # 纯红
   ffmpeg -re -f lavfi -i color=c=red@0.2:s=qcif:r=25 -vcodec libx264 -r:v 25 output.mp4
   # 随机雪花
   ffmpeg -re -f lavfi -i "nullsrc=s=256x256,geq=random(1)*255:128:128" -vcodec libx264 -r:v 25 output.mp4
   ```

### FFmpeg对音视频倍速处理

1. `atempo`音频倍速处理
   取值范围：0.5 ~ 2.0

   ```
   # 半速处理
   ffmpeg -i input.wav -filter_complex "atempo=tempo=0.5" -acodec aac output.aac
   ```

2. `setpts`视频倍速处理
   使用PTS控制播放速度的

   ```
   # 半速处理
   ffmpeg -re -i input.mp4 -filter_complex "setpts=PTS*2" output.mp4
   ```

## ffmpeg采集设备

1. Linux下查看设备列表
   `ffmpeg -h demuxer=fbdev`

2. Linux采集设备fbdev
   FrameBuffer是一个比较有年份的设备，专门用于图像展示操作，早期的图形界面也是基于FrameBuffer进行绘制的，有时在向外界展示Linux的命令行操作又不希望别人看到你的桌面时，可以通过获取FrameBuffer设备图像数据进行编码后推流或录制：

   ```
   ffmpeg -framerate 30 -f fbdev -i /dev/fb0 output.mp4
   # ctrl+alt+F1 进入命令行界面
   # ctrl+alt+F7 进入图形界面
   ```

3. Linux采集设备v4l2
   v4l2主要用来采集摄像头，而摄像头通常支持多种像素格式，有些摄像头还支持直接输出已经编码好的H.264数据

   - 查看参数
     `ffmpeg -h demuxer=v4l2`

   - 查看v4l2摄像头锁支持的色彩格式及分辨率
     `ffmpeg -hide_banner -f v4l2 -list_formats all -i /dev/vide0`

   - 采集摄像头
     `ffmpeg -hide_banner -s 1920x1080 -i /dev/vide0 output.avi`

1. Linux采集设备x11grab

   Linux下面采集桌面图像时，通常采用x11grab设备采集图像，输入设备的设备名规则：

   [主机名]: 显示编号id.屏幕编号id+起始x轴,起始y轴

   ```
   # 桌面录制(帧率:25,图像分辨率:1366x768,采集的设备:0.0)
   ffmpeg -f x11grab -framerate 25 -video_size 1366x768 -i :0.0 out.mp4
   # 桌面录制指定起始位置(:0.0+300,200 指定了x坐标300,y坐标200)
   # 注意:video_size不要超过实际采集区域的大小
   ffmpeg -f x11grab -framerate 25 -video_size 352x288 -i :0.0+300,200 out.mp4
   # 桌面录制带鼠标记录的视频
   ffmpeg -f x11grab -video_size 1366x768 -follow_mouse 1 -i :0.0 out.mp4
   ```

## 其他

### x265安装

1. 下载
   网站1：http://www.videolan.org/developers/x265.html
   `hg clone http://hg.videolan.org/x265`
   网站2：https://bitbucket.org/multicoreware/x265
   `hg clone https://bitbucket.org/multicoreware/x265`

2. 编译

   ```
   sudo apt-get install mercurial cmake cmake-curses-gui build-essential yasm
   cd x265/build/linux
   ./make-Makefiles.bash
   make
   sudo make install
   ```

### DTS、PTS 的概念

DTS、PTS 的概念如下所述：

- DTS（Decoding Time Stamp）：即解码时间戳，这个时间戳的意义在于告诉播放器该在什么时候解码这一帧的数据。
- PTS（Presentation Time Stamp）：即显示时间戳，这个时间戳用来告诉播放器该在什么时候显示这一帧的数据。

需要注意的是：虽然 DTS、PTS 是用于指导播放端的行为，但它们是在编码的时候由编码器生成的。

当视频流中没有 B 帧时，通常 DTS 和 PTS 的顺序是一致的。但如果有 B 帧时，就回到了我们前面说的问题：解码顺序和播放顺序不一致了。

比如一个视频中，帧的显示顺序是：I B B P，现在我们需要在解码 B 帧时知道 P 帧中信息，因此这几帧在视频流中的顺序可能是：I P B B，这时候就体现出每帧都有 DTS 和 PTS 的作用了。DTS 告诉我们该按什么顺序解码这几帧图像，PTS 告诉我们该按什么顺序显示这几帧图像。顺序大概如下：

```
   PTS: 1 4 2 3
   DTS: 1 2 3 4
Stream: I P B B
```

### 其他常用命令

1、将文件当作源推送到RTMP服务器

```
ffmpeg -re -i localFile.mp4 -c copy -f flv rtmp://server/live/streamName
```

参数解释
-r 以本地帧频读数据，主要用于模拟捕获设备。表示ffmpeg将按照帧率发送数据，不会按照最高的效率发送

2、将直播文件保存至本地

```
ffmpeg -i rtmp://server/live/streamName -c copy dump.flv
```

3、将其中一个直播流中的视频改用H.264压缩，音频不变，推送到另外一个直播服务器

```
ffmpeg -i rtmp://server/live/originalStream -c:a copy -c:v libx264 -vpre slow -f flv rtmp://server/live/h264Stream
```

4、将其中一个直播流中的视频改用H.264压缩，音频改用aac压缩，推送到另外一个直播服务器

```
ffmpeg -i rtmp://server/live/originalStream -c:a libfaac -ar 44100 -ab 48k -c:v libx264 -vpre slow -vpre baseline -f flv rtmp://server/live/h264Stream
```

5、将其中一个直播流中的视频不变，音频改用aac压缩，推送到另外一个直播服务器

```
ffmpeg -i rtmp://server/live/originalStream -acodec libfaac -ar 44100 -ab 48k -vcodec copy -f flv rtmp://server/live/h264_AAC_Stream
```

6、将一个高清流复制为几个不同清晰度的流重新发布，其中音频不变

```
ffmpeg -re -i rtmp://server/live/high_FMLE_stream -acodec copy -vcodec x264lib -s 640×360 -b 500k -vpre medium -vpre baseline rtmp://server/live/baseline_500k -acodec copy -vcodec x264lib -s 480×272 -b 300k -vpre medium -vpre baseline rtmp://server/live/baseline_300k -acodec copy -vcodec x264lib -s 320×200 -b 150k -vpre medium -vpre baseline rtmp://server/live/baseline_150k -acodec libfaac -vn -ab 48k rtmp://server/live/audio_only_AAC_48k
```

7、将当前摄像头以及扬声器通过DSHOW采集，使用H.264/AAC压缩后推送到RTMP服务器

```
ffmpeg -r 25 -f dshow -s 640×480 -i video=”video source name”:audio=”audio source name” -vcodec libx264 -b 600k -vpre slow -acodec libfaac -ab 128k -f flv rtmp://server/application/stream_name
```

8、将一个JPG图片经过H.264压缩后输出为MP4文件

```
ffmpeg -i INPUT.jpg -an -vcodec libx264 -coder 1 -flags +loop -cmp +chroma -subq 10 -qcomp 0.6 -qmin 10 -qmax 51 -qdiff 4 -flags2 +dct8x8 -trellis 2 -partitions +parti8x8+parti4x4 -crf 24 -threads 0 -r 25 -g 25 -y OUTPUT.mp4
```

9、将MP3转化为AAC

```
ffmpeg -i 20120814164324_205.wav -acodec  libfaac -ab 64k -ar 44100  output.aac
```

10、将AAC文件转化为flv文件，编码格式采用AAC

```
ffmpeg -i output.aac -acodec libfaac -y -ab 32 -ar 44100 -qscale 10 -s 640*480 -r 15 outp
```

