# iOS AVDemo（6）：音频渲染

iOS/Android 客户端开发同学如果想要开始学习音视频开发，最丝滑的方式是对[音视频基础概念知识](https://mp.weixin.qq.com/mp/appmsgalbum?__biz=MjM5MTkxOTQyMQ==&action=getalbum&album_id=2140155659944787969#wechat_redirect)有一定了解后，再借助 iOS/Android 平台的音视频能力上手去实践音视频的`采集 → 编码 → 封装 → 解封装 → 解码 → 渲染`过程，并借助[音视频工具](https://mp.weixin.qq.com/mp/appmsgalbum?__biz=MjM5MTkxOTQyMQ==&action=getalbum&album_id=2216997905264082945#wechat_redirect)来分析和理解对应的音视频数据。

在[音视频工程示例](https://mp.weixin.qq.com/mp/appmsgalbum?__biz=MjM5MTkxOTQyMQ==&action=getalbum&album_id=2273301900659851268#wechat_redirect)这个栏目，我们将通过拆解`采集 → 编码 → 封装 → 解封装 → 解码 → 渲染`流程并实现 Demo 来向大家介绍如何在 iOS/Android 平台上手音视频开发。

这里是第六篇：**iOS 音频渲染 Demo**。这个 Demo 里包含以下内容：

- 1）实现一个音频解封装模块；
- 2）实现一个音频解码模块；
- 3）实现一个音频渲染模块；
- 4）实现对 MP4 文件中音频部分的解封装和解码逻辑，并将解封装、解码后的数据送给渲染模块播放；
- 5）详尽的代码注释，帮你理解代码逻辑和原理。

## 1、音频解封装模块

在这个 Demo 中，解封装模块 `KFMP4Demuxer` 的实现与 [《iOS 音频解封装 Demo》](https://mp.weixin.qq.com/s?__biz=MjM5MTkxOTQyMQ==&mid=2257484932&idx=1&sn=04fa6fb220574c0a5d417f4b527c0142&scene=21#wechat_redirect) 中一样，这里就不再重复介绍了，其接口如下：

```
KFMP4Demuxer.h
#import <Foundation/Foundation.h>
#import <CoreMedia/CoreMedia.h>
#import "KFDemuxerConfig.h"

NS_ASSUME_NONNULL_BEGIN

typedef NS_ENUM(NSInteger, KFMP4DemuxerStatus) {
    KFMP4DemuxerStatusUnknown = 0,
    KFMP4DemuxerStatusRunning = 1,
    KFMP4DemuxerStatusFailed = 2,
    KFMP4DemuxerStatusCompleted = 3,
    KFMP4DemuxerStatusCancelled = 4,
};

@interface KFMP4Demuxer : NSObject
+ (instancetype)new NS_UNAVAILABLE;
- (instancetype)init NS_UNAVAILABLE;
- (instancetype)initWithConfig:(KFDemuxerConfig *)config;

@property (nonatomic, strong, readonly) KFDemuxerConfig *config;
@property (nonatomic, copy) void (^errorCallBack)(NSError *error);
@property (nonatomic, assign, readonly) BOOL hasAudioTrack; // 是否包含音频数据。
@property (nonatomic, assign, readonly) BOOL hasVideoTrack; // 是否包含视频数据。
@property (nonatomic, assign, readonly) CGSize videoSize; // 视频大小。
@property (nonatomic, assign, readonly) CMTime duration; // 媒体时长。
@property (nonatomic, assign, readonly) CMVideoCodecType codecType; // 编码类型。
@property (nonatomic, assign, readonly) KFMP4DemuxerStatus demuxerStatus; // 解封装器状态。
@property (nonatomic, assign, readonly) BOOL audioEOF; // 是否音频结束。
@property (nonatomic, assign, readonly) BOOL videoEOF; // 是否视频结束。
@property (nonatomic, assign, readonly) CGAffineTransform preferredTransform; // 图像的变换信息。比如：视频图像旋转。

- (void)startReading:(void (^)(BOOL success, NSError *error))completeHandler; // 开始读取数据解封装。
- (void)cancelReading; // 取消读取。

- (BOOL)hasAudioSampleBuffer; // 是否还有音频数据。
- (CMSampleBufferRef)copyNextAudioSampleBuffer CF_RETURNS_RETAINED; // 拷贝下一份音频采样。

- (BOOL)hasVideoSampleBuffer; // 是否还有视频数据。
- (CMSampleBufferRef)copyNextVideoSampleBuffer CF_RETURNS_RETAINED; // 拷贝下一份视频采样。
@end

NS_ASSUME_NONNULL_END
```

## 2、音频解码模块

同样的，解封装模块 `KFAudioDecoder` 的实现与 [《iOS 音频解码 Demo》](https://mp.weixin.qq.com/s?__biz=MjM5MTkxOTQyMQ==&mid=2257484944&idx=1&sn=63616655888d93557f935bc12088873e&scene=21#wechat_redirect) 中一样，这里就不再重复介绍了，其接口如下：

```
KFAudioDecoder.h
#import <Foundation/Foundation.h>
#import <CoreMedia/CoreMedia.h>

NS_ASSUME_NONNULL_BEGIN

@interface KFAudioDecoder : NSObject
@property (nonatomic, copy) void (^sampleBufferOutputCallBack)(CMSampleBufferRef sample); // 解码器数据回调。
@property (nonatomic, copy) void (^errorCallBack)(NSError *error); // 解码器错误回调。

- (void)decodeSampleBuffer:(CMSampleBufferRef)sampleBuffer; // 解码。
@end

NS_ASSUME_NONNULL_END
```

## 3、音频渲染模块

接下来，我们来实现一个音频渲染模块 `KFAudioRender`，在这里输入解码后的数据进行渲染播放。

```
KFAudioRender.h
#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>

@class KFAudioRender;

NS_ASSUME_NONNULL_BEGIN

@interface KFAudioRender : NSObject
+ (instancetype)new NS_UNAVAILABLE;
- (instancetype)init NS_UNAVAILABLE;
- (instancetype)initWithChannels:(NSInteger)channels bitDepth:(NSInteger)bitDepth sampleRate:(NSInteger)sampleRate;

@property (nonatomic, copy) void (^audioBufferInputCallBack)(AudioBufferList *audioBufferList); // 音频渲染数据输入回调。
@property (nonatomic, copy) void (^errorCallBack)(NSError *error); // 音频渲染错误回调。
@property (nonatomic, assign, readonly) NSInteger audioChannels; // 声道数。
@property (nonatomic, assign, readonly) NSInteger bitDepth; // 采样位深。
@property (nonatomic, assign, readonly) NSInteger audioSampleRate; // 采样率。

- (void)startPlaying; // 开始渲染。
- (void)stopPlaying; // 结束渲染。
@end

NS_ASSUME_NONNULL_END
```

上面是 `KFAudioRender` 接口的设计，除了`初始化`接口，主要是有音频渲染`数据输入回调`和`错误回调`的接口，另外就是`获取声道数`和`获取采样率`的接口，以及`开始渲染`和`结束渲染`的接口。

这里重点需要看一下音频渲染`数据输入回调`接口，系统的音频渲染单元每次会主动通过回调的方式要数据，我们这里封装的 `KFAudioRender` 则是用`数据输入回调`接口来从外部获取一组待渲染的音频数据送给系统的音频渲染单元。

```
KFAudioRender.m
#import "KFAudioRender.h"

#define OutputBus 0

@interface KFAudioRender ()
@property (nonatomic, assign) AudioComponentInstance audioRenderInstance; // 音频渲染实例。
@property (nonatomic, assign, readwrite) NSInteger audioChannels; // 声道数。
@property (nonatomic, assign, readwrite) NSInteger bitDepth; // 采样位深。
@property (nonatomic, assign, readwrite) NSInteger audioSampleRate; // 采样率。
@property (nonatomic, strong) dispatch_queue_t renderQueue;
@property (nonatomic, assign) BOOL isError;
@end

@implementation KFAudioRender
#pragma mark - Lifecycle
- (instancetype)initWithChannels:(NSInteger)channels bitDepth:(NSInteger)bitDepth sampleRate:(NSInteger)sampleRate {
    self = [super init];
    if (self) {
        _audioChannels = channels;
        _bitDepth = bitDepth;
        _audioSampleRate = sampleRate;
        _renderQueue = dispatch_queue_create("com.KeyFrameKit.audioRender", DISPATCH_QUEUE_SERIAL);
    }
    
    return self;
}

- (void)dealloc {
    // 清理音频渲染实例。
    if (_audioRenderInstance) {
        AudioOutputUnitStop(_audioRenderInstance);
        AudioUnitUninitialize(_audioRenderInstance);
        AudioComponentInstanceDispose(_audioRenderInstance);
        _audioRenderInstance = nil;
    }
}

#pragma mark - Action
- (void)startPlaying {
    __weak typeof(self) weakSelf = self;
    dispatch_async(_renderQueue, ^{
        if (!weakSelf.audioRenderInstance) {
            NSError *error = nil;
            // 第一次 startPlaying 时创建音频渲染实例。
            [weakSelf _setupAudioRenderInstance:&error];
            if (error) {
                // 捕捉并回调创建音频渲染实例时的错误。
                [weakSelf _callBackError:error];
                return;
            }
        }
        
        // 开始渲染。
        OSStatus status = AudioOutputUnitStart(weakSelf.audioRenderInstance);
        if (status != noErr) {
            // 捕捉并回调开始渲染时的错误。
            [weakSelf _callBackError:[NSError errorWithDomain:NSStringFromClass([KFAudioRender class]) code:status userInfo:nil]];
        }
    });
}

- (void)stopPlaying {
    __weak typeof(self) weakSelf = self;
    dispatch_async(_renderQueue, ^{
        if (weakSelf.audioRenderInstance && !self.isError) {
            // 停止渲染。
            OSStatus status = AudioOutputUnitStop(weakSelf.audioRenderInstance);
            // 捕捉并回调停止渲染时的错误。
            if (status != noErr) {
                [weakSelf _callBackError:[NSError errorWithDomain:NSStringFromClass([KFAudioRender class]) code:status userInfo:nil]];
            }
        }
    });
}

#pragma mark - Private Method
- (void)_setupAudioRenderInstance:(NSError**)error {
    // 1、设置音频组件描述。
    AudioComponentDescription audioComponentDescription = {
        .componentType = kAudioUnitType_Output,
        //.componentSubType = kAudioUnitSubType_VoiceProcessingIO, // 回声消除模式
        .componentSubType = kAudioUnitSubType_RemoteIO,
        .componentManufacturer = kAudioUnitManufacturer_Apple,
        .componentFlags = 0,
        .componentFlagsMask = 0
    };
    
    // 2、查找符合指定描述的音频组件。
    AudioComponent inputComponent = AudioComponentFindNext(NULL, &audioComponentDescription);

    // 3、创建音频组件实例。
    OSStatus status = AudioComponentInstanceNew(inputComponent, &_audioRenderInstance);
    if (status != noErr) {
        *error = [NSError errorWithDomain:NSStringFromClass(self.class) code:status userInfo:nil];
        return;
    }
    
    // 4、设置实例的属性：可读写。0 不可读写，1 可读写。
    UInt32 flag = 1;
    status = AudioUnitSetProperty(_audioRenderInstance, kAudioOutputUnitProperty_EnableIO, kAudioUnitScope_Output, OutputBus, &flag, sizeof(flag));
    if (status != noErr) {
        *error = [NSError errorWithDomain:NSStringFromClass(self.class) code:status userInfo:nil];
        return;
    }
    
    // 5、设置实例的属性：音频参数，如：数据格式、声道数、采样位深、采样率等。
    AudioStreamBasicDescription inputFormat = {0};
    inputFormat.mFormatID = kAudioFormatLinearPCM; // 原始数据为 PCM，采用声道交错格式。
    inputFormat.mFormatFlags = kAudioFormatFlagIsSignedInteger | kAudioFormatFlagsNativeEndian | kAudioFormatFlagIsPacked;
    inputFormat.mChannelsPerFrame = (UInt32) self.audioChannels; // 每帧的声道数。
    inputFormat.mFramesPerPacket = 1; // 每个数据包帧数。
    inputFormat.mBitsPerChannel = (UInt32) self.bitDepth; // 采样位深。
    inputFormat.mBytesPerFrame = inputFormat.mChannelsPerFrame * inputFormat.mBitsPerChannel / 8; // 每帧字节数 (byte = bit / 8)。
    inputFormat.mBytesPerPacket = inputFormat.mFramesPerPacket * inputFormat.mBytesPerFrame; // 每个包字节数。
    inputFormat.mSampleRate = self.audioSampleRate; // 采样率
    status = AudioUnitSetProperty(_audioRenderInstance, kAudioUnitProperty_StreamFormat, kAudioUnitScope_Input, OutputBus, &inputFormat, sizeof(inputFormat));
    if (status != noErr) {
        *error = [NSError errorWithDomain:NSStringFromClass(self.class) code:status userInfo:nil];
        return;
    }

    // 6、设置实例的属性：数据回调函数。
    AURenderCallbackStruct renderCallbackRef = {
        .inputProc = audioRenderCallback,
        .inputProcRefCon = (__bridge void *) (self) // 对应回调函数中的 *inRefCon。
    };
    status = AudioUnitSetProperty(_audioRenderInstance, kAudioUnitProperty_SetRenderCallback, kAudioUnitScope_Global, OutputBus, &renderCallbackRef, sizeof(renderCallbackRef));
    if (status != noErr) {
        *error = [NSError errorWithDomain:NSStringFromClass(self.class) code:status userInfo:nil];
        return;
    }
    
    // 7、初始化实例。
    status = AudioUnitInitialize(_audioRenderInstance);
    if (status != noErr) {
        *error = [NSError errorWithDomain:NSStringFromClass(self.class) code:status userInfo:nil];
        return;
    }
}

- (void)_callBackError:(NSError*)error {
    self.isError = YES;
    if (self.errorCallBack) {
        dispatch_async(dispatch_get_main_queue(), ^{
            self.errorCallBack(error);
        });
    }
}

#pragma mark - Render Callback
static OSStatus audioRenderCallback(void *inRefCon, AudioUnitRenderActionFlags *ioActionFlags, const AudioTimeStamp *inTimeStamp, UInt32 inOutputBusNumber, UInt32 inNumberFrames, AudioBufferList *ioData) {
    // 通过音频渲染数据输入回调从外部获取待渲染的数据。
    KFAudioRender *audioRender = (__bridge KFAudioRender *) inRefCon;
    if (audioRender.audioBufferInputCallBack) {
        audioRender.audioBufferInputCallBack(ioData);
    }
    
    return noErr;
}

@end
```

上面是 `KFAudioRender` 的实现，从代码上可以看到主要有这几个部分：

- 1）创建音频渲染实例。第一次调用 `-startPlaying` 才会创建音频渲染实例。

- - 在 `-_setupAudioRenderInstance:` 方法中实现。

- 2）处理音频渲染实例的数据回调，并在回调中通过 KFAudioRender 的对外数据输入回调接口向更外层要待渲染的数据。

- - 在 `audioRenderCallback(...)` 方法中实现回调处理逻辑。通过 `audioBufferInputCallBack` 回调接口向更外层要数据。

- 3）实现开始渲染和停止渲染逻辑。

- - 分别在 `-startPlaying` 和 `-stopPlaying` 方法中实现。注意，这里是开始和停止操作都是放在串行队列中通过 `dispatch_async` 异步处理的，这里主要是为了防止主线程卡顿。

- 4）捕捉音频渲染开始和停止操作中的错误，抛给 KFAudioRender 的对外错误回调接口。

- - 在 `-startPlaying` 和 `-stopPlaying` 方法中捕捉错误，在 `-_callBackError:` 方法向外回调。

- 5）清理音频渲染实例。

- - 在 `-dealloc` 方法中实现。

更具体细节见上述代码及其注释。

## 4、解封装和解码 MP4 文件中的音频部分并渲染播放

我们在一个 ViewController 中来实现从 MP4 文件中解封装和解码音频数据进行渲染播放。

```
KFAudioRenderViewController.m
#import "KFAudioRenderViewController.h"
#import <AVFoundation/AVFoundation.h>
#import "KFAudioRender.h"
#import "KFMP4Demuxer.h"
#import "KFAudioDecoder.h"
#import "KFWeakProxy.h"

#define KFDecoderMaxCache 4096 * 5 // 解码数据缓冲区最大长度。

@interface KFAudioRenderViewController ()
@property (nonatomic, strong) KFDemuxerConfig *demuxerConfig;
@property (nonatomic, strong) KFMP4Demuxer *demuxer;
@property (nonatomic, strong) KFAudioDecoder *decoder;
@property (nonatomic, strong) KFAudioRender *audioRender;
@property (nonatomic, strong) dispatch_semaphore_t semaphore;
@property (nonatomic, strong) NSMutableData *pcmDataCache; // 解码数据缓冲区。
@property (nonatomic, assign) NSInteger pcmDataCacheLength;
@property (nonatomic, strong) CADisplayLink *timer;
@end

@implementation KFAudioRenderViewController
#pragma mark - Property
- (KFDemuxerConfig *)demuxerConfig {
    if (!_demuxerConfig) {
        _demuxerConfig = [[KFDemuxerConfig alloc] init];
        _demuxerConfig.demuxerType = KFMediaAudio;
        NSString *videoPath = [[NSBundle mainBundle] pathForResource:@"input" ofType:@"mp4"];
        _demuxerConfig.asset = [AVAsset assetWithURL:[NSURL fileURLWithPath:videoPath]];
    }
    
    return _demuxerConfig;
}

- (KFMP4Demuxer *)demuxer {
    if (!_demuxer) {
        _demuxer = [[KFMP4Demuxer alloc] initWithConfig:self.demuxerConfig];
        _demuxer.errorCallBack = ^(NSError *error) {
            NSLog(@"KFMP4Demuxer error:%zi %@", error.code, error.localizedDescription);
        };
    }
    
    return _demuxer;
}

- (KFAudioDecoder *)decoder {
    if (!_decoder) {
        __weak typeof(self) weakSelf = self;
        _decoder = [[KFAudioDecoder alloc] init];
        _decoder.errorCallBack = ^(NSError *error) {
            NSLog(@"KFAudioDecoder error:%zi %@", error.code, error.localizedDescription);
        };
        // 解码数据回调。在这里把解码后的音频 PCM 数据缓冲起来等待渲染。
        _decoder.sampleBufferOutputCallBack = ^(CMSampleBufferRef sampleBuffer) {
            if (sampleBuffer) {
                CMBlockBufferRef blockBuffer = CMSampleBufferGetDataBuffer(sampleBuffer);
                size_t totolLength;
                char *dataPointer = NULL;
                CMBlockBufferGetDataPointer(blockBuffer, 0, NULL, &totolLength, &dataPointer);
                if (totolLength == 0 || !dataPointer) {
                    return;
                }
                dispatch_semaphore_wait(weakSelf.semaphore, DISPATCH_TIME_FOREVER);
                [weakSelf.pcmDataCache appendData:[NSData dataWithBytes:dataPointer length:totolLength]];
                weakSelf.pcmDataCacheLength += totolLength;
                dispatch_semaphore_signal(weakSelf.semaphore);
            }
        };
    }
    
    return _decoder;
}

- (KFAudioRender *)audioRender {
    if (!_audioRender) {
        __weak typeof(self) weakSelf = self;
        // 这里设置的音频声道数、采样位深、采样率需要跟输入源的音频参数一致。
        _audioRender = [[KFAudioRender alloc] initWithChannels:1 bitDepth:16 sampleRate:44100];
        _audioRender.errorCallBack = ^(NSError* error) {
            NSLog(@"KFAudioRender error:%zi %@", error.code, error.localizedDescription);
        };
        // 渲染输入数据回调。在这里把缓冲区的数据交给系统音频渲染单元渲染。
        _audioRender.audioBufferInputCallBack = ^(AudioBufferList * _Nonnull audioBufferList) {
            if (weakSelf.pcmDataCacheLength < audioBufferList->mBuffers[0].mDataByteSize) {
                memset(audioBufferList->mBuffers[0].mData, 0, audioBufferList->mBuffers[0].mDataByteSize);
            } else {
                dispatch_semaphore_wait(weakSelf.semaphore, DISPATCH_TIME_FOREVER);
                memcpy(audioBufferList->mBuffers[0].mData, weakSelf.pcmDataCache.bytes, audioBufferList->mBuffers[0].mDataByteSize);
                [weakSelf.pcmDataCache replaceBytesInRange:NSMakeRange(0, audioBufferList->mBuffers[0].mDataByteSize) withBytes:NULL length:0];
                weakSelf.pcmDataCacheLength -= audioBufferList->mBuffers[0].mDataByteSize;
                dispatch_semaphore_signal(weakSelf.semaphore);
            }
        };
    }
    
    return _audioRender;
}

#pragma mark - Lifecycle
- (void)viewDidLoad {
    [super viewDidLoad];
        
    _semaphore = dispatch_semaphore_create(1);
    _pcmDataCache = [[NSMutableData alloc] init];
    
    [self setupAudioSession];
    [self setupUI];
    
    // 通过一个 timer 来保证持续从文件中解封装和解码一定量的数据。
    _timer = [CADisplayLink displayLinkWithTarget:[KFWeakProxy proxyWithTarget:self] selector:@selector(timerCallBack:)];
    [_timer addToRunLoop:[NSRunLoop mainRunLoop] forMode:NSRunLoopCommonModes];
    [_timer setPaused:NO];
    
    [self.demuxer startReading:^(BOOL success, NSError * _Nonnull error) {
        NSLog(@"KFMP4Demuxer start:%d", success);
    }];
}

- (void)dealloc {
    
}

#pragma mark - Setup
- (void)setupUI {
    self.edgesForExtendedLayout = UIRectEdgeAll;
    self.extendedLayoutIncludesOpaqueBars = YES;
    self.title = @"Audio Render";
    self.view.backgroundColor = [UIColor whiteColor];
    
    
    // Navigation item.
    UIBarButtonItem *startRenderBarButton = [[UIBarButtonItem alloc] initWithTitle:@"Start" style:UIBarButtonItemStylePlain target:self action:@selector(startRender)];
    UIBarButtonItem *stopRenderBarButton = [[UIBarButtonItem alloc] initWithTitle:@"Stop" style:UIBarButtonItemStylePlain target:self action:@selector(stopRender)];
    self.navigationItem.rightBarButtonItems = @[startRenderBarButton, stopRenderBarButton];
}

#pragma mark - Action
- (void)startRender {
    [self.audioRender startPlaying];
}

- (void)stopRender {
    [self.audioRender stopPlaying];
}

#pragma mark - Utility
- (void)setupAudioSession {
    // 1、获取音频会话实例。
    AVAudioSession *session = [AVAudioSession sharedInstance];
    
    // 2、设置分类。
    NSError *error = nil;
    [[AVAudioSession sharedInstance] setCategory:AVAudioSessionCategoryPlayback error:&error];
    if (error) {
        NSLog(@"AVAudioSession setCategory error");
    }
    
    // 3、激活会话。
    [session setActive:YES error:&error];
    if (error) {
        NSLog(@"AVAudioSession setActive error");
    }
}

- (void)timerCallBack:(CADisplayLink *)link {
    // 定时从文件中解封装和解码一定量（不超过 KFDecoderMaxCache）的数据。
    if (self.pcmDataCacheLength <  KFDecoderMaxCache && self.demuxer.demuxerStatus == KFMP4DemuxerStatusRunning && self.demuxer.hasAudioSampleBuffer) {
        CMSampleBufferRef audioBuffer = [self.demuxer copyNextAudioSampleBuffer];
        if (audioBuffer) {
            [self decodeSampleBuffer:audioBuffer];
            CFRelease(audioBuffer);
        }
    }
}

- (void)decodeSampleBuffer:(CMSampleBufferRef)sampleBuffer {
    // 获取解封装后的 AAC 编码裸数据。
    CMBlockBufferRef blockBuffer = CMSampleBufferGetDataBuffer(sampleBuffer);
    size_t totolLength;
    char *dataPointer = NULL;
    CMBlockBufferGetDataPointer(blockBuffer, 0, NULL, &totolLength, &dataPointer);
    if (totolLength == 0 || !dataPointer) {
        return;
    }
    
    // 目前 AudioDecoder 的解码接口实现的是单包（packet，1 packet 有 1024 帧）解码。而从 Demuxer 获取的一个 CMSampleBuffer 可能包含多个包，所以这里要拆一下包，再送给解码器。
    NSLog(@"SampleNum: %ld", CMSampleBufferGetNumSamples(sampleBuffer));
    for (NSInteger index = 0; index < CMSampleBufferGetNumSamples(sampleBuffer); index++) {
        // 1、获取一个包的数据。
        size_t sampleSize = CMSampleBufferGetSampleSize(sampleBuffer, index);
        CMSampleTimingInfo timingInfo;
        CMSampleBufferGetSampleTimingInfo(sampleBuffer, index, &timingInfo);
        char *sampleDataPointer = malloc(sampleSize);
        memcpy(sampleDataPointer, dataPointer, sampleSize);
        
        // 2、将数据封装到 CMBlockBuffer 中。
        CMBlockBufferRef packetBlockBuffer;
        OSStatus status = CMBlockBufferCreateWithMemoryBlock(kCFAllocatorDefault,
                                                              sampleDataPointer,
                                                              sampleSize,
                                                              NULL,
                                                              NULL,
                                                              0,
                                                              sampleSize,
                                                              0,
                                                              &packetBlockBuffer);
        
        if (status == noErr) {
            // 3、将 CMBlockBuffer 封装到 CMSampleBuffer 中。
            CMSampleBufferRef packetSampleBuffer = NULL;
            const size_t sampleSizeArray[] = {sampleSize};
            status = CMSampleBufferCreateReady(kCFAllocatorDefault,
                                               packetBlockBuffer,
                                               CMSampleBufferGetFormatDescription(sampleBuffer),
                                               1,
                                               1,
                                               &timingInfo,
                                               1,
                                               sampleSizeArray,
                                               &packetSampleBuffer);
            CFRelease(packetBlockBuffer);
            
            // 4、解码这个包的数据。
            if (packetSampleBuffer) {
                [self.decoder decodeSampleBuffer:packetSampleBuffer];
                CFRelease(packetSampleBuffer);
            }
        }
        dataPointer += sampleSize;
    }
}

@end
```

上面是 `KFAudioRenderViewController` 的实现，其中主要包含这几个部分：

- 1）在页面加载完成后就启动解封装和解码模块，并通过一个 timer 来驱动解封装器和解码器。

- - 在 `-viewDidLoad` 中实现。

- 2）定时从文件中解封装一定量（不超过 KFDecoderMaxCache）的数据送给解码器。

- - 在 `-timerCallBack:` 方法中实现。

- 3）解封装后，需要将数据拆包，以包为单位封装为 `CMSampleBuffer` 送给解码器解码。

- - 在 `-decodeSampleBuffer:` 方法中实现。

- 4）在解码模块 `KFAudioDecoder` 的数据回调中获取解码后的 PCM 数据缓冲起来等待渲染。

- - 在 `KFAudioDecoder` 的 `sampleBufferOutputCallBack` 回调中实现。

- 5）在渲染模块 `KFAudioRender` 的输入数据回调中把缓冲区的数据交给系统音频渲染单元渲染。

- - 在 `KFAudioRender` 的 `audioBufferInputCallBack` 回调中实现。

更具体细节见上述代码及其注释。



原文链接：https://mp.weixin.qq.com/s/xrt277Ia1OFP_XtwK1qlQg