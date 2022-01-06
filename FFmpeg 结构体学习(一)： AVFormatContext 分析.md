在 FFmpeg 学习(六)：FFmpeg 核心模块 libavformat 与 libavcodec 分析 中，我们分析了FFmpeg中最重要的两个模块以及重要的结构体之间的关系。

后面的文章，我们先不去继续了解其他模块，先针对在之前的学习中接触到的结构体进行分析，然后在根据功能源码，继续了解FFmpeg。

**AVFormatContext是包含码流参数较多的结构体。本文将会详细分析一下该结构体里每个变量的含义和作用。**

# 一、源码整理

首先我们先看一下结构体AVFormatContext的定义的结构体源码(位于libavformat/avformat.h，本人已经将相关注释翻译成中文，方便大家理解)：

View Code

# 二、AVForamtContext 重点字段

在使用FFMPEG进行开发的时候，AVFormatContext是一个贯穿始终的数据结构，很多函数都要用到它作为参数。它是FFMPEG解封装（flv，mp4，rmvb，avi）功能的结构体。下面看几个主要变量的作用（在这里考虑解码的情况）：



```
struct AVInputFormat *iformat：输入数据的封装格式
AVIOContext *pb：输入数据的缓存
unsigned int nb_streams：视音频流的个数
AVStream **streams：视音频流
char filename[1024]：文件名
int64_t duration：时长（单位：微秒us，转换为秒需要除以1000000）
int bit_rate：比特率（单位bps，转换为kbps需要除以1000）
AVDictionary *metadata：元数据
```



视频的时长可以转换成HH:MM:SS的形式，示例代码如下：



```
AVFormatContext *pFormatCtx;
CString timelong;
...
//duration是以微秒为单位
//转换成hh:mm:ss形式
int tns, thh, tmm, tss;
tns  = (pFormatCtx->duration)/1000000;
thh  = tns / 3600;
tmm  = (tns % 3600) / 60;
tss  = (tns % 60);
timelong.Format("%02d:%02d:%02d",thh,tmm,tss);
```



视频的原数据（metadata）信息可以通过AVDictionary获取。元数据存储在AVDictionaryEntry结构体中，如下所示：

```
typedef struct AVDictionaryEntry {
    char *key;
    char *value;
} AVDictionaryEntry;
```

每一条元数据分为key和value两个属性。

在ffmpeg中通过av_dict_get()函数获得视频的原数据。

下列代码显示了获取元数据并存入meta字符串变量的过程，注意每一条key和value之间有一个"\t:"，value之后有一个"\r\n"



```
//MetaData------------------------------------------------------------
//从AVDictionary获得
//需要用到AVDictionaryEntry对象
//CString author,copyright,description;
CString meta=NULL,key,value;
AVDictionaryEntry *m = NULL;
//不用一个一个找出来
/*    m=av_dict_get(pFormatCtx->metadata,"author",m,0);
author.Format("作者：%s",m->value);
m=av_dict_get(pFormatCtx->metadata,"copyright",m,0);
copyright.Format("版权：%s",m->value);
m=av_dict_get(pFormatCtx->metadata,"description",m,0);
description.Format("描述：%s",m->value);
*/
//使用循环读出
//(需要读取的数据，字段名称，前一条字段（循环时使用），参数)
while(m=av_dict_get(pFormatCtx->metadata,"",m,AV_DICT_IGNORE_SUFFIX)){
    key.Format(m->key);
    value.Format(m->value);
    meta+=key+"\t:"+value+"\r\n" ;
}
```

