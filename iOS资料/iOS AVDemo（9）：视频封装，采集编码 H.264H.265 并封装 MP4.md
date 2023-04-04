# iOS AVDemo（9）：视频封装，采集编码 H.264/H.265 并封装 MP4

iOS/Android 客户端开发同学如果想要开始学习音视频开发，最丝滑的方式是对[音视频基础概念知识](https://mp.weixin.qq.com/mp/appmsgalbum?__biz=MjM5MTkxOTQyMQ==&action=getalbum&album_id=2140155659944787969#wechat_redirect)有一定了解后，再借助 iOS/Android 平台的音视频能力上手去实践音视频的`采集 → 编码 → 封装 → 解封装 → 解码 → 渲染`过程，并借助[音视频工具](https://mp.weixin.qq.com/mp/appmsgalbum?__biz=MjM5MTkxOTQyMQ==&action=getalbum&album_id=2216997905264082945#wechat_redirect)来分析和理解对应的音视频数据。

在[音视频工程示例](https://mp.weixin.qq.com/mp/appmsgalbum?__biz=MjM5MTkxOTQyMQ==&action=getalbum&album_id=2273301900659851268#wechat_redirect)这个栏目，我们将通过拆解`采集 → 编码 → 封装 → 解封装 → 解码 → 渲染`流程并实现 Demo 来向大家介绍如何在 iOS/Android 平台上手音视频开发。

这里是第九篇：**iOS 视频封装 Demo**。这个 Demo 里包含以下内容：

- 1）实现一个视频采集模块；
- 2）实现一个视频编码模块，支持 H.264/H.265；
- 3）实现一个视频封装模块；
- 4）串联视频采集、编码、封装模块，将采集到的视频数据输入给编码模块进行编码，再将编码后的数据输入给 MP4 封装模块封装和存储；
- 5）详尽的代码注释，帮你理解代码逻辑和原理。

在本文中，我们将详解一下 Demo 的具体实现和源码。读完本文内容相信就能帮你掌握相关知识。

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

同样的，视频编码模块 `KFVideoEncoder` 的实现与[《iOS 视频编码 Demo》](https://mp.weixin.qq.com/s?__biz=MjM5MTkxOTQyMQ==&mid=2257485273&idx=1&sn=0d876a49c4e46f369f6a578856221f5d&scene=21#wechat_redirect)中一样，这里就不再重复介绍了，其接口如下：

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

## 3、视频封装模块

视频编码模块即 `KFMP4Muxer`，复用了[《iOS 音频封装 Demo》](https://mp.weixin.qq.com/s?__biz=MjM5MTkxOTQyMQ==&mid=2257484907&idx=1&sn=e2418939db2e199c42130e20cb9e1a34&scene=21#wechat_redirect)中介绍的 muxer，这里就不再重复介绍了，其接口如下：

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

## 4、采集视频数据进行 H.264/H.265 编码以及 MP4 封装和存储

我们还是在一个 ViewController 中来实现采集视频数据进行 H.264/H.265 编码以及 MP4 封装和存储的逻辑。

```
KFVideoMuxerViewController.m
#import "KFVideoMuxerViewController.h"
#import "KFVideoCapture.h"
#import "KFVideoEncoder.h"
#import "KFMP4Muxer.h"

@interface KFVideoMuxerViewController ()
@property (nonatomic, strong) KFVideoCaptureConfig *videoCaptureConfig;
@property (nonatomic, strong) KFVideoCapture *videoCapture;
@property (nonatomic, strong) KFVideoEncoderConfig *videoEncoderConfig;
@property (nonatomic, strong) KFVideoEncoder *videoEncoder;
@property (nonatomic, strong) KFMuxerConfig *muxerConfig;
@property (nonatomic, strong) KFMP4Muxer *muxer;
@property (nonatomic, assign) BOOL isWriting;
@end

@implementation KFVideoMuxerViewController
#pragma mark - Property
- (KFVideoCaptureConfig *)videoCaptureConfig {
    if (!_videoCaptureConfig) {
        _videoCaptureConfig = [[KFVideoCaptureConfig alloc] init];
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
            if (sampleBuffer && weakSelf.isWriting) {
                // 编码。
                [weakSelf.videoEncoder encodePixelBuffer:CMSampleBufferGetImageBuffer(sampleBuffer) ptsTime:CMSampleBufferGetPresentationTimeStamp(sampleBuffer)];
            }
        };
        _videoCapture.sessionErrorCallBack = ^(NSError *error) {
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
            // 视频编码数据回调。
            if (weakSelf.isWriting) {
                // 当标记封装写入中时，将编码的 H.264/H.265 数据送给封装器。
                [weakSelf.muxer appendSampleBuffer:sampleBuffer];
            }
        };
    }
    
    return _videoEncoder;
}

- (KFMuxerConfig *)muxerConfig {
    if (!_muxerConfig) {
        _muxerConfig = [[KFMuxerConfig alloc] init];
        NSString *videoPath = [[NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) lastObject] stringByAppendingPathComponent:@"test.mp4"];
        NSLog(@"MP4 file path: %@", videoPath);
        [[NSFileManager defaultManager] removeItemAtPath:videoPath error:nil];
        _muxerConfig.outputURL = [NSURL fileURLWithPath:videoPath];
        _muxerConfig.muxerType = KFMediaVideo;
    }
    
    return _muxerConfig;
}

- (KFMP4Muxer *)muxer {
    if (!_muxer) {
        _muxer = [[KFMP4Muxer alloc] initWithConfig:self.muxerConfig];
    }
    
    return _muxer;
}

#pragma mark - Lifecycle
- (void)viewDidLoad {
    [super viewDidLoad];

    // 启动后即开始请求视频采集权限并开始采集。
    [self requestAccessForVideo];
    [self setupUI];
}

- (void)viewWillLayoutSubviews {
    [super viewWillLayoutSubviews];
    self.videoCapture.previewLayer.frame = self.view.bounds;
}

- (void)dealloc {
    
}

#pragma mark - Action
- (void)start {
    if (!self.isWriting) {
        // 启动封装，
        [self.muxer startWriting];
        // 标记开始封装写入。
        self.isWriting = YES;
    }
}

- (void)stop {
    if (self.isWriting) {
        __weak typeof(self) weakSelf = self;
        [self.videoEncoder flushWithCompleteHandler:^{
            weakSelf.isWriting = NO;
            [weakSelf.muxer stopWriting:^(BOOL success, NSError * _Nonnull error) {
                NSLog(@"muxer stop %@", success ? @"success" : @"failed");
            }];
        }];
    }
}

- (void)changeCamera {
    [self.videoCapture changeDevicePosition:self.videoCapture.config.position == AVCaptureDevicePositionBack ? AVCaptureDevicePositionFront : AVCaptureDevicePositionBack];
}

- (void)singleTap:(UIGestureRecognizer *)sender {
    
}

-(void)handleDoubleTap:(UIGestureRecognizer *)sender {
    [self.videoCapture changeDevicePosition:self.videoCapture.config.position == AVCaptureDevicePositionBack ? AVCaptureDevicePositionFront : AVCaptureDevicePositionBack];
}

#pragma mark - Private Method
- (void)requestAccessForVideo {
    __weak typeof(self) weakSelf = self;
    AVAuthorizationStatus status = [AVCaptureDevice authorizationStatusForMediaType:AVMediaTypeVideo];
    switch (status) {
        case AVAuthorizationStatusNotDetermined:{
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
        case AVAuthorizationStatusAuthorized:{
            // 已经开启授权，可继续。
            [weakSelf.videoCapture startRunning];
            break;
        }
        default:
            break;
    }
}

- (void)setupUI {
    self.edgesForExtendedLayout = UIRectEdgeAll;
    self.extendedLayoutIncludesOpaqueBars = YES;
    self.title = @"Video Muxer";
    self.view.backgroundColor = [UIColor whiteColor];
    
    // 添加手势。
    UITapGestureRecognizer *singleTapGesture = [[UITapGestureRecognizer alloc]initWithTarget:self action:@selector(singleTap:)];
    singleTapGesture.numberOfTapsRequired = 1;
    singleTapGesture.numberOfTouchesRequired = 1;
    [self.view addGestureRecognizer:singleTapGesture];
    
    UITapGestureRecognizer *doubleTapGesture = [[UITapGestureRecognizer alloc]initWithTarget:self action:@selector(handleDoubleTap:)];
    doubleTapGesture.numberOfTapsRequired = 2;
    doubleTapGesture.numberOfTouchesRequired = 1;
    [self.view addGestureRecognizer:doubleTapGesture];
    
    [singleTapGesture requireGestureRecognizerToFail:doubleTapGesture];

    
    // Navigation item.
    UIBarButtonItem *startBarButton = [[UIBarButtonItem alloc] initWithTitle:@"Start" style:UIBarButtonItemStylePlain target:self action:@selector(start)];
    UIBarButtonItem *stopBarButton = [[UIBarButtonItem alloc] initWithTitle:@"Stop" style:UIBarButtonItemStylePlain target:self action:@selector(stop)];
    UIBarButtonItem *cameraBarButton = [[UIBarButtonItem alloc] initWithTitle:@"Camera" style:UIBarButtonItemStylePlain target:self action:@selector(changeCamera)];
    self.navigationItem.rightBarButtonItems = @[stopBarButton, startBarButton, cameraBarButton];
}

@end
```

上面是 `KFVideoMuxerViewController` 的实现，其中主要包含这几个部分：

- 1）启动后即开始请求视频采集权限并开始采集。

- - 在 `-requestAccessForVideo` 方法中实现。

- 2）在采集会话初始化成功的回调中，对采集预览渲染视图层进行布局。

- - 在 `KFVideoCapture` 的 `sessionInitSuccessCallBack` 回调中实现。

- 2）在采集模块的数据回调中将数据交给编码模块进行编码。

- - 在 `KFVideoCapture` 的 `sampleBufferOutputCallBack` 回调中实现。

- 3）在编码模块的数据回调中获取编码后的 H.264/H.265 数据，并将数据交给封装器 `KFMP4Muxer` 进行封装。

- - 在 `KFVideoEncoder` 的 `sampleBufferOutputCallBack` 回调中实现。

- 4）在调用 `-stop` 停止整个流程后，如果没有出现错误，封装的 MP4 文件会被存储到 `muxerConfig` 设置的路径。

## 5、用工具播放 MP4 文件

完成 Demo 后，可以将 App Document 文件夹下面的 `test.mp4` 文件拷贝到电脑上，使用 `ffplay` 播放来验证一下效果是否符合预期：

```
$ ffplay -i test.mp4
```

关于播放 MP4 文件的工具，可以参考[《FFmpeg 工具》第 2 节 ffplay 命令行工具](https://mp.weixin.qq.com/s?__biz=MjM5MTkxOTQyMQ==&mid=2257484831&idx=1&sn=6bab905a5040c46b971bab05f787788b&scene=21#wechat_redirect)和[《可视化音视频分析工具》第 3.5 节 VLC 播放器](https://mp.weixin.qq.com/s?__biz=MjM5MTkxOTQyMQ==&mid=2257484834&idx=1&sn=5dd9768bfc0d01ca1b036be8dd2f5fa1&scene=21#wechat_redirect)。

我们还可以用[《可视化音视频分析工具》第 3.1 节 MP4Box.js](https://mp.weixin.qq.com/s?__biz=MjM5MTkxOTQyMQ==&mid=2257484834&idx=1&sn=5dd9768bfc0d01ca1b036be8dd2f5fa1&scene=21#wechat_redirect) 等工具来查看它的格式：

![图片](https://mmbiz.qpic.cn/mmbiz_png/gUnqKPeSueiavb3sxPjmibY0C1fFYXUXcI1CP7x0G6f5eNZ4G8pcEojOya39XgK1icVz7MCTQrlG0micRUkF84wfpA/640?wx_fmt=png&wxfrom=5&wx_lazy=1&wx_co=1)Demo 生成的 MP4 文件结构





原文链接：https://mp.weixin.qq.com/s/W17eLiUeCszNM8Kg-rlmBg

