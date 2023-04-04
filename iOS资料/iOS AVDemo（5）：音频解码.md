# iOS AVDemo（5）：音频解码

iOS/Android 客户端开发同学如果想要开始学习音视频开发，最丝滑的方式是对[音视频基础概念知识](https://mp.weixin.qq.com/mp/appmsgalbum?__biz=MjM5MTkxOTQyMQ==&action=getalbum&album_id=2140155659944787969#wechat_redirect)有一定了解后，再借助本地平台的音视频能力上手去实践音视频的`采集 → 编码 → 封装 → 解封装 → 解码 → 渲染`过程，并借助[音视频工具](https://mp.weixin.qq.com/mp/appmsgalbum?__biz=MjM5MTkxOTQyMQ==&action=getalbum&album_id=2216997905264082945#wechat_redirect)来分析和理解对应的音视频数据。

在[音视频工程示例](https://mp.weixin.qq.com/mp/appmsgalbum?__biz=MjM5MTkxOTQyMQ==&action=getalbum&album_id=2273301900659851268#wechat_redirect)这个栏目，我们将通过拆解`采集 → 编码 → 封装 → 解封装 → 解码 → 渲染`流程并实现 Demo 来向大家介绍如何在 iOS/Android 平台上手音视频开发。

这里是第五篇：**iOS 音频解码 Demo**。这个 Demo 里包含以下内容：

- 1）实现一个音频解封装模块；
- 2）实现一个音频解码模块；
- 3）实现对 MP4 文件中音频部分的解封装和解码逻辑，并将解封装、解码后的数据存储为 PCM 文件；
- 4）详尽的代码注释，帮你理解代码逻辑和原理。

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

接下来，我们来实现一个音频解码模块 `KFAudioDecoder`，在这里输入解封装后的编码数据，输出解码后的数据。

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

上面是 `KFAudioDecoder` 接口的设计，主要是有音频解码`数据回调`和`错误回调`的接口，另外就是`解码`的接口。

在上面的`解码`接口和`解码器数据回调`接口中，我们使用的是依然 **CMSampleBufferRef**[1] 作为参数或返回值类型。

在`解码`接口中，我们通过 `CMSampleBufferRef` 打包的是解封装后得到的 AAC 编码数据。

在`解码器数据回调`接口中，我们通过 `CMSampleBufferRef` 打包的是对 AAC 解码后得到的音频 PCM 数据。

```
KFAudioDecoder.m
#import "KFAudioDecoder.h"
#import <AudioToolbox/AudioToolbox.h>

// 自定义数据，用于封装音频解码回调中用到的数据。
typedef struct KFAudioUserData {
    UInt32 mChannels;
    UInt32 mDataSize;
    void *mData;
    AudioStreamPacketDescription mPacketDesc;
} KFAudioUserData;

@interface KFAudioDecoder () {
    UInt8 *_pcmBuffer; // 解码缓冲区。
}
@property (nonatomic, assign) AudioConverterRef audioDecoderInstance; // 音频解码器实例。
@property (nonatomic, assign) CMFormatDescriptionRef pcmFormat; // 音频解码参数。
@property (nonatomic, strong) dispatch_queue_t decoderQueue;
@property (nonatomic, assign) BOOL isError;
@end

@implementation KFAudioDecoder
#pragma mark - Lifecycle
- (instancetype)init {
    self = [super init];
    if (self) {
        _decoderQueue = dispatch_queue_create("com.KeyFrameKit.audioDecoder", DISPATCH_QUEUE_SERIAL);
    }
    
    return self;
}

- (void)dealloc {
    // 清理解码器。
    if (_audioDecoderInstance) {
        AudioConverterDispose(_audioDecoderInstance);
        _audioDecoderInstance = nil;
    }
    if (_pcmFormat) {
        CFRelease(_pcmFormat);
        _pcmFormat = NULL;
    }
    
    // 清理缓冲区。
    if (_pcmBuffer) {
        free(_pcmBuffer);
        _pcmBuffer = NULL;
    }
}

#pragma mark - Public Method
- (void)decodeSampleBuffer:(CMSampleBufferRef)sampleBuffer {
    if (!sampleBuffer || !CMSampleBufferGetDataBuffer(sampleBuffer) || self.isError) {
        return;
    }
    
    // 异步处理，防止主线程卡顿。
    __weak typeof(self) weakSelf = self;
    CFRetain(sampleBuffer);
    dispatch_async(_decoderQueue, ^{
        [weakSelf _decodeSampleBuffer:sampleBuffer];
        CFRelease(sampleBuffer);
    });
}

#pragma mark - Private Method
- (void)_setupAudioDecoderInstanceWithInputAudioFormat:(AudioStreamBasicDescription)inputFormat error:(NSError **)error{
    if (_audioDecoderInstance != nil) {
        return;
    }
    
    // 1、设置音频解码器输出参数。其中一些参数与输入的音频数据参数一致。
    AudioStreamBasicDescription outputFormat = {0};
    outputFormat.mSampleRate = inputFormat.mSampleRate; // 输出采样率与输入一致。
    outputFormat.mFormatID = kAudioFormatLinearPCM; // 输出的 PCM 格式。
    outputFormat.mFormatFlags = kAudioFormatFlagIsSignedInteger | kAudioFormatFlagsNativeEndian | kAudioFormatFlagIsPacked;
    outputFormat.mChannelsPerFrame = (UInt32) inputFormat.mChannelsPerFrame; // 输出声道数与输入一致。
    outputFormat.mFramesPerPacket = 1; // 每个包的帧数。对于 PCM 这样的非压缩音频数据，设置为 1。
    outputFormat.mBitsPerChannel = 16; // 对于 PCM，表示采样位深。
    outputFormat.mBytesPerFrame = outputFormat.mChannelsPerFrame * outputFormat.mBitsPerChannel / 8; // 每帧字节数 (byte = bit / 8)。
    outputFormat.mBytesPerPacket = outputFormat.mFramesPerPacket * outputFormat.mBytesPerFrame; // 每个包的字节数。
    outputFormat.mReserved = 0; // 对齐方式，0 表示 8 字节对齐。

    // 2、基于音频输入和输出参数创建音频解码器。
    OSStatus status = AudioConverterNew(&inputFormat, &outputFormat, &_audioDecoderInstance);
    if (status != 0) {
        *error = [NSError errorWithDomain:NSStringFromClass(self.class) code:status userInfo:nil];
    }
    
    // 3、创建编码格式信息 _pcmFormat。
    OSStatus result = CMAudioFormatDescriptionCreate(kCFAllocatorDefault, &outputFormat, 0, NULL, 0, NULL, NULL, &_pcmFormat);
    if (result != noErr) {
        *error = [NSError errorWithDomain:NSStringFromClass(self.class) code:result userInfo:nil];
        return;
    }
}

- (void)_decodeSampleBuffer:(CMSampleBufferRef)sampleBuffer {
    // 1、从输入数据中获取音频格式信息。
    CMAudioFormatDescriptionRef audioFormatRef = CMSampleBufferGetFormatDescription(sampleBuffer);
    if (!audioFormatRef) {
        return;
    }
    // 获取音频参数信息，AudioStreamBasicDescription 包含了音频的数据格式、声道数、采样位深、采样率等参数。
    AudioStreamBasicDescription audioFormat = *CMAudioFormatDescriptionGetStreamBasicDescription(audioFormatRef);
    
    // 2、根据音频参数创建解码器实例。
    NSError *error = nil;
    // 第一次解码时创建解码器。
    if (!_audioDecoderInstance) {
        [self _setupAudioDecoderInstanceWithInputAudioFormat:audioFormat error:&error];
        if (error) {
            [self _callBackError:error];
            return;
        }
        if (!_audioDecoderInstance) {
            return;
        }
    }
    
    // 3、获取输入数据中的 AAC 编码数据。
    CMBlockBufferRef blockBuffer = CMSampleBufferGetDataBuffer(sampleBuffer);
    size_t audioLength;
    char *dataPointer = NULL;
    CMBlockBufferGetDataPointer(blockBuffer, 0, NULL, &audioLength, &dataPointer);
    if (audioLength == 0 || !dataPointer) {
        return;
    }
    
    // 4、创建解码回调中要用到的自定义数据。
    KFAudioUserData userData = {0};
    userData.mChannels = (UInt32) audioFormat.mChannelsPerFrame;
    userData.mDataSize = (UInt32) audioLength;
    userData.mData = (void *) dataPointer; // 绑定 AAC 编码数据。
    userData.mPacketDesc.mDataByteSize = (UInt32) audioLength;
    userData.mPacketDesc.mStartOffset = 0;
    userData.mPacketDesc.mVariableFramesInPacket = 0;

    // 5、创建解码输出数据缓冲区内存空间。
    // AAC 编码的每个包有 1024 帧。
    UInt32 pcmDataPacketSize = 1024;
    // 缓冲区长度：pcmDataPacketSize * 2(16 bit 采样深度) * 声道数量。
    UInt32 pcmBufferSize = (UInt32) (pcmDataPacketSize * 2 * audioFormat.mChannelsPerFrame);
    if (!_pcmBuffer) {
        _pcmBuffer = malloc(pcmBufferSize);
    }
    memset(_pcmBuffer, 0, pcmBufferSize);
    
    // 6、创建解码器接口对应的解码缓冲区 AudioBufferList，绑定缓冲区的内存空间。
    AudioBufferList outAudioBufferList = {0};
    outAudioBufferList.mNumberBuffers = 1;
    outAudioBufferList.mBuffers[0].mNumberChannels = (UInt32) audioFormat.mChannelsPerFrame;
    outAudioBufferList.mBuffers[0].mDataByteSize = (UInt32) pcmBufferSize; // 设置解码缓冲区大小。
    outAudioBufferList.mBuffers[0].mData = _pcmBuffer; // 绑定缓冲区空间。

    // 7、输出数据描述。
    AudioStreamPacketDescription outputPacketDesc = {0};
    
    // 9、解码。
    OSStatus status = AudioConverterFillComplexBuffer(self.audioDecoderInstance, inputDataProcess, &userData, &pcmDataPacketSize, &outAudioBufferList, &outputPacketDesc);
    if (status != noErr) {
        [self _callBackError:[NSError errorWithDomain:NSStringFromClass(self.class) code:status userInfo:nil]];
        return;
    }
    
    if (outAudioBufferList.mBuffers[0].mDataByteSize > 0) {
        // 10、获取解码后的 PCM 数据并进行封装。
        // 把解码后的 PCM 数据先封装到 CMBlockBuffer 中。
        CMBlockBufferRef pcmBlockBuffer;
        size_t pcmBlockBufferSize = outAudioBufferList.mBuffers[0].mDataByteSize;
        char *pcmBlockBufferDataPointer = malloc(pcmBlockBufferSize);
        memcpy(pcmBlockBufferDataPointer, outAudioBufferList.mBuffers[0].mData, pcmBlockBufferSize);
        OSStatus status  = CMBlockBufferCreateWithMemoryBlock(kCFAllocatorDefault,
                                                              pcmBlockBufferDataPointer,
                                                              pcmBlockBufferSize,
                                                              NULL,
                                                              NULL,
                                                              0,
                                                              pcmBlockBufferSize,
                                                              0,
                                                              &pcmBlockBuffer);
        if (status != noErr) {
            return;
        }
        
        // 把 PCM 数据所在的 CMBlockBuffer 封装到 CMSampleBuffer 中。
        CMSampleBufferRef pcmSampleBuffer = NULL;
        CMSampleTimingInfo timingInfo = {CMTimeMake(1, audioFormat.mSampleRate), CMSampleBufferGetPresentationTimeStamp(sampleBuffer), kCMTimeInvalid };
        status = CMSampleBufferCreateReady(kCFAllocatorDefault,
                                           pcmBlockBuffer,
                                           _pcmFormat,
                                           pcmDataPacketSize,
                                           1,
                                           &timingInfo,
                                           0,
                                           NULL,
                                           &pcmSampleBuffer);
        CFRelease(pcmBlockBuffer);
        
        // 11、回调解码数据。
        if (pcmSampleBuffer) {
            if (self.sampleBufferOutputCallBack) {
                self.sampleBufferOutputCallBack(pcmSampleBuffer);
            }
            CFRelease(pcmSampleBuffer);
        }
        
    }
}

- (void)_callBackError:(NSError*)error {
    self.isError = YES;
    if (error && self.errorCallBack) {
        dispatch_async(dispatch_get_main_queue(), ^{
            self.errorCallBack(error);
        });
    }
}

#pragma mark - Decoder CallBack
static OSStatus inputDataProcess(AudioConverterRef inConverter, UInt32 *ioNumberDataPackets, AudioBufferList *ioData, AudioStreamPacketDescription **outDataPacketDescription, void *inUserData) {
    KFAudioUserData *userData = (KFAudioUserData *) inUserData;
    if (userData->mDataSize <= 0) {
        ioNumberDataPackets = 0;
        return -1;
    }
    
    // 设置解码输出数据格式信息。
    *outDataPacketDescription = &userData->mPacketDesc;
    (*outDataPacketDescription)[0].mStartOffset = 0;
    (*outDataPacketDescription)[0].mDataByteSize = userData->mDataSize;
    (*outDataPacketDescription)[0].mVariableFramesInPacket = 0;

    // 将待解码的数据拷贝到解码器的缓冲区的对应位置进行解码。
    ioData->mBuffers[0].mData = userData->mData;
    ioData->mBuffers[0].mDataByteSize = userData->mDataSize;
    ioData->mBuffers[0].mNumberChannels = userData->mChannels;
    
    return noErr;
}

@end
```

上面是 `KFAudioDecoder` 的实现，从代码上可以看到主要有这几个部分：

- 1）创建音频解码实例。第一次调用 `-decodeSampleBuffer:` → `-_decodeSampleBuffer:` 才会创建音频解码实例。

- - 在 `-_setupAudioDecoderInstanceWithInputAudioFormat:error:` 方法中实现。

- 2）实现音频解码逻辑，并在将数据封装到 `CMSampleBufferRef` 结构中，抛给 KFAudioDecoder 的对外数据回调接口。

- - 在 `-decodeSampleBuffer:` → `-_decodeSampleBuffer:` 中实现解码流程，其中涉及到待解码缓冲区、解码缓冲区的管理，并最终在 `inputDataProcess(...)` 回调中将待解码的数据拷贝到解码器的缓冲区进行解码，并设置对应的解码数据格式。

- 3）捕捉音频解码过程中的错误，抛给 KFAudioDecoder 的对外错误回调接口。

- - 在 `-_decodeSampleBuffer:` 方法中捕捉错误，在 `-_callBackError:` 方法向外回调。

- 4）清理音频解码器实例、解码缓冲区。

- - 在 `-dealloc` 方法中实现。

更具体细节见上述代码及其注释。

## 3、解封装和解码 MP4 文件中的音频部分存储为 PCM 文件

我们在一个 ViewController 中来实现音频解封装及解码逻辑，并将解码后的数据存储为 PCM 文件。

```
#import "KFAudioDecoderViewController.h"
#import "KFMP4Demuxer.h"
#import "KFAudioDecoder.h"

@interface KFAudioDecoderViewController ()
@property (nonatomic, strong) KFDemuxerConfig *demuxerConfig;
@property (nonatomic, strong) KFMP4Demuxer *demuxer;
@property (nonatomic, strong) KFAudioDecoder *decoder;
@property (nonatomic, strong) NSFileHandle *fileHandle;
@end

@implementation KFAudioDecoderViewController
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
        // 解码数据回调。在这里把解码后的音频 PCM 数据存储为文件。
        _decoder.sampleBufferOutputCallBack = ^(CMSampleBufferRef sampleBuffer) {
            if (sampleBuffer) {
                CMBlockBufferRef blockBuffer = CMSampleBufferGetDataBuffer(sampleBuffer);
                size_t totolLength;
                char *dataPointer = NULL;
                CMBlockBufferGetDataPointer(blockBuffer, 0, NULL, &totolLength, &dataPointer);
                if (totolLength == 0 || !dataPointer) {
                    return;
                }
                
                [weakSelf.fileHandle writeData:[NSData dataWithBytes:dataPointer length:totolLength]];
            }
        };
    }
    
    return _decoder;
}

- (NSFileHandle *)fileHandle {
    if (!_fileHandle) {
        NSString *videoPath = [[NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) lastObject] stringByAppendingPathComponent:@"output.pcm"];
        [[NSFileManager defaultManager] removeItemAtPath:videoPath error:nil];
        [[NSFileManager defaultManager] createFileAtPath:videoPath contents:nil attributes:nil];
        _fileHandle = [NSFileHandle fileHandleForWritingAtPath:videoPath];
    }

    return _fileHandle;
}

#pragma mark - Lifecycle
- (void)viewDidLoad {
    [super viewDidLoad];

    [self setupUI];
    
    // 完成音频解码后，可以将 App Document 文件夹下面的 output.pcm 文件拷贝到电脑上，使用 ffplay 播放：
    // ffplay -ar 44100 -channels 1 -f s16le -i output.pcm
}

- (void)dealloc {
    if (_fileHandle) {
        [_fileHandle closeFile];
        _fileHandle = nil;
    }
}

#pragma mark - Setup
- (void)setupUI {
    self.edgesForExtendedLayout = UIRectEdgeAll;
    self.extendedLayoutIncludesOpaqueBars = YES;
    self.title = @"Audio Decoder";
    self.view.backgroundColor = [UIColor whiteColor];
    
    // Navigation item.
    UIBarButtonItem *startBarButton = [[UIBarButtonItem alloc] initWithTitle:@"Start" style:UIBarButtonItemStylePlain target:self action:@selector(start)];
    self.navigationItem.rightBarButtonItems = @[startBarButton];
}

#pragma mark - Action
- (void)start {
    __weak typeof(self) weakSelf = self;
    NSLog(@"KFMP4Demuxer start");
    [self.demuxer startReading:^(BOOL success, NSError * _Nonnull error) {
        if (success) {
            // Demuxer 启动成功后，就可以从它里面获取解封装后的数据了。
            [weakSelf fetchAndDecodeDemuxedData];
        } else {
            NSLog(@"KFMP4Demuxer error:%zi %@", error.code, error.localizedDescription);
        }
    }];
}

#pragma mark - Utility
- (void)fetchAndDecodeDemuxedData {
    // 异步地从 Demuxer 获取解封装后的 AAC 编码数据，送给解码器进行解码。
    __weak typeof(self) weakSelf = self;
    dispatch_async(dispatch_get_global_queue(0, 0), ^{
        while (weakSelf.demuxer.hasAudioSampleBuffer) {
            CMSampleBufferRef audioBuffer = [weakSelf.demuxer copyNextAudioSampleBuffer];
            if (audioBuffer) {
                [weakSelf decodeSampleBuffer:audioBuffer];
                CFRelease(audioBuffer);
            }
        }
        if (weakSelf.demuxer.demuxerStatus == KFMP4DemuxerStatusCompleted) {
            NSLog(@"KFMP4Demuxer complete");
        }
    });
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
            CMSampleBufferRef frameSampleBuffer = NULL;
            const size_t sampleSizeArray[] = {sampleSize};
            status = CMSampleBufferCreateReady(kCFAllocatorDefault,
                                               packetBlockBuffer,
                                               CMSampleBufferGetFormatDescription(sampleBuffer),
                                               1,
                                               1,
                                               &timingInfo,
                                               1,
                                               sampleSizeArray,
                                               &frameSampleBuffer);
            CFRelease(packetBlockBuffer);
            
            // 4、解码这个包的数据。
            if (frameSampleBuffer) {
                [self.decoder decodeSampleBuffer:frameSampleBuffer];
                CFRelease(frameSampleBuffer);
            }
        }
        dataPointer += sampleSize;
    }
}

@end
```

上面是 `KFAudioDecoderViewController` 的实现，其中主要包含这几个部分：

- 1）通过启动音频解封装来驱动整个解封装和解码流程。

- - 在 `-start` 中实现开始动作。

- 2）在解封装模块 `KFMP4Demuxer` 启动成功后，开始读取解封装数据并启动解码。

- - 在 `-fetchAndDecodeDemuxedData` 方法中实现。

- 3）将解封装后的数据拆包，以包为单位封装为 `CMSampleBuffer` 送给解码器解码。

- - 在 `-decodeSampleBuffer:` 方法中实现。

- 4）在解码模块 `KFAudioDecoder` 的数据回调中获取解码后的 PCM 数据存储为文件。

- - 在 `KFAudioDecoder` 的 `sampleBufferOutputCallBack` 回调中实现。

## 4、用工具播放 PCM 文件

完成音频解码后，可以将 App Document 文件夹下面的 `output.pcm` 文件拷贝到电脑上，使用 `ffplay` 播放来验证一下音频采集是效果是否符合预期：

```
$ ffplay -ar 44100 -channels 1 -f s16le -i output.pcm
```

注意这里的参数要对齐在工程中输入视频源的`采样率`、`声道数`、`采样位深`。比如我们的 Demo 中输入视频源的声道数是 1，所以上面的声道数需要设置为 1 才能播放正常的声音。

关于播放 PCM 文件的工具，可以参考[《FFmpeg 工具》第 2 节 ffplay 命令行工具](https://mp.weixin.qq.com/s?__biz=MjM5MTkxOTQyMQ==&mid=2257484831&idx=1&sn=6bab905a5040c46b971bab05f787788b&scene=21#wechat_redirect)和[《可视化音视频分析工具》第 1.1 节 Adobe Audition](https://mp.weixin.qq.com/s?__biz=MjM5MTkxOTQyMQ==&mid=2257484834&idx=1&sn=5dd9768bfc0d01ca1b036be8dd2f5fa1&scene=21#wechat_redirect)。

## 5、参考资料

[1]CMSampleBufferRef: *https://developer.apple.com/documentation/coremedia/cmsamplebufferref/*



原文链接：https://mp.weixin.qq.com/s/7Db81B9i16cLuq0jS42bmg

