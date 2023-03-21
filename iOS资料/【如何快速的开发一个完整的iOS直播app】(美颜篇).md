# 【如何快速的开发一个完整的iOS直播app】(美颜篇)

## 1、前言

在看这篇之前，如果您还不了解直播原理，请查看这篇文章[如何快速的开发一个完整的iOS直播app(原理篇)](https://www.jianshu.com/p/bd42bacbe4cc)

开发一款直播app，美颜功能是很重要的，如果没有美颜功能，可能分分钟钟掉粉千万，本篇主要讲解直播中`美颜功能的实现原理`，并且实现美颜功能。

如果喜欢我的文章，可以关注我微博:[袁峥Seemygo](https://link.jianshu.com?t=http://weibo.com/2034818060/profile?rightmod=1&wvr=6&mod=personinfo)

### 1.1 利用GPUImage处理直播过程中美颜的流程

- 采集视频 => 获取每一帧图片 => 滤镜处理 => GPUImageView展示

![img](https:////upload-images.jianshu.io/upload_images/304825-5faefe55f9296071.png?imageMogr2/auto-orient/strip|imageView2/2/w/1200/format/webp)

美颜原理.png

### 1.2 美颜基本概念

`GPU`：（Graphic Processor Unit图形处理单元）手机或者电脑用于图像处理和渲染的硬件

`GPU工作原理`：采集数据-> 存入主内存(RAM) -> CPU(计算处理) -> 存入显存(VRAM) -> GPU(完成图像渲染) -> 帧缓冲区 -> 显示器

![img](https:////upload-images.jianshu.io/upload_images/304825-67fd2250df30924d.jpg?imageMogr2/auto-orient/strip|imageView2/2/w/1200/format/webp)

GPU工作原理.jpg

`OpenGL ES`：（Open Graphics Library For Embedded(嵌入的) Systems `开源嵌入式系统图形处理框架`），一套图形与硬件接口，用于把处理好的图片显示到屏幕上。

`GPUImage`:是一个基于OpenGL ES 2.0图像和视频处理的开源iOS框架，提供各种各样的图像处理滤镜，并且支持照相机和摄像机的实时滤镜，内置120多种滤镜效果，并且能够自定义图像滤镜。

`滤镜处理的原理`:就是把静态图片或者视频的每一帧进行图形变换再显示出来。它的本质就是像素点的坐标和颜色变化

### 1.3 GPUImage处理画面原理

- GPUImage采用链式方式来处理画面,通过addTarget:方法为链条添加每个环节的对象，处理完一个target,就会把上一个环节处理好的图像数据传递下一个target去处理，称为GPUImage处理链。

  - 比如：墨镜原理，`从外界传来光线，会经过墨镜过滤，在传给我们的眼睛，就能感受到大白天也是乌黑一片，哈哈`。
  - 一般的target可分为两类
    - `中间环节`的target,  一般是各种filter, 是GPUImageFilter或者是子类.
    - `最终环节`的target, GPUImageView：用于显示到屏幕上, 或者GPUImageMovieWriter：写成视频文件。

- GPUImage处理主要分为3个环节

  - `source(视频、图片源) -> filter（滤镜） -> final target (处理后视频、图片)`

  - ```
    GPUImaged的Source
    ```

    :都继承GPUImageOutput的子类，作为GPUImage的数据源,就好比外界的光线，作为眼睛的输出源

    - GPUImageVideoCamera：用于实时拍摄视频
    - GPUImageStillCamera：用于实时拍摄照片
    - GPUImagePicture：用于处理已经拍摄好的图片，比如png,jpg图片
    - GPUImageMovie：用于处理已经拍摄好的视频,比如mp4文件

  - `GPUImage的filter`:GPUimageFilter类或者子类，这个类继承自GPUImageOutput,并且遵守GPUImageInput协议，这样既能流进，又能流出，就好比我们的墨镜，光线通过墨镜的处理，最终进入我们眼睛

  - `GPUImage的final target`:GPUImageView,GPUImageMovieWriter就好比我们眼睛，最终输入目标。

![img](https:////upload-images.jianshu.io/upload_images/304825-84ff54fb516a70ce.png?imageMogr2/auto-orient/strip|imageView2/2/w/1200/format/webp)

GPUImage处理原理.png

### 1.4 美颜原理

- `磨皮(GPUImageBilateralFilter)`：本质就是让像素点模糊，可以使用高斯模糊，但是可能导致边缘会不清晰，用双边滤波(Bilateral Filter) ，有针对性的模糊像素点，能保证边缘不被模糊。
- `美白(GPUImageBrightnessFilter)`：本质就是提高亮度。

### 1.5 美颜效果

- `关注效果，忽悠本人`

**GPUImage原生美颜效果**

![img](https:////upload-images.jianshu.io/upload_images/304825-9e89812af6ad2de9.gif?imageMogr2/auto-orient/strip|imageView2/2/w/333/format/webp)

GPUImage原生.gif

**利用美颜滤镜实现效果**

![img](https:////upload-images.jianshu.io/upload_images/304825-9bbdeb87569ebab3.gif?imageMogr2/auto-orient/strip|imageView2/2/w/333/format/webp)

美颜滤镜处理.gif

### 1.6 GPUImage实战

#### 1.6.1 GPUImage原生美颜

- 步骤一：使用Cocoapods导入GPUImage
- 步骤二：创建视频源GPUImageVideoCamera
- 步骤三：创建最终目的源：GPUImageView
- 步骤四：创建滤镜组(GPUImageFilterGroup)，需要组合`亮度(GPUImageBrightnessFilter)`和`双边滤波(GPUImageBilateralFilter)`这两个滤镜达到美颜效果.
- 步骤五：设置滤镜组链
- 步骤六：设置GPUImage处理链，从数据源 => 滤镜 => 最终界面效果
- 步骤七：开始采集视频

注意点：

- `SessionPreset`最好使用AVCaptureSessionPresetHigh，会自动识别，如果用太高分辨率，当前设备不支持会直接报错
- `GPUImageVideoCamera必须要强引用`，否则会被销毁，不能持续采集视频.
- `必须调用startCameraCapture`，底层才会把采集到的视频源，渲染到GPUImageView中，就能显示了。
- GPUImageBilateralFilter的`distanceNormalizationFactor`值越小，磨皮效果越好,`distanceNormalizationFactor`取值范围: `大于1`。

```csharp
- (void)viewDidLoad {
    [super viewDidLoad];
    
    // 创建视频源
    // SessionPreset:屏幕分辨率，AVCaptureSessionPresetHigh会自适应高分辨率
    // cameraPosition:摄像头方向
    GPUImageVideoCamera *videoCamera = [[GPUImageVideoCamera alloc] initWithSessionPreset:AVCaptureSessionPresetHigh cameraPosition:AVCaptureDevicePositionFront];
     videoCamera.outputImageOrientation = UIInterfaceOrientationPortrait;
    _videoCamera = videoCamera;

    // 创建最终预览View
    GPUImageView *captureVideoPreview = [[GPUImageView alloc] initWithFrame:self.view.bounds];
    [self.view insertSubview:captureVideoPreview atIndex:0];
    
    // 创建滤镜：磨皮，美白，组合滤镜
    GPUImageFilterGroup *groupFilter = [[GPUImageFilterGroup alloc] init];
    
    // 磨皮滤镜
    GPUImageBilateralFilter *bilateralFilter = [[GPUImageBilateralFilter alloc] init];
    [groupFilter addTarget:bilateralFilter];
    _bilateralFilter = bilateralFilter;
    
    // 美白滤镜
    GPUImageBrightnessFilter *brightnessFilter = [[GPUImageBrightnessFilter alloc] init];
    [groupFilter addTarget:brightnessFilter];
    _brightnessFilter = brightnessFilter;
    
    // 设置滤镜组链
    [bilateralFilter addTarget:brightnessFilter];
    [groupFilter setInitialFilters:@[bilateralFilter]];
    groupFilter.terminalFilter = brightnessFilter;
    
    // 设置GPUImage响应链，从数据源 => 滤镜 => 最终界面效果
    [videoCamera addTarget:groupFilter];
    [groupFilter addTarget:captureVideoPreview];
    
    // 必须调用startCameraCapture，底层才会把采集到的视频源，渲染到GPUImageView中，就能显示了。
    // 开始采集视频
    [videoCamera startCameraCapture];
}

- (IBAction)brightnessFilter:(UISlider *)sender {
    _brightnessFilter.brightness = sender.value;
}

- (IBAction)bilateralFilter:(UISlider *)sender {
    // 值越小，磨皮效果越好
    CGFloat maxValue = 10;
    [_bilateralFilter setDistanceNormalizationFactor:(maxValue - sender.value)];
}
```

#### 1.6.2 利用美颜滤镜实现

- 步骤一：使用Cocoapods导入GPUImage
- 步骤二：导入GPUImageBeautifyFilter文件夹
- 步骤三：创建视频源GPUImageVideoCamera
- 步骤四：创建最终目的源：GPUImageView
- 步骤五：创建最终美颜滤镜：GPUImageBeautifyFilter
- 步骤六：设置GPUImage处理链，从数据源 => 滤镜 => 最终界面效果

注意：

- `切换美颜效果原理`：移除之前所有处理链，重新设置处理链

```csharp
- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
    // 创建视频源
    // SessionPreset:屏幕分辨率，AVCaptureSessionPresetHigh会自适应高分辨率
    // cameraPosition:摄像头方向
    GPUImageVideoCamera *videoCamera = [[GPUImageVideoCamera alloc] initWithSessionPreset:AVCaptureSessionPresetHigh cameraPosition:AVCaptureDevicePositionFront];
    videoCamera.outputImageOrientation = UIInterfaceOrientationPortrait;
    _videoCamera = videoCamera;
    
    // 创建最终预览View
    GPUImageView *captureVideoPreview = [[GPUImageView alloc] initWithFrame:self.view.bounds];
    [self.view insertSubview:captureVideoPreview atIndex:0];
    _captureVideoPreview = captureVideoPreview;
    
    // 设置处理链
    [_videoCamera addTarget:_captureVideoPreview];
    
    // 必须调用startCameraCapture，底层才会把采集到的视频源，渲染到GPUImageView中，就能显示了。
    // 开始采集视频
    [videoCamera startCameraCapture];

}

- (IBAction)openBeautifyFilter:(UISwitch *)sender {
    
    // 切换美颜效果原理：移除之前所有处理链，重新设置处理链
    if (sender.on) {
        
        // 移除之前所有处理链
        [_videoCamera removeAllTargets];
        
        // 创建美颜滤镜
        GPUImageBeautifyFilter *beautifyFilter = [[GPUImageBeautifyFilter alloc] init];
        
        // 设置GPUImage处理链，从数据源 => 滤镜 => 最终界面效果
        [_videoCamera addTarget:beautifyFilter];
        [beautifyFilter addTarget:_captureVideoPreview];
        
    } else {
        
        // 移除之前所有处理链
        [_videoCamera removeAllTargets];
        [_videoCamera addTarget:_captureVideoPreview];
    }
    
    
}
```

### 1.7 GPUImage扩展

- [GPUImage所有滤镜介绍](https://link.jianshu.com?t=http://www.360doc.com/content/15/0907/10/19175681_497418716.shtml)
- [美颜滤镜](https://www.jianshu.com/p/945fc806a9b4)
- [美图秀秀滤镜大汇总](https://link.jianshu.com?t=http://www.tuicool.com/articles/6bIbQbQ)

## 2、源码下载

[源码](https://link.jianshu.com?t=https://github.com/iThinkerYZ/GPUImgeDemo)
 注意：第一次打开需要 `pod install`

## 3、结束语

后续还会讲解GPUImage原理openGL ES，视频编码，推流，聊天室，礼物系统等更多功能，敬请关注！！！



原文链接：https://www.jianshu.com/p/4646894245ba