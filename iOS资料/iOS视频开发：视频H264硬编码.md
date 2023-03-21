# iOS视频开发：视频H264硬编码

## 1、前言

前面我们已经介绍了在iOS开发中如果调用摄像头进行视频数据的采集和编解码。但折腾了这么多，对于YUV这玩意儿还是不是特别理解。其实在我的个人实践过程中我也一直搞不懂这个YUV，一顿恶补之后，我们来通俗一点地讲YUV这个数据格式。

1、YUV & RGB概述
 2、YUV的采样方式
 3、YUV的储存方式及常见格式
 4、YUV数据量计算
 5、YUV裁剪

## 2、YUV & RGB概述

RGB色彩模式是工业界的一种颜色标准，我们知道三原色（RGB）通过互相叠加可以得到各式各样的颜色，是目前运用最广的颜色系统之一。
 与我们熟知的RGB一样，YUV也是一种颜色编码方法，主要运用在电视系统及模拟视频领域。YUV的原理是把亮度和色度分离，利用人眼对亮度的敏感度超过色度这个特性，我们偷摸摸把色度信息减少一些，人眼也很难察觉到。甚至没有色度信息（UV分量）依旧可以显示完整的图像，只不过是黑白的。这样的设计就很好地解决了彩色电视机跟黑白电视机的兼容问题。并且由于可以减少一些色度信息，YUV数据的总尺寸相对RGB数据要小一些。YUV这三个字母中，Y表示亮度（灰度值），U和V表示色度（色彩及饱和度）。YUV是编译true-color颜色空间（colorspace）的种类，Y'UV,YUV,[YCbCr](https://baike.baidu.com/item/YCbCr)，[YPbPr](https://baike.baidu.com/item/YPbPr)等专有名词都可以称为YUV。

## 3、YUV的采样方式

YUV的主流采样方式有：`YUV4:4:4`、`YUV4:2:2`、`YUV4:2:0`，这是个什么意思呢？我们看下面这张图，黑色实心的点表示Y分量，黑色空心的点表示UV分量，那么下图的意思就是：

`YUV4:4:4` 每一个Y分量都有对应一组UV分量
 `YUV4:2:2` 每两个Y分量共用一组UV分量
 `YUV4:2:0` 每四个Y分量共用一组UV分量

![img](https:////upload-images.jianshu.io/upload_images/7682685-0a3c6e7b455ae850.jpg?imageMogr2/auto-orient/strip|imageView2/2/w/930/format/webp)

YUV采样示意图.jpg


 上图我们可以看到：
`YUV4:4:4`：也就是说每4个Y采样，就有相对应的4个U和4个V采样，即画面中每个像素都有Y分量和UV分量，这种格式储存了图像所有的亮度和色度信息。
`YUV4:2:2`：也就是说每4个Y采样，就有相对应的2个U和2个V采样。它的色度信号的扫描线（也就是上面图里的横线）和亮度信号一样多，但每条扫描线上的色度采样点只有亮度信号的一半（看4:2:2图一根横线上，空心圆只有实心圆的一半）。当4:2:2信号被解码的时候，“缺失”的色度采样通常由一定的内插补点算法通过它两侧的色度信息运算补充。
`YUV4:2:0`按字面意思应该4个Y采样，就有相对应的2个U和0个V采样，**事实上并不是这样的**，事实上，4:2:0的意思是，色度采样在每条横向扫描线上只有亮度采样的一半，扫描线的条数上，也只有亮度的一半！（看上面4:2:0这个图，空心是上下共用的，如果放到第一根线上，那就是空心只有实心的一半。再看前4根线，4根亮度线对应两行色度，也就是说空心的扫描线也只有实心的一半。）换句话说，无论是横向还是纵向， 色度信号的分辨率都只有亮度信号的一半。说得再通俗一些就是，如果第一行是4:2:0，那么第二行就是4:0:2，第三行就是4:2:0这样交替。举个例子，如果整张画面的尺寸是720x480，那么亮度信号是720x480，色度信号只有360x240。

## 4、YUV的储存方式

YUV的储存格式有两大类：
 `planar`：先连续存储所有像素点的Y，紧接着存储所有像素点的U，随后是所有像素点的V。
 `packed`：每个像素点的Y、U、V是连续交替存储的

### 4.1 YUYV(YUY2)格式（属于YUV422）

![img](https:////upload-images.jianshu.io/upload_images/7682685-4e02972d7d289899.png?imageMogr2/auto-orient/strip|imageView2/2/w/623/format/webp)

`YUYV`格式为像素保留Y，而UV在水平空间上相隔二个像素采样一次（Y0U0Y1V0），（Y2U2Y3V2）…其中，（Y0U0Y1V0）就是一个macro-pixel（宏像素），它表示了2个像素，（Y2U2Y3V2）是另外的2个像素。以此类推。

### 4.2 YVYU、UYVY格式（属于YUV422）

跟YUY2类似，只不过是排列顺序不一样而已。
 例如UYVY格式，那么排列顺序就是(U0Y0V0Y1)、(U2Y2V2Y3)...

### 4.3 YUV422P格式（属于YUV422）

`YUV422P`也属于YUV422的一种，但它并不是将YUV数据交错存储，而是先存放所有的Y分量，然后存储所有的U（Cb）分量，最后存储所有的V（Cr）分量，其每一个像素点的YUV值提取方法也是遵循YUV422格式的最基本提取方法，即两个Y共用一个UV。

### 4.4 YV12格式（属于YUV420）

![img](https:////upload-images.jianshu.io/upload_images/7682685-5642eaa12e43b6e3.png?imageMogr2/auto-orient/strip|imageView2/2/w/626/format/webp)
`YV12`是Plane模式的，也就是Y、U、V三个分量分别打包，依次存储。例如2x2图像：YYYYVU，4*4图像：YYYYYYYYYYYYYYYYVVVVUUUU

### 4.5 NV12、NV21格式（属于YUV420）

![img](https:////upload-images.jianshu.io/upload_images/7682685-88887da62b28a2e8.png?imageMogr2/auto-orient/strip|imageView2/2/w/605/format/webp)
`NV12`和`NV21`是一种two-plane模式，即Y和UV分为两个Plane，其中U和V是交错储存的。`NV12`是U在前，V在后，而`NV21`是V在前U在后。例如4x4图像：
`NV12`：YYYYYYYYYYYYYYYYUVUVUVUV
`NV21`：YYYYYYYYYYYYYYYYVUVUVUVU

### 4.6 I420格式（属于YUV420）

I420是 planar 存储方式，分量存储顺序依次是 Y, Cb(U), Cr(V)，例如4x4图像：YYYYYYYYYYYYYYYYUUUUVVVV

更多格式可参考：[YUV pixel formats](https://www.fourcc.org/yuv.php)

------

## 5、YUV数据量计算

我们以`YUV444`、`YUV422`、`YUV420`这三种采样格式举例。我们知道，RGB图像三个分量都必须全部存储，例如一张4x4像素的RGB图像大小为4x4x3=48字节。

- `YUV444`为一个像素点有一个Y、一个U、一个V，所以一张4x4像素的`YUV444`图像大小为4x4x3=48字节，跟RGB一样大。Y、U、V三个分量的大小都是4x4=16字节
- `YUV422`为两个Y共用一个U和V，Y分量为全采样，即4x4=16字节，U分量和V分量只有Y分量的一半，即U分量为4x4/2=8字节，V分量也是4x4/2=8字节，也就是说一张4x4像素的`YUV422`图像大小为4x4x2=32字节。
- `YUV420`为4个Y共用一个U和V，Y分量为全采样，及4x4=16字节，U分量和V分量只有Y分量的四分之一，即U分量和V分量的大小均为4x4/4=4字节，也就是说一张4x4像素的`YUV420`图像大小为：4x4x(3/2)=24字节。

------

## 6、YUV裁剪

我们来运用一下上面所讲的关于YUV的知识，下面我们拿一张I420的YUV图像来对其进行裁剪。
 上面说到，I420的排列方式就是先全部放Y，然后放U，然后放V，由于I420是4:2:0采样的，那么一幅YUV图像的总大小为4x4x(3/2)=24字节，前4x4字节存放全部的Y，从4x4到4x4x(5/4)存放U，最后从4x4x(5/4)到4x4x(3/2)存放V。

### 6.1先从简单的入手

假如把一幅4x4的图像，将其左上角2x2的画面裁剪出来，应该怎么做呢？我们把YUV三个分量画成下面的图来分析一下：

![img](https:////upload-images.jianshu.io/upload_images/7682685-b4ae3aa84c35519c.png?imageMogr2/auto-orient/strip|imageView2/2/w/1200/format/webp)

如上图所示，首先Y分量每个像素点都有一个Y，那么总用就有16个Y，U和V水平和垂直都是Y的一半，也就是只有4个U和4个V，其中，Y1、Y2、Y5、Y6共用U1和V1，以此类推，那么我们要裁剪左上角2x2像素，不就是把Y1，Y2，Y5，Y6，U1，V1取出来，按I420的方式排列，就这么简单！
 也就是说，原来4x4的I420图像数据为Y1Y2Y3Y4Y5Y6Y7Y8Y9Y10Y11Y12Y13Y14Y15Y16U1U2U3U4V1V2V3V4，我们裁出的左上角2x2图像数据为Y1Y2Y5Y6U1V1。就这么简单！

### 6.2 加强一下

我们用一张256x256大小的图片，来做裁剪，这次我们要裁中间的那部分：将256x256的图片裁出(x, y, w, h) = (64, 64, 128, 128)的那部分。大致就是下面这张图上的红框区域

![img](https:////upload-images.jianshu.io/upload_images/7682685-a4bb15ca01b9eff0.jpg?imageMogr2/auto-orient/strip|imageView2/2/w/256/format/webp)

我们分几步走：

- 读取源图像数据，记为sourceData
- 开辟一块内存来放裁剪后的数据，记为destData
- 读取sourceData的Y分量并提取要裁剪的Y分量数据写入destData
- 读取sourceData的U分量并提取要裁剪的U分量数据写入destData
- 读取sourceData的V分量并提取要裁剪的V分量数据写入destData
- destData写入到文件

我们直接把I420裁剪过程封装成一个方法，代码加注释看看如何一步一步实现：



```c
/**
 I420裁剪

 @param sourceData 源图像数据
 @param sourceW 源图像宽度
 @param sourceH 源图像高度
 @param destData 裁剪后图像数据
 @param x 从源图像的x坐标开始裁剪
 @param y 从源图像的y坐标开始裁剪
 @param w 裁剪宽度
 @param h 裁剪高度
 */
void clipI420(const unsigned char *sourceData,  const int sourceW, const int sourceH, unsigned char *destData, int x, int y, const int w, const int h)
{
    // 拷贝一只指针出来使唤
    const unsigned char *source = sourceData;
    // 把要裁剪的x和y都换成偶数，因为我们2行Y用一行U和一行V，奇数就难搞了
    x = (int)(x + 1) / 2 * 2;
    y  = (int)(y + 1) / 2 * 2;
    
    // 指针移动到要裁剪的Y数据的开头
    source += y * sourceW + x;

    //总共要扫描要裁剪的高的行数，将里面在区域内的Y值提取出来
    for (int i = 0; i < h; i++)
    {
        // 裁剪这一行的目标Y分量
        memcpy(destData, source, w);
        // 移动指针到下一行
        source += sourceW;
        destData += w;
    }
    
    //把源数据指针移到U分量的开头
    source  = sourceData + sourceW * sourceH;
    // 把源数据的指针移到要裁剪的U分量的地方，UV分量都是行列只有Y分量的一半，所以U分量的要裁剪的y坐标应该是在(y/2)*(sourceW/2)的地方，再加上UV分量的x坐标的偏移量
    source += (y * sourceW / 4 + x / 2);
    // UV分量都是行列都只有Y分量的一半，所以指针的移动也是一半的步长
    for (int i = 0; i < h / 2; i++)
    {
        memcpy(destData, source, w / 2);
        source += sourceW / 2;
        destData += w / 2;
    }
    
    //把源数据指针移到V分量的开头
    source  = sourceData + sourceW * sourceH * 5 / 4;
    // 以下跟U分量一个道理
    source += (y * sourceW / 4 + x / 2);

    for (int i = 0; i < h / 2; i++)
    {
        memcpy(destData, source, w / 2);
        source += sourceW / 2;
        destData += w / 2;
    }
}

void clipLena()
{
    int sourceW = 256;
    int sourceH = 256;
    int souceDataLength = sourceW * sourceH * 3 / 2;
    // 读取源图像
    FILE *fp = fopen("lena_256x256_yuv420p.yuv", "rb+");
    unsigned char *sourceData = (unsigned char *)malloc(souceDataLength);
    fread(sourceData, 1, souceDataLength, fp);
    fclose(fp);
    
    // 开辟储存裁剪后的图像的内存
    int clipX = 64;
    int clipY = 64;
    int clipW = 128;
    int clipH = 128;
    int destDataLength = clipW * clipH * 3 / 2;
    unsigned char *destData = (unsigned char *)malloc(souceDataLength);
    
    // 进行裁剪
    clipI420(sourceData, sourceW, sourceH, destData, clipX, clipY, clipW, clipH);
    // 裁剪后数据写入文件
    FILE *fp1 = fopen("lena_64x64_yuv420p","wb+");
    fwrite(destData, 1, destDataLength, fp1);
    fclose(fp1);
    free(sourceData);
    free(destData);
}
```

我自己在做这个的时候的一些疑问：
 **1、裁剪的x和y为什么得偶数？**
 4:2:0这种格式不就是两行用的是同一个UV，两列用的也是同一个UV，例如第一行和第二行Y共用了第一行UV，针对这个256x256的图像来说，我们把数据分为三部分：|----Y----|-U-|-V-|，前两行像素(0, 0, 256, 2)，取的是数据是Y部分的前256x2字节，U部分的前128字节，V部分的前128字节，也就是两行Y，一行U和一行V。如果我们只取一行像素(0, 0, 256, 1)，那就没法玩了，因为这第一行的U和V跟第二行Y共享的。
 **2、怎么计算UV分量数据从哪里开始取？**
 还是拿4x4图像举例，4行4列有16个像素，也就是有16个Y，4个U和4个V。也就是数据最前面4x4=16字节是Y分量，U分量是接着2x2=4字节，V分量是最后2x2=4字节。UV的行列数都是Y分量的一半。回到256x256的图像，我们裁(64, 64, 128, 128)这一块，第64行Y的地方是第32行U，每行U是128字节，那就是U分量128x32，这也就是要裁剪的y坐标。一行U是128字节，那么在原图64列的位置U应该是在这一行的64/2=32的位置。这也就是source += (y * sourceW / 4 + x / 2);这一行的由来。
 **3、换成其他格式怎么裁剪呢？**
 例如NV21怎么裁呢？其实也就是根据数据储存格式和采样方式从源数据中取出我们想要的数据并按规则排列就可以实现了。
 **4、还有哪些知识点没碰到？**
 给YUV图像加水印、YUV旋转、翻转、YUV与RGB互转等等。留坑后面再补。。。

## 7、总结

YUV其实没那么神秘，主要是理解采样比例和储存方式也就没什么太大的难度了。本篇没单独整理Demo，上面I420裁剪的代码是在Mac上写的，理论上Windows也可以跑。YUV文件大家可以在下面雷神的文章《[视音频数据处理入门：RGB、YUV像素数据处理》里头找到。
 最近在学习研究OpenGL和Metal渲染相关的知识，下一篇来讲一下关于视频渲染相关的知识吧！

## 8、参考文献

[YUV pixel formats](https://www.fourcc.org/yuv.php)
 [视音频数据处理入门：RGB、YUV像素数据处理](https://blog.csdn.net/leixiaohua1020/article/details/50534150)
 [YUV格式详解](https://www.cnblogs.com/ALittleDust/p/5935983.html)
 [YUV和RGB格式分析](https://www.cnblogs.com/silence-hust/p/4465354.html)
 [YUV 数据格式完全解析](https://blog.piasy.com/2018/04/27/YUV/index.html)



原文链接：https://www.jianshu.com/p/d9631596e9c7