# iOS AVDemo（8）：视频编码，H.264 和 H.265 都支持

iOS/Android 客户端开发同学如果想要开始学习音视频开发，最丝滑的方式是对[音视频基础概念知识](https://mp.weixin.qq.com/mp/appmsgalbum?__biz=MjM5MTkxOTQyMQ==&action=getalbum&album_id=2140155659944787969#wechat_redirect)有一定了解后，再借助 iOS/Android 平台的音视频能力上手去实践音视频的`采集 → 编码 → 封装 → 解封装 → 解码 → 渲染`过程，并借助[音视频工具](https://mp.weixin.qq.com/mp/appmsgalbum?__biz=MjM5MTkxOTQyMQ==&action=getalbum&album_id=2216997905264082945#wechat_redirect)来分析和理解对应的音视频数据。

在[音视频工程示例](https://mp.weixin.qq.com/mp/appmsgalbum?__biz=MjM5MTkxOTQyMQ==&action=getalbum&album_id=2273301900659851268#wechat_redirect)这个栏目，我们将通过拆解`采集 → 编码 → 封装 → 解封装 → 解码 → 渲染`流程并实现 Demo 来向大家介绍如何在 iOS/Android 平台上手音视频开发。

这里是第八篇：**iOS 视频编码 Demo**。这个 Demo 里包含以下内容：

- 1）实现一个视频采集模块；
- 2）实现一个视频编码模块，支持 H.264/H.265；
- 3）串联视频采集和编码模块，将采集到的视频数据输入给编码模块进行编码，并存储为文件；
- 4）详尽的代码注释，帮你理解代码逻辑和原理。

想要了解视频编码，可以看看这几篇：

- [《视频编码（1）：H.264（AVC）》](https://mp.weixin.qq.com/s?__biz=MjM5MTkxOTQyMQ==&mid=2257484471&idx=1&sn=421be18e5b591043f13996734c60780b&scene=21#wechat_redirect)
- [《视频编码（2）：H.265（HEVC）》](https://mp.weixin.qq.com/s?__biz=MjM5MTkxOTQyMQ==&mid=2257484563&idx=1&sn=f08f9994ef7d8a6ee09491e870c6e843&scene=21#wechat_redirect)
- [《视频编码（3）：H.266（VVC）》](https://mp.weixin.qq.com/s?__biz=MjM5MTkxOTQyMQ==&mid=2257484673&idx=1&sn=9a6f5e69d9af825b85210c023b85b81a&scene=21#wechat_redirect)

## 1、视频采集模块

在这个 Demo 中，视频采集模块 `KFVideoCapture` 的实现与 [《iOS 视频采集 Demo》](https://mp.weixin.qq.com/s?__biz=MjM5MTkxOTQyMQ==&mid=2257485011&idx=1&sn=8bb9cfa01deba9670e9999bd20892440&scene=21#wechat_redirect) 中一样，这里就不再重复介绍了，其接口如下：

```
KFVideoCapture.h
#import <Foundation/Foundation.h>
#import "KFVideoCaptureConfig.h"

NS_ASSUME_NONNULL_BEGIN

@interface KFVideoCapture : NSObject
+ (instancetype)new NS_UNAVAILABLE;
- (instancetype)init NS_UNAVAILABLE;
- (instancetype)initWithConfig:(KFVideoCaptureConfig *)config;

@property (nonatomic, strong, readonly) KFVideoCaptureConfig *config;
@property (nonatomic, strong, readonly) AVCaptureVideoPreviewLayer *previewLayer; // 视频预览渲染 layer。
@property (nonatomic, copy) void (^sampleBufferOutputCallBack)(CMSampleBufferRef sample); // 视频采集数据回调。
@property (nonatomic, copy) void (^sessionErrorCallBack)(NSError *error); // 视频采集会话错误回调。
@property (nonatomic, copy) void (^sessionInitSuccessCallBack)(void); // 视频采集会话初始化成功回调。

- (void)startRunning; // 开始采集。
- (void)stopRunning; // 停止采集。
- (void)changeDevicePosition:(AVCaptureDevicePosition)position; // 切换摄像头。
@end

NS_ASSUME_NONNULL_END
```

## 2、视频编码模块

在实现视频编码模块之前，我们先实现一个视频编码配置类 `KFVideoEncoderConfig`：

```
KFVideoEncoderConfig.h
#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface KFVideoEncoderConfig : NSObject
@property (nonatomic, assign) CGSize size; // 分辨率。
@property (nonatomic, assign) NSInteger bitrate; // 码率。
@property (nonatomic, assign) NSInteger fps; // 帧率。
@property (nonatomic, assign) NSInteger gopSize; // GOP 帧数。
@property (nonatomic, assign) BOOL openBFrame; // 编码是否使用 B 帧。
@property (nonatomic, assign) CMVideoCodecType codecType; // 编码器类型。
@property (nonatomic, assign) NSString *profile; // 编码 profile。
@end

NS_ASSUME_NONNULL_END
KFVideoEncoderConfig.m
#import "KFVideoEncoderConfig.h"
#import <VideoToolBox/VideoToolBox.h>

@implementation KFVideoEncoderConfig

- (instancetype)init {
    self = [super init];
    if (self) {
        _size = CGSizeMake(1080, 1920);
        _bitrate = 5000 * 1024;
        _fps = 30;
        _gopSize = _fps * 5;
        _openBFrame = YES;
        
        BOOL supportHEVC = NO;
        if (@available(iOS 11.0, *)) {
            if (&VTIsHardwareDecodeSupported) {
                supportHEVC = VTIsHardwareDecodeSupported(kCMVideoCodecType_HEVC);
            }
        }
        
        _codecType = supportHEVC ? kCMVideoCodecType_HEVC : kCMVideoCodecType_H264;
        _profile = supportHEVC ? (__bridge NSString *) kVTProfileLevel_HEVC_Main_AutoLevel : AVVideoProfileLevelH264HighAutoLevel;
    }
    
    return self;
}

@end
```

这里实现了在设备支持 H.265 时，默认选择 H.265 编码。

接下来，我们来实现一个视频编码模块 `KFVideoEncoder`，在这里输入采集后的数据，输出编码后的数据。

```
KFVideoEncoder.h
#import <Foundation/Foundation.h>
#import "KFVideoEncoderConfig.h"

NS_ASSUME_NONNULL_BEGIN

@interface KFVideoEncoder : NSObject
+ (instancetype)new NS_UNAVAILABLE;
- (instancetype)init NS_UNAVAILABLE;
- (instancetype)initWithConfig:(KFVideoEncoderConfig*)config;

@property (nonatomic, strong, readonly) KFVideoEncoderConfig *config; // 视频编码配置参数。
@property (nonatomic, copy) void (^sampleBufferOutputCallBack)(CMSampleBufferRef sampleBuffer); // 视频编码数据回调。
@property (nonatomic, copy) void (^errorCallBack)(NSError *error); // 视频编码错误回调。

- (void)encodePixelBuffer:(CVPixelBufferRef)pixelBuffer ptsTime:(CMTime)timeStamp; // 编码。
- (void)refresh; // 刷新重建编码器。
- (void)flush; // 清空编码缓冲区。
- (void)flushWithCompleteHandler:(void (^)(void))completeHandler; // 清空编码缓冲区并回调完成。
@end

NS_ASSUME_NONNULL_END
```

上面是 `KFVideoEncoder` 接口的设计，除了`初始化方法`，主要是有`获取视频编码配置`以及视频编码`数据回调`和`错误回调`的接口，另外就是`编码`、`刷新重建编码器`、`清空编码缓冲区`的接口。

其中`编码`接口对应着视频编码模块输入，`数据回调`接口则对应着输出。可以看到这里输出参数我们依然用的是 **CMSampleBufferRef**[1] 这个数据结构。不过输入的参数换成了 **CVPixelBufferRef**[2] 这个数据结构。它是对 `CVPixelBuffer` 的一个引用。

之前我们介绍过，`CMSampleBuffer` 中包含着零个或多个某一类型（audio、video、muxed 等）的采样数据。比如：

- 要么是一个或多个媒体采样的 **CMBlockBuffer**[3]。其中可以封装：音频采集后、编码后、解码后的数据（如：PCM 数据、AAC 数据）；视频编码后的数据（如：H.264/H.265 数据）。
- 要么是一个 **CVImageBuffer**[4]（也作 **CVPixelBuffer**[5]）。其中包含媒体流中 CMSampleBuffers 的格式描述、每个采样的宽高和时序信息、缓冲级别和采样级别的附属信息。缓冲级别的附属信息是指缓冲区整体的信息，比如播放速度、对后续缓冲数据的操作等。采样级别的附属信息是指单个采样的信息，比如视频帧的时间戳、是否关键帧等。其中可以封装：视频采集后、解码后等未经编码的数据（如：YCbCr 数据、RGBA 数据）。

所以，因为是视频编码的接口，这里用 `CVPixelBufferRef` 也就是图一个方便，其实也可以用 `CMSampleBufferRef`，只要编码用 `CMSampleBufferGetImageBuffer(...)` 取出对应的 `CVPixelBufferRef` 即可。

```
KFVideoEncoder.m
#import "KFVideoEncoder.h"
#import <VideoToolBox/VideoToolBox.h>
#import <UIKit/UIKit.h>

#define KFEncoderRetrySessionMaxCount 5
#define KFEncoderEncodeFrameFailedMaxCount 20

@interface KFVideoEncoder ()
@property (nonatomic, assign) VTCompressionSessionRef compressionSession;
@property (nonatomic, strong, readwrite) KFVideoEncoderConfig *config; // 视频编码配置参数。
@property (nonatomic, strong) dispatch_queue_t encoderQueue;
@property (nonatomic, strong) dispatch_semaphore_t semaphore;
@property (nonatomic, assign) BOOL needRefreshSession; // 是否需要刷新重建编码器。
@property (nonatomic, assign) NSInteger retrySessionCount; // 刷新重建编码器的次数。
@property (nonatomic, assign) NSInteger encodeFrameFailedCount; // 编码失败次数。
@end

@implementation KFVideoEncoder
#pragma mark - LifeCycle
- (instancetype)initWithConfig:(KFVideoEncoderConfig *)config {
    self = [super init];
    if (self) {
        _config = config;
        _encoderQueue = dispatch_queue_create("com.KeyFrameKit.videoEncoder", DISPATCH_QUEUE_SERIAL);
        _semaphore = dispatch_semaphore_create(1);
        [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(didEnterBackground:) name:UIApplicationDidEnterBackgroundNotification object:nil];
    }
    
    return self;
}

- (void)dealloc {
    [[NSNotificationCenter defaultCenter] removeObserver:self];
    dispatch_semaphore_wait(_semaphore, DISPATCH_TIME_FOREVER);
    [self _releaseCompressionSession];
    dispatch_semaphore_signal(_semaphore);
}

#pragma mark - Public Method
- (void)refresh {
    self.needRefreshSession = YES; // 标记位待刷新重建编码器。
}

- (void)flush {
    // 清空编码缓冲区。
    __weak typeof(self) weakSelf = self;
    dispatch_async(self.encoderQueue, ^{
        dispatch_semaphore_wait(weakSelf.semaphore, DISPATCH_TIME_FOREVER);
        [weakSelf _flush];
        dispatch_semaphore_signal(weakSelf.semaphore);
    });
}

- (void)flushWithCompleteHandler:(void (^)(void))completeHandler {
    // 清空编码缓冲区并回调完成。
    __weak typeof(self) weakSelf = self;
    dispatch_async(self.encoderQueue, ^{
        dispatch_semaphore_wait(weakSelf.semaphore, DISPATCH_TIME_FOREVER);
        [weakSelf _flush];
        dispatch_semaphore_signal(weakSelf.semaphore);
        if (completeHandler) {
            completeHandler();
        }
    });
}

- (void)encodePixelBuffer:(CVPixelBufferRef)pixelBuffer ptsTime:(CMTime)timeStamp {
    // 编码。
    if (!pixelBuffer || self.retrySessionCount >= KFEncoderRetrySessionMaxCount || self.encodeFrameFailedCount >= KFEncoderEncodeFrameFailedMaxCount) {
        return;
    }
    
    CFRetain(pixelBuffer);
    __weak typeof(self) weakSelf = self;
    dispatch_async(self.encoderQueue, ^{
        dispatch_semaphore_wait(weakSelf.semaphore, DISPATCH_TIME_FOREVER);
        OSStatus setupStatus = noErr;
        // 1、如果还没创建过编码器或者需要刷新重建编码器，就创建编码器。
        if (!weakSelf.compressionSession || weakSelf.needRefreshSession) {
            [weakSelf _releaseCompressionSession];
            setupStatus = [weakSelf _setupCompressionSession];
            // 支持重试，记录重试次数。
            weakSelf.retrySessionCount = setupStatus == noErr ? 0 : (weakSelf.retrySessionCount + 1);
            if (setupStatus != noErr) {
                [weakSelf _releaseCompressionSession];
                NSLog(@"KFVideoEncoder setupCompressionSession error:%d", setupStatus);
            } else {
                weakSelf.needRefreshSession = NO;
            }
        }
        
        // 重试超过 KFEncoderRetrySessionMaxCount 次仍然失败则认为创建失败，报错。
        if (!weakSelf.compressionSession) {
            CFRelease(pixelBuffer);
            dispatch_semaphore_signal(weakSelf.semaphore);
            if (weakSelf.retrySessionCount >= KFEncoderRetrySessionMaxCount && weakSelf.errorCallBack) {
                dispatch_async(dispatch_get_main_queue(), ^{
                    weakSelf.errorCallBack([NSError errorWithDomain:NSStringFromClass([KFVideoEncoder class]) code:setupStatus userInfo:nil]);
                });
            }
            return;
        }
        
        // 2、对 pixelBuffer 进行编码。
        VTEncodeInfoFlags flags;
        OSStatus encodeStatus = VTCompressionSessionEncodeFrame(weakSelf.compressionSession, pixelBuffer, timeStamp, CMTimeMake(1, (int32_t) weakSelf.config.fps), NULL, NULL, &flags);
        if (encodeStatus == kVTInvalidSessionErr) {
            // 编码失败进行重建编码器重试。
            [weakSelf _releaseCompressionSession];
            setupStatus = [weakSelf _setupCompressionSession];
            weakSelf.retrySessionCount = setupStatus == noErr ? 0 : (weakSelf.retrySessionCount + 1);
            if (setupStatus == noErr) {
                encodeStatus = VTCompressionSessionEncodeFrame(weakSelf.compressionSession, pixelBuffer, timeStamp, CMTimeMake(1, (int32_t) weakSelf.config.fps), NULL, NULL, &flags);
            } else {
                [weakSelf _releaseCompressionSession];
            }
            
            NSLog(@"KFVideoEncoder kVTInvalidSessionErr");
        }
        // 记录编码失败次数。
        if (encodeStatus != noErr) {
            NSLog(@"KFVideoEncoder VTCompressionSessionEncodeFrame error:%d", encodeStatus);
        }
        weakSelf.encodeFrameFailedCount = encodeStatus == noErr ? 0 : (weakSelf.encodeFrameFailedCount + 1);
        
        CFRelease(pixelBuffer);
        dispatch_semaphore_signal(weakSelf.semaphore);
        
        // 编码失败次数超过 KFEncoderEncodeFrameFailedMaxCount 次，报错。
        if (weakSelf.encodeFrameFailedCount >= KFEncoderEncodeFrameFailedMaxCount && weakSelf.errorCallBack) {
            dispatch_async(dispatch_get_main_queue(), ^{
                weakSelf.errorCallBack([NSError errorWithDomain:NSStringFromClass([KFVideoEncoder class]) code:encodeStatus userInfo:nil]);
            });
        }
    });
}

#pragma mark - Privte Method
- (OSStatus)_setupCompressionSession {
    if (_compressionSession) {
        return noErr;
    }
    
    // 1、创建视频编码器实例。
    // 这里要设置画面尺寸、编码器类型、编码数据回调。
    OSStatus status = VTCompressionSessionCreate(NULL, _config.size.width, _config.size.height, _config.codecType, NULL, NULL, NULL, encoderOutputCallback, (__bridge void *) self, &_compressionSession);
    if (status != noErr) {
        return status;
    }
    
    // 2、设置编码器属性：实时编码。
    VTSessionSetProperty(_compressionSession, kVTCompressionPropertyKey_RealTime, (__bridge CFTypeRef) @(YES));
    
    // 3、设置编码器属性：编码 profile。
    status = VTSessionSetProperty(_compressionSession, kVTCompressionPropertyKey_ProfileLevel, (__bridge CFStringRef) self.config.profile);
    if (status != noErr) {
        return status;
    }
    
    // 4、设置编码器属性：是否支持 B 帧。
    status = VTSessionSetProperty(_compressionSession, kVTCompressionPropertyKey_AllowFrameReordering, (__bridge CFTypeRef) @(self.config.openBFrame));
    if (status != noErr) {
        return status;
    }
    
    if (self.config.codecType == kCMVideoCodecType_H264) {
        // 5、如果是 H.264 编码，设置编码器属性：熵编码类型为 CABAC，上下文自适应的二进制算术编码。
        status = VTSessionSetProperty(_compressionSession, kVTCompressionPropertyKey_H264EntropyMode, kVTH264EntropyMode_CABAC);
        if (status != noErr) {
            return status;
        }
    }
    
    // 6、设置编码器属性：画面填充模式。
    NSDictionary *transferDic= @{
        (__bridge NSString *) kVTPixelTransferPropertyKey_ScalingMode: (__bridge NSString *) kVTScalingMode_Letterbox,
    };
    status = VTSessionSetProperty(_compressionSession, kVTCompressionPropertyKey_PixelTransferProperties, (__bridge CFTypeRef) (transferDic));
    if (status != noErr) {
        return status;
    }
    
    // 7、设置编码器属性：平均码率。
    status = VTSessionSetProperty(_compressionSession, kVTCompressionPropertyKey_AverageBitRate, (__bridge CFTypeRef) @(self.config.bitrate));
    if (status != noErr) {
        return status;
    }
    
    // 8、设置编码器属性：码率上限。
    if (!self.config.openBFrame && self.config.codecType == kCMVideoCodecType_H264) {
        NSArray *limit = @[@(self.config.bitrate * 1.5 / 8), @(1)];
        status = VTSessionSetProperty(_compressionSession, kVTCompressionPropertyKey_DataRateLimits, (__bridge CFArrayRef) limit);
        if (status != noErr) {
            return status;
        }
    }
    
    // 9、设置编码器属性：期望帧率。
    status = VTSessionSetProperty(_compressionSession, kVTCompressionPropertyKey_ExpectedFrameRate, (__bridge CFTypeRef) @(self.config.fps));
    if (status != noErr) {
        return status;
    }
    
    // 10、设置编码器属性：最大关键帧间隔帧数，也就是 GOP 帧数。
    status = VTSessionSetProperty(_compressionSession, kVTCompressionPropertyKey_MaxKeyFrameInterval, (__bridge CFTypeRef) @(self.config.gopSize));
    if (status != noErr) {
        return status;
    }
    
    // 11、设置编码器属性：最大关键帧间隔时长。
    status = VTSessionSetProperty(_compressionSession, kVTCompressionPropertyKey_MaxKeyFrameIntervalDuration, (__bridge CFTypeRef)@(_config.gopSize / _config.fps));
    if (status != noErr) {
        return status;
    }
    
    // 12、预备编码。
    status = VTCompressionSessionPrepareToEncodeFrames(_compressionSession);
    return status;
}

- (void)_releaseCompressionSession {
    if (_compressionSession) {
        // 强制处理完所有待编码的帧。
        VTCompressionSessionCompleteFrames(_compressionSession, kCMTimeInvalid);
        // 销毁编码器。
        VTCompressionSessionInvalidate(_compressionSession);
        CFRelease(_compressionSession);
        _compressionSession = NULL;
    }
}

- (void)_flush {
    // 清空编码缓冲区。
    if (_compressionSession) {
        // 传入 kCMTimeInvalid 时，强制处理完所有待编码的帧，清空缓冲区。
        VTCompressionSessionCompleteFrames(_compressionSession, kCMTimeInvalid);
    }
}

#pragma mark - NSNotification
- (void)didEnterBackground:(NSNotification *)notification {
    self.needRefreshSession = YES; // 退后台回来后需要刷新重建编码器。
}

#pragma mark - EncoderOutputCallback
static void encoderOutputCallback(void * CM_NULLABLE outputCallbackRefCon,
                                  void * CM_NULLABLE sourceFrameRefCon,
                                  OSStatus status,
                                  VTEncodeInfoFlags infoFlags,
                                  CMSampleBufferRef sampleBuffer) {
    if (!sampleBuffer) {
        if (infoFlags & kVTEncodeInfo_FrameDropped) {
            NSLog(@"VideoToolboxEncoder kVTEncodeInfo_FrameDropped");
        }
        return;
    }
    
    // 向外层回调编码数据。
    KFVideoEncoder *videoEncoder = (__bridge KFVideoEncoder *) outputCallbackRefCon;
    if (videoEncoder && videoEncoder.sampleBufferOutputCallBack) {
        videoEncoder.sampleBufferOutputCallBack(sampleBuffer);
    }
}

@end
```

上面是 `KFVideoEncoder` 的实现，从代码上可以看到主要有这几个部分：

- 1）创建视频编码实例。

- - 在 `-_setupCompressionSession` 方法中实现。

- 2）实现视频编码逻辑，并在编码实例的数据回调中接收编码后的数据，抛给对外数据回调接口。

- - 在 `-encodePixelBuffer:ptsTime:` 方法中实现。
  - 回调在 `encoderOutputCallback` 中实现。

- 3）实现清空编码缓冲区功能。

- - 在 `-_flush` 方法中实现。

- 4）刷新重建编码器功能。

- - 在 `-refresh` 方法中标记需要刷新重建，在 `-encodePixelBuffer:ptsTime:` 方法检查标记并重建编码器实例。

- 5）捕捉视频编码过程中的错误，抛给对外错误回调接口。

- - 主要在 `-encodePixelBuffer:ptsTime:` 方法捕捉错误。

- 6）清理视频编码器实例。

- - 在 `-_releaseCompressionSession` 方法中实现。

更具体细节见上述代码及其注释。

## 3、采集视频数据进行 H.264/H.265 编码和存储

我们在一个 ViewController 中来实现视频采集及编码逻辑，并且示范了将 iOS 编码的 AVCC/HVCC 码流格式转换为 AnnexB 码流格式后再存储。

我们先来简单介绍一下这两种格式的区别：

AVCC/HVCC 码流格式类似：

```
[extradata]|[length][NALU]|[length][NALU]|...
```

- VPS、SPS、PPS 不用 NALU 来存储，而是存储在 `extradata` 中；
- 每个 NALU 前有个 `length` 字段表示这个 NALU 的长度（不包含 `length` 字段），`length` 字段通常是 4 字节。

AnnexB 码流格式：

```
[startcode][NALU]|[startcode][NALU]|...
```

需要注意的是：

- 每个 NALU 前要添加起始码：`0x00000001`；
- VPS、SPS、PPS 也都用这样的 NALU 来存储，一般在码流最前面。

iOS 的 VideoToolbox 编码和解码只支持 AVCC/HVCC 的码流格式。但是 Android 的 MediaCodec 只支持 AnnexB 的码流格式。

```
KFVideoEncoderViewController.m
#import "KFVideoEncoderViewController.h"
#import "KFVideoCapture.h"
#import "KFVideoEncoder.h"

@interface KFVideoPacketExtraData : NSObject
@property (nonatomic, strong) NSData *sps;
@property (nonatomic, strong) NSData *pps;
@property (nonatomic, strong) NSData *vps;
@end

@implementation KFVideoPacketExtraData
@end


@interface KFVideoEncoderViewController ()
@property (nonatomic, strong) KFVideoCaptureConfig *videoCaptureConfig;
@property (nonatomic, strong) KFVideoCapture *videoCapture;
@property (nonatomic, strong) KFVideoEncoderConfig *videoEncoderConfig;
@property (nonatomic, strong) KFVideoEncoder *videoEncoder;
@property (nonatomic, assign) BOOL isEncoding;
@property (nonatomic, strong) NSFileHandle *fileHandle;
@end

@implementation KFVideoEncoderViewController
#pragma mark - Property
- (KFVideoCaptureConfig *)videoCaptureConfig {
    if (!_videoCaptureConfig) {
        _videoCaptureConfig = [[KFVideoCaptureConfig alloc] init];
        // 这里我们采集数据用于编码，颜色格式用了默认的：kCVPixelFormatType_420YpCbCr8BiPlanarFullRange。
    }
    return _videoCaptureConfig;
}

- (KFVideoCapture *)videoCapture {
    if (!_videoCapture) {
        _videoCapture = [[KFVideoCapture alloc] initWithConfig:self.videoCaptureConfig];
        __weak typeof(self) weakSelf = self;
        _videoCapture.sessionInitSuccessCallBack = ^() {
            dispatch_async(dispatch_get_main_queue(), ^{
                // 预览渲染。
                [weakSelf.view.layer insertSublayer:weakSelf.videoCapture.previewLayer atIndex:0];
                weakSelf.videoCapture.previewLayer.backgroundColor = [UIColor blackColor].CGColor;
                weakSelf.videoCapture.previewLayer.frame = weakSelf.view.bounds;
            });
        };
        _videoCapture.sampleBufferOutputCallBack = ^(CMSampleBufferRef sampleBuffer) {
            if (weakSelf.isEncoding && sampleBuffer) {
                // 编码。
                [weakSelf.videoEncoder encodePixelBuffer:CMSampleBufferGetImageBuffer(sampleBuffer) ptsTime:CMSampleBufferGetPresentationTimeStamp(sampleBuffer)];
            }
        };
        _videoCapture.sessionErrorCallBack = ^(NSError* error) {
            NSLog(@"KFVideoCapture Error:%zi %@", error.code, error.localizedDescription);
        };
    }
    
    return _videoCapture;
}

- (KFVideoEncoderConfig *)videoEncoderConfig {
    if (!_videoEncoderConfig) {
        _videoEncoderConfig = [[KFVideoEncoderConfig alloc] init];
    }
    
    return _videoEncoderConfig;
}

- (KFVideoEncoder *)videoEncoder {
    if (!_videoEncoder) {
        _videoEncoder = [[KFVideoEncoder alloc] initWithConfig:self.videoEncoderConfig];
        __weak typeof(self) weakSelf = self;
        _videoEncoder.sampleBufferOutputCallBack = ^(CMSampleBufferRef sampleBuffer) {
            // 保存编码后的数据。
            [weakSelf saveSampleBuffer:sampleBuffer];
        };
    }
    return _videoEncoder;
}

- (NSFileHandle *)fileHandle {
    if (!_fileHandle) {
        NSString *fileName = @"test.h264";
        if (self.videoEncoderConfig.codecType == kCMVideoCodecType_HEVC) {
            fileName = @"test.h265";
        }
        NSString *videoPath = [[NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) lastObject] stringByAppendingPathComponent:fileName];
        [[NSFileManager defaultManager] removeItemAtPath:videoPath error:nil];
        [[NSFileManager defaultManager] createFileAtPath:videoPath contents:nil attributes:nil];
        _fileHandle = [NSFileHandle fileHandleForWritingAtPath:videoPath];
    }

    return _fileHandle;
}

#pragma mark - Lifecycle
- (void)viewDidLoad {
    [super viewDidLoad];
    
    // Navigation item.
    UIBarButtonItem *startBarButton = [[UIBarButtonItem alloc] initWithTitle:@"Start" style:UIBarButtonItemStylePlain target:self action:@selector(start)];
    UIBarButtonItem *stopBarButton = [[UIBarButtonItem alloc] initWithTitle:@"Stop" style:UIBarButtonItemStylePlain target:self action:@selector(stop)];
    UIBarButtonItem *cameraBarButton = [[UIBarButtonItem alloc] initWithTitle:@"Camera" style:UIBarButtonItemStylePlain target:self action:@selector(changeCamera)];
    self.navigationItem.rightBarButtonItems = @[stopBarButton,startBarButton,cameraBarButton];
    
    [self requestAccessForVideo];
    
    UITapGestureRecognizer *doubleTapGesture = [[UITapGestureRecognizer alloc]initWithTarget:self action:@selector(handleDoubleTap:)];
    doubleTapGesture.numberOfTapsRequired = 2;
    doubleTapGesture.numberOfTouchesRequired = 1;
    [self.view addGestureRecognizer:doubleTapGesture];
}

- (void)viewWillLayoutSubviews {
    [super viewWillLayoutSubviews];
    self.videoCapture.previewLayer.frame = self.view.bounds;
}

- (void)dealloc {
    
}

#pragma mark - Action
- (void)start {
    if (!self.isEncoding) {
        self.isEncoding = YES;
        [self.videoEncoder refresh];
    }
}

- (void)stop {
    if (self.isEncoding) {
        self.isEncoding = NO;
        [self.videoEncoder flush];
    }
}

- (void)onCameraSwitchButtonClicked:(UIButton *)button {
    [self.videoCapture changeDevicePosition:self.videoCapture.config.position == AVCaptureDevicePositionBack ? AVCaptureDevicePositionFront : AVCaptureDevicePositionBack];
}

- (void)changeCamera {
    [self.videoCapture changeDevicePosition:self.videoCapture.config.position == AVCaptureDevicePositionBack ? AVCaptureDevicePositionFront : AVCaptureDevicePositionBack];
}

-(void)handleDoubleTap:(UIGestureRecognizer *)sender {
    [self.videoCapture changeDevicePosition:self.videoCapture.config.position == AVCaptureDevicePositionBack ? AVCaptureDevicePositionFront : AVCaptureDevicePositionBack];
}

#pragma mark - Private Method
- (void)requestAccessForVideo{
    __weak typeof(self) weakSelf = self;
    AVAuthorizationStatus status = [AVCaptureDevice authorizationStatusForMediaType:AVMediaTypeVideo];
    switch (status) {
        case AVAuthorizationStatusNotDetermined: {
            // 许可对话没有出现，发起授权许可。
            [AVCaptureDevice requestAccessForMediaType:AVMediaTypeVideo completionHandler:^(BOOL granted) {
                if (granted) {
                    [weakSelf.videoCapture startRunning];
                } else {
                    // 用户拒绝。
                }
            }];
            break;
        }
        case AVAuthorizationStatusAuthorized: {
            // 已经开启授权，可继续。
            [weakSelf.videoCapture startRunning];
            break;
        }
        default:
            break;
    }
}

- (KFVideoPacketExtraData *)getPacketExtraData:(CMSampleBufferRef)sampleBuffer {
    // 从 CMSampleBuffer 中获取 extra data。
    if (!sampleBuffer) {
        return nil;
    }
    
    // 获取编码类型。
    CMVideoCodecType codecType = CMVideoFormatDescriptionGetCodecType(CMSampleBufferGetFormatDescription(sampleBuffer));
    
    KFVideoPacketExtraData *extraData = nil;
    if (codecType == kCMVideoCodecType_H264) {
        // 获取 H.264 的 extra data：sps、pps。
        CMFormatDescriptionRef format = CMSampleBufferGetFormatDescription(sampleBuffer);
        size_t sparameterSetSize, sparameterSetCount;
        const uint8_t *sparameterSet;
        OSStatus statusCode = CMVideoFormatDescriptionGetH264ParameterSetAtIndex(format, 0, &sparameterSet, &sparameterSetSize, &sparameterSetCount, 0);
        if (statusCode == noErr) {
            size_t pparameterSetSize, pparameterSetCount;
            const uint8_t *pparameterSet;
            OSStatus statusCode = CMVideoFormatDescriptionGetH264ParameterSetAtIndex(format, 1, &pparameterSet, &pparameterSetSize, &pparameterSetCount, 0);
            if (statusCode == noErr) {
                extraData = [[KFVideoPacketExtraData alloc] init];
                extraData.sps = [NSData dataWithBytes:sparameterSet length:sparameterSetSize];
                extraData.pps = [NSData dataWithBytes:pparameterSet length:pparameterSetSize];
            }
        }
    } else if (codecType == kCMVideoCodecType_HEVC) {
        // 获取 H.265 的 extra data：vps、sps、pps。
        CMFormatDescriptionRef format = CMSampleBufferGetFormatDescription(sampleBuffer);
        size_t vparameterSetSize, vparameterSetCount;
        const uint8_t *vparameterSet;
        if (@available(iOS 11.0, *)) {
            OSStatus statusCode = CMVideoFormatDescriptionGetHEVCParameterSetAtIndex(format, 0, &vparameterSet, &vparameterSetSize, &vparameterSetCount, 0);
            if (statusCode == noErr) {
                size_t sparameterSetSize, sparameterSetCount;
                const uint8_t *sparameterSet;
                OSStatus statusCode = CMVideoFormatDescriptionGetHEVCParameterSetAtIndex(format, 1, &sparameterSet, &sparameterSetSize, &sparameterSetCount, 0);
                if (statusCode == noErr) {
                    size_t pparameterSetSize, pparameterSetCount;
                    const uint8_t *pparameterSet;
                    OSStatus statusCode = CMVideoFormatDescriptionGetHEVCParameterSetAtIndex(format, 2, &pparameterSet, &pparameterSetSize, &pparameterSetCount, 0);
                    if (statusCode == noErr) {
                        extraData = [[KFVideoPacketExtraData alloc] init];
                        extraData.vps = [NSData dataWithBytes:vparameterSet length:vparameterSetSize];
                        extraData.sps = [NSData dataWithBytes:sparameterSet length:sparameterSetSize];
                        extraData.pps = [NSData dataWithBytes:pparameterSet length:pparameterSetSize];
                    }
                }
            }
        } else {
            // 其他编码格式。
        }
    }
    
    return extraData;
}

- (BOOL)isKeyFrame:(CMSampleBufferRef)sampleBuffer {
    CFArrayRef array = CMSampleBufferGetSampleAttachmentsArray(sampleBuffer, true);
    if (!array) {
        return NO;
    }
    
    CFDictionaryRef dic = (CFDictionaryRef)CFArrayGetValueAtIndex(array, 0);
    if (!dic) {
        return NO;
    }
    
    // 检测 sampleBuffer 是否是关键帧。
    BOOL keyframe = !CFDictionaryContainsKey(dic, kCMSampleAttachmentKey_NotSync);
    
    return keyframe;
}

- (void)saveSampleBuffer:(CMSampleBufferRef)sampleBuffer {
    // 将编码数据存储为文件。
    // iOS 的 VideoToolbox 编码和解码只支持 AVCC/HVCC 的码流格式。但是 Android 的 MediaCodec 只支持 AnnexB 的码流格式。这里我们做一下两种格式的转换示范，将 AVCC/HVCC 格式的码流转换为 AnnexB 再存储。
    // 1、AVCC/HVCC 码流格式：[extradata]|[length][NALU]|[length][NALU]|...
    // VPS、SPS、PPS 不用 NALU 来存储，而是存储在 extradata 中；每个 NALU 前有个 length 字段表示这个 NALU 的长度（不包含 length 字段），length 字段通常是 4 字节。
    // 2、AnnexB 码流格式：[startcode][NALU]|[startcode][NALU]|...
    // 每个 NAL 前要添加起始码：0x00000001；VPS、SPS、PPS 也都用这样的 NALU 来存储，一般在码流最前面。
    if (sampleBuffer) {
        NSMutableData *resultData = [NSMutableData new];
        uint8_t nalPartition[] = {0x00, 0x00, 0x00, 0x01};
        
        // 关键帧前添加 vps（H.265)、sps、pps。这里要注意顺序别乱了。
        if ([self isKeyFrame:sampleBuffer]) {
            KFVideoPacketExtraData *extraData = [self getPacketExtraData:sampleBuffer];
            if (extraData.vps) {
                [resultData appendBytes:nalPartition length:4];
                [resultData appendData:extraData.vps];
            }
            [resultData appendBytes:nalPartition length:4];
            [resultData appendData:extraData.sps];
            [resultData appendBytes:nalPartition length:4];
            [resultData appendData:extraData.pps];
        }
        
        // 获取编码数据。这里的数据是 AVCC/HVCC 格式的。
        CMBlockBufferRef dataBuffer = CMSampleBufferGetDataBuffer(sampleBuffer);
        size_t length, totalLength;
        char *dataPointer;
        OSStatus statusCodeRet = CMBlockBufferGetDataPointer(dataBuffer, 0, &length, &totalLength, &dataPointer);
        if (statusCodeRet == noErr) {
            size_t bufferOffset = 0;
            static const int NALULengthHeaderLength = 4;
            // 拷贝编码数据。
            while (bufferOffset < totalLength - NALULengthHeaderLength) {
                // 通过 length 字段获取当前这个 NALU 的长度。
                uint32_t NALUnitLength = 0;
                memcpy(&NALUnitLength, dataPointer + bufferOffset, NALULengthHeaderLength);
                NALUnitLength = CFSwapInt32BigToHost(NALUnitLength);
                
                // 拷贝 AnnexB 起始码字节。
                [resultData appendData:[NSData dataWithBytes:nalPartition length:4]];
                // 拷贝这个 NALU 的字节。
                [resultData appendData:[NSData dataWithBytes:(dataPointer + bufferOffset + NALULengthHeaderLength) length:NALUnitLength]];
                
                // 步进。
                bufferOffset += NALULengthHeaderLength + NALUnitLength;
            }
        }
        
        [self.fileHandle writeData:resultData];
    }
}

@end
```

上面是 `KFVideoEncoderViewController` 的实现，主要分为以下几个部分：

- 1）在 `-videoCaptureConfig` 中初始化采集配置参数，在 `-videoEncoderConfig` 中初始化编码配置参数。

- - 这里需要注意的是，由于采集的数据后续用于编码，我们设置了采集的颜色空间格式为默认的 `kCVPixelFormatType_420YpCbCr8BiPlanarFullRange`。
  - 编码参数配置这里，默认是在设备支持 H.265 时，选择 H.265 编码。

- 2）在 `-videoCapture` 中初始化采集器，并实现了采集会话初始化成功的回调、采集数据回调、采集错误回调。

- 3）在采集会话初始化成功的回调 `sessionInitSuccessCallBack` 中，对采集预览渲染视图层进行布局。

- 4）在采集数据回调 `sampleBufferOutputCallBack` 中，从 CMSampleBufferRef 中取出 CVPixelBufferRef 送给编码器编码。

- 5）在编码数据回调 `sampleBufferOutputCallBack` 中，调用 `-saveSampleBuffer:` 将编码数据存储为 H.264/H.265 文件。

- - 这里示范了将 AVCC/HVCC 格式的码流转换为 AnnexB 再存储的过程。

## 4、用工具播放 H.264/H.265 文件

完成视频采集和编码后，可以将 App Document 文件夹下面的 `test.h264` 或 `test.h265` 文件拷贝到电脑上，使用 `ffplay` 播放来验证一下视频采集是效果是否符合预期：

```
$ ffplay -i test.h264
$ ffplay -i test.h265
```

关于播放 H.264/H.265 文件的工具，可以参考[《FFmpeg 工具》第 2 节 ffplay 命令行工具](https://mp.weixin.qq.com/s?__biz=MjM5MTkxOTQyMQ==&mid=2257484831&idx=1&sn=6bab905a5040c46b971bab05f787788b&scene=21#wechat_redirect)和[《可视化音视频分析工具》第 2.1 节 StreamEye](https://mp.weixin.qq.com/s?__biz=MjM5MTkxOTQyMQ==&mid=2257484834&idx=1&sn=5dd9768bfc0d01ca1b036be8dd2f5fa1&scene=21#wechat_redirect)。

## 5、参考资料

[1]CMSampleBufferRef: *https://developer.apple.com/documentation/coremedia/cmsamplebufferref/*

[2]CVPixelBufferRef: *https://developer.apple.com/documentation/corevideo/cvpixelbufferref/*

[3]CMBlockBuffer: *https://developer.apple.com/documentation/coremedia/cmblockbuffer-u9i*

[4]CVImageBuffer: *https://developer.apple.com/documentation/corevideo/cvimagebuffer-q40*

[5]CVPixelBuffer: *https://developer.apple.com/documentation/corevideo/cvpixelbuffer-q2e*



原文链接：https://mp.weixin.qq.com/s/M2l-9_W8heu_NjSYKQLCRA

