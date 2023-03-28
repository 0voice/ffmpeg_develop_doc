# iOS AVDemo：音频解封装，从 MP4 中解封装出 AAC

iOS/Android 客户端开发同学如果想要开始学习音视频开发，最丝滑的方式是对[音视频基础概念知识](https://mp.weixin.qq.com/mp/appmsgalbum?__biz=MjM5MTkxOTQyMQ==&action=getalbum&album_id=2140155659944787969#wechat_redirect)有一定了解后，再借助本地平台的音视频能力上手去实践音视频的`采集 → 编码 → 封装 → 解封装 → 解码 → 渲染`过程，并借助[音视频工具](https://mp.weixin.qq.com/mp/appmsgalbum?__biz=MjM5MTkxOTQyMQ==&action=getalbum&album_id=2216997905264082945#wechat_redirect)来分析和理解对应的音视频数据。

在[音视频工程示例](https://mp.weixin.qq.com/mp/appmsgalbum?__biz=MjM5MTkxOTQyMQ==&action=getalbum&album_id=2273301900659851268#wechat_redirect)这个栏目，我们将通过拆解`采集 → 编码 → 封装 → 解封装 → 解码 → 渲染`流程并实现 Demo 来向大家介绍如何在 iOS/Android 平台上手音视频开发。

这里是第四篇：**iOS 音频解封装 Demo**。这个 Demo 里包含以下内容：

- 1）实现一个音频解封装模块；
- 2）实现对 MP4 文件中音频部分的解封装逻辑并将解封装后的编码数据存储为 AAC 文件；
- 3）详尽的代码注释，帮你理解代码逻辑和原理。

## 1、音频解封装模块

首先，实现一个 `KFDemuxerConfig` 类用于定义音频解封装参数的配置。这里包括了：待解封装的资源、解封装类型这几个参数。这样设计是因为这个配置类不仅会用于音频解封装，后续的视频解封装也会使用。

```
KFDemuxerConfig.h
#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>
#import <CoreMedia/CoreMedia.h>
#import "KFMediaBase.h"

NS_ASSUME_NONNULL_BEGIN

@interface KFDemuxerConfig : NSObject
@property (nonatomic, strong) AVAsset *asset; // 待解封装的资源。
@property (nonatomic, assign) KFMediaType demuxerType; // 解封装类型。
@end

NS_ASSUME_NONNULL_END
KFDemuxerConfig.m
#import "KFDemuxerConfig.h"

@implementation KFDemuxerConfig

- (instancetype)init {
    self = [super init];
    if (self) {
        _demuxerType = KFMediaAV;
    }
    
    return self;
}

@end
```

其中用到的 `KFMediaType` 是定义在 `KFMediaBase.h` 中的一个枚举：

```
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

接下来，我们实现一个 `KFMP4Demuxer` 类来实现 MP4 的解封装。它能从符合 MP4 标准的文件中解封装出音频编码数据。

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

上面是 `KFMP4Demuxer` 的接口设计，除了初始化方法，主要还有一些获取解封装器信息或者状态的属性接口，此外就是执行开始/取消读取数据、拷贝音频/视频采样数据的操作接口。

这里大家可能会疑惑，为什么 `KFMP4Demuxer` 不像前面的 Demo 中设计的 `KFAudioCapture`、`KFAudioEncoder` 的接口那样，有一个解封装后的数据回调接口。主要是因为解封装的速度是非常快的，不会成为一个音视频 pipeline 的瓶颈，而且考虑到解封装的资源可能会很大，所以一般不会一直不停地解出数据往外抛，这样下一个处理节点可能处理不过来这些数据。基于这些原因，解封装器的接口设计是让外部调用方主动找解封装器要数据来触发解封装操作，并且还要控制一定的缓存量防止内存占用过大。

在上面的`拷贝下一份音频/视频采样数据`接口中，我们使用的是依然 **CMSampleBufferRef**[1] 作为返回值类型。在这个接口中我们通过 `CMSampleBufferRef` 打包的是从 MP4/M4A 文件解封装后得到的 AAC 编码数据。

```
KFMP4Demuxer.m
#import "KFMP4Demuxer.h"

#define KFMP4DemuxerBadFileError 2000
#define KFMP4DemuxerAddVideoOutputError 2001
#define KFMP4DemuxerAddAudioOutputError 2002
#define KFMP4DemuxerQueueMaxCount 3

@interface KFMP4Demuxer () {
    CMSimpleQueueRef _audioQueue;
    CMSimpleQueueRef _videoQueue;
}
@property (nonatomic, strong, readwrite) KFDemuxerConfig* config;
@property (nonatomic, strong) AVAssetReader *demuxReader; // 解封装器实例。
@property (nonatomic, strong) AVAssetReaderTrackOutput *readerAudioOutput; // Demuxer 的音频输出。
@property (nonatomic, strong) AVAssetReaderTrackOutput *readerVideoOutput; // Demuxer 的视频输出。
@property (nonatomic, strong) dispatch_queue_t demuxerQueue;
@property (nonatomic, strong) dispatch_semaphore_t demuxerSemaphore;
@property (nonatomic, strong) dispatch_semaphore_t audioQueueSemaphore;
@property (nonatomic, strong) dispatch_semaphore_t videoQueueSemaphore;
@property (nonatomic, assign) CMTime lastAudioCopyNextTime; // 上一次拷贝的音频采样的时间戳。
@property (nonatomic, assign) CMTime lastVideoCopyNextTime; // 上一次拷贝的视频采样的时间戳。
@property (nonatomic, assign, readwrite) BOOL hasAudioTrack; // 是否包含音频数据。
@property (nonatomic, assign, readwrite) BOOL hasVideoTrack; // 是否包含视频数据。
@property (nonatomic, assign, readwrite) CGSize videoSize; // 视频大小。
@property (nonatomic, assign, readwrite) CMTime duration; // 媒体时长。
@property (nonatomic, assign, readwrite) CMVideoCodecType codecType; // 编码类型。
@property (nonatomic, assign, readwrite) KFMP4DemuxerStatus demuxerStatus; // 解封装器状态。
@property (nonatomic, assign, readwrite) BOOL audioEOF; // 是否音频结束。
@property (nonatomic, assign, readwrite) BOOL videoEOF; // 是否视频结束。
@property (nonatomic, assign, readwrite) CGAffineTransform preferredTransform; // 图像的变换信息。比如：视频图像旋转。
@end

@implementation KFMP4Demuxer
#pragma mark - LifeCycle
- (instancetype)initWithConfig:(KFDemuxerConfig *)config {
    self = [super init];
    if (self) {
        _config = config;
        _demuxerSemaphore = dispatch_semaphore_create(1);
        _audioQueueSemaphore = dispatch_semaphore_create(1);
        _videoQueueSemaphore = dispatch_semaphore_create(1);
        _demuxerStatus = KFMP4DemuxerStatusUnknown;
        _demuxerQueue = dispatch_queue_create("com.KeyFrameKit.demuxerQueue", DISPATCH_QUEUE_SERIAL);
        CMSimpleQueueCreate(kCFAllocatorDefault, KFMP4DemuxerQueueMaxCount, &_audioQueue);
        CMSimpleQueueCreate(kCFAllocatorDefault, KFMP4DemuxerQueueMaxCount, &_videoQueue);
    }
    return self;
}

- (void)dealloc {
    // 清理状态机。
    if (self.demuxerStatus == KFMP4DemuxerStatusRunning) {
        self.demuxerStatus = KFMP4DemuxerStatusCancelled;
    }
    
    // 清理解封装器实例。
    dispatch_semaphore_wait(_demuxerSemaphore, DISPATCH_TIME_FOREVER);
    if (self.demuxReader && self.demuxReader.status == AVAssetReaderStatusReading) {
        [self.demuxReader cancelReading];
    }
    dispatch_semaphore_signal(_demuxerSemaphore);
    
    // 清理音频数据队列。
    dispatch_semaphore_wait(_audioQueueSemaphore, DISPATCH_TIME_FOREVER);
    while (CMSimpleQueueGetCount(_audioQueue) > 0) {
        CMSampleBufferRef sampleBuffer = (CMSampleBufferRef)CMSimpleQueueDequeue(_audioQueue);
        CFRelease(sampleBuffer);
    }
    dispatch_semaphore_signal(_audioQueueSemaphore);
    
    // 清理视频数据队列。
    dispatch_semaphore_wait(_videoQueueSemaphore, DISPATCH_TIME_FOREVER);
    while (CMSimpleQueueGetCount(_videoQueue) > 0) {
        CMSampleBufferRef sampleBuffer = (CMSampleBufferRef)CMSimpleQueueDequeue(_videoQueue);
        CFRelease(sampleBuffer);
    }
    dispatch_semaphore_signal(_videoQueueSemaphore);
}

#pragma mark - Public Method
- (void)startReading:(void (^)(BOOL success, NSError *error))completeHandler {
    __weak typeof(self) weakSelf = self;
    dispatch_async(_demuxerQueue, ^{
        dispatch_semaphore_wait(weakSelf.demuxerSemaphore, DISPATCH_TIME_FOREVER);
        
        // 在第一次开始读数据时，创建解封装器实例。
        if (!weakSelf.demuxReader) {
            NSError *error;
            [weakSelf _setupDemuxReader:&error];
            weakSelf.audioEOF = !weakSelf.hasAudioTrack;
            weakSelf.videoEOF = !weakSelf.hasVideoTrack;
            weakSelf.demuxerStatus = error ? KFMP4DemuxerStatusFailed : KFMP4DemuxerStatusRunning;
            dispatch_semaphore_signal(weakSelf.demuxerSemaphore);
            if (completeHandler) {
                dispatch_async(dispatch_get_main_queue(), ^{
                    completeHandler(error ? NO : YES, error);
                });
            }
            return;
        }

        dispatch_semaphore_signal(weakSelf.demuxerSemaphore);
    });
}

- (void)cancelReading {
    __weak typeof(self) weakSelf = self;
    dispatch_async(_demuxerQueue, ^{
        dispatch_semaphore_wait(weakSelf.demuxerSemaphore, DISPATCH_TIME_FOREVER);
        
        // 取消读数据。
        if (weakSelf.demuxReader && weakSelf.demuxReader.status == AVAssetReaderStatusReading) {
            [weakSelf.demuxReader cancelReading];
        }
        weakSelf.demuxerStatus = KFMP4DemuxerStatusCancelled;
        
        dispatch_semaphore_signal(weakSelf.demuxerSemaphore);
    });
}

- (BOOL)hasAudioSampleBuffer {
    // 是否还有音频数据。
    if (self.hasAudioTrack && self.demuxerStatus == KFMP4DemuxerStatusRunning && !self.audioEOF) {
        int32_t audioCount = 0;
        dispatch_semaphore_wait(_audioQueueSemaphore, DISPATCH_TIME_FOREVER);
        if (CMSimpleQueueGetCount(_audioQueue) > 0) {
            audioCount = CMSimpleQueueGetCount(_audioQueue);
        }
        dispatch_semaphore_signal(_audioQueueSemaphore);
        
        return (audioCount == 0 && self.audioEOF) ? NO : YES;
    }
    
    return NO;
}

- (CMSampleBufferRef)copyNextAudioSampleBuffer CF_RETURNS_RETAINED {
    // 拷贝下一份音频采样。
    CMSampleBufferRef sampleBuffer = NULL;
    while (!sampleBuffer && self.demuxerStatus == KFMP4DemuxerStatusRunning && !self.audioEOF) {
        // 先从缓冲队列取数据。
        dispatch_semaphore_wait(_audioQueueSemaphore, DISPATCH_TIME_FOREVER);
        if (CMSimpleQueueGetCount(_audioQueue) > 0) {
            sampleBuffer = (CMSampleBufferRef) CMSimpleQueueDequeue(_audioQueue);
        }
        dispatch_semaphore_signal(_audioQueueSemaphore);
        
        // 缓冲队列没有数据，就同步加载一下试试。
        if (!sampleBuffer && self.demuxerStatus == KFMP4DemuxerStatusRunning) {
            [self _syncLoadNextSampleBuffer];
        }
    }
    
    // 异步加载一下，先缓冲到数据队列中，等下次取。
    [self _asyncLoadNextSampleBuffer];
    
    return sampleBuffer;
}

- (BOOL)hasVideoSampleBuffer {
    // 是否还有视频数据。
    if (self.hasVideoTrack && self.demuxerStatus == KFMP4DemuxerStatusRunning && !self.videoEOF) {
        int32_t videoCount = 0;
        dispatch_semaphore_wait(_videoQueueSemaphore, DISPATCH_TIME_FOREVER);
        if (CMSimpleQueueGetCount(_videoQueue) > 0) {
            videoCount = CMSimpleQueueGetCount(_videoQueue);
        }
        dispatch_semaphore_signal(_videoQueueSemaphore);
        
        return (videoCount == 0 && self.videoEOF) ? NO : YES;
    }
    
    return NO;
}

- (CMSampleBufferRef)copyNextVideoSampleBuffer CF_RETURNS_RETAINED {
    // 拷贝下一份视频采样。
    CMSampleBufferRef sampleBuffer = NULL;
    while (!sampleBuffer && self.demuxerStatus == KFMP4DemuxerStatusRunning && !self.videoEOF) {
        // 先从缓冲队列取数据。
        dispatch_semaphore_wait(_videoQueueSemaphore, DISPATCH_TIME_FOREVER);
        if (CMSimpleQueueGetCount(_videoQueue) > 0) {
            sampleBuffer = (CMSampleBufferRef) CMSimpleQueueDequeue(_videoQueue);
        }
        dispatch_semaphore_signal(_videoQueueSemaphore);
        
        // 缓冲队列没有数据，就同步加载一下试试。
        if (!sampleBuffer && self.demuxerStatus == KFMP4DemuxerStatusRunning) {
            [self _syncLoadNextSampleBuffer];
        }
    }
    
    // 异步加载一下，先缓冲到数据队列中，等下次取。
    [self _asyncLoadNextSampleBuffer];
    
    return sampleBuffer;
}

#pragma mark - Private Method
- (void)_setupDemuxReader:(NSError**)error {
    if (!self.config.asset) {
        *error = [NSError errorWithDomain:NSStringFromClass([self class]) code:40003 userInfo:nil];
        return;
    }
    
    // 1、创建解封装器实例。
    // 使用 AVAssetReader 作为解封装器。解封装的目标是 config 中的 AVAsset 资源。
    _demuxReader = [[AVAssetReader alloc] initWithAsset:self.config.asset error:error];
    if (!_demuxReader) {
        return;
    }
    
    // 2、获取时间信息。
    _duration = [self.config.asset duration];
    
    // 3、处理待解封装的资源中的视频。
    if (self.config.demuxerType & KFMediaVideo) {
        // 取出视频轨道。
        AVAssetTrack *videoTrack = [[self.config.asset tracksWithMediaType:AVMediaTypeVideo] firstObject];
        _hasVideoTrack = videoTrack ? YES : NO;
        if (_hasVideoTrack) {
            // 获取图像变换信息。
            _preferredTransform = videoTrack.preferredTransform;
            
            // 获取图像大小。要应用上图像变换信息。
            _videoSize = CGSizeApplyAffineTransform(videoTrack.naturalSize, videoTrack.preferredTransform);
            _videoSize = CGSizeMake(fabs(_videoSize.width), fabs(_videoSize.height));
            
            // 获取编码格式。
            CMVideoFormatDescriptionRef formatDescription = (__bridge CMVideoFormatDescriptionRef)[[videoTrack formatDescriptions] firstObject];
            if (formatDescription) {
                _codecType = CMVideoFormatDescriptionGetCodecType(formatDescription);
            }
            
            // 基于轨道创建视频输出。
            _readerVideoOutput = [[AVAssetReaderTrackOutput alloc] initWithTrack:videoTrack outputSettings:nil];
            _readerVideoOutput.alwaysCopiesSampleData = NO; // 避免总是做数据拷贝，影响性能。
            
            // 给解封装器绑定视频输出。
            if ([_demuxReader canAddOutput:_readerVideoOutput]) {
                [_demuxReader addOutput:_readerVideoOutput];
            } else {
                *error = _demuxReader.error ? _demuxReader.error : [NSError errorWithDomain:NSStringFromClass([self class]) code:KFMP4DemuxerAddVideoOutputError userInfo:nil];
                return;
            }
        }
    }
    
    // 4、处理待解封装的资源中的音频。
    if (self.config.demuxerType & KFMediaAudio) {
        // 取出音频轨道。
        AVAssetTrack *audioTrack = [[self.config.asset tracksWithMediaType:AVMediaTypeAudio] firstObject];
        _hasAudioTrack = audioTrack ? YES : NO;
        if (_hasAudioTrack) {
            // 基于轨道创建音频输出。
            _readerAudioOutput = [[AVAssetReaderTrackOutput alloc] initWithTrack:audioTrack outputSettings:nil];
            _readerAudioOutput.alwaysCopiesSampleData = NO; // 避免总是做数据拷贝，影响性能。
            
            // 给解封装器绑定音频输出。
            if ([_demuxReader canAddOutput:_readerAudioOutput]) {
                [_demuxReader addOutput:_readerAudioOutput];
            } else {
                *error = _demuxReader.error ? _demuxReader.error : [NSError errorWithDomain:NSStringFromClass([self class]) code:KFMP4DemuxerAddAudioOutputError userInfo:nil];
                return;
            }
        }
    }
    
    // 5、音频和视频数据都没有，就报错。
    if (!_hasVideoTrack && !_hasAudioTrack) {
        *error = [NSError errorWithDomain:NSStringFromClass([self class]) code:KFMP4DemuxerBadFileError userInfo:nil];
        return;
    }
    
    // 6、启动解封装。
    BOOL startSuccess = [self.demuxReader startReading];
    if (!startSuccess) {
        *error = self.demuxReader.error;
    }
}

- (void)_asyncLoadNextSampleBuffer {
    // 异步加载下一份采样数据。
    __weak typeof(self) weakSelf = self;
    dispatch_async(_demuxerQueue, ^{
        dispatch_semaphore_wait(weakSelf.demuxerSemaphore, DISPATCH_TIME_FOREVER);
        [weakSelf _loadNextSampleBuffer];
        dispatch_semaphore_signal(weakSelf.demuxerSemaphore);
    });
}

- (void)_syncLoadNextSampleBuffer {
    // 同步加载下一份采样数据。
    dispatch_semaphore_wait(self.demuxerSemaphore, DISPATCH_TIME_FOREVER);
    [self _loadNextSampleBuffer];
    dispatch_semaphore_signal(self.demuxerSemaphore);
}

- (void)_loadNextSampleBuffer {
    if (self.demuxerStatus != KFMP4DemuxerStatusRunning) {
        return;
    }
    
    // 1、根据解封装器的状态，处理异常情况。
    if (self.demuxReader.status == AVAssetWriterStatusCompleted) {
        self.demuxerStatus = KFMP4DemuxerStatusCompleted;
        return;
    } else if (self.demuxReader.status == AVAssetWriterStatusFailed) {
        if (self.demuxReader.error.code == AVErrorOperationInterrupted) {
            // 如果当前解封装器的状态是被打断而失败，就尝试重新创建一下。
            NSError *error;
            [self _setupDemuxReader:&error];
            if (!error) {
                // 同时做一下恢复处理。
                [self _resumeLastTime];
            }
        }
        
        if (self.demuxReader.status == AVAssetWriterStatusFailed) {
            // 如果状态依然是失败，就上报错误。
            self.demuxerStatus = KFMP4DemuxerStatusFailed;
            if (self.errorCallBack) {
                NSError *error = self.demuxReader.error;
                dispatch_async(dispatch_get_main_queue(), ^{
                    self.errorCallBack(error);
                });
            }
            return;
        }
    } else if (self.demuxReader.status == AVAssetWriterStatusCancelled) {
        // 如果状态是取消，就直接 return。
        self.demuxerStatus = KFMP4DemuxerStatusCancelled;
        return;
    }
    
    // 2、解封装器状态正常，加载下一份采样数据。
    BOOL audioNeedLoad = (self.config.demuxerType & KFMediaAudio) && !self.audioEOF;
    BOOL videoNeedLoad = (self.config.demuxerType & KFMediaVideo) && !self.videoEOF;
    while (self.demuxReader && self.demuxReader.status == AVAssetReaderStatusReading && (audioNeedLoad || videoNeedLoad)) {
        // 加载音频数据。
        if (audioNeedLoad) {
            dispatch_semaphore_wait(_audioQueueSemaphore, DISPATCH_TIME_FOREVER);
            int32_t audioCount = CMSimpleQueueGetCount(_audioQueue);
            dispatch_semaphore_signal(_audioQueueSemaphore);
            if (audioCount < KFMP4DemuxerQueueMaxCount) {
                // 从音频输出源读取音频数据。
                CMSampleBufferRef next = [self.readerAudioOutput copyNextSampleBuffer];
                if (next) {
                    if (!CMSampleBufferGetDataBuffer(next)) {
                        CFRelease(next);
                    } else {
                        // 将数据从音频输出源 readerAudioOutput 拷贝到缓冲队列 _audioQueue 中。
                        self.lastAudioCopyNextTime = CMSampleBufferGetPresentationTimeStamp(next);
                        dispatch_semaphore_wait(_audioQueueSemaphore, DISPATCH_TIME_FOREVER);
                        CMSimpleQueueEnqueue(_audioQueue, next);
                        audioCount = CMSimpleQueueGetCount(_audioQueue);
                        dispatch_semaphore_signal(_audioQueueSemaphore);
                    }
                } else {
                    self.audioEOF = self.demuxReader.status == AVAssetReaderStatusReading || self.demuxReader.status == AVAssetWriterStatusCompleted;
                    audioNeedLoad = NO;
                }
            } else {
                audioNeedLoad = NO;
            }
        }
        
        // 加载视频数据。
        if (videoNeedLoad) {
            dispatch_semaphore_wait(_videoQueueSemaphore, DISPATCH_TIME_FOREVER);
            int32_t videoCount = CMSimpleQueueGetCount(_videoQueue);
            dispatch_semaphore_signal(_videoQueueSemaphore);
            if (videoCount < KFMP4DemuxerQueueMaxCount) {
                // 从视频输出源读取视频数据。
                CMSampleBufferRef next = [self.readerVideoOutput copyNextSampleBuffer];
                if (next) {
                    if (!CMSampleBufferGetDataBuffer(next)) {
                        CFRelease(next);
                    } else {
                        // 将数据从视频输出源 readerVideoOutput 拷贝到缓冲队列 _videoQueue 中。
                        self.lastVideoCopyNextTime = CMSampleBufferGetDecodeTimeStamp(next);
                        dispatch_semaphore_wait(_videoQueueSemaphore, DISPATCH_TIME_FOREVER);
                        CMSimpleQueueEnqueue(_videoQueue, next);
                        videoCount = CMSimpleQueueGetCount(_videoQueue);
                        dispatch_semaphore_signal(_videoQueueSemaphore);
                    }
                } else {
                    self.videoEOF = self.demuxReader.status == AVAssetReaderStatusReading || self.demuxReader.status == AVAssetWriterStatusCompleted;
                    videoNeedLoad = NO;
                }
            } else {
                videoNeedLoad = NO;
            }
        }
    }
}

- (void)_resumeLastTime {
    // 对于异常中断后的处理，需要根据记录的时间戳 _lastAudioCopyNextTime/_lastVideoCopyNextTime 做恢复操作。
    BOOL audioNeedLoad = (_lastAudioCopyNextTime.value > 0) && !self.audioEOF;
    BOOL videoNeedLoad = (_lastVideoCopyNextTime.value > 0) && !self.videoEOF;
    while (self.demuxReader && self.demuxReader.status == AVAssetReaderStatusReading && (audioNeedLoad || videoNeedLoad)) {
        if (audioNeedLoad) {
            // 从音频输出源读取音频数据。
            CMSampleBufferRef next = [self.readerAudioOutput copyNextSampleBuffer];
            if (next) {
                if (CMTimeGetSeconds(CMSampleBufferGetPresentationTimeStamp(next)) <= CMTimeGetSeconds(_lastAudioCopyNextTime) || !CMSampleBufferGetDataBuffer(next)) {
                    // 从输出源取出的数据时间戳小于上次标记的时间，则表示这份采样数据已经处理过了。
                    CFRelease(next);
                } else {
                    dispatch_semaphore_wait(_audioQueueSemaphore, DISPATCH_TIME_FOREVER);
                    CMSimpleQueueEnqueue(_audioQueue, next);
                    dispatch_semaphore_signal(_audioQueueSemaphore);
                    audioNeedLoad = NO;
                }
            } else {
                self.audioEOF = self.demuxReader.status == AVAssetReaderStatusReading || self.demuxReader.status == AVAssetWriterStatusCompleted;
                audioNeedLoad = NO;
            }
        }
        
        if (videoNeedLoad) {
            // 从视频输出源读取视频数据。
            CMSampleBufferRef next = [self.readerVideoOutput copyNextSampleBuffer];
            if (next) {
                if (CMTimeGetSeconds(CMSampleBufferGetDecodeTimeStamp(next)) <= CMTimeGetSeconds(_lastVideoCopyNextTime) || !CMSampleBufferGetDataBuffer(next)) {
                    // 从输出源取出的数据时间戳小于上次标记的时间，则表示这份采样数据已经处理过了。
                    CFRelease(next);
                } else {
                    dispatch_semaphore_wait(_videoQueueSemaphore, DISPATCH_TIME_FOREVER);
                    CMSimpleQueueEnqueue(_videoQueue, next);
                    dispatch_semaphore_signal(_videoQueueSemaphore);
                    videoNeedLoad = NO;
                }
            } else {
                self.videoEOF = self.demuxReader.status == AVAssetReaderStatusReading || self.demuxReader.status == AVAssetWriterStatusCompleted;
                videoNeedLoad = NO;
            }
        }
    }
}

@end
```

上面是 `KFMP4Demuxer` 的实现，从代码上可以看到主要有这几个部分：

- 1）创建解封装器实例及对应的音频和视频数据输出源。第一次调用 `-startReading:` 时会创建解封装器实例，另外在 `-_loadNextSampleBuffer` 时如果发现当前解封装器的状态是被打断而失败时，会尝试重新创建解封装器实例。

- - 在 `-_setupDemuxReader:` 方法中实现。音频和视频的输出源分别是 `readerAudioOutput` 和 `readerVideoOutput`。

- 2）用两个队列作为缓冲区，分别管理音频和视频解封装后的数据。

- - 这两个队列分别是 `_audioQueue` 和 `_videoQueue`。
  - 当外部向解封装器要数据而触发数据加载时，会把解封装后的数据先缓存到这两个队列中，缓冲的采样数不超过 `KFMP4DemuxerQueueMaxCount`，以减少内存占用。

- 3）从音视频输出源读取数据。

- - 核心逻辑在 `-_loadNextSampleBuffer` 方法中实现：从输出源 `readerAudioOutput` 和 `readerVideoOutput` 读取数据放入缓冲区队列 `_audioQueue` 和 `_videoQueue`。
  - 在外部调用 `-copyNextAudioSampleBuffer`、`-copyNextVideoSampleBuffer` 时，触发读取数据。

- 4）从中断中恢复解封装。

- - 在 `-_resumeLastTime` 方法中实现。

- 5）停止解封装。

- - 在 `-cancelReading` 方法中实现。

- 6）解封装状态机管理。

- - 在枚举 `KFMP4DemuxerStatus` 中定义了解封装器的各种状态，对于解封装器的状态机管理贯穿在解封装的整个过程中。

- 7）错误回调。

- - 在 `-callBackError:` 方法向外回调错误。

- 8）清理封装器实例及数据缓冲区。

- - 在 `-dealloc` 方法中实现。

更具体细节见上述代码及其注释。

## 2、解封装 MP4 文件中的音频部分存储为 AAC 文件

我们还是在一个 ViewController 中来实现对一个 MP4 文件解封装、获取其中的音频编码数据并存储为 AAC 文件。

```
KFAudioDemuxerViewController.m
#import "KFAudioDemuxerViewController.h"
#import "KFMP4Demuxer.h"
#import "KFAudioTools.h"

@interface KFAudioDemuxerViewController ()
@property (nonatomic, strong) KFDemuxerConfig *demuxerConfig;
@property (nonatomic, strong) KFMP4Demuxer *demuxer;
@property (nonatomic, strong) NSFileHandle *fileHandle;
@end

@implementation KFAudioDemuxerViewController
#pragma mark - Property
- (KFDemuxerConfig *)demuxerConfig {
    if (!_demuxerConfig) {
        _demuxerConfig = [[KFDemuxerConfig alloc] init];
        // 只解封装音频。
        _demuxerConfig.demuxerType = KFMediaAudio;
        // 待解封装的资源。
        NSString *assetPath = [[NSBundle mainBundle] pathForResource:@"input" ofType:@"mp4"];
        _demuxerConfig.asset = [AVAsset assetWithURL:[NSURL fileURLWithPath:assetPath]];
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

- (NSFileHandle *)fileHandle {
    if (!_fileHandle) {
        NSString *audioPath = [[NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) lastObject] stringByAppendingPathComponent:@"output.aac"];
        [[NSFileManager defaultManager] removeItemAtPath:audioPath error:nil];
        [[NSFileManager defaultManager] createFileAtPath:audioPath contents:nil attributes:nil];
        _fileHandle = [NSFileHandle fileHandleForWritingAtPath:audioPath];
    }

    return _fileHandle;
}

#pragma mark - Lifecycle
- (void)viewDidLoad {
    [super viewDidLoad];

    [self setupUI];
    
    // 完成音频解封装后，可以将 App Document 文件夹下面的 output.aac 文件拷贝到电脑上，使用 ffplay 播放：
    // ffplay -i output.aac
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
    self.title = @"Audio Demuxer";
    self.view.backgroundColor = [UIColor whiteColor];
    
    // Navigation item.
    UIBarButtonItem *startBarButton = [[UIBarButtonItem alloc] initWithTitle:@"Start" style:UIBarButtonItemStylePlain target:self action:@selector(start)];
    self.navigationItem.rightBarButtonItems = @[startBarButton];
}

#pragma mark - Action
- (void)start {
    NSLog(@"KFMP4Demuxer start");
    __weak typeof(self) weakSelf = self;
    [self.demuxer startReading:^(BOOL success, NSError * _Nonnull error) {
        if (success) {
            // Demuxer 启动成功后，就可以从它里面获取解封装后的数据了。
            [weakSelf fetchAndSaveDemuxedData];
        } else {
            NSLog(@"KFMP4Demuxer error: %zi %@", error.code, error.localizedDescription);
        }
    }];
}

#pragma mark - Utility
- (void)fetchAndSaveDemuxedData {
    // 异步地从 Demuxer 获取解封装后的 AAC 编码数据。
    dispatch_async(dispatch_get_global_queue(0, 0), ^{
        while (self.demuxer.hasAudioSampleBuffer) {
            CMSampleBufferRef audioBuffer = [self.demuxer copyNextAudioSampleBuffer];
            if (audioBuffer) {
                [self saveSampleBuffer:audioBuffer];
                CFRelease(audioBuffer);
            }
        }
        if (self.demuxer.demuxerStatus == KFMP4DemuxerStatusCompleted) {
            NSLog(@"KFMP4Demuxer complete");
        }
    });
}

- (void)saveSampleBuffer:(CMSampleBufferRef)sampleBuffer {
    // 将解封装后的数据存储为 AAC 文件。
    if (sampleBuffer) {
        // 获取解封装后的 AAC 编码裸数据。
        AudioStreamBasicDescription streamBasicDescription = *CMAudioFormatDescriptionGetStreamBasicDescription(CMSampleBufferGetFormatDescription(sampleBuffer));
        CMBlockBufferRef blockBuffer = CMSampleBufferGetDataBuffer(sampleBuffer);
        size_t totolLength;
        char *dataPointer = NULL;
        CMBlockBufferGetDataPointer(blockBuffer, 0, NULL, &totolLength, &dataPointer);
        if (totolLength == 0 || !dataPointer) {
            return;
        }
        
        // 将 AAC 编码裸数据存储为 AAC 文件，这时候需要在每个包前增加 ADTS 头信息。
        for (NSInteger index = 0; index < CMSampleBufferGetNumSamples(sampleBuffer); index++) {
            size_t sampleSize = CMSampleBufferGetSampleSize(sampleBuffer, index);
            [self.fileHandle writeData:[KFAudioTools adtsDataWithChannels:streamBasicDescription.mChannelsPerFrame sampleRate:streamBasicDescription.mSampleRate rawDataLength:sampleSize]];
            [self.fileHandle writeData:[NSData dataWithBytes:dataPointer length:sampleSize]];
            dataPointer += sampleSize;
        }
    }
}

@end
```

上面是 `KFAudioDemuxerViewController` 的实现，其中主要包含这几个部分：

- 1）设置好待解封装的资源。

- - 在 `-demuxerConfig` 中实现，我们这里是一个 MP4 文件。

- 2）启动解封装器。

- - 在 `-start` 中实现。

- 3）读取解封装后的音频编码数据并存储为 AAC 文件。

- - 在 `-fetchAndSaveDemuxedData` → `-saveSampleBuffer` 中实现。
  - 需要注意的是，我们从解封装器读取的音频 AAC 编码数据在存储为 AAC 文件时需要条件 ADTS 头。生成一个 AAC packet 对应的 ADTS 头数据在 `KFAudioTools` 类的工具方法 `+adtsDataWithChannels:sampleRate:rawDataLength:` 中实现。这个在前面的音频编码的 Demo 中已经介绍过了。

## 3、用工具播放 AAC 文件

完成音频采集和编码后，可以将 App Document 文件夹下面的 `output.aac` 文件拷贝到电脑上，使用 `ffplay` 播放来验证一下音频采集是效果是否符合预期：

```
$ ffplay -i output.aac
```

关于播放 AAC 文件的工具，可以参考[《FFmpeg 工具》第 2 节 ffplay 命令行工具](https://mp.weixin.qq.com/s?__biz=MjM5MTkxOTQyMQ==&mid=2257484831&idx=1&sn=6bab905a5040c46b971bab05f787788b&scene=21#wechat_redirect)和[《可视化音视频分析工具》第 1.1 节 Adobe Audition](https://mp.weixin.qq.com/s?__biz=MjM5MTkxOTQyMQ==&mid=2257484834&idx=1&sn=5dd9768bfc0d01ca1b036be8dd2f5fa1&scene=21#wechat_redirect)。

## 4、参考资料

[1]CMSampleBufferRef: *https://developer.apple.com/documentation/coremedia/cmsamplebufferref/*

原文链接：https://mp.weixin.qq.com/s/fCZfIXriTXUPcI4d4te_ew