# iOS音视频的那些事儿：数据的采集和编码

## 1、AVFoundation简介

AVFoundation是苹果在iOS和OS X系统中用于处理基于时间的媒体数据的Objective-C框架. 供使用者来开发媒体类型的应用程序。

![img](https://p1-jj.byteimg.com/tos-cn-i-t2oaga2asx/gold-user-assets/2018/6/1/163b9ca795fb6eb9~tplv-t2oaga2asx-zoom-in-crop-mark:4536:0:0:0.image)

如果只是进行简单的视频录制，使用UIKit中的`UIImagePickerController`就可以了。如果需要播放视频，使用AVKit框架也足够了。但是如果需要进行视频文件的处理等更灵活等操作，就需要使用到AVFoundation等底层的框架了。

- CoreAudio : 处理所有音频事件.是由多个框架整合在一起的总称,为音频和MIDI内容的录制,播放和处理提供相应接口.甚至可以针对音频信号进行完全控制,并通过Audio Units来构建一些复杂的音频处理.有兴趣的可以单独了解一下这个框架.
- Core Video : 是Mac OS和ios系统上针对数字视频所提供的普通模式。Core Video为其相对的Core Media提供图片缓存和缓存池支持，提供了一个能够对数字视频逐帧访问的接口。
- CoreMedia: 是AVFoundation所用到低层级媒体管道的一部分.提供音频样本和视频帧处理所需的低层级数据类型和接口.
- CoreAnimation: 动画相关框架, 封装了支持OpenGL和OpenGL ES功能的ObjC各种类. AVFoundation可以利用CoreAnimation让开发者能够在视频的编辑和播放过程中添加动画和图片效果.

## 2、数据采集

### 3.1 采集过程

为了管理从相机或者麦克风等这样的设备捕获到的信息，我们需要输入对象(input)和输出对象(output)，并且使用一个会话(AVCaptureSession)来管理 input 和 output 之前的数据流：

![img](https://p1-jj.byteimg.com/tos-cn-i-t2oaga2asx/gold-user-assets/2018/6/1/163b9ca8047af3ed~tplv-t2oaga2asx-zoom-in-crop-mark:4536:0:0:0.image)

通过单个 session，也可以管理多个 input 和 output 对象之间的数据流，从而得到视频、静态图像和预览视图。input 可以有一个或多个输入端口，output 也可以有一个或多个数据来源。

当添加 input 和 output 到 session 中时，session 会自动建立起一个连接(AVCaptureConnection)。我们可以使用这个 connection 来设置从 input 或者 从 output 得到的数据的有效性，也可以用来监控在音频信道中功率的平均值和峰值。

![img](https://p1-jj.byteimg.com/tos-cn-i-t2oaga2asx/gold-user-assets/2018/6/1/163b9ca7959426cb~tplv-t2oaga2asx-zoom-in-crop-mark:4536:0:0:0.image)

#### 3.1.1 示例代码

仅录制音频

```
let audioSettings = [AVFormatIDKey: kAudioFormatLinearPCM, // 格式
                     AVSampleRateKey: 44100, // 采样率
                     AVNumberOfChannelsKey: 1, // 声道数
                     AVLinearPCMBitDepthKey: 16] // 位深度
let url = URL(fileURLWithPath: "\(NSHomeDirectory())/tmp/audio.pcm")
do {
    let recorder = try AVAudioRecorder(url: url, settings: audioSettings)
    recorder.record()
} catch let error {
    print(error)
}
```

录制视频

```
  let session = AVCaptureSession()
if session.canSetSessionPreset(.hd1280x720) {
    session.sessionPreset = .hd1280x720
}
self.session = session

// 添加视频源
let videoDevice = AVCaptureDevice.default(for: .video)
if let device = videoDevice {
    do {
        let input = try AVCaptureDeviceInput(device: device)
        if session.canAddInput(input) {
            session.addInput(input)
        }
    } catch let error {
        print(error)
    }
}

// 添加音频源
let audioDevice = AVCaptureDevice.default(for: .audio)
if let device = audioDevice {
    do {
        let input = try AVCaptureDeviceInput(device: device)
        if session.canAddInput(input) {
            session.addInput(input)
        }
    } catch let error {
        print(error)
    }
}

// 输出每一帧
let dataOutput = AVCaptureVideoDataOutput()
dataOutput.videoSettings = [kCVPixelBufferPixelFormatTypeKey: kCVPixelFormatType_420YpCbCr8BiPlanarFullRange] as [String : Any]
dataOutput.setSampleBufferDelegate(self, queue: DispatchQueue.main)
if session.canAddOutput(dataOutput) {
    session.addOutput(dataOutput)
}

// 输出为文件
let fileOutput = AVCaptureMovieFileOutput()
if session.canAddOutput(fileOutput) {
    session.addOutput(fileOutput)
}
let connection = fileOutput.connection(with: .video)
if let conn = connection, conn.isVideoStabilizationSupported {
    conn.preferredVideoStabilizationMode = .auto
}
let url = URL(fileURLWithPath: "\(NSHomeDirectory())/tmp/movie.mov")
// 开始录制
fileOutput.startRecording(to: url, recordingDelegate: self)

let previewLayer = AVCaptureVideoPreviewLayer(session: session)
previewLayer.frame = view.bounds
view.layer.addSublayer(previewLayer)

session.startRunning()        
```

`AVCaptureMovieFileOutput`和`AVCaptureVideoDataOutput`的区别在于，前者是把采集到到数据直接写入到文件，而后者通过下面到代理方法将每一帧到图像数据发送过来，以便于进行一些处理。

```
func captureOutput(_ output: AVCaptureOutput, didOutput sampleBuffer: CMSampleBuffer, from connection: AVCaptureConnection) {
    print(sampleBuffer)
}
```

### 3.2音频采集相关概念

音频的采集过程主要是通过设备将环境中的模拟信号转换成`PCM`编码的原始数据，然后编码压缩成`MP3`、`AAC`、`WMA`、`m4a`、`APE`、`FLAC`等格式。

#### 3.2.1PCM

> PCM全称Pulse Code Modulation，中文名为脉冲编码调制。脉冲编码调制就是对模拟信号先抽样，再对样值幅度量化，编码的过程。

衡量一个音频文件的质量的一个重要指标是比特率(码率)，单位为bps(bit per second)，也就是单位时间内传输的比特数。

而影响比特率的因素有：

1. 采样率(Sample Rate)：采样的频率，采样的频率越高，数据量就越大，音质就越高。

![img](https://p1-jj.byteimg.com/tos-cn-i-t2oaga2asx/gold-user-assets/2018/6/1/163b9ca7957dc61a~tplv-t2oaga2asx-zoom-in-crop-mark:4536:0:0:0.image)

1. 位深度(Bit Depth)：表示每一个采样点所需要的数值的大小。可以是4bit、8bit、16bit等，位数越多，采样点表示的精度就越高，音质就越高，数据量也会成倍的增加。
2. 声道数(Number of Channels)：由于音频的采集和播放是可以叠加的，因此，可以同时从多个音频源采集声音，并分别输出到不同的扬声器，故声道数一般表示声音录制时的音源数量或回放时相应的扬声器数量。声道数为 1 和 2 分别称为单声道和双声道。

根据上面的这些信息我们就可以计算出一个采样率为44100Hz，位深度为16bit，时长为3分钟的单声道CD歌曲的数据量为：
`44100Hz x 16bit x 5 x 60s x 2 = 423360000bit = 52.9MB`
比特率为：`423360000bit / (5 x 60s) = 1441kbps`
这样的数据大小显然是不能接受的，所以才有了上面提到的`MP3`、`AAC`、`WMA`、`m4a`、`APE`、`FLAC`这些压缩格式的出现。

音频的压缩分为无损压缩和有损压缩，简单的来说，有损压缩就是通过删除一些已有数据中不太重要的数据来达到压缩目的；无损压缩就是通过优化排列方式来达到压缩目的。

`MP3`、`AAC`、`WMA`、`m4a`都属于有损压缩，假设把上面的歌曲压缩为码率128kpbs的普通音质的MP3，压缩比为`1441kpbs / 128kbps = 11`，因此压缩后的文件大小为`52.9MB / 11 = 5MB`。

`APE`、`FLAC`属于无损压缩，文件相对较大。

### 3.3 图像采集相关概念

图像采集的图片结果组合成一组连续播放的动画，即构成视频中可肉眼观看的内容。图像的采集过程主要由摄像头等设备拍摄成 YUV 编码的原始数据，然后经过编码压缩成 H.264 等格式的数据分发出去。常见的视频封装格式有：MP4、3GP、AVI、MKV、WMV、MPG、VOB、FLV、SWF、MOV、RMVB 和 WebM 等。

图像数据质量的同样可以通过码率来进行衡量，而影响码率的因素主要有：

1. 图像的分辨率(Resolution)：视频采集过程中的原始分辨率决定了视频整体的清晰度，分辨率越高，视频越清晰。常见的分辨率有`480p`、`720p`、`1080p`、`4k`。
2. 图像的格式：视频图像一般采用YUV格式存储原始的数据信息，而不是采用常见的RGB格式。
3. 帧率(Frame Rate)：单位时间内捕获到到图像到数量，帧率越高，画面的流畅度就越高。一般的视频的帧率为30fps。延时摄影就是以远低于普通帧率的频率进行图像的采集然后以正常的帧率进行播放，就会有一种加速的效果。而高速摄影（慢动作）就是以远高于普通帧率的频率进行图像的采集，然后以正常的帧率进行播放，就会有一种变慢的效果。
4. 传输通道数：一般情况下都只有一个摄像头进行数据的采样，而随着VR和AR技术的发展，要拍摄一个360度的视频，就需要多个摄像头来同时进行采集了。

#### 3.3.1 图像的位深度

我们知道对于一个普通的不包含Alpha通道的位图图像而言，每一个像素点的数据都可以通过R、G、B三个分量数据来进行表示。每个数据用用多大的数值来表示就决定了这个图像的位深度，也即图像的色彩范围。假设我们用1个字节来表示一个分量的数值，即24位真彩色，那么总共就有`2^8 x 2^8 x 2^8 = 1600万`种颜色，而如果对RGB分别用3bit、3bit、2bit也即8位色来表示的话只有`2^3 x 2^3 x 2^2 = 256`种颜色。

![img](https://p1-jj.byteimg.com/tos-cn-i-t2oaga2asx/gold-user-assets/2018/6/1/163b9ca80e8204a3~tplv-t2oaga2asx-zoom-in-crop-mark:4536:0:0:0.image)

24位色的图片
![img](https://p1-jj.byteimg.com/tos-cn-i-t2oaga2asx/gold-user-assets/2018/6/1/163b9ca821d330d7~tplv-t2oaga2asx-zoom-in-crop-mark:4536:0:0:0.image)
8位色的图片

#### 3.3.2 YUV格式

与我们所熟知的RGB类似，YUV也是一种颜色的编码方法，主要用于视频领域。它将亮度信息(Y)与色彩信息(UV也称CbCr)分离，没有UV信息一样可以显示完整的图像，只不过是黑白的。这样的设计很好地解决了彩色电视机与黑白电视的兼容问题。并且，YUV不像RGB那样要求三个独立的视频信号同时传输，所以用YUV方式传送占用极少的频宽。我们人眼对于亮度的感知比对色彩的感知更为强烈，所以可以通过色彩信息(UV)的采样来减少数据量。

![img](https://p1-jj.byteimg.com/tos-cn-i-t2oaga2asx/gold-user-assets/2018/6/1/163b9ca84a4c3779~tplv-t2oaga2asx-zoom-in-crop-mark:4536:0:0:0.image)

色彩信息(UV)
![img](https://p1-jj.byteimg.com/tos-cn-i-t2oaga2asx/gold-user-assets/2018/6/1/163b9ca85482d7da~tplv-t2oaga2asx-zoom-in-crop-mark:4536:0:0:0.image)
亮度和色度的对比

YUV码流的存储格式其实与其采样的方式密切相关，主流的采样方式有三种，YUV4:4:4、YUV4:2:2、YUV4:2:0，如下图所示：
![img](https://p1-jj.byteimg.com/tos-cn-i-t2oaga2asx/gold-user-assets/2018/6/1/163b9ca86742912c~tplv-t2oaga2asx-zoom-in-crop-mark:4536:0:0:0.image)

对于YUV 4:4:4采样，每一个Y分量对应一组UV分量
对于YUV 4:2:2采样，每两个Y分量公用一组UV分量
对于YUV 4:2:0采样，每四个Y分量公用一组UV分量

YUV格式有两大类：planar和packed。
对于planar的YUV格式，先连续存储所有像素点的Y，紧接着存储所有像素点的U，随后是所有像素点的V。
对于packed的YUV格式，每个像素点的Y,U,V是连续交叉存储的。

一般移动平台视频录制使用的是YUV 4:2:0，而根据各个分量的排列顺序不同又分为`YV12`、`I420(YU12)`、`NV12`、`NV21`。

`YV12`和`I420`也称为YUV420P，即planar平面格式。YV12和I420的区别仅在与UV顺序的不同。顾名思义，YV12中Y平面后面紧跟的是V平面，然后是U平面，12表示它的位深度为12，也就是一个像素占12bit。而I420(YU12)刚好相反。

`NV12`和`NV21`都属于YUV420SP，即Y分量平面格式，UV分量打包格式。也即先存储Y平面，后面是UV分量交错存储。iOS平台使用的是`NV12`格式。

​                        `I420: YYYYYYYY UU VV  => YUV420P YV12: YYYYYYYY VV UU  => YUV420P NV12: YYYYYYYY UVUV   => YUV420SP NV21: YYYYYYYY VUVU   => YUV420SP 复制代码` 

I420(Planar)的单帧结构示意图如下：

![img](https://p1-jj.byteimg.com/tos-cn-i-t2oaga2asx/gold-user-assets/2018/6/1/163b9ca89c1aeeb8~tplv-t2oaga2asx-zoom-in-crop-mark:4536:0:0:0.image)

NV12(Planar)的单帧结构示意图如下:

![img](https://p1-jj.byteimg.com/tos-cn-i-t2oaga2asx/gold-user-assets/2018/6/1/163b9ca8b133d70e~tplv-t2oaga2asx-zoom-in-crop-mark:4536:0:0:0.image)

根据上图所示：

一张分辨率为6 x 4的图片采用YUV420的方式存储所占的空间为`6 x 4 + 6 + 6 = 36`个字节，而如果采用RGB的方式存储就需要占用`6 x 4 x 3 = 72`个字节，数据量减少了一半。

如果以YUV420的方式编码一部720p、帧率为30fps、时长两小时的电影所需要的空间为

```
12bit x 1280 x 720 x 30 x 120 x 60 = 2.38 x 10^12bit = 298.6GB
```

这个大小显然是不能接受的，经过编码我们可以把它压缩到2G左右，而画面到清晰度不会有多少损失。

### 3.4 视频编码

#### 3.4.1 基本原理

为什么视频文件的压缩比可以达到100:1甚至更高？核心的思想就是去除冗余信息。

视频文件中的冗余信息分为以下几类：

1. 空间冗余：每一帧图像相邻像素之间有较强的关联性
2. 时间冗余：视频相邻帧之间的内容相似
3. 编码冗余：不同像素值出现的概率不同
4. 视觉冗余：人眼对于某些细节不敏感
5. 知识冗余：规规律性的结构可由先验知识和背景知识得到

对于视频文件最简单的压缩方式就是对每一帧图像进行压缩，叫做帧内压缩。有一种比较古老的 MJPEG 编码就是这种编码方式。可以理解为把视频的每一帧当作一张图片，然后按照JPEG的方式来进行压缩。这样的压缩方式只处理了空间冗余信息，离理想的压缩比还差得远。

视频的图像的相邻帧之间具有很大的相似性，因此去除时间冗余信息对于提高压缩比有很大的提升空间。比如说一些高级的编码器可以采用帧间编码，简单点说就是通过搜索算法选定了帧上的某些区域，然后通过计算当前帧和前后参考帧的向量差进行编码的一种形式，通过下面两个图 2 连续帧我们可以看到，滑雪的同学是向前位移的，但实际上是雪景在向后位移。后面的帧就可以参考前面的帧进行编码，这样后面的帧编码的结果就非常小，压缩比很高。

![img](https://p1-jj.byteimg.com/tos-cn-i-t2oaga2asx/gold-user-assets/2018/6/1/163b9ca8df2f0176~tplv-t2oaga2asx-zoom-in-crop-mark:4536:0:0:0.image)

去除其他的冗余信息这里就不多说了。

#### 3.4.2 常用编码器

**H.264**

> H.264/AVC 是现在使用最广泛的编码标准。与旧标准相比，它能够在更低带宽下提供优质视频（换言之，只有 MPEG-2，H.263 或 MPEG-4 第 2 部分的一半带宽或更少），也不增加太多设计复杂度使得无法实现或实现成本过高。另一目的是提供足够的灵活性以在各种应用、网络及系统中使用，包括高、低带宽，高、低视频分辨率，广播，DVD 存储，RTP/IP 网络，以及 ITU-T 多媒体电话系统。

**H.265**

2017年6月6日凌晨召开WWDC 2017大会上苹果在iOS11上推出了HEVC，用来取代H.264

> 高效率视频编码（High Efficiency Video Coding，简称HEVC）是一种视频压缩标准，被视为是 ITU-T H.264/MPEG-4 AVC 标准的继任者。2004 年开始由 ISO/IEC Moving Picture Experts Group（MPEG）和 ITU-T Video Coding Experts Group（VCEG）作为 ISO/IEC 23008-2 MPEG-H Part 2 或称作 ITU-T H.265 开始制定。第一版的                HEVC/H.265 视频压缩标准在 2013 年 4 月 13 日被接受为国际电信联盟（ITU-T）的正式标准。HEVC 被认为不仅提升视频质量，同时也能达到 H.264/MPEG-4 AVC 两倍之压缩率（等同于同样画面质量下比特率减少了 50%），可支持 4K 分辨率甚至到超高清电视（UHDTV），最高分辨率可达到 8192×4320（8K分辨率）。

#### 3.4.3 硬件编解码和软件编解码

iOS8开始，苹果通过`Video ToolBox`开放了系统的硬件编解码能力。在这之前，开发者基本上都是使用的是一个叫做[FFmpeg](https://link.juejin.cn?target=https%3A%2F%2Fwww.ffmpeg.org%2F)的多媒体库，利用CPU做视频的编解码，俗称软解码。

> FFmpeg 是一个自由软件，可以运行音频和视频多种格式的录影、转换、流功能，包含了 libavcodec ——这是一个用于多个项目中音频和视频的解码器库，以及 libavformat —— 一个音频与视频格式转换库。

#### 3.4.4 视频容器(格式)

所谓容器，就是把编码器生成的多媒体内容（视频，音频，字幕，章节信息等）混合封装在一起的标准。容器使得不同多媒体内容同步播放变得很简单，而容器的另一个作用就是为多媒体内容提供索引，也就是说如果没有容器存在的话一部影片你只能从一开始看到最后，不能拖动进度条（当然这种情况下有的播放器会花比较长的时间临时创建索引），而且如果你不自己去手动另外载入音频就没有声音。

![img](https://p1-jj.byteimg.com/tos-cn-i-t2oaga2asx/gold-user-assets/2018/6/1/163b9ca8ef9f8a95~tplv-t2oaga2asx-zoom-in-crop-mark:4536:0:0:0.image)

MOV文件的结构

常见的视频容器格式有：

- AVI 格式（后缀为 .AVI）: 它的英文全称为 Audio Video Interleaved ，即音频视频交错格式。它于 1992 年被 Microsoft 公司推出。
  这种视频格式的优点是图像质量好。由于无损AVI可以保存 alpha 通道，经常被我们使用。缺点太多，体积过于庞大，而且更加糟糕的是压缩标准不统一，最普遍的现象就是高版本 Windows 媒体播放器播放不了采用早期编码编辑的AVI格式视频，而低版本 Windows 媒体播放器又播放不了采用最新编码编辑的AVI格式视频，所以我们在进行一些AVI格式的视频播放时常会出现由于视频编码问题而造成的视频不能播放或即使能够播放，但存在不能调节播放进度和播放时只有声音没有图像等一些莫名其妙的问题。
- QuickTime File Format 格式（后缀为 .MOV）: 美国Apple公司开发的一种视频格式，默认的播放器是苹果的QuickTime。
  具有较高的压缩比率和较完美的视频清晰度等特点，并可以保存alpha通道。
- MPEG 格式（文件后缀可以是 .MPG .MPEG .MPE .DAT .VOB .ASF .3GP .MP4等) : 它的英文全称为 Moving Picture Experts Group，即运动图像专家组格式，该专家组建于1988年，专门负责为 CD 建立视频和音频标准，而成员都是为视频、音频及系统领域的技术专家。
  MPEG 文件格式是运动图像压缩算法的国际标准。MPEG 格式目前有三个压缩标准，分别是 MPEG－1、MPEG－2、和MPEG－4 。MPEG－1、MPEG－2                    目前已经使用较少，着重介绍 MPEG－4，其制定于1998年，MPEG－4 是为了播放流式媒体的高质量视频而专门设计的，以求使用最少的数据获得最佳的图像质量。目前 MPEG-4 最有吸引力的地方在于它能够保存接近于DVD画质的小体积视频文件。
- Real Video 格式（后缀为 .RM .RMVB）: Real Networks 公司所制定的音频视频压缩规范称为Real Media。
  用户可以使用 RealPlayer 根据不同的网络传输速率制定出不同的压缩比率，从而实现在低速率的网络上进行影像数据实时传送和播放。RMVB 格式：这是一种由RM视频格式升级延伸出的新视频格式，当然性能上有很大的提升。RMVB 视频也是有着较明显的优势，一部大小为700MB左右的 DVD 影片，如果将其转录成同样品质的                    RMVB 格式，其个头最多也就 400MB 左右。大家可能注意到了，以前在网络上下载电影和视频的时候，经常接触到 RMVB 格式，但是随着时代的发展这种格式被越来越多的更优秀的格式替代，著名的人人影视字幕组在2013年已经宣布不再压制 RMVB 格式视频。
- Matroska 格式（后缀位 .MKV）:是一种新的多媒体封装格式，这个封装格式可把多种不同编码的视频及16条或以上不同格式的音频和语言不同的字幕封装到一个Matroska Media档内。它也是其中一种开放源代码的多媒体封装格式。Matroska同时还可以提供非常好的交互功能，而且比MPEG更方便、强大。




原文链接：https://juejin.cn/post/6844903615342051336