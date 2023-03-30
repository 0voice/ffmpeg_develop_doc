# 音视频学习--iOS适配H265实战踩坑记

## 1.背景介绍

熟悉webrtc都知道：谷歌的webrtc，默认不支持h265，毕竟涉及到很多专利的事宜，这中间的八卦就暂时不做探究。但是今天拿到一个IPC，该设备会发送H265数据，如下图所示，要做到兼容相关IPC，只能适配H265编解码了。所以最近熬秃了好几把头发就做了一下相关知识的学习，以下是自己的学习笔记，提供大家一个解决问题思路，由于iOS刚接触，同时H265调试经验也欠缺，很多都是一边查找资料，一边学习的，难免有处理不妥当之处，欢迎一起讨论学习，开发大神请绕行。

![img](https://pic4.zhimg.com/80/v2-6746a34cecfce9a3d96574f10be3d8bf_720w.webp)

## 2.补充1H265编码特性

学习之前先了解一下H265的编码特性，有的放矢才能遇到问题及时排查。H265的经典编码框架如下图：



![img](https://pic1.zhimg.com/80/v2-b7a8b1fd214402208b2450bef92a596c_720w.webp)

HEVC 的编码框架是在H26X的基础上逐步发展起来的，主要包括变换、量化、熵编码、帧内预测、帧间预测以及环路滤波等模块。相关内容推荐阅读万帅老师的书《新一代高效视频编码H.265/HEVC：原理、标准与实现》。

一般提到H265，都难免要和H264对比一番，以下是这次需求端要求适配H265的基本理论依据，如下表格：

![img](https://pic2.zhimg.com/80/v2-8b02b03fdfeb82c405611a201920006d_720w.webp)

理论上要直接适配一种编解码格式，肯定要学习相关理论，然后再做对应适配。然而时间紧张，任务繁杂，根本没有足够时间积累。想要有一个全面直观的认识，同时为了验证IPC是否正常，所以首先用VLC进行播放尝试：PC端确认该IPC可以通过 VLC进行监控，这时我们可以通过wireshark抓包，拿到第一手数据进行分析。通过抓包可以看出：1920*1080的视频流除了IDR数据帧大小略大，其他数据都是一个NALU单元就完成封装了，文件大小确实小很多。

接下来我们依次认识H265的封包和关键信息。

## 3.H265关键信息

通过相关抓包，可以看到整理结构如下图，包含了VPS，SPS，PPS， FU分片包，Trail_R的包等。

![img](https://pic4.zhimg.com/80/v2-819861cd09b0d2724727ae8e6d639183_720w.webp)

![img](https://pic4.zhimg.com/80/v2-9e41ceeb0fb2935279250b2ecce4164b_720w.webp)

### 3.1VPS结构

VPS（Video Parameter Set， 视频参数集）依据ITUHEVC的标准文档，VPS的参数结构以及每一个条款的解释，很多大佬已经有写明了，HEVC来说自己还是小萌新在此不再累述，可以参考自己关注的一个大佬的博文：

![img](https://pic1.zhimg.com/v2-95203dc981a68b102381f5bf23acf8b0_120x160.jpg)A//blog.csdn.net/Dillon2015/article/details/104142144)

其抓包中VPS数据参数如下：

![img](https://pic3.zhimg.com/80/v2-2579ff7e1c9b9a27b47f1a2f434c311a_720w.webp)

### 3.2SPS结构

SPS的内容大致包括解码相关信息，如档次级别、分辨率、某档次中编码工具开关标识和涉及的参数、时域可分级信息等。

其抓包中SPS数据参数如下：

![img](https://pic4.zhimg.com/80/v2-3ea2e44759f1d48dc2819df3c3664a23_720w.webp)

### 3.3PPS结构

HEVC的图像参数集PPS包含每一帧可能不同的设置信息；其内容大致包括初始图像控制信息，如量化参数（QP，Quantization Parament）、分块信息等。即PPS包含了每一帧图像所用的公共参数，即一帧图像中的所有SS会引用同一个PPS；每张图像的PPS可能不同。详细介绍说明可以参考：

其抓包中PPS数据参数如下：

![img](https://pic3.zhimg.com/80/v2-dbff33f0871d7ae00e68bfb38f06a866_720w.webp)

有了这些数据，至少可以解析出来该IPC支持的分辨率，profile，level等基本数据信息，为初始化做准备。

## 4.H265解封包

由上面信息可以知道H265的封包格式和H264基本上保持一致，也是通过NALU单元进行分装，不过不太一样的地方是 H265的NALU Header长度是2个字节，而 H264的NALU Header的长度是1个字节，所以解析时候需要进行移位操作，否则读取数据异常，导致包类型无法辨别（在这里踩了一个小坑，印象深刻）。

H26封包中NALU type主要类型如下图所示。由于不同厂家支持程度不一，本次适配过程中主要关注的几个类型包括：kTrailR(1)，kIdrwRadl(1)，kVps(1)，kSps(1)，kPps(1)，kFU(1)等。

![img](https://pic2.zhimg.com/80/v2-d0c4f9c852cffdf051eb7b3b6763d7a5_720w.webp)

### 4.1FUNALU

当 NALU 的长度超过 MTU 时, 用于把当前NALU单元封装成多个 RTP 包，HEVC的FU单元的type值为49，具体组织结构如下：

![img](https://pic2.zhimg.com/80/v2-10c6bb4748daf92590d10d63bf6d36f9_720w.webp)

FU Nalu相关解析代码如下：

![img](https://pic4.zhimg.com/80/v2-9105f7823e83bae92b455a1165fa1137_720w.webp)

### 4.2SingleNALU

单个 NAL 单元数据包只包含一个 NAL 单元，由一个有效载荷头（表示为 PayloadHdr）、一个有条件的16位DONL字段和NAL 单元有效载荷数据。

![img](https://pic3.zhimg.com/80/v2-ddb09b350ee2182e7ca61897a272fc9e_720w.webp)

### 4.3AP NALU

HEVC封包另外一个种封包格式：聚合模式（Aggregation Packets，APs)，主要为了减少小型 NAL 单元的打包开销，例如大多数非 VCL NAL 单元，它们的大小通常只有几个字节。AP 将 NAL 单元聚合在一个访问单元内。AP 中要携带的每个 NAL 单元都封装在聚合单元中。聚合在一个 AP 中的 NAL 单元按 NAL 单元解码顺序排列。

![img](https://pic1.zhimg.com/80/v2-874b4764c6017d093175d6bbf0e90930_720w.webp)

AP Nalu和Single Nalu相关代码解析如下：

![img](https://pic3.zhimg.com/80/v2-742eb210ca022ef787efb1792e33080e_720w.webp)

当接收到每一个HEVC包之后，一次送到Jitterbuffer中，完成数据帧的重新组装和排序，必要时候进行数据矫正和重传操作，这是另外的技术，此处暂不做讨论。

## 5.H265 VideoToolBox解码

收到完整数据帧时候，VCM会依据时间戳一次获取相关数据，送解码器，iOS平台就是VideoToolBox解码了。这部分自己是小白，简单说明关键函数，大佬勿喷。

### 5.1ResetDecompressionSession

ResetDecompressionSession完成解码参数的构建，以及Session的创建。主要注意iOS支持的色彩是有差别的，设置时候需要明确是否支持，比如这次就设置了kCVPixelFormatType_420YpCbCr8BiPlanarFullRange类型。

同时在reset函数中注册callback函数，用于接收解码完成后的视频帧。

![img](https://pic2.zhimg.com/80/v2-dad83cacfc891c741e4427ee9163aed5_720w.webp)

### 5.2decode

iOS VideoToolBox相关开发说明，可以参考

![img](https://pic3.zhimg.com/80/v2-45f14ce7aa9e11dbae029395116afc0e_720w.webp)

![img](https://pic1.zhimg.com/80/v2-323670c3f1501798744085bebe4bb3bc_720w.webp)

获取VPS，SPS，PPS相关数据，构建CMVideoFormatDescription相关数据；

![img](https://pic2.zhimg.com/80/v2-b33f0650bca613e3740704a8c5d8c709_720w.webp)

### 5.3callback

callback函数用于接收H265解码数据，并用于送到显示端进行渲染的。

![img](https://pic3.zhimg.com/80/v2-76d4b4d9b7a3b08978a818fb64ce5de6_720w.webp)

## 6.补充2 Annexb和AVCC格式转换

Android的硬解码接口MediaCodec只能接收Annex-B格式的H264数据，而iOS平台的VideoToolBox则相反，只支持AVCC格式。所以要进行一次转换，相关转换规则有大佬做了说明，可以参考

![img](https://pic1.zhimg.com/80/v2-26c14b9967d5fc492aa6584a9101fc7c_720w.webp)

## 7.补充3iOS编解码错误说明

在调试iOS编解码过程碰到几个CallBack的错误，于是找了一下相关错误代码，常见的错误如下所示：

![img](https://pic3.zhimg.com/80/v2-dcb0bbeba51baac22487d5e1257e8dfe_720w.webp)

其中一个错误kVTVideoDecoderBadDataErr = -12909，该错误找了很多久，也查了很多资料，一直卡住2天，每天早出晚归，熬最深的夜，加最晚的班，我可怜的头发又少了好几根。RIP.

期间尝试各种办法验证确认：

（1）将所有数据包和数据类型打印；

（2）送解码前数据打印和保存；

（3）反复确认VPS，SPS，PPS数据内容；

（4）确认调用流程；

（5）查找githubdemo。

最后确认经过多次反复确认代码，验证裸流，比较大小之后，最后发现计算长度时候H265 NAL头计算错误，导致IDR帧无法正确解码，最后找到问题，一行代码解决问题。其实该问题在RFC7798中有说明，只是自己还是按照H264惯性思维处理，这也是基础知识不扎实的根本原因（不过回想一下好像本来也没有这块知识，惭愧，惭愧）。

![img](https://pic2.zhimg.com/80/v2-dfa623fced640a237a9e8869a64517a5_720w.webp)

题外话：排查问题过程很艰辛，最后一行代码处理完成，这是目前工作中很常见的，所以也是自己给新人，或者要入门音视频强调的一点：保持足够投入度，最好是兴趣驱动。《格局》一书中说：主动做事的收益，或许不会在一两天内显现出来但是长期坚持下来，主动做事的人，就能和其他人拉开距离。

扯远了，最后分享一下排查问题期间也查阅其他人调试过程发现的问题点，在此一起收集一下，方便后来者遇到问题可以快速查阅：

（1）比如，省流模式下解码失败

该文章提到一种思路：可以借鉴比较成熟的ijkplayer，对比流程和处理细节，查找得到解决办法。

（2）比如，解码器Session失效问题

该文章中提到，如果VideoToolBox返回码是 kVTInvalidSessionErr =-12903，也就是说解码器Session异常或者失效了。可以在收到该返回码时调用ResetDecompressionSession操作完成重置，再进行切换时就会正常了。该部分优化已经同步了，防止切换异常；手动狗头，感谢大佬。

（3）比如，annexB与hvcc转换异常问题

该Issue中提到，ios硬解h265 NALU失败，看了代码发现在vtbformat_init中对265从annexB=>mp4转换时需要使用ff_isom_write_hvcc，而不能重用264的ff_isom_write_avcc。两者区别还是比较多的，有兴趣的翻阅一下FFMPEG代码自行补充。

原文https://zhuanlan.zhihu.com/p/589832516