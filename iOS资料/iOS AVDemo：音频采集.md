# iOS AVDemo：音频采集

iOS/Android 客户端开发同学如果想要开始学习音视频开发，最丝滑的方式是对[音视频基础概念知识](https://mp.weixin.qq.com/mp/appmsgalbum?__biz=MjM5MTkxOTQyMQ==&action=getalbum&album_id=2140155659944787969#wechat_redirect)有一定了解后，再借助本地平台的音视频能力上手去实践音视频的`采集 → 编码 → 封装 → 解封装 → 解码 → 渲染`过程，并借助[音视频工具](https://mp.weixin.qq.com/mp/appmsgalbum?__biz=MjM5MTkxOTQyMQ==&action=getalbum&album_id=2216997905264082945#wechat_redirect)来分析和理解对应的音视频数据。

在[音视频工程示例](https://mp.weixin.qq.com/mp/appmsgalbum?__biz=MjM5MTkxOTQyMQ==&action=getalbum&album_id=2273301900659851268#wechat_redirect)这个栏目，我们将通过拆解`采集 → 编码 → 封装 → 解封装 → 解码 → 渲染`流程并实现 Demo 来向大家介绍如何在 iOS/Android 平台上手音视频开发。

这里是第一篇：**iOS 音频采集 Demo**。这个 Demo 里包含以下内容：

- 1）实现一个音频采集模块；
- 2）实现音频采集逻辑并将采集的音频存储为 PCM 数据；
- 3）详尽的代码注释，帮你理解代码逻辑和原理。

## 1、音频采集模块

首先，实现一个 `KFAudioConfig` 类用于定义音频采集参数的配置。这里包括了：采样率、量化位深、声道数这几个参数。这几个参数的含义在前面介绍声音基础的文章[《声音的表示（3）：声音的数字化》](https://mp.weixin.qq.com/s?__biz=MjM5MTkxOTQyMQ==&mid=2257484445&idx=1&sn=a1b2f1c71e54ca5c311fedd561fdee4d&scene=21#wechat_redirect)中有过介绍。

```
KFAudioConfig.h
#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface KFAudioConfig : NSObject
+ (instancetype)defaultConfig;

@property (nonatomic, assign) NSUInteger channels; // 声道数，default: 2。
@property (nonatomic, assign) NSUInteger sampleRate; // 采样率，default: 44100。
@property (nonatomic, assign) NSUInteger bitDepth; // 量化位深，default: 16。
@end

NS_ASSUME_NONNULL_END
KFAudioConfig.m
#import "KFAudioConfig.h"

@implementation KFAudioConfig

+ (instancetype)defaultConfig {
    KFAudioConfig *config = [[self alloc] init];
    config.channels = 2;
    config.sampleRate = 44100;
    config.bitDepth = 16;
    
    return config;
}

@end
```

接下来，我们实现一个 `KFAudioCapture` 类来实现音频采集。

```
KFAudioCapture.h
#import <Foundation/Foundation.h>
#import <CoreMedia/CoreMedia.h>
#import "KFAudioConfig.h"

NS_ASSUME_NONNULL_BEGIN

@interface KFAudioCapture : NSObject
+ (instancetype)new NS_UNAVAILABLE;
- (instancetype)init NS_UNAVAILABLE;
- (instancetype)initWithConfig:(KFAudioConfig *)config;

@property (nonatomic, strong, readonly) KFAudioConfig *config;
@property (nonatomic, copy) void (^sampleBufferOutputCallBack)(CMSampleBufferRef sample); // 音频采集数据回调。
@property (nonatomic, copy) void (^errorCallBack)(NSError *error); // 音频采集错误回调。

- (void)startRunning; // 开始采集音频数据。
- (void)stopRunning; // 停止采集音频数据。
@end

NS_ASSUME_NONNULL_END
```

上面是 `KFAudioCapture` 的接口设计，可以看到这里除了`初始化方法`，主要是有`获取音频配置`以及音频采集`数据回调`和`错误回调`的接口，另外就是`开始采集`和`停止采集`的接口。

在上面的音频采集`数据回调`接口中，我们返回的是 **CMSampleBufferRef**[1] 这个数据结构，这里我们重点介绍一下。官方文档对 `CMSampleBufferRef` 描述如下：

> A reference to a CMSampleBuffer. A CMSampleBuffer is a Core Foundation object containing zero or more compressed (or uncompressed) samples of a particular media type (audio, video, muxed, and so on).

即 `CMSampleBufferRef` 是对 **CMSampleBuffer**[2] 的一个引用。所里这里核心的数据结构是 `CMSampleBuffer`，关于它有如下几点需要注意：

- `CMSampleBuffer` 则是一个 Core Foundation 的对象，这意味着它的接口是 C 语言实现，它的内存管理是非 ARC 的，需要手动管理，它与 Foundation 对象之间需要进行桥接转换。

- `CMSampleBuffer` 是系统用来在音视频处理的 pipeline 中使用和传递媒体采样数据的核心数据结构。你可以认为它是 iOS 音视频处理 pipeline 中的流通货币，摄像头采集的视频数据接口、麦克风采集的音频数据接口、编码和解码数据接口、读取和存储视频接口、视频渲染接口等等，都以它作为参数。

- `CMSampleBuffer` 中包含着零个或多个某一类型（audio、video、muxed 等）的采样数据。比如：

- - 要么是一个或多个媒体采样的 **CMBlockBuffer**[3]。其中可以封装：音频采集后、编码后、解码后的数据（如：PCM 数据、AAC 数据）；视频编码后的数据（如：H.264 数据）。
  - 要么是一个 **CVImageBuffer**[4]（也作 **CVPixelBuffer**[5]）。其中包含媒体流中 CMSampleBuffers 的格式描述、每个采样的宽高和时序信息、缓冲级别和采样级别的附属信息。缓冲级别的附属信息是指缓冲区整体的信息，比如播放速度、对后续缓冲数据的操作等。采样级别的附属信息是指单个采样的信息，比如视频帧的时间戳、是否关键帧等。其中可以封装：视频采集后、解码后等未经编码的数据（如：YCbCr 数据、RGBA 数据）。

所以，了解完这些，就知道上面的音频采集`数据回调`接口为什么会返回 `CMSampleBufferRef` 这个数据结构了。因为它通用，同时我们也可以从里面获取到我们想要的 PCM 数据。

```
KFAudioCapture.m
#import "KFAudioCapture.h"
#import <AVFoundation/AVFoundation.h>
#import <mach/mach_time.h>

@interface KFAudioCapture ()
@property (nonatomic, assign) AudioComponentInstance audioCaptureInstance; // 音频采集实例。
@property (nonatomic, assign) AudioStreamBasicDescription audioFormat; // 音频采集参数。
@property (nonatomic, strong, readwrite) KFAudioConfig *config;
@property (nonatomic, strong) dispatch_queue_t captureQueue;
@property (nonatomic, assign) BOOL isError;
@end

@implementation KFAudioCapture

#pragma mark - Lifecycle
- (instancetype)initWithConfig:(KFAudioConfig *)config {
    self = [super init];
    if (self) {
        _config = config;
        _captureQueue = dispatch_queue_create("com.KeyFrameKit.audioCapture", DISPATCH_QUEUE_SERIAL);
    }
    
    return self;
}

- (void)dealloc {
    // 清理音频采集实例。
    if (_audioCaptureInstance) {
        AudioOutputUnitStop(_audioCaptureInstance);
        AudioComponentInstanceDispose(_audioCaptureInstance);
        _audioCaptureInstance = nil;
    }
}

#pragma mark - Action
- (void)startRunning {
    if (self.isError) {
        return;
    }
    
    __weak typeof(self) weakSelf = self;
    dispatch_async(_captureQueue, ^{
        if (!weakSelf.audioCaptureInstance) {
            NSError *error = nil;
            // 第一次 startRunning 时创建音频采集实例。
            [weakSelf setupAudioCaptureInstance:&error];
            if (error) {
                // 捕捉并回调创建音频实例时的错误。
                [weakSelf callBackError:error];
                return;
            }
        }
        
        // 开始采集。
        OSStatus startStatus = AudioOutputUnitStart(weakSelf.audioCaptureInstance);
        if (startStatus != noErr) {
            // 捕捉并回调开始采集时的错误。
            [weakSelf callBackError:[NSError errorWithDomain:NSStringFromClass([KFAudioCapture class]) code:startStatus userInfo:nil]];
        }
    });
}

- (void)stopRunning {
    if (self.isError) {
        return;
    }
    
    __weak typeof(self) weakSelf = self;
    dispatch_async(_captureQueue, ^{
        if (weakSelf.audioCaptureInstance) {
            // 停止采集。
            OSStatus stopStatus = AudioOutputUnitStop(weakSelf.audioCaptureInstance);
            if (stopStatus != noErr) {
                // 捕捉并回调停止采集时的错误。
                [weakSelf callBackError:[NSError errorWithDomain:NSStringFromClass([KFAudioCapture class]) code:stopStatus userInfo:nil]];
            }
        }
    });
}

#pragma mark - Utility
- (void)setupAudioCaptureInstance:(NSError **)error {
    // 1、设置音频组件描述。
    AudioComponentDescription acd = {
        .componentType = kAudioUnitType_Output,
        //.componentSubType = kAudioUnitSubType_VoiceProcessingIO, // 回声消除模式
        .componentSubType = kAudioUnitSubType_RemoteIO,
        .componentManufacturer = kAudioUnitManufacturer_Apple,
        .componentFlags = 0,
        .componentFlagsMask = 0,
    };
    
    // 2、查找符合指定描述的音频组件。
    AudioComponent component = AudioComponentFindNext(NULL, &acd);
    
    // 3、创建音频组件实例。
    OSStatus status = AudioComponentInstanceNew(component, &_audioCaptureInstance);
    if (status != noErr) {
        *error = [NSError errorWithDomain:NSStringFromClass(self.class) code:status userInfo:nil];
        return;
    }
        
    // 4、设置实例的属性：可读写。0 不可读写，1 可读写。
    UInt32 flagOne = 1;
    AudioUnitSetProperty(_audioCaptureInstance, kAudioOutputUnitProperty_EnableIO, kAudioUnitScope_Input, 1, &flagOne, sizeof(flagOne));
    
    // 5、设置实例的属性：音频参数，如：数据格式、声道数、采样位深、采样率等。
    AudioStreamBasicDescription asbd = {0};
    asbd.mFormatID = kAudioFormatLinearPCM; // 原始数据为 PCM，采用声道交错格式。
    asbd.mFormatFlags = kAudioFormatFlagIsSignedInteger | kAudioFormatFlagsNativeEndian | kAudioFormatFlagIsPacked;
    asbd.mChannelsPerFrame = (UInt32) self.config.channels; // 每帧的声道数
    asbd.mFramesPerPacket = 1; // 每个数据包帧数
    asbd.mBitsPerChannel = (UInt32) self.config.bitDepth; // 采样位深
    asbd.mBytesPerFrame = asbd.mChannelsPerFrame * asbd.mBitsPerChannel / 8; // 每帧字节数 (byte = bit / 8)
    asbd.mBytesPerPacket = asbd.mFramesPerPacket * asbd.mBytesPerFrame; // 每个包的字节数
    asbd.mSampleRate = self.config.sampleRate; // 采样率
    self.audioFormat = asbd;
    status = AudioUnitSetProperty(_audioCaptureInstance, kAudioUnitProperty_StreamFormat, kAudioUnitScope_Output, 1, &asbd, sizeof(asbd));
    if (status != noErr) {
        *error = [NSError errorWithDomain:NSStringFromClass(self.class) code:status userInfo:nil];
        return;
    }
    
    // 6、设置实例的属性：数据回调函数。
    AURenderCallbackStruct cb;
    cb.inputProcRefCon = (__bridge void *) self;
    cb.inputProc = audioBufferCallBack;
    status = AudioUnitSetProperty(_audioCaptureInstance, kAudioOutputUnitProperty_SetInputCallback, kAudioUnitScope_Global, 1, &cb, sizeof(cb));
    if (status != noErr) {
        *error = [NSError errorWithDomain:NSStringFromClass(self.class) code:status userInfo:nil];
        return;
    }
    
    // 7、初始化实例。
    status = AudioUnitInitialize(_audioCaptureInstance);
    if (status != noErr) {
        *error = [NSError errorWithDomain:NSStringFromClass(self.class) code:status userInfo:nil];
        return;
    }
}

- (void)callBackError:(NSError *)error {
    self.isError = YES;
    if (error && self.errorCallBack) {
        dispatch_async(dispatch_get_main_queue(), ^{
            self.errorCallBack(error);
        });
    }
}

+ (CMSampleBufferRef)sampleBufferFromAudioBufferList:(AudioBufferList)buffers inTimeStamp:(const AudioTimeStamp *)inTimeStamp inNumberFrames:(UInt32)inNumberFrames description:(AudioStreamBasicDescription)description {
    CMSampleBufferRef sampleBuffer = NULL; // 待生成的 CMSampleBuffer 实例的引用。
    
    // 1、创建音频流的格式描述信息。
    CMFormatDescriptionRef format = NULL;
    OSStatus status = CMAudioFormatDescriptionCreate(kCFAllocatorDefault, &description, 0, NULL, 0, NULL, NULL, &format);
    if (status != noErr) {
        CFRelease(format);
        return nil;
    }
    
    // 2、处理音频帧的时间戳信息。
    mach_timebase_info_data_t info = {0, 0};
    mach_timebase_info(&info);
    uint64_t time = inTimeStamp->mHostTime;
    // 转换为纳秒。
    time *= info.numer;
    time /= info.denom;
    // PTS。
    CMTime presentationTime = CMTimeMake(time, 1000000000.0f);
    // 对于音频，PTS 和 DTS 是一样的。
    CMSampleTimingInfo timing = {CMTimeMake(1, description.mSampleRate), presentationTime, presentationTime};
    
    // 3、创建 CMSampleBuffer 实例。
    status = CMSampleBufferCreate(kCFAllocatorDefault, NULL, false, NULL, NULL, format, (CMItemCount) inNumberFrames, 1, &timing, 0, NULL, &sampleBuffer);
    if (status != noErr) {
        CFRelease(format);
        return nil;
    }
    
    // 4、创建 CMBlockBuffer 实例。其中数据拷贝自 AudioBufferList，并将 CMBlockBuffer 实例关联到 CMSampleBuffer 实例。
    status = CMSampleBufferSetDataBufferFromAudioBufferList(sampleBuffer, kCFAllocatorDefault, kCFAllocatorDefault, 0, &buffers);
    if (status != noErr) {
        CFRelease(format);
        return nil;
    }
    
    CFRelease(format);
    return sampleBuffer;
}

#pragma mark - Capture CallBack
static OSStatus audioBufferCallBack(void *inRefCon,
                                    AudioUnitRenderActionFlags *ioActionFlags,
                                    const AudioTimeStamp *inTimeStamp,
                                    UInt32 inBusNumber,
                                    UInt32 inNumberFrames,
                                    AudioBufferList *ioData) {
    @autoreleasepool {
        KFAudioCapture *capture = (__bridge KFAudioCapture *) inRefCon;
        if (!capture) {
            return -1;
        }
        
        // 1、创建 AudioBufferList 空间，用来接收采集回来的数据。
        AudioBuffer buffer;
        buffer.mData = NULL;
        buffer.mDataByteSize = 0;
        // 采集的时候设置了数据格式是 kAudioFormatLinearPCM，即声道交错格式，所以即使是双声道这里也设置 mNumberChannels 为 1。
        // 对于双声道的数据，会按照采样位深 16 bit 每组，一组接一组地进行两个声道数据的交错拼装。
        buffer.mNumberChannels = 1;
        AudioBufferList buffers;
        buffers.mNumberBuffers = 1;
        buffers.mBuffers[0] = buffer;
        
        // 2、获取音频 PCM 数据，存储到 AudioBufferList 中。
        // 这里有几个问题要说明清楚：
        // 1）每次回调会过来多少数据？
        // 按照上面采集音频参数的设置：PCM 为声道交错格式、每帧的声道数为 2、采样位深为 16 bit。这样每帧的字节数是 4 字节（左右声道各 2 字节）。
        // 返回数据的帧数是 inNumberFrames。这样一次回调回来的数据字节数是多少就是：mBytesPerFrame(4) * inNumberFrames。
        // 2）这个数据回调的频率跟音频采样率有关系吗？
        // 这个数据回调的频率与音频采样率（上面设置的 mSampleRate 44100）是没关系的。声道数、采样位深、采样率共同决定了设备单位时间里采样数据的大小，这些数据是会缓冲起来，然后一块一块的通过这个数据回调给我们，这个回调的频率是底层一块一块给我们数据的速度，跟采样率无关。
        // 3）这个数据回调的频率是多少？
        // 这个数据回调的间隔是 [AVAudioSession sharedInstance].preferredIOBufferDuration，频率即该值的倒数。我们可以通过 [[AVAudioSession sharedInstance] setPreferredIOBufferDuration:1 error:nil] 设置这个值来控制回调频率。
        OSStatus status = AudioUnitRender(capture.audioCaptureInstance,
                                          ioActionFlags,
                                          inTimeStamp,
                                          inBusNumber,
                                          inNumberFrames,
                                          &buffers);
        
        // 3、数据封装及回调。
        if (status == noErr) {
            // 使用工具方法将数据封装为 CMSampleBuffer。
            CMSampleBufferRef sampleBuffer = [KFAudioCapture sampleBufferFromAudioBufferList:buffers inTimeStamp:inTimeStamp inNumberFrames:inNumberFrames description:capture.audioFormat];
            // 回调数据。
            if (capture.sampleBufferOutputCallBack) {
                capture.sampleBufferOutputCallBack(sampleBuffer);
            }
            if (sampleBuffer) {
                CFRelease(sampleBuffer);
            }
        }
        
        return status;
    }
}

@end
```

上面是 `KFAudioCapture` 的实现，从代码上可以看到主要有这几个部分：

- 1）创建音频采集实例。第一次调用 `-startRunning` 才会创建音频采集实例。

- - 在 `-setupAudioCaptureInstance:` 方法中实现。

- 2）处理音频采集实例的数据回调，并在回调中将数据封装到 `CMSampleBufferRef` 结构中，抛给 KFAudioCapture 的对外数据回调接口。

- - 在 `audioBufferCallBack(...)` 方法中实现回调处理逻辑。
  - 其中封装 `CMSampleBufferRef` 用到了 `+sampleBufferFromAudioBufferList:inTimeStamp:inNumberFrames:description:` 方法。

- 3）实现开始采集和停止采集逻辑。

- - 分别在 `-startRunning` 和 `-stopRunning` 方法中实现。注意，这里是开始和停止操作都是放在串行队列中通过 `dispatch_async` 异步处理的，这里主要是为了防止主线程卡顿。

- 4）捕捉音频采集开始和停止操作中的错误，抛给 KFAudioCapture 的对外错误回调接口。

- - 在 `-startRunning` 和 `-stopRunning` 方法中捕捉错误，在 `-callBackError:` 方法向外回调。

- 5）清理音频采集实例。

- - 在 `-dealloc` 方法中实现。

更具体细节见上述代码及其注释。

## 2、采集音频存储为 PCM 文件

我们在一个 ViewController 中来实现音频采集逻辑并将采集的音频存储为 PCM 数据。

```
KFAudioCaptureViewController.m
#import "KFAudioCaptureViewController.h"
#import <AVFoundation/AVFoundation.h>
#import "KFAudioCapture.h"

@interface KFAudioCaptureViewController ()
@property (nonatomic, strong) KFAudioConfig *audioConfig;
@property (nonatomic, strong) KFAudioCapture *audioCapture;
@property (nonatomic, strong) NSFileHandle *fileHandle;
@end

@implementation KFAudioCaptureViewController
#pragma mark - Property
- (KFAudioConfig *)audioConfig {
    if (!_audioConfig) {
        _audioConfig = [KFAudioConfig defaultConfig];
    }
    
    return _audioConfig;
}

- (KFAudioCapture *)audioCapture {
    if (!_audioCapture) {
        __weak typeof(self) weakSelf = self;
        _audioCapture = [[KFAudioCapture alloc] initWithConfig:self.audioConfig];
        _audioCapture.errorCallBack = ^(NSError* error) {
            NSLog(@"KFAudioCapture error: %zi %@", error.code, error.localizedDescription);
        };
        // 音频采集数据回调。在这里将 PCM 数据写入文件。
        _audioCapture.sampleBufferOutputCallBack = ^(CMSampleBufferRef sampleBuffer) {
            if (sampleBuffer) {
                // 1、获取 CMBlockBuffer，这里面封装着 PCM 数据。
                CMBlockBufferRef blockBuffer = CMSampleBufferGetDataBuffer(sampleBuffer);
                size_t lengthAtOffsetOutput, totalLengthOutput;
                char *dataPointer;
                
                // 2、从 CMBlockBuffer 中获取 PCM 数据存储到文件中。
                CMBlockBufferGetDataPointer(blockBuffer, 0, &lengthAtOffsetOutput, &totalLengthOutput, &dataPointer);
                [weakSelf.fileHandle writeData:[NSData dataWithBytes:dataPointer length:totalLengthOutput]];
            }
        };
    }
    
    return _audioCapture;
}

- (NSFileHandle *)fileHandle {
    if (!_fileHandle) {
        NSString *audioPath = [[NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) lastObject] stringByAppendingPathComponent:@"test.pcm"];
        NSLog(@"PCM file path: %@", audioPath);
        [[NSFileManager defaultManager] removeItemAtPath:audioPath error:nil];
        [[NSFileManager defaultManager] createFileAtPath:audioPath contents:nil attributes:nil];
        _fileHandle = [NSFileHandle fileHandleForWritingAtPath:audioPath];
    }

    return _fileHandle;
}

#pragma mark - Lifecycle
- (void)viewDidLoad {
    [super viewDidLoad];
    
    [self setupAudioSession];
    [self setupUI];
    
    // 完成音频采集后，可以将 App Document 文件夹下面的 test.pcm 文件拷贝到电脑上，使用 ffplay 播放：
    // ffplay -ar 44100 -channels 2 -f s16le -i test.pcm
}

- (void)dealloc {
    if (_fileHandle) {
        [_fileHandle closeFile];
    }
}

#pragma mark - Setup
- (void)setupUI {
    self.edgesForExtendedLayout = UIRectEdgeAll;
    self.extendedLayoutIncludesOpaqueBars = YES;
    self.title = @"Audio Capture";
    self.view.backgroundColor = [UIColor whiteColor];
    
    
    // Navigation item.
    UIBarButtonItem *startBarButton = [[UIBarButtonItem alloc] initWithTitle:@"Start" style:UIBarButtonItemStylePlain target:self action:@selector(start)];
    UIBarButtonItem *stopBarButton = [[UIBarButtonItem alloc] initWithTitle:@"Stop" style:UIBarButtonItemStylePlain target:self action:@selector(stop)];
    self.navigationItem.rightBarButtonItems = @[startBarButton, stopBarButton];

}

- (void)setupAudioSession {
    NSError *error = nil;
    
    // 1、获取音频会话实例。
    AVAudioSession *session = [AVAudioSession sharedInstance];

    // 2、设置分类和选项。
    [session setCategory:AVAudioSessionCategoryPlayAndRecord withOptions:AVAudioSessionCategoryOptionMixWithOthers | AVAudioSessionCategoryOptionDefaultToSpeaker error:&error];
    if (error) {
        NSLog(@"AVAudioSession setCategory error.");
        error = nil;
        return;
    }
    
    // 3、设置模式。
    [session setMode:AVAudioSessionModeVideoRecording error:&error];
    if (error) {
        NSLog(@"AVAudioSession setMode error.");
        error = nil;
        return;
    }

    // 4、激活会话。
    [session setActive:YES error:&error];
    if (error) {
        NSLog(@"AVAudioSession setActive error.");
        error = nil;
        return;
    }
}

#pragma mark - Action
- (void)start {
    [self.audioCapture startRunning];
}

- (void)stop {
    [self.audioCapture stopRunning];
}

@end
```

上面是 `KFAudioCaptureViewController` 的实现，这里需要注意的是在采集音频前需要设置 **AVAudioSession**[6] 为正确的采集模式。

## 3、用工具播放 PCM 文件

完成音频采集后，可以将 App Document 文件夹下面的 `test.pcm` 文件拷贝到电脑上，使用 `ffplay` 播放来验证一下音频采集是效果是否符合预期：

```
$ ffplay -ar 44100 -channels 2 -f s16le -i test.pcm
```

注意这里的参数要对齐在工程代码中设置的`采样率`、`声道数`、`采样位深`。

关于播放 PCM 文件的工具，可以参考[《FFmpeg 工具》第 2 节 ffplay 命令行工具](https://mp.weixin.qq.com/s?__biz=MjM5MTkxOTQyMQ==&mid=2257484831&idx=1&sn=6bab905a5040c46b971bab05f787788b&scene=21#wechat_redirect)和[《可视化音视频分析工具》第 1.1 节 Adobe Audition](https://mp.weixin.qq.com/s?__biz=MjM5MTkxOTQyMQ==&mid=2257484834&idx=1&sn=5dd9768bfc0d01ca1b036be8dd2f5fa1&scene=21#wechat_redirect)。

## 4、参考资料

[1]CMSampleBufferRef: *https://developer.apple.com/documentation/coremedia/cmsamplebufferref/*

[2]CMSampleBuffer: *https://developer.apple.com/documentation/coremedia/cmsamplebuffer-u71*

[3]CMBlockBuffer: *https://developer.apple.com/documentation/coremedia/cmblockbuffer-u9i*

[4]CVImageBuffer: *https://developer.apple.com/documentation/corevideo/cvimagebuffer-q40*

[5]CVPixelBuffer: *https://developer.apple.com/documentation/corevideo/cvpixelbuffer-q2e*

[6]AVAudioSession: *https://developer.apple.com/documentation/avfaudio/avaudiosession/*





原文链接：https://mp.weixin.qq.com/s/FDR_5cMfAJQgZhSvjgeWYA