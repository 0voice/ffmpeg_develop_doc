# iOS音视频 -- AVFoundation捕捉

## 1、视频捕捉

### **1.1、实现QuickTime视频的录制**

在上文中，简述了通过AVCapturePhotoOutput、AVCapturePhotoSettings来实现代理，获取当前摄像头所捕捉到的photo数据，生成一张图片。

视频录制过程大致也是如此，通过AVCaptureMovieFileOutput来获取视频数据，大致流程如下：

\1. 开启录制之前需要判断当前是否处于录制状态，只有在非录制状态才能进入录制状态

```text
/// 是否在录制状态
- (BOOL)isRecording {
 return self.movieOutput.isRecording;
}
```

\2. 通过AVCaptureConnection来获取当前视频捕捉的连接信息

\1. 调整视频方向

\2. 判断是否支持视频稳定功能（非必要）

\3. 判读是否支持平滑对焦（非必要）

\4. 为视频输出配置输出路径

\5. 开始视频recording

```text
/// 开始录制
- (void)startRecording {
 
 if (![self isRecording]) {
        //获取当前视频捕捉连接信息
        AVCaptureConnection *videoConnection = [self.movieOutput connectionWithMediaType:AVMediaTypeVideo];
 
        //调整方向
 if ([videoConnection isVideoOrientationSupported]) {
            videoConnection.videoOrientation = [self currentVideoOrientation];
        }
 
        //判断是否支持视频稳定功能(保证视频质量)
 if ([videoConnection isVideoStabilizationSupported]) {
            videoConnection.preferredVideoStabilizationMode = YES;
        }
 
        //拿到活跃的摄像头
        AVCaptureDevice *device = [self activeCamera];
        //判断是否支持平滑对焦(当用户移动设备时, 能自动且快速的对焦)
 if (device.isSmoothAutoFocusEnabled) {
            NSError *error;
 if ([device lockForConfiguration:&error]) {
                device.smoothAutoFocusEnabled = YES;
                [device unlockForConfiguration];
            } else {
                //失败回调
 
            }
        }
 
        //获取路径
        self.outputURL = [self uniqueURL];
 
        //摄像头的相关配置完成, 也获取到路径, 开始录制(这里录制QuckTime视频文件, 保存到相册)
        [self.movieOutput startRecordingToOutputFileURL:self.outputURL recordingDelegate:self];
 
    }
}
```

\3. 停止视频recording

```text
/// 停止录制
- (void)stopRecording {
 if ([self isRecording]) {
        [self.movieOutput stopRecording];
    }
}
///路径转换
- (NSURL *)uniqueURL {
    NSURL *url = [NSURL fileURLWithPath:[NSString stringWithFormat:@"%@%@", NSTemporaryDirectory(), @"output.mov"]];
 return url;
}
///获取方向值
- (AVCaptureVideoOrientation)currentVideoOrientation {
    AVCaptureVideoOrientation result;
 
    UIDeviceOrientation deviceOrientation = [UIDevice currentDevice].orientation;
    switch (deviceOrientation) {
 case UIDeviceOrientationPortrait:
 case UIDeviceOrientationFaceUp:
 case UIDeviceOrientationFaceDown:
            result = AVCaptureVideoOrientationPortrait;
 break;
 case UIDeviceOrientationPortraitUpsideDown:
            //如果这里设置成AVCaptureVideoOrientationPortraitUpsideDown，则视频方向和拍摄时的方向是相反的。
            result = AVCaptureVideoOrientationPortrait;
 break;
 case UIDeviceOrientationLandscapeLeft:
            result = AVCaptureVideoOrientationLandscapeRight;
 break;
 case UIDeviceOrientationLandscapeRight:
            result = AVCaptureVideoOrientationLandscapeLeft;
 break;
        default:
            result = AVCaptureVideoOrientationPortrait;
 break;
    }
 return result;
}
```

\4. 保存影片至相册

```text
///通过代理来获取视频数据#pragma mark - AVCaptureFileOutputRecordingDelegate
- (void)captureOutput:(AVCaptureFileOutput *)captureOutput
didFinishRecordingToOutputFileAtURL:(NSURL *)outputFileURL
      fromConnections:(NSArray *)connections
                error:(NSError *)error {
 if (error) {
        //错误回调
 
    } else {
        //视频写入到相册
        [self writeVideoToAssetsLibrary:[self.outputURL copy]];
    }
    self.outputURL = nil;
}
//写入捕捉到的视频
- (void)writeVideoToAssetsLibrary:(NSURL *)videoURL {
 
    __block PHObjectPlaceholder *assetPlaceholder = nil;
    [[PHPhotoLibrary sharedPhotoLibrary] performChanges:^{
        //保存进相册
        PHAssetChangeRequest *changeRequest = [PHAssetChangeRequest creationRequestForAssetFromVideoAtFileURL:videoURL];
        assetPlaceholder = changeRequest.placeholderForCreatedAsset;
 
    } completionHandler:^(BOOL success, NSError * _Nullable error) {
        NSLog(@"OK");
        //保存成功
        dispatch_async(dispatch_get_main_queue(), ^{
 
            //通知外部一个略缩图
            [self generateThumbnailForVideoAtURL:videoURL];
 
        });
 
    }];
}
```

\5. 生成一个略缩图通知外部

```text
///通过视频获取视频的第一帧图片当做略缩图
- (void)generateThumbnailForVideoAtURL:(NSURL *)videoURL {
 
    dispatch_async(self.videoQueue, ^{
 
        //拿到视频信息
        AVAsset *asset = [AVAsset assetWithURL:videoURL];
        AVAssetImageGenerator *imageGenerator = [AVAssetImageGenerator assetImageGeneratorWithAsset:asset];
        imageGenerator.maximumSize = CGSizeMake(100, 0);
        imageGenerator.appliesPreferredTrackTransform = YES;
 
        //通过视频将第一帧图片数据转化为CGImage
        CGImageRef imageRef = [imageGenerator copyCGImageAtTime:kCMTimeZero actualTime:NULL error:nil];
        UIImage *image = [UIImage imageWithCGImage:imageRef];
 
        //通知外部
        NSNotificationCenter *nc = [NSNotificationCenter defaultCenter];
        [nc postNotificationName:ThumbnailCreatedNotification object:image];
 
    });
 
 
}
```

### **1.2、关于QuickTime**

上文主要是通过AVCaptureMovieFileOutput将QuickTime影片捕捉到磁盘，这个类大多数核心功能继承与超类AVCaptureFileOutput。它有很多实用的功能，例如：录制到最长时限或录制到特定文件大小为止。

通常当QuickTime影片准备发布时，影片头的元数据处于文件的开始位置。这样可以让视频播放器快速读取头包含信息，来确定文件的内容、结构和其包含的多个样本的位置。当录制一个QuickTime影片时，直到所有的样片都完成捕捉后才能创建信息头。当录制结束时，创建头数据并将它附在文件结尾。

![img](https://pic2.zhimg.com/80/v2-02766b0c53845574a1dbe90928e475e1_720w.webp)

将创建头的过程放在所有影片样本完成捕捉之后存在一个问题。 在移动设备中，比如录制的时候接到电话或者程序崩溃等问题，影片头就不能被正确写入。会在磁盘生成一个不可读的影片文件。AVCaptureMovieFileOutput提供一个核心功能就是分段捕捉QuickTime影片。

![img](https://pic2.zhimg.com/80/v2-6c28625c289244f30bd03bf76b185be5_720w.webp)

## 2、AVFoundation的人脸识别

人脸识别实际上是非常复杂的一个功能，要想自己完全实现人脸识别是非常困难的。苹果为我们做了很多人脸识别的功能，例如CoreImage、AVFoundation，都是有人脸识别的功能的。还有Vision face++ 等。这里就简单介绍一下AVFoundation中的人脸识别。

在拍摄视频中，我们通过AVFoundation的人脸识别，在屏幕界面上用一个红色矩形来标识识别到的人脸。

### **2.1、人脸识别流程**

\1. 使用AVCaptureMetadataOutput来建立输出

\1. 添加进session

\2. 设置获取数据类型

\3. 在主线程中执行任务

```text
- (BOOL)setupSessionOutputs:(NSError **)error {
    //配置输入信息
    self.metadataOutput = [[AVCaptureMetadataOutput alloc] init];
    //对session添加输出
 if ([self.captureSession canAddOutput:self.metadataOutput]) {
        [self.captureSession addOutput:self.metadataOutput];
        //从输出数据中设置只获取人脸数据(可以是人脸、二维码、一维码....)
        NSArray *metadataObjectType = @[AVMetadataObjectTypeFace];
        self.metadataOutput.metadataObjectTypes = metadataObjectType;
 
        //因为人脸检测使用了硬件加速器GPU, 所以它的任务需要在主线程中执行
        dispatch_queue_t mainQueue = dispatch_get_main_queue();
        //设置metadataOutput代理方法, 检测视频中一帧一帧数据里是否包含人脸数据. 如果包含则调用回调方法
        [self.metadataOutput setMetadataObjectsDelegate:self queue:mainQueue];
 return YES;
    } else {
        //错误回调
    }
 return NO;
}
```

\2. 实现相关代理方法，将捕捉到的人脸数据传递给layer层

```text
- (void)captureOutput:(AVCaptureOutput *)output didOutputMetadataObjects:(NSArray<__kindof AVMetadataObject *> *)metadataObjects fromConnection:(AVCaptureConnection *)connection {
 
    //metadataObjects包含了捕获到的人脸数据(人脸数据会重复, 会一直捕获人脸数据)
 for (AVMetadataFaceObject *face in metadataObjects) {
        NSLog(@"Face ID:%li",(long)face.faceID);
    }
    //将人脸数据通过代理发送给外部的layer层
    [self.faceDetectionDelegate didDetectFaces:metadataObjects];
}
```

\3. 配置相关显示的图层。在layer层的视图中配置图层，我们在人脸四周添加一个矩形是在这个AVCaptureVideoPreviewLayer上进行一个个添加矩形。（因为人脸在识别过程中会出现旋转抖动等，需要进行一些3D转换等操作，后续也会出现此类操作，不在此篇作过多讲解）

```text
- (void)setupView {
 
    //用来记录人脸图层
    self.faceLayers = [NSMutableDictionary dictionary];
    //图层的填充方式: 设置videoGravity 使用AVLayerVideoGravityResizeAspectFill 铺满整个预览层的边界范围
    self.previewLayer.videoGravity = AVLayerVideoGravityResizeAspectFill;
    //在previewLayer上添加一个透明的图层
    self.overlayLayer = [CALayer layer];
    self.overlayLayer.frame = self.bounds;
    //假设你的图层上的图形会发生3D变换, 设置投影方式
    self.overlayLayer.sublayerTransform = CATransform3DMakePerspective(1000);
    [self.previewLayer addSublayer:self.overlayLayer];
}
static CATransform3D CATransform3DMakePerspective(CGFloat eyePosition) {
    //CATransform3D 图层的旋转，缩放，偏移，歪斜和应用的透
    //CATransform3DIdentity是单位矩阵，该矩阵没有缩放，旋转，歪斜，透视。该矩阵应用到图层上，就是设置默认值。
    CATransform3D  transform = CATransform3DIdentity;
    //透视效果（就是近大远小），是通过设置m34 m34 = -1.0/D 默认是0.D越小透视效果越明显
    //D:eyePosition 观察者到投射面的距离
    transform.m34 = -1.0/eyePosition;
 
 return transform;
}
```

\4. 处理通过代理传递过来的人脸数据

\1. 将人脸在摄像头中的坐标转化为屏幕坐标

\2. 定义一个数组，保存所有的人脸数据，用于存放待从屏幕上删除的人脸数据

\3. 遍历人脸数据

\1. 通过对比屏幕上的layer（框框）数量来与传递过来的人脸进行对比，判断是否需要移除layer（框框）

\2. 根据人脸数据的ID来从屏幕上的layer（框框）中查找是否已经存在，不存在则需要生成一个layer（框框），并更新屏幕的layer（框框）数组。

\3. 根据传递过来的人脸数据来设置layer（框框）的位置，注意：当最后一个人脸离开屏幕，此时代理方法不会调用，会导致最后一个layer（框框）仍停留在屏幕上，所以需要处理一下人脸将要离开屏幕就对其进行移除处理。

\4. 在捕捉过程中，人脸会左右前后摆动（即z、y轴变化），来做不同的处理

\4. 遍历一下待删除数组，将之与传递过来的人脸数据进行对比，删除多余的人脸数据

注意：此处省略了一些3D转换的方法

```text
- (void)didDetectFaces:(NSArray *)faces {
    //人脸数据位置信息(摄像头坐标系)转换为屏幕坐标系
    NSArray *transfromedFaces = [self transformedFacesFromFaces:faces];
 
    //人脸消失, 删除图层
 
    //需要删除的人脸数据列表
    NSMutableArray *lostFaces = [self.faceLayers.allValues mutableCopy];
 
    //遍历每个人脸数据
 for (AVMetadataFaceObject *face in transfromedFaces) {
 
        //face ID
        NSNumber *faceID = @(face.faceID);
        //face ID存在即不需要删除(从删除列表中移除)
        [lostFaces removeObject:faceID];
 
        //假如有新的人脸加入
        CALayer *layer = self.faceLayers[faceID];
 if (!layer) {
            NSLog(@"新增人脸");
            layer = [self makeFaceLayer];
            [self.overlayLayer addSublayer:layer];
 
            //更新字典
            self.faceLayers[faceID] = layer;
        }
 
        //根据人脸的bounds设置layer的frame
        layer.frame = face.bounds;
        CGSize size = self.bounds.size;
        //当人脸特别靠近屏幕边缘, 直接当作无法识别此人脸(因为人脸离开屏幕不会走此代理方法, 需要提前做移除)
 if (face.bounds.origin.x < 3 ||
            face.bounds.origin.x > size.width - layer.frame.size.width - 3 ||
            face.bounds.origin.y < 3 ||
            face.bounds.origin.y > size.height - layer.frame.size.height - 3 ) {
            [layer removeFromSuperlayer];
            [self.faceLayers removeObjectForKey:faceID];
        }
 
        //设置3D属性(人脸是3D的, 需要根据人脸的3D变化做不同的变化处理)
        layer.transform = CATransform3DIdentity;
        //人脸z轴变化
 if (face.hasRollAngle) {
            CATransform3D t = [self transformForRollAngle:face.rollAngle];
            //矩阵相乘
            layer.transform = CATransform3DConcat(layer.transform, t);
        }
        //人脸y轴变化
 if (face.hasYawAngle) {
            CATransform3D t = [self transformForYawAngle:face.hasYawAngle];
            //矩阵相乘
            layer.transform = CATransform3DConcat(layer.transform, t);
        }
    }
    //处理已经从镜头消失的人脸(人脸消失,图层并没有消失)
 for (NSNumber *faceID in lostFaces) {
        CALayer *layer = self.faceLayers[faceID];
        [self.faceLayers removeObjectForKey:faceID];
        [layer removeFromSuperlayer];
    }
}
```

### **2.1、其他类型数据的识别**

有的同学在设置AVMetadataObjectTypeFace的可能会发现，还有会有一些其他的类型，例如AVMetadataObjectTypeQRCode等，就是从摄像头中捕获二维码数据，它的流程与人脸识别极度相似，甚至要更为简单一些，因为二维码并不像人脸一样需要做一些3D的转换等操作，所以此处不再示例捕捉二维码。

原文https://zhuanlan.zhihu.com/p/222418988