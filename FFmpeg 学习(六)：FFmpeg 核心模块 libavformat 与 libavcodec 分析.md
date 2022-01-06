# 一、libavformat介绍

libavformat的主要组成与层次调用关系如下图：

![image](https://user-images.githubusercontent.com/87457873/148345134-91ce7724-18ec-4b1b-823b-12238e9c7a31.png)

AVFromatContext是API层直接接触到的结构体，它会进行格式的封装和解封装，它的数据部分由底层提供，底层使用了AVIOContext，这个AVIOContext实际上就是为普通的I/O增加了一层Buffer缓冲区，再往底层就是URLContext，也就是达到了协议层，协议层的实现由很多，如rtmp、http、hls、file等，这个就是libavformat的内部封装结构了。

# 二、libavcodec介绍

libavcodec模块的主要组成和数据结构图如下：

 ![img](https://images2018.cnblogs.com/blog/682616/201807/682616-20180720180926182-1853199081.png)

对于开发者来说，这个模块我们能接触到的最顶层的数据结构就是AVCodecContext，该结构体包含的就是与实际的编解码有关的部分。

首先AVCodecContext是包含在一个AVStream里面的，即描述了这路流的编码格式是什么，然后利用该编码器或者解码器进行AVPacket与AVFrame之间的转换（实际上就是编码或者解码的过程），这是FFmpeg中最重要的一部分。

