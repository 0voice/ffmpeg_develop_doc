> 本文转载自博客园：https://www.cnblogs.com/renhui/p/8458150.html

# 一、安装ffmpeg

分为两种安装方式：

### 1. 命令行安装

```
brew install ffmpeg
```

### 2. 下载压缩包安装

去 http://evermeet.cx/ffmpeg/ 下载7z压缩包，解压缩后，将ffmpeg文件拷贝到一个地方，然后在bash_profile里面配置好环境变量

# 二、安装ffplay

分为两种安装方式：

### 1. 命令行安装

执行下面的命令就可以进行安装操作

```
brew install ffmpeg --with-ffplay
```

> - 注：目前使用此安装方式安装后，执行ffplay会出现command not found的问题，可能是因为SDL的配置问题导致的。

### 2. 下载压缩包安装

去 http://evermeet.cx/ffmpeg/ 下载7z压缩包，解压缩后，将ffplay文件拷贝到一个地方，然后在bash_profile里面配置好环境变量

# 三、附言

在上面我们接触到了命令行安装ffmpeg的方法，除了安装选项 --with-ffplay外还有更多的选项如下：

```xml
–with-fdk-aac  (Enable the Fraunhofer FDK AAC library)
–with-ffplay  (Enable FFplay media player)
–with-freetype  (Build with freetype support)
–with-frei0r  (Build with frei0r support)
–with-libass  (Enable ASS/SSA subtitle format)
–with-libcaca  (Build with libcaca support)
–with-libvo-aacenc  (Enable VisualOn AAC encoder)
–with-libvorbis  (Build with libvorbis support)
–with-libvpx  (Build with libvpx support)
–with-opencore-amr  (Build with opencore-amr support)
–with-openjpeg  (Enable JPEG 2000 image format)
–with-openssl  (Enable SSL support)
–with-opus  (Build with opus support)
–with-rtmpdump  (Enable RTMP protocol)
–with-schroedinger  (Enable Dirac video format)
–with-speex  (Build with speex support)
–with-theora  (Build with theora support)
–with-tools  (Enable additional FFmpeg tools)
–without-faac  (Build without faac support)
–without-lame  (Disable MP3 encoder)
–without-x264  (Disable H.264 encoder)
–without-xvid  (Disable Xvid MPEG-4 video encoder)
–devel  (install development version 2.1.1)
–HEAD  (install HEAD version)
```

