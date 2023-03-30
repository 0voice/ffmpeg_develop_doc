# iOS音视频开发——视频采集

## 1.认识 AVCapture 系列

AVCapture 系列是 AVFoundation 框架为我们提供的用于管理输入设备、采集、输出、预览等一系列接口，其工作原理如下：

![img](https://pic1.zhimg.com/80/v2-5e1574831251fe8f220730e11c69a6ec_720w.webp)

\1. AVCaptureDevice: 信号采集硬件设备(摄像头、麦克风、屏幕等)
`AVCaptureDevice` 代表硬件设备，并且为 `AVCaptureSession` 提供 input，要想使用 `AVCaptureDevice`，应该先将设备支持的 `device` 枚举出来, 根据摄像头的位置( 前置或者后置摄像头 )获取需要用的那个摄像头, 再使用；
如果想要对 `AVCaptureDevice` 对象的一些属性进行设置，应该先调用 `lockForConfiguration:` 方法, 设置结束后，调用 `unlockForConfiguration` 方法；

```text
    [self.device lockForConfiguration:&error];
    // 设置 ***
    [self.device unlockForConfiguration];
```

## 2. AVCaptureInput: 输入数据管理

AVCaptureInput 继承自 `NSObject`，是向 `AVCaptureSession` 提供输入数据的对象的抽象超类;
要将 `AVCaptureInput` 对象与会话 `AVCaptureSession` 关联，需要 `AVCaptureSession`实例调用 `-addInput:` 方法。
由于 `AVCaptureInput` 是个抽象类，无法直接使用，所以我们一般使用它的子类类管理输入数据。我们常用的 `AVCaptureInput` 的子类有三个：

![img](https://pic1.zhimg.com/80/v2-66c51c4c4ea37d25769283b95b808c78_720w.webp)

`AVCaptureDeviceInput`：用于从 `AVCaptureDevice` 对象捕获数据;
`AVCaptureScreenInput`：从 macOS 屏幕上录制的一种捕获输入;
`AVCaptureMetadataInput`：它为 `AVCaptureSession` 提供 `AVMetadataItems`。

## 3. AVCaptureOutput：输出数据管理

AVCaptureOutput 继承自 `NSObject`，是输出数据管理，该对象将会被添加到会话`AVCaptureSession`中，用于接收会话`AVCaptureSession`各类输出数据; `AVCaptureOutput`提供了一个抽象接口，用于将捕获输出数据(如文件和视频预览)连接到捕获会话`AVCaptureSession`的实例，捕获输出可以有多个由`AVCaptureConnection`对象表示的连接，一个连接对应于它从捕获输入(`AVCaptureInput`的实例)接收的每个媒体流，捕获输出在首次创建时没有任何连接，当向捕获会话添加输出时，将创建连接，将该会话的输入的媒体数据映射到其输出，调用`AVCaptureSession`的`-addOutput:`方法将`AVCaptureOutput`与`AVCaptureSession`关联。
`AVCaptureOutput` 是个抽象类，我们必须使用它的子类，常用的 `AVCaptureOutput`的子类如下所示：

![img](https://pic4.zhimg.com/80/v2-0e629068674d0290807a5d48343edf9f_720w.webp)

`AVCaptureAudioDataOutput`：一种捕获输出，用于记录音频，并在录制音频时提供对音频样本缓冲区的访问；
`AVCaptureAudioPreviewOutput` ：一种捕获输出，与一个核心音频输出设备相关联、可用于播放由捕获会话捕获的音频；
`AVCaptureDepthDataOutput` ：在兼容的摄像机设备上记录场景深度信息的捕获输出；
`AVCaptureMetadataOutput` ：用于处理捕获会话 `AVCaptureSession` 产生的定时元数据的捕获输出；
`AVCaptureStillImageOutput`：在macOS中捕捉静止照片的捕获输出。该类在 iOS 10.0 中被弃用，并且不支持新的相机捕获功能，例如原始图像输出和实时照片，在 iOS 10.0 或更高版本中，使用 `AVCapturePhotoOutput` 类代替；
`AVCapturePhotoOutput` ：静态照片、动态照片和其他摄影工作流的捕获输出；
`AVCaptureVideoDataOutput` ：记录视频并提供对视频帧进行处理的捕获输出；
`AVCaptureFileOutput`：用于捕获输出的抽象超类，可将捕获数据记录到文件中；
`AVCaptureMovieFileOutput` ：继承自 `AVCaptureFileOutput`，将视频和音频记录到 QuickTime 电影文件的捕获输出；
`AVCaptureAudioFileOutput` ：继承自 `AVCaptureFileOutput`，记录音频并将录制的音频保存到文件的捕获输出。

## 4. AVCaptureSession：

用来管理采集数据和输出数据，它负责协调从哪里采集数据，输出到哪里，它是整个Capture的核心，类似于RunLoop，它不断的从输入源获取数据，然后分发给各个输出源
AVCaptureSession 继承自`NSObject`，是`AVFoundation`的核心类，用于管理捕获对象`AVCaptureInput`的视频和音频的输入，协调捕获的输出`AVCaptureOutput`

![img](https://pic3.zhimg.com/80/v2-bcd3fa28a8d2d755fac1fd228e77fe82_720w.webp)

## 5. AVCaptureConnection：

用于 `AVCaptureSession` 来建立和维护 `AVCaptureInput` 和 `AVCaptureOutput` 之间的连接
AVCaptureConnection 是 `Session` 和 `Output` 中间的控制节点，每个 `Output` 与 `Session` 建立连接后，都会分配一个默认的 `AVCpatureConnection`。



## 6. AVCapturePreviewLayer：

预览层，`AVCaptureSession` 的一个属性，继承自 `CALayer`，提供摄像头的预览功能，照片以及视频就是通过把 `AVCapturePreviewLayer` 添加到 `UIView` 的 `layer` 上来显示

开始视频采集
1、创建并初始化输入`AVCaptureInput`: `AVCaptureDeviceInput` 和输出`AVCaptureOutput`: `AVCaptureVideoDataOutput`;
2、创建并初始化 `AVCaptureSession`，把 `AVCaptureInput` 和 `AVCaptureOutput` 添加到 `AVCaptureSession` 中;
3、调用 `AVCaptureSession` 的 `startRunning` 开启采集
初始化输入
通过 `AVCaptureDevice` 的 `devicesWithMediaType:` 方法获取摄像头，iPhone 都是有前后摄像头的，这里获取到的是一个设备的数组，要从数组里面拿到我们想要的前摄像头或后摄像头，然后将 `AVCaptureDevice` 转化为 `AVCaptureDeviceInput`，添加到 `AVCaptureSession`中

```text
        /**************************  设置输入设备  *************************/
        // ---  获取所有摄像头  ---
        NSArray *cameras = [AVCaptureDevice devicesWithMediaType:AVMediaTypeVideo];
        // ---  获取当前方向摄像头  ---
        NSArray *captureDeviceArray = [cameras filteredArrayUsingPredicate:[NSPredicate predicateWithFormat:@"position == %d", _capturerParam.devicePosition]];
        
        if (captureDeviceArray.count == 0) {
            return nil;
        }
        
        // ---  转化为输入设备  ---
        AVCaptureDevice *camera = captureDeviceArray.firstObject;
        self.captureDeviceInput = [AVCaptureDeviceInput deviceInputWithDevice:camera
                                                                        error:&error];
      
```

设置视频采集参数

```text
@implementation VideoCapturerParam

- (instancetype)init {
    self = [super init];
    if (self) {
        _devicePosition = AVCaptureDevicePositionFront;    // 摄像头位置，默认为前置摄像头
        _sessionPreset = AVCaptureSessionPreset1280x720;   // 视频分辨率 默认 AVCaptureSessionPreset1280x720
        _frameRate = 15;  // 帧 单位为 帧/秒，默认为15帧/秒
        _videoOrientation = AVCaptureVideoOrientationPortrait;   // 摄像头方向 默认为当前手机屏幕方向
        
        switch ([UIDevice currentDevice].orientation) {
            case UIDeviceOrientationPortrait:
            case UIDeviceOrientationPortraitUpsideDown:
                _videoOrientation = AVCaptureVideoOrientationPortrait;
                break;
                
            case UIDeviceOrientationLandscapeRight:
                _videoOrientation = AVCaptureVideoOrientationLandscapeRight;
                break;
                
            case UIDeviceOrientationLandscapeLeft:
                _videoOrientation = AVCaptureVideoOrientationLandscapeLeft;
                break;
                
            default:
                break;
        }
    }
    
    return self;
}
```

初始化输出
初始化视频输出 `AVCaptureVideoDataOutput`，并设置视频数据格式，设置采集数据回调线程，这里视频输出格式选的是 kCVPixelFormatType_420YpCbCr8BiPlanarFullRange，YUV 数据格式

```text
        /**************************  设置输出设备  *************************/
        // ---  设置视频输出  ---
        self.captureVideoDataOutput = [[AVCaptureVideoDataOutput alloc] init];
        
        NSDictionary *videoSetting = [NSDictionary dictionaryWithObjectsAndKeys:[NSNumber numberWithInt:kCVPixelFormatType_420YpCbCr8BiPlanarVideoRange], kCVPixelBufferPixelFormatTypeKey, nil];   // kCVPixelFormatType_420YpCbCr8BiPlanarVideoRange 表示输出的视频格式为NV12
        [self.captureVideoDataOutput setVideoSettings:videoSetting];
        
        // ---  设置输出串行队列和数据回调  ---
        dispatch_queue_t outputQueue = dispatch_queue_create("VideoCaptureOutputQueue", DISPATCH_QUEUE_SERIAL);
        // ---  设置代理  ---
        [self.captureVideoDataOutput setSampleBufferDelegate:self queue:outputQueue];
        // ---  丢弃延迟的帧  ---
        self.captureVideoDataOutput.alwaysDiscardsLateVideoFrames = YES;
```

**初始化 AVCaptureSession 并设置输入输出**
1、初始化 `AVCaptureSession`，把上面的输入和输出加进来，在添加输入和输出到 `AVCaptureSession` 先查询一下 `AVCaptureSession` 是否支持添加该输入或输出端口;
2、设置视频分辨率及图像质量（AVCaptureSessionPreset），设置之前同样需要先查询一下 `AVCaptureSession` 是否支持这个分辨率;
3、如果在已经开启采集的情况下需要修改分辨率或输入输出，需要用 `beginConfiguration` 和`commitConfiguration` 把修改的代码包围起来。在调用 `beginConfiguration` 后，可以配置分辨率、输入输出等，直到调用 `commitConfiguration` 了才会被应用;
4、`AVCaptureSession` 管理了采集过程中的状态，当开始采集、停止采集、出现错误等都会发起通知，我们可以监听通知来获取 `AVCaptureSession` 的状态，也可以调用其属性来获取当前 `AVCaptureSession` 的状态， `AVCaptureSession` 相关的通知都是在主线程的。

前置摄像头采集到的画面是翻转的，若要解决画面翻转问题，需要设置 `AVCaptureConnection` 的 `videoMirrored` 为 YES。

```text
/**************************  初始化会话  *************************/
        self.captureSession = [[AVCaptureSession alloc] init];
        self.captureSession.usesApplicationAudioSession = NO;
        
        // ---  添加输入设备到会话  ---
        if ([self.captureSession canAddInput:self.captureDeviceInput]) {
            [self.captureSession addInput:self.captureDeviceInput];
        }
        else {
            NSLog(@"VideoCapture:: Add captureVideoDataInput Faild!");
            return nil;
        }
        
        // ---  添加输出设备到会话  ---
        if ([self.captureSession canAddOutput:self.captureVideoDataOutput]) {
            [self.captureSession addOutput:self.captureVideoDataOutput];
        }
        else {
            NSLog(@"VideoCapture:: Add captureVideoDataOutput Faild!");
            return nil;
        }
        
        // ---  设置分辨率  ---
        if ([self.captureSession canSetSessionPreset:self.capturerParam.sessionPreset]) {
            self.captureSession.sessionPreset = self.capturerParam.sessionPreset;
        }
        
        /**************************  初始化连接  *************************/
        self.captureConnection = [self.captureVideoDataOutput connectionWithMediaType:AVMediaTypeVideo];
        
        // ---  设置摄像头镜像，不设置的话前置摄像头采集出来的图像是反转的  ---
        if (self.capturerParam.devicePosition == AVCaptureDevicePositionFront && self.captureConnection.supportsVideoMirroring) { // supportsVideoMirroring 视频是否支持镜像
            self.captureConnection.videoMirrored = YES;
        }
        
        self.captureConnection.videoOrientation = self.capturerParam.videoOrientation;
        
        self.videoPreviewLayer = [AVCaptureVideoPreviewLayer layerWithSession:self.captureSession];
        self.videoPreviewLayer.connection.videoOrientation = self.capturerParam.videoOrientation;
        self.videoPreviewLayer.videoGravity = AVLayerVideoGravityResizeAspectFill;
```

采集视频 / 回调

```text
/**
 * 开始采集
 */
- (NSError *)startCpture {
    if (self.isCapturing) {
        return [NSError errorWithDomain:@"VideoCapture:: startCapture faild: is capturing" code:1 userInfo:nil];
    }
    
    // ---  摄像头权限判断  ---
    AVAuthorizationStatus videoAuthStatus = [AVCaptureDevice authorizationStatusForMediaType:AVMediaTypeVideo];
    
    if (videoAuthStatus != AVAuthorizationStatusAuthorized) {
        return [NSError errorWithDomain:@"VideoCapture:: Camera Authorizate faild!" code:1 userInfo:nil];
    }
    
    [self.captureSession startRunning];
    self.isCapturing = YES;
    
    kLOGt(@"开始采集视频");
    
    return nil;
}


/**
 * 停止采集
 */
- (NSError *)stopCapture {
    if (!self.isCapturing) {
        return [NSError errorWithDomain:@"VideoCapture:: stop capture faild! is not capturing!" code:1 userInfo:nil];
    }
    
    [self.captureSession stopRunning];
    self.isCapturing = NO;
    
    kLOGt(@"停止采集视频");
    
    return nil;
}

#pragma mark ————— AVCaptureVideoDataOutputSampleBufferDelegate —————
/**
 * 摄像头采集数据回调
 @prama output       输出设备
 @prama sampleBuffer 帧缓存数据，描述当前帧信息
 @prama connection   连接
 */
- (void)captureOutput:(AVCaptureOutput *)output didOutputSampleBuffer:(CMSampleBufferRef)sampleBuffer fromConnection:(AVCaptureConnection *)connection {
    if ([self.delagate respondsToSelector:@selector(videoCaptureOutputDataCallback:)]) {
        [self.delagate videoCaptureOutputDataCallback:sampleBuffer];
    }
}
```

## 7.调用 / 获取数据

调用很简单，初始化视频采集参数 `VideoCapturerParam` 和 视频采集器 `VideoVapturer` , 设置预览图层 `videoPreviewLayer` , 调用 `startCpture` 就可以开始采集了，然后实现数据采集回调的代理方法 `videoCaptureOutputDataCallback` 获取数据

```text
    // --- 初始化视频采集参数  ---
    VideoCapturerParam *param = [[VideoCapturerParam alloc] init];
    
    // ---  初始化视频采集器  ---
    self.videoCapture = [[VideoVapturer alloc] initWithCaptureParam:param error:nil];
    self.videoCapture.delagate = self;
    
    // ---  开始采集  ---
    [self.videoCapture startCpture];
    

    // ---  初始化预览View  ---
    self.recordLayer = self.videoCapture.videoPreviewLayer;
    self.recordLayer.frame = CGRectMake(0, 0, CGRectGetWidth(self.view.bounds), CGRectGetHeight(self.view.bounds));
    [self.view.layer addSublayer:self.recordLayer];
```



```text
#pragma mark ————— VideoCapturerDelegate —————  视频采集回调
- (void)videoCaptureOutputDataCallback:(CMSampleBufferRef)sampleBuffer {
    NSLog(@"%@ sampleBuffer : %@ ", kLOGt(@"视频采集回调"), sampleBuffer);
}
```

至此，我们就完成了视频的采集，在采集前和过程中，我们可能会对采集参数、摄像头方向、帧率等进行修改。

原文https://zhuanlan.zhihu.com/p/485646912