# iOS AVDemo：音频封装，采集编码并封装为 M4A

iOS/Android 客户端开发同学如果想要开始学习音视频开发，最丝滑的方式是对[音视频基础概念知识](https://mp.weixin.qq.com/mp/appmsgalbum?__biz=MjM5MTkxOTQyMQ==&action=getalbum&album_id=2140155659944787969#wechat_redirect)有一定了解后，再借助本地平台的音视频能力上手去实践音视频的`采集 → 编码 → 封装 → 解封装 → 解码 → 渲染`过程，并借助[音视频工具](https://mp.weixin.qq.com/mp/appmsgalbum?__biz=MjM5MTkxOTQyMQ==&action=getalbum&album_id=2216997905264082945#wechat_redirect)来分析和理解对应的音视频数据。

在[音视频工程示例](https://mp.weixin.qq.com/mp/appmsgalbum?__biz=MjM5MTkxOTQyMQ==&action=getalbum&album_id=2273301900659851268#wechat_redirect)这个栏目，我们将通过拆解`采集 → 编码 → 封装 → 解封装 → 解码 → 渲染`流程并实现 Demo 来向大家介绍如何在 iOS/Android 平台上手音视频开发。

这里是第三篇：**iOS 音频封装 Demo**。这个 Demo 里包含以下内容：

- 1）实现一个音频采集模块；
- 2）实现一个音频编码模块；
- 3）实现一个音频封装模块；
- 4）串联音频采集、编码、封装模块，将采集到的音频数据输入给 AAC 编码模块进行编码，再将编码后的数据输入给 M4A 封装模块封装和存储；
- 5）详尽的代码注释，帮你理解代码逻辑和原理。

## 1、音频采集模块

在这个 Demo 中，音频采集模块 `KFAudioCapture` 的实现与 [《iOS 音频采集 Demo》](https://mp.weixin.qq.com/s?__biz=MjM5MTkxOTQyMQ==&mid=2257484867&idx=1&sn=d857104930a86de8ab0bdf2358ca6283&scene=21#wechat_redirect) 中一样，这里就不再重复介绍了，其接口如下：

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

## 2、音频编码模块

同样的，音频编码模块 `KFAudioEncoder` 的实现与[《iOS 音频编码 Demo》](https://mp.weixin.qq.com/s?__biz=MjM5MTkxOTQyMQ==&mid=2257484887&idx=1&sn=ac142cbeafddc27f3a8c2902524831c8&scene=21#wechat_redirect)中一样，这里就不再重复介绍了，其接口如下：

```
#import <Foundation/Foundation.h>
#import <CoreMedia/CoreMedia.h>

NS_ASSUME_NONNULL_BEGIN

@interface KFAudioEncoder : NSObject
+ (instancetype)new NS_UNAVAILABLE;
- (instancetype)init NS_UNAVAILABLE;
- (instancetype)initWithAudioBitrate:(NSInteger)audioBitrate;

@property (nonatomic, assign, readonly) NSInteger audioBitrate; // 音频编码码率。
@property (nonatomic, copy) void (^sampleBufferOutputCallBack)(CMSampleBufferRef sample); // 音频编码数据回调。
@property (nonatomic, copy) void (^errorCallBack)(NSError *error); // 音频编码错误回调。

- (void)encodeSampleBuffer:(CMSampleBufferRef)buffer; // 编码。
@end

NS_ASSUME_NONNULL_END
```

## 3、音频封装模块

接下来，我们来实现一个音频封装模块，在这里输入编码后的数据，输出封装后的文件。

这次我们要封装的格式是 M4A，属于 MPEG-4 标准，通常普通的 MPEG-4 文件扩展名是 `.mp4`，只包含音频的 MPEG-4 文件扩展名用 `.m4a`。所以，其实我们这里实现的是一个 MP4 封装模块，支持将音频编码数据封装成 M4A，也支持将音视频数据封装成 MP4。关于 MP4 格式，可以看一看[《MP4 格式》](https://mp.weixin.qq.com/s?__biz=MjM5MTkxOTQyMQ==&mid=2257484677&idx=1&sn=a868911489519592930e18a60966d6a1&scene=21#wechat_redirect)这篇文章了解一下。

由于 MP4 封装涉及到一些参数设置，所以我们先实现一个 `KFMuxerConfig` 类用于定义 MP4 封装的参数的配置。这里包括了：封装文件输出地址、封装文件类型、图像变换信息这几个参数。

```
KFMuxerConfig.h
#import <Foundation/Foundation.h>
#import <CoreGraphics/CoreGraphics.h>
#import "KFMediaBase.h"

NS_ASSUME_NONNULL_BEGIN

@interface KFMuxerConfig : NSObject
@property (nonatomic, strong) NSURL *outputURL; // 封装文件输出地址。
@property (nonatomic, assign) KFMediaType muxerType; // 封装文件类型。
@property (nonatomic, assign) CGAffineTransform preferredTransform; // 图像的变换信息。比如：视频图像旋转。
@end

NS_ASSUME_NONNULL_END
KFMuxerConfig.m
#import "KFMuxerConfig.h"

@implementation KFMuxerConfig

- (instancetype)init {
    self = [super init];
    if (self) {
        _muxerType = KFMediaAV;
        _preferredTransform = CGAffineTransformIdentity;
    }
    return self;
}

@end
```

其中用到的 `KFMediaType` 是定义在 `KFMediaBase.h` 中的一个枚举：

```
KFMediaBase.h
#ifndef KFMediaBase_h
#define KFMediaBase_h

#import <Foundation/Foundation.h>

typedef NS_ENUM(NSInteger, KFMediaType) {
    KFMediaNone = 0,
    KFMediaAudio = 1 << 0, // 仅音频。
    KFMediaVideo = 1 << 1, // 仅视频。
    KFMediaAV = KFMediaAudio | KFMediaVideo,  // 音视频都有。
};

#endif /* KFMediaBase_h */
```

接下来，我们来实现 `KFMP4Muxer` 模块。

```
KFMP4Muxer.h
#import <Foundation/Foundation.h>
#import <CoreMedia/CoreMedia.h>
#import "KFMuxerConfig.h"

NS_ASSUME_NONNULL_BEGIN

@interface KFMP4Muxer : NSObject
+ (instancetype)new NS_UNAVAILABLE;
- (instancetype)init NS_UNAVAILABLE;
- (instancetype)initWithConfig:(KFMuxerConfig *)config;

@property (nonatomic, strong, readonly) KFMuxerConfig *config;
@property (nonatomic, copy) void (^errorCallBack)(NSError *error); // 封装错误回调。

- (void)startWriting; // 开始封装写入数据。
- (void)cancelWriting; // 取消封装写入数据。
- (void)appendSampleBuffer:(CMSampleBufferRef)sampleBuffer; // 添加封装数据。
- (void)stopWriting:(void (^)(BOOL success, NSError *error))completeHandler; // 停止封装写入数据。
@end

NS_ASSUME_NONNULL_END
```

上面是 `KFMP4Muxer` 的接口设计，除了`初始化方法`，主要是有`获取封装配置`以及`封装错误回调`的接口，另外就是`开始写入封装数据`、`取消写入封装数据`、`添加封装数据`、`停止写入封装数据`的接口。

在上面的`添加封装数据`接口中，我们使用的是依然 **CMSampleBufferRef**[1] 作为参数类型，再次体现了它作为 `iOS 音视频处理 pipeline 中的流通货币`的通用性。关于这点，我们在[《iOS 音频采集 Demo》](https://mp.weixin.qq.com/s?__biz=MjM5MTkxOTQyMQ==&mid=2257484867&idx=1&sn=d857104930a86de8ab0bdf2358ca6283&scene=21#wechat_redirect)和[《iOS 音频编码 Demo》](https://mp.weixin.qq.com/s?__biz=MjM5MTkxOTQyMQ==&mid=2257484887&idx=1&sn=ac142cbeafddc27f3a8c2902524831c8&scene=21#wechat_redirect)两篇文章中都提到过。

在这个 Demo 里我们通过 `CMSampleBufferRef` 打包的是编码后的 AAC 数据，将其作为输入送给封装模块。

```
KFMP4Muxer.m
#import "KFMP4Muxer.h"
#import <AVFoundation/AVFoundation.h>

#define KFMP4MuxerAddOutputError 1000
#define KFMP4MuxerMaxQueueCount 10000

// 封装器的状态机。
typedef NS_ENUM(NSInteger, KFMP4MuxerStatus) {
    KFMP4MuxerStatusUnknown = 0,
    KFMP4MuxerStatusRunning = 1,
    KFMP4MuxerStatusFailed = 2,
    KFMP4MuxerStatusCompleted = 3,
    KFMP4MuxerStatusCancelled = 4,
};

@interface KFMP4Muxer () {
    CMSimpleQueueRef _audioQueue; // 音频数据队列。
    CMSimpleQueueRef _videoQueue; // 视频数据队列。
}
@property (nonatomic, strong, readwrite) KFMuxerConfig *config;
@property (nonatomic, strong) AVAssetWriter *muxWriter; // 封装器实例。
@property (nonatomic, strong) AVAssetWriterInput *writerVideoInput; // Muxer 的视频输入。
@property (nonatomic, strong) AVAssetWriterInput *writerAudioInput; // Muxer 的音频输入。
@property (nonatomic, strong) dispatch_queue_t muxerQueue;
@property (nonatomic, strong) dispatch_semaphore_t semaphore;
@property (nonatomic, assign) KFMP4MuxerStatus muxerStatus;
@end

@implementation KFMP4Muxer
#pragma mark - LifeCycle
- (instancetype)initWithConfig:(KFMuxerConfig *)config {
    self = [super init];
    if (self) {
        _config = config;
        _muxerQueue = dispatch_queue_create("com.KeyFrameKit.muxerQueue", DISPATCH_QUEUE_SERIAL); // 封装任务队列。
        _semaphore = dispatch_semaphore_create(1);
        CMSimpleQueueCreate(kCFAllocatorDefault, KFMP4MuxerMaxQueueCount, &_audioQueue);
        CMSimpleQueueCreate(kCFAllocatorDefault, KFMP4MuxerMaxQueueCount, &_videoQueue);
    }
    
    return self;
}

- (void)dealloc {
    dispatch_semaphore_wait(_semaphore, DISPATCH_TIME_FOREVER);
    [self _reset]; // 清理。
    dispatch_semaphore_signal(_semaphore);
}

#pragma mark - Public Method
- (void)startWriting {
    // 开始写入。
    __weak typeof(self) weakSelf = self;
    dispatch_async(self.muxerQueue, ^{
        dispatch_semaphore_wait(weakSelf.semaphore, DISPATCH_TIME_FOREVER);
        [weakSelf _reset]; // 清理。
        weakSelf.muxerStatus = KFMP4MuxerStatusRunning; // 标记状态。
        dispatch_semaphore_signal(weakSelf.semaphore);
    });
}

- (void)cancelWriting {
    // 取消写入。
    __weak typeof(self) weakSelf = self;
    dispatch_async(self.muxerQueue, ^{
        dispatch_semaphore_wait(weakSelf.semaphore, DISPATCH_TIME_FOREVER);
        if (weakSelf.muxWriter && weakSelf.muxWriter.status == AVAssetWriterStatusWriting) {
            [weakSelf.muxWriter cancelWriting];
        }
        weakSelf.muxerStatus = KFMP4MuxerStatusCancelled; // 标记状态。
        dispatch_semaphore_signal(weakSelf.semaphore);
    });
}

- (void)appendSampleBuffer:(CMSampleBufferRef)sampleBuffer {
    if (!sampleBuffer || !CMSampleBufferGetDataBuffer(sampleBuffer) || self.muxerStatus != KFMP4MuxerStatusRunning) {
        return;
    }

    // 异步添加数据。

    __weak typeof(self) weakSelf = self;
    CFRetain(sampleBuffer);
    dispatch_async(self.muxerQueue, ^{
        dispatch_semaphore_wait(weakSelf.semaphore, DISPATCH_TIME_FOREVER);
        
        // 1、添加数据到队列。
        [weakSelf _enqueueSampleBuffer:sampleBuffer];
        
        // 2、第一次添加数据时，创建 Muxer 实例并触发写数据操作。
        if (!weakSelf.muxWriter) {
            // 检查数据是否正常。队列里面有数据则表示对应的数据携带的音视频格式信息是正常的，这个在初始化 Muxer 的输入源时需要用到。
            if (![weakSelf _checkFormatDescriptionLoadSuccess]) {
                CFRelease(sampleBuffer);
                dispatch_semaphore_signal(weakSelf.semaphore);
                return;
            }
            
            // 创建 Muxer 实例。
            NSError *error = nil;
            BOOL success = [weakSelf _setupMuxWriter:&error];
            if (!success) {
                weakSelf.muxerStatus = KFMP4MuxerStatusFailed;
                CFRelease(sampleBuffer);
                dispatch_semaphore_signal(weakSelf.semaphore);
                [weakSelf _callBackError:error];
                return;
            }
            
            // 开始封装写入。
            success = [weakSelf.muxWriter startWriting];
            if (success) {
                // 启动封装会话，传入数据起始时间。这个起始时间是音视频 pts 的最小值。
                [weakSelf.muxWriter startSessionAtSourceTime:[weakSelf _sessionSourceTime]];
            }
        }
        
        // 3、检查 Muxer 状态。
        if (!weakSelf.muxWriter || weakSelf.muxWriter.status != AVAssetWriterStatusWriting) {
            weakSelf.muxerStatus = KFMP4MuxerStatusFailed;
            CFRelease(sampleBuffer);
            dispatch_semaphore_signal(weakSelf.semaphore);
            [weakSelf _callBackError:weakSelf.muxWriter.error];
            return;
        }

        // 4、做音视频数据交织。
        [weakSelf _avInterLeavedSample];
        
        CFRelease(sampleBuffer);
        dispatch_semaphore_signal(weakSelf.semaphore);
    });
}

- (void)stopWriting:(void (^)(BOOL success, NSError *error))completeHandler {
    // 停止写入。
    __weak typeof(self) weakSelf = self;
    dispatch_async(self.muxerQueue, ^{
        dispatch_semaphore_wait(weakSelf.semaphore, DISPATCH_TIME_FOREVER);
        [weakSelf _stopWriting:^(BOOL success, NSError *error) {
            weakSelf.muxerStatus = success ? KFMP4MuxerStatusCompleted : KFMP4MuxerStatusFailed;
            dispatch_semaphore_signal(weakSelf.semaphore);
            if (completeHandler) {
                completeHandler(success, error);
            }
        }];
    });
}

#pragma mark - Private Method
- (BOOL)_setupMuxWriter:(NSError **)error {
    if (!self.config.outputURL) {
        *error = [NSError errorWithDomain:NSStringFromClass([KFMP4Muxer class]) code:40003 userInfo:nil];
        return NO;
    }
    
    // 1、清理写入路径的文件。
    if ([[NSFileManager defaultManager] fileExistsAtPath:self.config.outputURL.path]) {
        [[NSFileManager defaultManager] removeItemAtPath:self.config.outputURL.path error:nil];
    }
    
    
    // 2、创建封装器实例。
    if (_muxWriter) {
        return YES;
    }
    // 使用 AVAssetWriter 作为封装器，类型使用 AVFileTypeMPEG4。M4A 格式是遵循 MPEG4 规范的一种音频格式。
    _muxWriter = [[AVAssetWriter alloc] initWithURL:self.config.outputURL fileType:AVFileTypeMPEG4 error:error];
    if (*error) {
        return NO;
    }
    _muxWriter.movieTimeScale = 1000000000;
    _muxWriter.shouldOptimizeForNetworkUse = YES; // 这个选项会将 MP4 的 moov box 前置。
    
    // 3、当封装内容包含视频时，创建 Muxer 的视频输入。
    if ((self.config.muxerType & KFMediaVideo) && !_writerVideoInput) {
        // 从队列中的视频数据里获取视频格式信息，用于初始化视频输入源。
        CMVideoFormatDescriptionRef videoDecscription = CMSampleBufferGetFormatDescription((CMSampleBufferRef)CMSimpleQueueGetHead(_videoQueue));
        _writerVideoInput = [AVAssetWriterInput assetWriterInputWithMediaType:AVMediaTypeVideo outputSettings:nil sourceFormatHint:videoDecscription];
        _writerVideoInput.expectsMediaDataInRealTime = YES; // 输入是否为实时数据源，比如相机采集。
        _writerVideoInput.transform = self.config.preferredTransform; // 画面是否做变换。
        if ([self.muxWriter canAddInput:self.writerVideoInput]) {
            [self.muxWriter addInput:self.writerVideoInput];
        } else {
            *error = self.muxWriter.error ? self.muxWriter.error : [NSError errorWithDomain:NSStringFromClass([KFMP4Muxer class]) code:KFMP4MuxerAddOutputError userInfo:nil];
            return NO;
        }
    }
    
    // 4、当封装内容包含音频时，创建 Muxer 的音频输入。
    if ((self.config.muxerType & KFMediaAudio) && !_writerAudioInput) {
        // 从队列中的音频数据里获取音频格式信息，用于初始化音频输入源。
        CMAudioFormatDescriptionRef audioDecscription = CMSampleBufferGetFormatDescription((CMSampleBufferRef)CMSimpleQueueGetHead(_audioQueue));
        _writerAudioInput = [AVAssetWriterInput assetWriterInputWithMediaType:AVMediaTypeAudio outputSettings:nil sourceFormatHint:audioDecscription];
        _writerAudioInput.expectsMediaDataInRealTime = YES; // 输入是否为实时数据源，比如麦克风采集。
        if ([self.muxWriter canAddInput:self.writerAudioInput]) {
            [self.muxWriter addInput:self.writerAudioInput];
        } else {
            *error = self.muxWriter.error ? self.muxWriter.error : [NSError errorWithDomain:NSStringFromClass([KFMP4Muxer class]) code:KFMP4MuxerAddOutputError userInfo:nil];
            return NO;
        }
    }

    return YES;
}

- (void)_enqueueSampleBuffer:(CMSampleBufferRef)sampleBuffer {
    CFRetain(sampleBuffer);
    // 音频、视频的格式信息正确才能入队。因为后面创建 Muxer 实例的输入源时也需要从队列中的音视频数据中获取相关格式信息。
    if (CMFormatDescriptionGetMediaType(CMSampleBufferGetFormatDescription(sampleBuffer)) == kCMMediaType_Audio) {
        CMSimpleQueueEnqueue(_audioQueue, sampleBuffer); // 音频数据入队列。
    } else if (CMFormatDescriptionGetMediaType(CMSampleBufferGetFormatDescription(sampleBuffer)) == kCMMediaType_Video) {
        CMSimpleQueueEnqueue(_videoQueue, sampleBuffer); // 视频数据入队列。
    }
}

- (void)_flushMuxer {
    // 将队列数据消费掉。
    [self _appendAudioSample];
    [self _appendVideoSample];
}

- (void)_appendAudioSample {
    // 音频写入封装。
    while (self.writerAudioInput && self.writerAudioInput.readyForMoreMediaData && CMSimpleQueueGetCount(_audioQueue) > 0) {
        CMSampleBufferRef audioSample = (CMSampleBufferRef)CMSimpleQueueDequeue(_audioQueue);
        [self.writerAudioInput appendSampleBuffer:audioSample];
        CFRelease(audioSample);
    }
}

- (void)_appendVideoSample {
    // 视频写入封装。
    while (self.writerVideoInput && self.writerVideoInput.readyForMoreMediaData && CMSimpleQueueGetCount(_videoQueue) > 0) {
        CMSampleBufferRef videoSample = (CMSampleBufferRef)CMSimpleQueueDequeue(_videoQueue);
        [self.writerVideoInput appendSampleBuffer:videoSample];
        CFRelease(videoSample);
    }
}

- (void)_avInterLeavedSample {
    // 当同时封装音频和视频时，需要做好交织，这样可以提升音视频播放时的体验。
    if ((self.config.muxerType & KFMediaAudio) && (self.config.muxerType & KFMediaVideo)) { // 同时封装音频和视频。
        while (CMSimpleQueueGetCount(_audioQueue) > 0 && CMSimpleQueueGetCount(_videoQueue) > 0) {
            if (self.writerAudioInput.readyForMoreMediaData && self.writerVideoInput.readyForMoreMediaData) {
                // 音频、视频队列数据各出队 1 个。
                CMSampleBufferRef audioHeader = (CMSampleBufferRef)CMSimpleQueueGetHead(_audioQueue);
                CMTime audioDtsTime = CMSampleBufferGetPresentationTimeStamp(audioHeader);
                CMSampleBufferRef videoHeader = (CMSampleBufferRef)CMSimpleQueueGetHead(_videoQueue);
                CMTime videoDtsTime = CMSampleBufferGetDecodeTimeStamp(videoHeader).value > 0 ? CMSampleBufferGetDecodeTimeStamp(videoHeader) : CMSampleBufferGetPresentationTimeStamp(videoHeader);
                // 比较 dts 较小者写入封装。
                if (CMTimeGetSeconds(audioDtsTime) >= CMTimeGetSeconds(videoDtsTime)) {
                    CMSampleBufferRef videoSample = (CMSampleBufferRef)CMSimpleQueueDequeue(_videoQueue);
                    [self.writerVideoInput appendSampleBuffer:videoSample];
                    CFRelease(videoSample);
                } else {
                    CMSampleBufferRef audioSample = (CMSampleBufferRef)CMSimpleQueueDequeue(_audioQueue);
                    [self.writerAudioInput appendSampleBuffer:audioSample];
                    CFRelease(audioSample);
                }
            } else {
                break;
            }
        }
    } else if (self.config.muxerType & KFMediaAudio) { // 只封装音频。
        [self _appendAudioSample];
    } else if (self.config.muxerType & KFMediaVideo) { // 只封装视频。
        [self _appendVideoSample];
    }
}

- (BOOL)_checkFormatDescriptionLoadSuccess {
    // 检查数据是否正常。
    if (!_muxWriter) {
        if ((self.config.muxerType & KFMediaAudio) && (self.config.muxerType & KFMediaVideo)) {
            return CMSimpleQueueGetCount(_videoQueue) > 0 && CMSimpleQueueGetCount(_audioQueue) > 0;
        } else if (self.config.muxerType & KFMediaAudio) {
            return CMSimpleQueueGetCount(_audioQueue) > 0;
        } else if (self.config.muxerType & KFMediaVideo) {
            return CMSimpleQueueGetCount(_videoQueue) > 0;
        }
    }
    
    return NO;
}

- (CMTime)_sessionSourceTime {
    // 数据起始时间：音视频 pts 的最小值。
    CMSampleBufferRef audioFirstBuffer = (CMSampleBufferRef)CMSimpleQueueGetHead(_audioQueue);
    CMSampleBufferRef videoFirstBuffer = (CMSampleBufferRef)CMSimpleQueueGetHead(_videoQueue);
    if (audioFirstBuffer && videoFirstBuffer) {
        Float64 audioPtsTime = CMTimeGetSeconds(CMSampleBufferGetPresentationTimeStamp(audioFirstBuffer));
        Float64 videoPtsTime = CMTimeGetSeconds(CMSampleBufferGetPresentationTimeStamp(videoFirstBuffer));
        return audioPtsTime >= videoPtsTime ? CMSampleBufferGetPresentationTimeStamp(videoFirstBuffer) : CMSampleBufferGetPresentationTimeStamp(audioFirstBuffer);
    } else if (audioFirstBuffer) {
        return CMSampleBufferGetPresentationTimeStamp(audioFirstBuffer);
    } else if (videoFirstBuffer) {
        return CMSampleBufferGetPresentationTimeStamp(videoFirstBuffer);
    }
    
    return kCMTimeInvalid;
}

- (void)_stopWriting:(void (^)(BOOL success, NSError *error))completeHandler {
    // 1、状态不对，回调错误。
    if (!self.muxWriter || self.muxWriter.status == AVAssetWriterStatusCompleted || self.muxWriter.status == AVAssetWriterStatusCancelled || self.muxWriter.status == AVAssetWriterStatusUnknown) {
        if (completeHandler) {
            completeHandler(NO, self.muxWriter.error ? self.muxWriter.error : [NSError errorWithDomain:NSStringFromClass(self.class) code:self.muxWriter.status userInfo:nil]);
        }
        return;
    }
    
    // 2、消费掉队列中剩余的数据。
    // 先做剩余数据的音视频交织。
    [self _avInterLeavedSample];
    // 消费剩余数据。
    [self _flushMuxer];
    
    // 3、标记视频输入和音频输入为结束状态。
    [self _markVideoAsFinished];
    [self _markAudioAsFinished];
    
    // 4、结束写入。
    __weak typeof(self) weakSelf = self;
    [self.muxWriter finishWritingWithCompletionHandler:^{
        BOOL complete = weakSelf.muxWriter.status == AVAssetWriterStatusCompleted;
        if (completeHandler) {
            completeHandler(complete, complete ? nil : weakSelf.muxWriter.error);
        }
    }];
}

- (void)_markVideoAsFinished {
    // 标记视频输入源为结束状态。
    if (self.muxWriter.status == AVAssetWriterStatusWriting && self.writerVideoInput) {
        [self.writerVideoInput markAsFinished];
    }
}

- (void)_markAudioAsFinished {
    // 标记音频输入源为结束状态。
    if (self.muxWriter.status == AVAssetWriterStatusWriting && self.writerAudioInput) {
        [self.writerAudioInput markAsFinished];
    }
}

- (void)_reset {
    // 取消写入操作。
    if (_muxWriter && _muxWriter.status == AVAssetWriterStatusWriting) {
        [_muxWriter cancelWriting];
    }
    
    // 清理实例。
    _muxWriter = nil;
    _writerVideoInput = nil;
    _writerVideoInput = nil;
    
    // 清理音频和视频数据队列。
    while (CMSimpleQueueGetCount(_audioQueue) > 0) {
        CMSampleBufferRef sampleBuffer = (CMSampleBufferRef) CMSimpleQueueDequeue(_audioQueue);
        CFRelease(sampleBuffer);
    }
    while (CMSimpleQueueGetCount(_videoQueue) > 0) {
        CMSampleBufferRef sampleBuffer = (CMSampleBufferRef) CMSimpleQueueDequeue(_videoQueue);
        CFRelease(sampleBuffer);
    }
}

- (void)_callBackError:(NSError *)error {
    if (error && self.errorCallBack) {
        dispatch_async(dispatch_get_main_queue(), ^{
            self.errorCallBack(error);
        });
    }
}

@end
```

上面是 `KFMP4Muxer` 的实现，从代码上可以看到主要有这几个部分：

- 1）创建封装器实例及对应的音频和视频数据输入源。第一次调用 `-appendSampleBuffer:` 添加待封装数据时才会创建封装器实例。

- - 在 `-_setupMuxWriter:` 方法中实现。音频和视频的输入源分别是 `writerAudioInput` 和 `writerVideoInput`。

- 2）用两个队列作为缓冲区，分别管理音频和视频待封装数据。

- - 这两个队列分别是 `_audioQueue` 和 `_videoQueue`。
  - 每次当外部调用 `-appendSampleBuffer:` 方法送入待封装数据时，其实都是先调用 `-_enqueueSampleBuffer:` 把数据放入两个队列中的一个，以便根据情况进行后续的音视频数据交织。

- 3）同时封装音频和视频数据时，进行音视频数据交织。

- - 在 `-_avInterLeavedSample` 方法中实现音视频数据交织。当带封装的数据既有音频又有视频，就需要根据他们的时间戳信息进行交织，这样便于在播放该音视频时提升体验。

- 4）音视频数据写入封装。

- - 同时封装音频和视频数据时，在做完音视频交织后，即分别将交织后的音视频数据写入对应的 `writerAudioInput` 和 `writerVideoInput`。在 `-_avInterLeavedSample` 中实现。
  - 单独封装音频或视频数据时，则直接将数据写入对应的 `writerAudioInput` 和 `writerVideoInput`。分别在 `-_appendAudioSample` 和 `-_appendVideoSample` 方法中实现。

- 5）停止写入。

- - 在 `-stopWriting:` → `-_stopWriting:` 方法中实现。
  - 在停止前，还需要消费掉 `_audioQueue` 和 `_videoQueue` 的剩余数据，要调用 `-_avInterLeavedSample` → `-_flushMuxer`。
  - 并将视频输入源和音频输入源标记位结束，分别在 `-_markVideoAsFinished` 和 `-_markAudioAsFinished` 方法中实现。

- 6）贯穿整个封装过程的状态机管理。

- - 在枚举 `KFMP4MuxerStatus` 中定义了封装器的各种状态，对于封装器的状态机管理贯穿在封装的整个过程中。

- 7）错误回调。

- - 在 `-callBackError:` 方法向外回调错误。

- 8）清理封装器实例及数据缓冲区。

- - 在 `-dealloc` 方法中实现。需要调用 `-_reset` 方法清理封装器实例、音频和视频输入源、音频和视频缓冲区。

更具体细节见上述代码及其注释。

## 4、采集音频数据进行 AAC 编码以及 M4A 封装和存储

我们还是在一个 ViewController 中来实现采集音频数据进行 AAC 编码、M4A 封装和存储的逻辑。

```
KFAudioCaptureViewController.m
#import "KFAudioMuxerViewController.h"
#import <AVFoundation/AVFoundation.h>
#import "KFAudioCapture.h"
#import "KFAudioEncoder.h"
#import "KFMP4Muxer.h"

@interface KFAudioMuxerViewController ()
@property (nonatomic, strong) KFAudioConfig *audioConfig;
@property (nonatomic, strong) KFAudioCapture *audioCapture;
@property (nonatomic, strong) KFAudioEncoder *audioEncoder;
@property (nonatomic, strong) KFMuxerConfig *muxerConfig;
@property (nonatomic, strong) KFMP4Muxer *muxer;
@end

@implementation KFAudioMuxerViewController
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
            NSLog(@"KFAudioCapture error:%zi %@", error.code, error.localizedDescription);
        };
        // 音频采集数据回调。在这里采集的 PCM 数据送给编码器。
        _audioCapture.sampleBufferOutputCallBack = ^(CMSampleBufferRef sampleBuffer) {
            [weakSelf.audioEncoder encodeSampleBuffer:sampleBuffer];
        };
    }
    
    return _audioCapture;
}

- (KFAudioEncoder *)audioEncoder {
    if (!_audioEncoder) {
        __weak typeof(self) weakSelf = self;
        _audioEncoder = [[KFAudioEncoder alloc] initWithAudioBitrate:96000];
        _audioEncoder.errorCallBack = ^(NSError* error) {
            NSLog(@"KFAudioEncoder error:%zi %@", error.code, error.localizedDescription);
        };
        // 音频编码数据回调。这里编码的 AAC 数据送给封装器。
        // 与之前将编码后的 AAC 数据存储为 AAC 文件不同的是，这里编码后送给封装器的 AAC 数据是没有添加 ADTS 头的，因为我们这里封装的是 M4A 格式，不需要 ADTS 头。
        _audioEncoder.sampleBufferOutputCallBack = ^(CMSampleBufferRef sampleBuffer) {
            [weakSelf.muxer appendSampleBuffer:sampleBuffer];
        };
    }
    
    return _audioEncoder;
}

- (KFMuxerConfig *)muxerConfig {
    if (!_muxerConfig) {
        _muxerConfig = [[KFMuxerConfig alloc] init];
        NSString *audioPath = [[NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) lastObject] stringByAppendingPathComponent:@"test.m4a"];
        NSLog(@"M4A file path: %@", audioPath);
        [[NSFileManager defaultManager] removeItemAtPath:audioPath error:nil];
        _muxerConfig.outputURL = [NSURL fileURLWithPath:audioPath];
        _muxerConfig.muxerType = KFMediaAudio;
    }
    
    return _muxerConfig;
}

- (KFMP4Muxer *)muxer {
    if (!_muxer) {
        _muxer = [[KFMP4Muxer alloc] initWithConfig:self.muxerConfig];
        _muxer.errorCallBack = ^(NSError* error) {
            NSLog(@"KFMP4Muxer error:%zi %@", error.code, error.localizedDescription);
        };
    }
    
    return _muxer;
}

#pragma mark - Lifecycle
- (void)viewDidLoad {
    [super viewDidLoad];
    
    [self setupAudioSession];
    [self setupUI];
    
    // 完成音频编码后，可以将 App Document 文件夹下面的 test.m4a 文件拷贝到电脑上，使用 ffplay 播放：
    // ffplay -i test.m4a
}

#pragma mark - Setup
- (void)setupUI {
    self.edgesForExtendedLayout = UIRectEdgeAll;
    self.extendedLayoutIncludesOpaqueBars = YES;
    self.title = @"Audio Muxer";
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
    // 启动采集器。
    [self.audioCapture startRunning];
    // 启动封装器。
    [self.muxer startWriting];
}

- (void)stop {
    // 停止采集器。
    [self.audioCapture stopRunning];
    // 停止封装器。
    [self.muxer stopWriting:^(BOOL success, NSError * _Nonnull error) {
        NSLog(@"KFMP4Muxer %@", success ? @"success" : [NSString stringWithFormat:@"error %zi %@", error.code, error.localizedDescription]);
    }];
}

@end
```

上面是 `KFAudioMuxerViewController` 的实现，其中主要包含这几个部分：

- 1）在采集音频前需要设置 **AVAudioSession**[2] 为正确的采集模式。

- - 在 `-setupAudioSession` 中实现。

- 2）通过启动和关闭音频采集和封装来驱动整个采集、编码、封装流程。

- - 分别在 `-start` 和 `-stop` 中实现开始和停止动作。

- 3）在采集模块 `KFAudioCapture` 的数据回调中将数据交给编码模块 `KFAudioEncoder` 进行编码。

- - 在 `KFAudioCapture` 的 `sampleBufferOutputCallBack` 回调中实现。

- 4）在编码模块 `KFAudioEncoder` 的数据回调中获取编码后的 AAC 裸流数据，并将数据交给封装器 `KFMP4Muxer` 进行封装。

- - 在 `KFAudioEncoder` 的 `sampleBufferOutputCallBack` 回调中实现。

- 5）在调用 `-stop` 停止整个流程后，如果没有出现错误，封装的 M4A 文件会被存储到 `muxerConfig` 设置的路径。

## 5、用工具播放 M4A 文件

完成音频采集和编码后，可以将 App Document 文件夹下面的 `test.m4a` 文件拷贝到电脑上，使用 `ffplay` 播放来验证一下音频采集是效果是否符合预期：

```
$ ffplay -i test.m4a
```

关于播放 M4A 文件的工具，可以参考[《FFmpeg 工具》第 2 节 ffplay 命令行工具](https://mp.weixin.qq.com/s?__biz=MjM5MTkxOTQyMQ==&mid=2257484831&idx=1&sn=6bab905a5040c46b971bab05f787788b&scene=21#wechat_redirect)和[《可视化音视频分析工具》第 1.1 节 Adobe Audition](https://mp.weixin.qq.com/s?__biz=MjM5MTkxOTQyMQ==&mid=2257484834&idx=1&sn=5dd9768bfc0d01ca1b036be8dd2f5fa1&scene=21#wechat_redirect)。

上面我们讲过 M4A 格式是属于 MPEG-4 标准，所以我们这里还可以用[《可视化音视频分析工具》第 3.1 节 MP4Box.js](https://mp.weixin.qq.com/s?__biz=MjM5MTkxOTQyMQ==&mid=2257484834&idx=1&sn=5dd9768bfc0d01ca1b036be8dd2f5fa1&scene=21#wechat_redirect) 等工具来查看它的格式：

![图片](https://mmbiz.qpic.cn/mmbiz_png/gUnqKPeSuejDJHRicNdoGX06V5TeO2y8kKRhgQmZzal2dlyNdiaVRalLv4KHU1BlpTFPX4aS7oKqCM0jG7hVjW1w/640?wx_fmt=png&wxfrom=5&wx_lazy=1&wx_co=1)Demo 生成的 M4A 文件结构

## 6、参考资料

[1]CMSampleBufferRef: *https://developer.apple.com/documentation/coremedia/cmsamplebufferref/*

[2]AVAudioSession: *https://developer.apple.com/documentation/avfaudio/avaudiosession/*





原文链接：https://mp.weixin.qq.com/s/R86qnQAi2njr6k7tFvTF-w