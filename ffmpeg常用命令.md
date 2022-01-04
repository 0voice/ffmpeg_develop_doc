# ffmpeg常用命令

## `ffmpeg --help`大概分为6个部分，具体如下：

- ffmpeg信息查询部分
- 公共操作参数部分
- 文件主要操作参数部分
- 视频操作参数部分
- 音频操作参数部分
- 字母操作参数部分

## 查看支持的容器格式

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

## 查看支持的编解码格式

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

## 查看支持的滤镜

```
# 滤镜
ffmpeg -filters
# 查看colorkey滤镜的参数支持
ffmpeg -h filter=colorkey
```

## 转码

```
ffmpeg -i WMV9_1280x720.wmv -vcodec mpeg4 -b:v 200 -r 15 -an output.mp4 
# -i 文件 （后缀名）封装格式
# -vcodec 视频编码格式
# -b:v 视频码率
# -r 视频帧率
# -an 不包括音频
```

# ffprobe常用命令

## `-show_packets` 查看多媒体数据包信息

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

## `-show_format` 查看多媒体的封装格式

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

## `-show_frames` 查看视频文件中的帧信息

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

## `-show_streams` 查看多媒体文件中的流信息

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

## `-printf_format`或`-of` 格式化输出支持XML、INI、JSON、CSV、FLAT等

# ffplay

## 可视化

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

# ffmpeg转封装格式

- 需要知道 **源容器** 和 **目标容器** 的可容纳的编码格式

- 编码格式如果相互兼容，可以用`-c copy`拷贝原有的stream

  `ffmpeg -i input.mp4 -c copy -f flv output.flv`

- 编码格式如果不兼容，需要转化成目标文件支持的编码

  `ffmpeg -i input_ac3.mp4 -vcodec copy -acodec aac -f flv output.flv`

## HLS

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

## 音视频文件音视频流抽取

1. FFmpeg抽取音视频文件中的AAC音频流
   `ffmpeg -i input.mp4 -vn -acodec copy output.aac`

2. FFmpeg抽取音视频文件中的H.264视频流
   `ffmpeg -i input.mp4 -vcodec copy -an output.h264`

3. FFmpeg抽取音视频文件中的H.265视频流(前提文件视频编码格式为hevc)

   ```
   ffmpeg -i input.mp4 -vcodec copy -an -bsf hevc_mp4toannexb -f hevc output.hevc
   ```

   # ffmpeg转码

## h264转h265(HEVC)

```
ffmpeg -i input.mp4 -c:v libx265 -vtag hvc1 h265_output.mp4
```

## aac转MP3(需要安装libmp3lame)

```
ffmpeg -i AVC_high_1280x720_2013.mp4 -vn -acodec libmp3lame -f mp3 out.mp3
```

## x264

### 安装

```
$git clone git://git.videolan.org/x264.git
$cd x264
$./configure –enable-shared 
$make
$sudo make install
```

### 查看

```
x264 --full help
```

### 设置编码参数

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

## MP3/AAC

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

# ffmpeg流媒体

## ffmpeg发布与录制RTMP流

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

   ## ffmpeg录制RTSP流

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

   ## FFmpeg录制HTTP流

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

## FFmpeg录制和发布TCP与UDP流

略

## FFmpeg推多路流

1. 推流（tee协议输出多路流）
   `ffmpeg -re -i AVC_high_1280x720_2013.mp4 -vcodec libx264 -acodec aac -map 0 -f flv "tee:rtmp://127.0.0.1/live/p1|rtmp://127.0.0.1/live/p2"`
2. 验证
   `ffmpeg -i rtmp://127.0.0.1/live/p1 -i rtmp://127.0.0.1/live/p2`

# ffmpeg滤镜使用

http://ffmpeg.org/ffmpeg-filters.html

## FFmpeg滤镜Filter描述格式

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

## FFmpeg音频流滤镜操作

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

   ## FFmpeg音频音量探测

4. 音频音量获得
   `ffmpeg -i input.wav -filter_complex volumedetect -f null -`

5. 绘制音频波形

   ```
   ffmpeg -i input.wav -filter_complex "showwavespic=s=640x120" -frames:v 1 output.png
   # 不通声道的波形图
   ffmpeg -i input.wav -filter_complex "showwavespic=s=640x120:split_channels=1" -frames:v 1 output.png
   ```

## FFmpeg为视频加字母

1. ASS字母流写入视频流
   `ffmpeg -i input.mp4 -vf ass=t1.ass -f mp4 output.mp4`

2. ASS字母流写入封装容器

   ```
   ffmpeg -i input.mp4 -vf ass=t1.ass -acodec copy -vcodec copy -scodec copy output.mp4
   # 输入的视频文件汇总原本同样带有字幕流，希望使用t1.ass字幕流，通过map写入
   # 下面命令会分别将第一个输入文件的第一个流和第二个流与第二个输入文件的第一个流写入output.mkv
   ffmpeg -i input.mp4 -i t1.ass -map 0:0 -map 0:1 -map 1:0 -acodec copy -vcodec copy -scodec copy output.mkv
   ```

## FFmpeg视频抠图合并

1. chromakey 抠图和背景视频合并的操作

   ```
   # 查询颜色支持
   ffmpeg -colors
   # chromakey滤镜将绿色背景中的人物抠出来，贴到input.mp4为背景的视频中
   ffmpeg -i input.mp4 -i input_green.mp4 -filter_complex "[1:v]chromakey=Green:0.1:0.2[ckout];[0:v][ckout]overlay[out]" -map "[out]" output.mp4
   # FFmpeg中除了有chromakey滤镜外，还有colorkey参数，chromakey滤镜主要用于YUV数据，所以一般来说做绿幕处理更有优势；而colorkey处理纯色均可以，因为colorkey主要用于RGB数据。
   ```

## FFmpeg 3D视频处理

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

## FFmpeg定时视频截图

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

## FFmpeg 生成测试源数据

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

## FFmpeg对音视频倍速处理

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

# ffmpeg采集设备

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

# 其他

## x265安装

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

## DTS、PTS 的概念

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

## 其他常用命令

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

