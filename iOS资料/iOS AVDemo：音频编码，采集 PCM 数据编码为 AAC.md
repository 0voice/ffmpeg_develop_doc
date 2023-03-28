# iOS AVDemo：音频编码，采集 PCM 数据编码为 AAC

iOS/Android 客户端开发同学如果想要开始学习音视频开发，最丝滑的方式是对[音视频基础概念知识](https://mp.weixin.qq.com/mp/appmsgalbum?__biz=MjM5MTkxOTQyMQ==&action=getalbum&album_id=2140155659944787969#wechat_redirect)有一定了解后，再借助 本地平台的音视频能力上手去实践音视频的`采集 → 编码 → 封装 → 解封装 → 解码 → 渲染`过程，并借助[音视频工具](https://mp.weixin.qq.com/mp/appmsgalbum?__biz=MjM5MTkxOTQyMQ==&action=getalbum&album_id=2216997905264082945#wechat_redirect)来分析和理解对应的音视频数据。

在[音视频工程示例](https://mp.weixin.qq.com/mp/appmsgalbum?__biz=MjM5MTkxOTQyMQ==&action=getalbum&album_id=2273301900659851268#wechat_redirect)这个栏目，我们将通过拆解`采集 → 编码 → 封装 → 解封装 → 解码 → 渲染`流程并实现 Demo 来向大家介绍如何在 iOS/Android 平台上手音视频开发。

这里是第二篇：**iOS 音频编码 Demo**。这个 Demo 里包含以下内容：

- 1）实现一个音频采集模块；
- 2）实现一个音频编码模块；
- 3）串联音频采集和编码模块，将采集到的音频数据输入给 AAC 编码模块进行编码和存储；
- 4）详尽的代码注释，帮你理解代码逻辑和原理。

想要了解 AAC 编码，可以看看[《音频编码：PCM 和 AAC 编码》](https://mp.weixin.qq.com/s?__biz=MjM5MTkxOTQyMQ==&mid=2257484452&idx=1&sn=3b5fdd120be300b62a5334c073a9fcbf&scene=21#wechat_redirect)。

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

接下来，我们来实现一个音频编码模块 `KFAudioEncoder`，在这里输入采集后的数据，输出编码后的数据。

```
KFAudioEncoder.h
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

上面是 `KFAudioEncoder` 接口的设计，除了`初始化方法`，主要是有`获取音频编码码率`以及音频编码`数据回调`和`错误回调`的接口，另外就是`编码`的接口。

其中`编码`接口对应着音频编码模块输入，`数据回调`接口则对应着输出。可以看到这里输入输出的参数都是 **CMSampleBufferRef**[1] 这个数据结构。它是对 `CMSampleBuffer` 的一个引用。

`CMSampleBuffer` 是 iOS 系统用来在音视频处理的 pipeline 中使用和传递媒体采样数据的核心数据结构。你可以认为它是 iOS 音视频处理 pipeline 中的流通货币，摄像头采集的视频数据接口、麦克风采集的音频数据接口、编码和解码数据接口、读取和存储视频接口、视频渲染接口等等，都以它作为参数。我们在 [《iOS 音频采集 Demo》](https://mp.weixin.qq.com/s?__biz=MjM5MTkxOTQyMQ==&mid=2257484867&idx=1&sn=d857104930a86de8ab0bdf2358ca6283&scene=21#wechat_redirect) 一文中介绍音频采集接口的时候详细介绍过 `CMSampleBuffer`，可以去看看回顾一下。

所以，在这里我们也以 `CMSampleBufferRef` 作为编码模块输入和输出的接口参数。

```
KFAudioEncoder.m
#import "KFAudioEncoder.h"
#import <AudioToolbox/AudioToolbox.h>

@interface KFAudioEncoder () {
    char *_leftBuffer; // 待编码缓冲区。
    NSInteger _leftLength; // 待编码缓冲区的长度，动态。
    char *_aacBuffer; // 编码缓冲区。
    NSInteger _bufferLength; // 每次送给编码器的数据长度。
}
@property (nonatomic, assign) AudioConverterRef audioEncoderInstance; // 音频编码器实例。
@property (nonatomic, assign) CMFormatDescriptionRef aacFormat; // 音频编码参数。
@property (nonatomic, assign, readwrite) NSInteger audioBitrate; // 音频编码码率。
@property (nonatomic, assign) BOOL isError;
@property (nonatomic, strong) dispatch_queue_t encoderQueue;
@end

@implementation KFAudioEncoder

#pragma mark - Lifecycle
- (instancetype)initWithAudioBitrate:(NSInteger)audioBitrate {
    self = [super init];
    if (self) {
        _audioBitrate = audioBitrate;
        _encoderQueue = dispatch_queue_create("com.KeyFrameKit.audioEncoder", DISPATCH_QUEUE_SERIAL);
    }

    return self;
}

- (void)dealloc {
    // 清理编码器。
    if (_audioEncoderInstance) {
        AudioConverterDispose(_audioEncoderInstance);
        _audioEncoderInstance = nil;
    }
    if (_aacFormat) {
        CFRelease(_aacFormat);
        _aacFormat = NULL;
    }
    
    // 清理缓冲区。
    if (_aacBuffer) {
        free(_aacBuffer);
        _aacBuffer = NULL;
    }
    if (_leftBuffer) {
        free(_leftBuffer);
        _leftBuffer = NULL;
    }
}

#pragma mark - Utility
- (void)setupAudioEncoderInstanceWithInputAudioFormat:(AudioStreamBasicDescription)inputFormat error:(NSError **)error {
    // 1、设置音频编码器输出参数。其中一些参数与输入的音频数据参数一致。
    AudioStreamBasicDescription outputFormat = {0};
    outputFormat.mSampleRate = inputFormat.mSampleRate; // 输出采样率与输入一致。
    outputFormat.mFormatID = kAudioFormatMPEG4AAC; // AAC 编码格式。常用的 AAC 编码格式：kAudioFormatMPEG4AAC、kAudioFormatMPEG4AAC_HE_V2。
    outputFormat.mChannelsPerFrame = (UInt32) inputFormat.mChannelsPerFrame; // 输出声道数与输入一致。
    outputFormat.mFramesPerPacket = 1024; // 每个包的帧数。AAC 固定是 1024，这个是由 AAC 编码规范规定的。对于未压缩数据设置为 1。
    outputFormat.mBytesPerPacket = 0; // 每个包的大小。动态大小设置为 0。
    outputFormat.mBytesPerFrame = 0; // 每帧的大小。压缩格式设置为 0。
    outputFormat.mBitsPerChannel = 0; // 压缩格式设置为 0。
    
    // 2、基于音频输入和输出参数创建音频编码器。
    OSStatus result = AudioConverterNew(&inputFormat, &outputFormat, &_audioEncoderInstance);
    if (result != noErr) {
        *error = [NSError errorWithDomain:NSStringFromClass(self.class) code:result userInfo:nil];
        return;
    }
    
    // 3、设置编码器参数：音频编码码率。
    UInt32 outputBitrate = (UInt32) self.audioBitrate;
    result = AudioConverterSetProperty(_audioEncoderInstance, kAudioConverterEncodeBitRate, sizeof(outputBitrate), &outputBitrate);
    if (result != noErr) {
        *error = [NSError errorWithDomain:NSStringFromClass(self.class) code:result userInfo:nil];
        return;
    }
    
    // 4、创建编码格式信息。
    result = CMAudioFormatDescriptionCreate(kCFAllocatorDefault, &outputFormat, 0, NULL, 0, NULL, nil, &_aacFormat);
    if (result != noErr) {
        *error = [NSError errorWithDomain:NSStringFromClass(self.class) code:result userInfo:nil];
        return;
    }
    
    // 5、设置每次送给编码器的数据长度。
    // 这里设置每次送给编码器的数据长度为：1024 * 2(16 bit 采样深度) * 声道数量，这个长度为什么要这么计算呢？
    // 因为我们每次调用 AudioConverterFillComplexBuffer 编码时，是送进去一个包（packet），而对于 AAC 来讲，mFramesPerPacket 需要是 1024，即 1 个 packet 有 1024 帧，而每个音频帧的大小是：2(16 bit 采样深度) * 声道数量。
    _bufferLength = 1024 * 2 * inputFormat.mChannelsPerFrame;
    
    // 6、初始化待编码缓冲区和编码缓冲区。
    if (!_leftBuffer) {
        // 待编码缓冲区长度达到 _bufferLength，就会送一波给编码器，所以大小 _bufferLength 够用了。
        _leftBuffer = malloc(_bufferLength);
    }
    if (!_aacBuffer) {
        // AAC 编码缓冲区只要装得下 _bufferLength 长度的 PCM 数据编码后的数据就好了，编码是压缩，所以大小 _bufferLength 也够用了。
        _aacBuffer = malloc(_bufferLength);
    }
}

- (void)encodeSampleBuffer:(CMSampleBufferRef)buffer {
    if (!buffer || !CMSampleBufferGetDataBuffer(buffer) || self.isError) {
        return;
    }
    
    // 异步处理，防止主线程卡顿。
    __weak typeof(self) weakSelf = self;
    CFRetain(buffer);
    dispatch_async(_encoderQueue, ^{
        [weakSelf encodeSampleBufferInternal:buffer];
        CFRelease(buffer);
    });
}

- (void)encodeSampleBufferInternal:(CMSampleBufferRef)buffer {
    // 1、从输入数据中获取音频格式信息。
    CMAudioFormatDescriptionRef audioFormatRef = CMSampleBufferGetFormatDescription(buffer);
    if (!audioFormatRef) {
        return;
    }
    // 获取音频参数信息，AudioStreamBasicDescription 包含了音频的数据格式、声道数、采样位深、采样率等参数。
    AudioStreamBasicDescription audioFormat = *CMAudioFormatDescriptionGetStreamBasicDescription(audioFormatRef);

    // 2、根据音频参数创建编码器实例。
    NSError *error = nil;
    // 第一次编码时创建编码器。
    if (!_audioEncoderInstance) {
        [self setupAudioEncoderInstanceWithInputAudioFormat:audioFormat error:&error];
        if (error) {
            [self callBackError:error];
            return;
        }
        if (!_audioEncoderInstance) {
            return;
        }
    }
    
    // 3、获取输入数据中的 PCM 数据。
    CMBlockBufferRef blockBuffer = CMSampleBufferGetDataBuffer(buffer);
    size_t audioLength;
    char *dataPointer = NULL;
    CMBlockBufferGetDataPointer(blockBuffer, 0, NULL, &audioLength, &dataPointer);
    if (audioLength == 0 || !dataPointer) {
        return;
    }
    
    // 4、处理音频时间戳信息。
    CMSampleTimingInfo timingInfo = {CMTimeMake(CMSampleBufferGetNumSamples(buffer), audioFormat.mSampleRate), CMSampleBufferGetPresentationTimeStamp(buffer), kCMTimeInvalid};
    
    // 5、基于编码缓冲区对 PCM 数据进行编码。
    if (_leftLength + audioLength >= _bufferLength) {
        // 当待编码缓冲区遗留数据加上新来的数据长度(_leftLength + audioLength)大于每次给编码器的数据长度(_bufferLength)时，则进行循环编码，每次送给编码器长度为 _bufferLength 的数据量。
        
        // 拷贝待编码的数据到缓冲区 totalBuffer。
        NSInteger totalSize = _leftLength + audioLength; // 当前总数据长度。
        NSInteger encodeCount = totalSize / _bufferLength; // 计算给编码器送数据的次数。
        char *totalBuffer = malloc(totalSize);
        char *p = totalBuffer;
        memset(totalBuffer, 0, (int) totalSize);
        memcpy(totalBuffer, _leftBuffer, _leftLength); // 拷贝上次遗留的数据。
        memcpy(totalBuffer + _leftLength, dataPointer, audioLength); // 拷贝这次新来的数据。
        
        // 分 encodeCount 次给编码器送数据。
        for (NSInteger index = 0; index < encodeCount; index++) {
            [self encodeBuffer:p timing:timingInfo]; // 调用编码方法。
            p += _bufferLength;
        }
        
        // 处理不够 _bufferLength 长度的剩余数据，先存在 _leftBuffer 中，等下次凑足一次编码需要的数据再编码。
        _leftLength = totalSize % _bufferLength;
        memset(_leftBuffer, 0, _bufferLength);
        memcpy(_leftBuffer, totalBuffer + (totalSize - _leftLength), _leftLength);
        
        // 清理。
        free(totalBuffer);
    } else {
        // 否则，就先存到待编码缓冲区，等下一次数据够了再送给编码器。
        memcpy(_leftBuffer + _leftLength, dataPointer, audioLength);
        _leftLength = _leftLength + audioLength;
    }
}

- (void)encodeBuffer:(char *)buffer timing:(CMSampleTimingInfo)timing {
    // 1、创建编码器接口对应的待编码缓冲区 AudioBufferList，填充待编码的数据。
    AudioBuffer inBuffer;
    AudioStreamBasicDescription audioFormat = *CMAudioFormatDescriptionGetStreamBasicDescription(_aacFormat);
    inBuffer.mNumberChannels = (UInt32) audioFormat.mChannelsPerFrame;
    inBuffer.mData = buffer; // 填充待编码数据。
    inBuffer.mDataByteSize = (UInt32) _bufferLength; // 设置待编码数据长度。
    AudioBufferList inBufferList;
    inBufferList.mNumberBuffers = 1;
    inBufferList.mBuffers[0] = inBuffer;
    
    //  2、创建编码输出缓冲区 AudioBufferList 接收编码后的数据。
    AudioBufferList outBufferList;
    outBufferList.mNumberBuffers = 1;
    outBufferList.mBuffers[0].mNumberChannels = inBuffer.mNumberChannels;
    outBufferList.mBuffers[0].mDataByteSize = inBuffer.mDataByteSize; // 设置编码缓冲区大小。
    outBufferList.mBuffers[0].mData = _aacBuffer; // 绑定缓冲区空间。
    
    // 3、编码。
    UInt32 outputDataPacketSize = 1; // 每次编码 1 个包。1 个包有 1024 个帧，这个对应创建编码器实例时设置的 mFramesPerPacket。
    // 需要在回调方法 inputDataProcess 中将待编码的数据拷贝到编码器的缓冲区的对应位置。这里把我们自己创建的待编码缓冲区 AudioBufferList 作为 inInputDataProcUserData 传入，在回调方法中直接拷贝它。
    OSStatus status = AudioConverterFillComplexBuffer(_audioEncoderInstance, inputDataProcess, &inBufferList, &outputDataPacketSize, &outBufferList, NULL);
    if (status != noErr) {
        [self callBackError:[NSError errorWithDomain:NSStringFromClass(self.class) code:status userInfo:nil]];
        return;
    }
    
    // 4、获取编码后的 AAC 数据并进行封装。
    size_t aacEncoderSize = outBufferList.mBuffers[0].mDataByteSize;
    char *blockBufferDataPoter = malloc(aacEncoderSize);
    memcpy(blockBufferDataPoter, _aacBuffer, aacEncoderSize);
    // 编码数据封装到 CMBlockBuffer 中。
    CMBlockBufferRef blockBuffer = NULL;
    status = CMBlockBufferCreateWithMemoryBlock(kCFAllocatorDefault,
                                                 blockBufferDataPoter,
                                                 aacEncoderSize,
                                                 NULL,
                                                 NULL,
                                                 0,
                                                 aacEncoderSize,
                                                 0,
                                                 &blockBuffer);
    if (status != kCMBlockBufferNoErr) {
        return;
    }
    // 编码数据 CMBlockBuffer 再封装到 CMSampleBuffer 中。
    CMSampleBufferRef sampleBuffer = NULL;
    const size_t sampleSizeArray[] = {aacEncoderSize};
    status = CMSampleBufferCreateReady(kCFAllocatorDefault,
                                       blockBuffer,
                                       _aacFormat,
                                       1,
                                       1,
                                       &timing,
                                       1,
                                       sampleSizeArray,
                                       &sampleBuffer);
    CFRelease(blockBuffer);
    
    // 5、回调编码数据。
    if (self.sampleBufferOutputCallBack) {
        self.sampleBufferOutputCallBack(sampleBuffer);
    }
    if (sampleBuffer) {
        CFRelease(sampleBuffer);
    }
}

- (void)callBackError:(NSError*)error {
    self.isError = YES;
    if(error && self.errorCallBack){
        dispatch_async(dispatch_get_main_queue(), ^{
            self.errorCallBack(error);
        });
    }
}

#pragma mark - Encoder CallBack
static OSStatus inputDataProcess(AudioConverterRef inConverter,
                                 UInt32 *ioNumberDataPackets,
                                 AudioBufferList *ioData,
                                 AudioStreamPacketDescription **outDataPacketDescription,
                                 void *inUserData) {
    // 将待编码的数据拷贝到编码器的缓冲区的对应位置进行编码。
    AudioBufferList bufferList = *(AudioBufferList *) inUserData;
    ioData->mBuffers[0].mNumberChannels = 1;
    ioData->mBuffers[0].mData = bufferList.mBuffers[0].mData;
    ioData->mBuffers[0].mDataByteSize = bufferList.mBuffers[0].mDataByteSize;
    
    return noErr;
}

@end
```

上面是 `KFAudioEncoder` 的实现，从代码上可以看到主要有这几个部分：

- 1）创建音频编码实例。第一次调用 `-encodeSampleBuffer:` → `-encodeSampleBufferInternal:` 才会创建音频编码实例。

- - 在 `-setupAudioEncoderInstanceWithInputAudioFormat:` 方法中实现。

- 2）实现音频编码逻辑，并在将数据封装到 `CMSampleBufferRef` 结构中，抛给 KFAudioEncoder 的对外数据回调接口。

- - 在 `-encodeSampleBuffer:` → `-encodeSampleBufferInternal:` → `-encodeBuffer:timing:` 中实现编码流程，其中涉及到待编码缓冲区、编码缓冲区的管理，并最终在 `inputDataProcess(...)` 回调中将待编码的数据拷贝到编码器的缓冲区进行编码。

- 3）捕捉音频编码过程中的错误，抛给 KFAudioEncoder 的对外错误回调接口。

- - 在 `-encodeSampleBufferInternal:`、`-encodeBuffer:timing:` 等方法中捕捉错误，在 `-callBackError:` 方法向外回调。

- 4）清理音频编码器实例、待编码缓冲区、编码缓冲区。

- - 在 `-dealloc` 方法中实现。

更具体细节见上述代码及其注释。

## 3、采集音频数据进行 AAC 编码和存储

我们在一个 ViewController 中来实现音频采集及编码逻辑，并将编码后的数据加上 **ADTS**[2] 头信息存储为 AAC 数据。

关于 ADTS，在[《音频编码：PCM 和 AAC 编码》](https://mp.weixin.qq.com/s?__biz=MjM5MTkxOTQyMQ==&mid=2257484452&idx=1&sn=3b5fdd120be300b62a5334c073a9fcbf&scene=21#wechat_redirect)中也有介绍，可以去看看了解一下。

```
#import "KFAudioEncoderViewController.h"
#import <AVFoundation/AVFoundation.h>
#import "KFAudioCapture.h"
#import "KFAudioEncoder.h"
#import "KFAudioTools.h"

@interface KFAudioEncoderViewController ()
@property (nonatomic, strong) KFAudioConfig *audioConfig;
@property (nonatomic, strong) KFAudioCapture *audioCapture;
@property (nonatomic, strong) KFAudioEncoder *audioEncoder;
@property (nonatomic, strong) NSFileHandle *fileHandle;
@end

@implementation KFAudioEncoderViewController
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
        // 音频编码数据回调。在这里将 AAC 数据写入文件。
        _audioEncoder.sampleBufferOutputCallBack = ^(CMSampleBufferRef sampleBuffer) {
            if (sampleBuffer) {
                // 1、获取音频编码参数信息。
                AudioStreamBasicDescription audioFormat = *CMAudioFormatDescriptionGetStreamBasicDescription(CMSampleBufferGetFormatDescription(sampleBuffer));
                
                // 2、获取音频编码数据。AAC 裸数据。
                CMBlockBufferRef blockBuffer = CMSampleBufferGetDataBuffer(sampleBuffer);
                size_t totolLength;
                char *dataPointer = NULL;
                CMBlockBufferGetDataPointer(blockBuffer, 0, NULL, &totolLength, &dataPointer);
                if (totolLength == 0 || !dataPointer) {
                    return;
                }
                
                // 3、在每个 AAC packet 前先写入 ADTS 头数据。
                // 由于 AAC 数据存储文件时需要在每个包（packet）前添加 ADTS 头来用于解码器解码音频流，所以这里添加一下 ADTS 头。
                [weakSelf.fileHandle writeData:[KFAudioTools adtsDataWithChannels:audioFormat.mChannelsPerFrame sampleRate:audioFormat.mSampleRate rawDataLength:totolLength]];
                
                // 4、写入 AAC packet 数据。
                [weakSelf.fileHandle writeData:[NSData dataWithBytes:dataPointer length:totolLength]];
            }
        };
    }
    
    return _audioEncoder;
}

- (NSFileHandle *)fileHandle {
    if (!_fileHandle) {
        NSString *audioPath = [[NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) lastObject] stringByAppendingPathComponent:@"test.aac"];
        NSLog(@"AAC file path: %@", audioPath);
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
    
    // 完成音频编码后，可以将 App Document 文件夹下面的 test.aac 文件拷贝到电脑上，使用 ffplay 播放：
    // ffplay -i test.aac
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
    self.title = @"Audio Encoder";
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

上面是 `KFAudioEncoderViewController` 的实现，其中主要包含这几个部分：

- 1）在采集音频前需要设置 **AVAudioSession**[3] 为正确的采集模式。

- - 在 `-setupAudioSession` 中实现。

- 2）通过启动和停止音频采集来驱动整个采集和编码流程。

- - 分别在 `-start` 和 `-stop` 中实现开始和停止动作。

- 3）在采集模块 `KFAudioCapture` 的数据回调中将数据交给编码模块 `KFAudioEncoder` 进行编码。

- - 在 `KFAudioCapture` 的 `sampleBufferOutputCallBack` 回调中实现。

- 4）在编码模块 `KFAudioEncoder` 的数据回调中获取编码后的 AAC 裸流数据，并在每个 AAC packet 前写入 ADTS 头数据，存储到文件中。

- - 在 `KFAudioEncoder` 的 `sampleBufferOutputCallBack` 回调中实现。
  - 其中生成一个 AAC packet 对应的 ADTS 头数据在 `KFAudioTools` 类的工具方法 `+adtsDataWithChannels:sampleRate:rawDataLength:` 中实现。

```
KFAudioTools.m
#import "KFAudioTools.h"

@implementation KFAudioTools

// 按音频参数生产 AAC packet 对应的 ADTS 头数据。
// 当编码器编码的是 AAC 裸流数据时，需要在每个 AAC packet 前添加一个 ADTS 头用于解码器解码音频流。
// 参考文档：
// ADTS 格式参考：http://wiki.multimedia.cx/index.php?title=ADTS
// MPEG-4 Audio 格式参考：http://wiki.multimedia.cx/index.php?title=MPEG-4_Audio#Channel_Configurations
+ (NSData *)adtsDataWithChannels:(NSInteger)channels sampleRate:(NSInteger)sampleRate rawDataLength:(NSInteger)rawDataLength {
    // 1、创建数据缓冲区。
    int adtsLength = 7; // ADTS 头固定 7 字节。
    char *packet = malloc(sizeof(char) * adtsLength);
    
    // 2、设置各数据字段。
    int profile = 2; // 2 表示 AAC LC。
    NSInteger sampleRateIndex = [self.class sampleRateIndex:sampleRate]; // 取得采样率对应的 index。
    int channelCfg = (int) channels; // MPEG-4 Audio Channel Configuration。
    NSUInteger fullLength = adtsLength + rawDataLength; // 这里的长度字段是：ADTS 头数据和 AAC packet 数据的总长度。
    
    //  3、填充 ADTS 数据。
    packet[0] = (char) 0xFF; // 11111111     = syncword
    packet[1] = (char) 0xF9; // 1111 1 00 1  = syncword MPEG-2 Layer CRC
    packet[2] = (char) (((profile - 1) << 6) + (sampleRateIndex << 2) + (channelCfg >> 2));
    packet[3] = (char) (((channelCfg & 3) << 6) + (fullLength >> 11));
    packet[4] = (char) ((fullLength & 0x7FF) >> 3);
    packet[5] = (char) (((fullLength & 7) << 5) + 0x1F);
    packet[6] = (char) 0xFC;
    NSData *data = [NSData dataWithBytesNoCopy:packet length:adtsLength freeWhenDone:YES];
    
    return data;
}

// 音频采样率对应的 index。
+ (NSInteger)sampleRateIndex:(NSInteger)frequencyInHz {
    NSInteger sampleRateIndex = 0;
    switch (frequencyInHz) {
        case 96000:
            sampleRateIndex = 0;
            break;
        case 88200:
            sampleRateIndex = 1;
            break;
        case 64000:
            sampleRateIndex = 2;
            break;
        case 48000:
            sampleRateIndex = 3;
            break;
        case 44100:
            sampleRateIndex = 4;
            break;
        case 32000:
            sampleRateIndex = 5;
            break;
        case 24000:
            sampleRateIndex = 6;
            break;
        case 22050:
            sampleRateIndex = 7;
            break;
        case 16000:
            sampleRateIndex = 8;
            break;
        case 12000:
            sampleRateIndex = 9;
            break;
        case 11025:
            sampleRateIndex = 10;
            break;
        case 8000:
            sampleRateIndex = 11;
            break;
        case 7350:
            sampleRateIndex = 12;
            break;
        default:
            sampleRateIndex = 15;
    }
    
    return sampleRateIndex;
}

@end
```

## 4、用工具播放 AAC 文件

完成音频采集和编码后，可以将 App Document 文件夹下面的 `test.aac` 文件拷贝到电脑上，使用 `ffplay` 播放来验证一下音频采集是效果是否符合预期：

```
$ ffplay -i test.aac
```

这里在播放 AAC 文件时不必像播放 PCM 文件那样设置音频参数，这正是因为我们已经将对应的参数信息编码到 ADTS 头部数据中去了，播放解码时可以从中解析出这些信息从而正确的解码 AAC。

关于播放 AAC 文件的工具，可以参考[《FFmpeg 工具》第 2 节 ffplay 命令行工具](https://mp.weixin.qq.com/s?__biz=MjM5MTkxOTQyMQ==&mid=2257484831&idx=1&sn=6bab905a5040c46b971bab05f787788b&scene=21#wechat_redirect)和[《可视化音视频分析工具》第 1.1 节 Adobe Audition](https://mp.weixin.qq.com/s?__biz=MjM5MTkxOTQyMQ==&mid=2257484834&idx=1&sn=5dd9768bfc0d01ca1b036be8dd2f5fa1&scene=21#wechat_redirect)。

## 5、参考资料

[1]CMSampleBufferRef: *https://developer.apple.com/documentation/coremedia/cmsamplebufferref/*

[2]ADTS 格式: *http://wiki.multimedia.cx/index.php?title=ADTS*

[3]AVAudioSession: *https://developer.apple.com/documentation/avfaudio/avaudiosession/*





原文链接：https://mp.weixin.qq.com/s/q4n1dYTjcJVJolX-Wrdr9Q