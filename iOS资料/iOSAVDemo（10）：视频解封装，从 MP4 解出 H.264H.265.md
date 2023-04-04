# iOSAVDemo（10）：视频解封装，从 MP4 解出 H.264/H.265


iOS/Android 客户端开发同学如果想要开始学习音视频开发，最丝滑的方式是对[音视频基础概念知识](https://mp.weixin.qq.com/mp/appmsgalbum?__biz=MjM5MTkxOTQyMQ==&action=getalbum&album_id=2140155659944787969#wechat_redirect)有一定了解后，再借助 iOS/Android 平台的音视频能力上手去实践音视频的`采集 → 编码 → 封装 → 解封装 → 解码 → 渲染`过程，并借助[音视频工具](https://mp.weixin.qq.com/mp/appmsgalbum?__biz=MjM5MTkxOTQyMQ==&action=getalbum&album_id=2216997905264082945#wechat_redirect)来分析和理解对应的音视频数据。

在[音视频工程示例](https://mp.weixin.qq.com/mp/appmsgalbum?__biz=MjM5MTkxOTQyMQ==&action=getalbum&album_id=2273301900659851268#wechat_redirect)这个栏目，我们将通过拆解`采集 → 编码 → 封装 → 解封装 → 解码 → 渲染`流程并实现 Demo 来向大家介绍如何在 iOS/Android 平台上手音视频开发。

这里是第十篇：**iOS 视频解封装 Demo**。这个 Demo 里包含以下内容：

- 1）实现一个视频解封装模块；
- 2）实现对 MP4 文件中视频部分的解封装逻辑并将解封装后的编码数据存储为 H.264/H.265 文件；
- 3）详尽的代码注释，帮你理解代码逻辑和原理。

在本文中，我们将详解一下 Demo 的具体实现和源码。读完本文内容相信就能帮你掌握相关知识。

## 1、视频解封装模块

视频解封装模块即 `KFMP4Demuxer`，复用了[《iOS 音频解封装 Demo》](https://mp.weixin.qq.com/s?__biz=MjM5MTkxOTQyMQ==&mid=2257484932&idx=1&sn=04fa6fb220574c0a5d417f4b527c0142&scene=21#wechat_redirect)中介绍的 demuxer，这里就不再重复介绍了，其接口如下：

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

## 2、解封装 MP4 文件中的视频部分存储为 H.264/H.265 文件

我们还是在一个 ViewController 中来实现对一个 MP4 文件解封装、获取其中的视频编码数据并存储为 H.264/H.265 文件。

```
KFVideoDemuxerViewController.m
#import "KFVideoDemuxerViewController.h"
#import "KFMP4Demuxer.h"

@interface KFVideoPacketExtraData : NSObject
@property (nonatomic, strong) NSData *sps;
@property (nonatomic, strong) NSData *pps;
@property (nonatomic, strong) NSData *vps;
@end

@implementation KFVideoPacketExtraData
@end

@interface KFVideoDemuxerViewController ()
@property (nonatomic, strong) KFDemuxerConfig *demuxerConfig;
@property (nonatomic, strong) KFMP4Demuxer *demuxer;
@property (nonatomic, strong) NSFileHandle *fileHandle;
@end

@implementation KFVideoDemuxerViewController
#pragma mark - Property
- (KFDemuxerConfig *)demuxerConfig {
    if (!_demuxerConfig) {
        _demuxerConfig = [[KFDemuxerConfig alloc] init];
        // 只解封装视频。
        _demuxerConfig.demuxerType = KFMediaVideo;
        // 待解封装的资源。
        NSString *videoPath = [[NSBundle mainBundle] pathForResource:@"input" ofType:@"mp4"];
        _demuxerConfig.asset = [AVAsset assetWithURL:[NSURL fileURLWithPath:videoPath]];
    }
    
    return _demuxerConfig;
}

- (KFMP4Demuxer*)demuxer {
    if (!_demuxer) {
        _demuxer = [[KFMP4Demuxer alloc] initWithConfig:self.demuxerConfig];
        _demuxer.errorCallBack = ^(NSError* error) {
            NSLog(@"KFMP4Demuxer error:%zi %@", error.code, error.localizedDescription);
        };
    }
    
    return _demuxer;
}

- (NSFileHandle *)fileHandle {
    if (!_fileHandle) {
        NSString *videoPath = [[NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) lastObject] stringByAppendingPathComponent:@"output.h264"];
        [[NSFileManager defaultManager] removeItemAtPath:videoPath error:nil];
        [[NSFileManager defaultManager] createFileAtPath:videoPath contents:nil attributes:nil];
        _fileHandle = [NSFileHandle fileHandleForWritingAtPath:videoPath];
    }

    return _fileHandle;
}

#pragma mark - Lifecycle
- (void)viewDidLoad {
    [super viewDidLoad];

    self.view.backgroundColor = [UIColor whiteColor];
    self.title = @"Video Demuxer";
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
            [weakSelf fetchAndSaveDemuxedData];
        } else {
            NSLog(@"KFMP4Demuxer error: %zi %@", error.code, error.localizedDescription);
        }
    }];
}

#pragma mark - Utility
- (void)fetchAndSaveDemuxedData {
    // 异步地从 Demuxer 获取解封装后的 H.264/H.265 编码数据。
    dispatch_async(dispatch_get_global_queue(0, 0), ^{
        while (self.demuxer.hasVideoSampleBuffer) {
            CMSampleBufferRef videoBuffer = [self.demuxer copyNextVideoSampleBuffer];
            if (videoBuffer) {
                [self saveSampleBuffer:videoBuffer];
                CFRelease(videoBuffer);
            }
        }
        if (self.demuxer.demuxerStatus == KFMP4DemuxerStatusCompleted) {
            NSLog(@"KFMP4Demuxer complete");
        }
    });
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

上面是 `KFVideoDemuxerViewController` 的实现，其中主要包含这几个部分：

- 1）设置好待解封装的资源。

- - 在 `-demuxerConfig` 中实现，我们这里是一个 MP4 文件。

- 2）启动解封装器。

- - 在 `-start` 中实现。

- 3）读取解封装后的音频编码数据并存储为 H.264/H.265 文件。

- - 在 `-fetchAndSaveDemuxedData` → `-saveSampleBuffer:` 中实现。
  - 需要注意的是，我们从解封装器读取的视频 H.264/H.265 编码数据是 AVCC/HVCC 码流格式，我们在这里示范了将 AVCC/HVCC 格式的码流转换为 AnnexB 再存储的过程。这个在前面的[《iOS 视频编码 Demo》](https://mp.weixin.qq.com/s?__biz=MjM5MTkxOTQyMQ==&mid=2257485273&idx=1&sn=0d876a49c4e46f369f6a578856221f5d&scene=21#wechat_redirect)中已经介绍过了。

## 3、用工具播放 H.264/H.265 文件

完成视频解封装后，可以将 App Document 文件夹下面的 `output.h264` 或 `output.h265` 文件拷贝到电脑上，使用 `ffplay` 播放来验证一下视频解封装的效果是否符合预期：

```
$ ffplay -i output.h264
$ ffplay -i output.h265
```

关于播放 H.264/H.265 文件的工具，可以参考[《FFmpeg 工具》第 2 节 ffplay 命令行工具](https://mp.weixin.qq.com/s?__biz=MjM5MTkxOTQyMQ==&mid=2257484831&idx=1&sn=6bab905a5040c46b971bab05f787788b&scene=21#wechat_redirect)和[《可视化音视频分析工具》第 2.1 节 StreamEye](https://mp.weixin.qq.com/s?__biz=MjM5MTkxOTQyMQ==&mid=2257484834&idx=1&sn=5dd9768bfc0d01ca1b036be8dd2f5fa1&scene=21#wechat_redirect)。



原文链接：https://mp.weixin.qq.com/s/4Ua9PZllWRLYF79hwsH0DQ

