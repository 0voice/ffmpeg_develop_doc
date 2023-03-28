# AVFoundation之视频捕捉

## 1.概念

### 1.1 捕捉会话

AV Foundation 捕捉栈核心类是AVCaptureSession。一个捕捉会话相当于一个虚拟的“插线板”。用于连接输入和输出的资源。

### 1.2 捕捉设备

AVCaptureDevice为摄像头、麦克风等物理设备提供接口。大部分我们使用的设备都是内置于MAC或者iPhone、iPad上的。当然也可能出现外部设备。但是AVCaptureDevice 针对物理设备提供了大量的控制方法。比如控制摄像头聚焦、曝光、白平衡、闪光灯等。

### 1.3 捕捉设备的输入

注意：为捕捉设备添加输入，不能添加到AVCaptureSession 中，必须通过将它封装到一个AVCaptureDeviceInputs实例中。这个对象在设备输出数据和捕捉会话间扮演接线板的作用。

### 1.4 捕捉的输出

AVCaptureOutput 是一个抽象类。用于为捕捉会话得到的数据寻找输出的目的地。框架定义了一些抽象类的高级扩展类。例如 AVCaptureStillImageOutput 和 AVCaptureMovieFileOutput类。使用它们来捕捉静态照片、视频。例如 AVCaptureAudioDataOutput 和 AVCaptureVideoDataOutput ,使用它们来直接访问硬件捕捉到的数字样本。

### 1.5 捕捉连接

AVCaptureConnection类.捕捉会话先确定由给定捕捉设备输入渲染的媒体类型，并自动建立其到能够接收该媒体类型的捕捉输出端的连接。

### 1.6 捕捉预览

如果不能在影像捕捉中看到正在捕捉的场景，那么应用程序用户体验就会很差。幸运的是框架定义了
AVCaptureVideoPreviewLayer 类来满足该需求。这样就可以对捕捉的数据进行实时预览。

## 2. 创建预览视图

### 2.1 创建预览视图

图2-1是项目的用户界面的组成图示。我们把重点放在中间层THPreview View的实现上。因为它直接包含了AV Foundation 的用户界面。

![img](https://p3-sign.toutiaoimg.com/tos-cn-i-qvj2lq49k0/97b8ce0f077a4437b2db557425802903~noop.image?_iz=58558&from=article.pc_detail&x-expires=1679402552&x-signature=tzSzjuY1UOAY1nwbQMhLCDMSSes%3D)



图2-1所示的THPreview View 类提供给用户用i 个摄像头当前拍摄内容的实时预览图。我们将使用
AVCaptureVideoPreviewLayer方法实现这个行为。首先我们通过认识THPreview View 的接口开始了解如何实现。

#### 2.1.1THPreviewView 接口

![img](https://p3-sign.toutiaoimg.com/tos-cn-i-qvj2lq49k0/8ffe110e42414d15b877bb2f4ae00e57~noop.image?_iz=58558&from=article.pc_detail&x-expires=1679402552&x-signature=hoaTJPClxcEAZVMyi9ezCKNE7ns%3D)



#### 2.1.2THPreviewView 实现

![img](https://p3-sign.toutiaoimg.com/tos-cn-i-qvj2lq49k0/abd585067b624409bcea8ea059be0db3~noop.image?_iz=58558&from=article.pc_detail&x-expires=1679402552&x-signature=PHbdx0dkHlwYZ2dmrsRd5ICWPHU%3D)



### 2.2 坐标空间转换

当使用AV Foundation 的捕捉API时，一定要理解屏幕坐标系和捕捉设备坐标系不同。iOS6.0之前的版本，要在这2个坐标空间进行转换非常困难。要精确的将屏幕坐标点转换为摄像头坐标点。开发者必须考虑诸如视频重力、镜像、图层变换和方向等因素进行综合计算。幸运的是，
AVCaptureVideoPreviewLayer现在定义了一个转换方法让这一过程变得简单多了。


AVCaptureVideoPreviewLayer定义了2个方法用于坐标系间进行转换：

- captureDevicePointOfInterestForPoint:获取屏幕坐标系的CGPoint 数据，返回转换得到的设备坐标系CGPoint数据。
- pointForCaptureDevicePointOfInterest:获取摄像头坐标系的CGPoint数据，返回转换得到的屏幕坐标系CGPoint 数据。

THPreview View 使用
captureDevicePointOfInterestForPoint:方法将用户触点信息转换为摄像头设备坐标系的中的点。在项目中点击对焦和点击曝光功能实现会用到这个转换坐标点。

## 3.创建捕捉控制器

### 3.1 在THCameraController 类中实现

用于配置不同的捕捉设备，同时对捕捉的输出进行控制和交互。

![img](https://p3-sign.toutiaoimg.com/tos-cn-i-qvj2lq49k0/5c5dfd39e9154a068507a2f776d443c1~noop.image?_iz=58558&from=article.pc_detail&x-expires=1679402552&x-signature=ixPXlqM5TvyQfLExtH9BZJXCBVk%3D)



![img](https://p3-sign.toutiaoimg.com/tos-cn-i-qvj2lq49k0/16d1828e1ace4de889235d53b9d77676~noop.image?_iz=58558&from=article.pc_detail&x-expires=1679402552&x-signature=MNP4kjNXVmwVjNKj5O%2B%2BEPXaR%2Fs%3D)



![img](https://p3-sign.toutiaoimg.com/tos-cn-i-qvj2lq49k0/220a430de18440ac9c7f3a425bfb853e~noop.image?_iz=58558&from=article.pc_detail&x-expires=1679402552&x-signature=y3EcxH%2BTrCU%2Ff3t4vy3Upc8Laig%3D)



### 3.2 在设置捕捉会话 在 THCameraController.m

需要导入系统框架<
AVFoundation/AVFoundation.h><AssetsLibrary/AssetsLibrary.h>

![img](https://p3-sign.toutiaoimg.com/tos-cn-i-qvj2lq49k0/c6e13258eddd452287a98dfc14f30522~noop.image?_iz=58558&from=article.pc_detail&x-expires=1679402552&x-signature=v01VMyWDlInvJuGVyVZWfZi%2FYe8%3D)



### 3.3 设置捕捉会话 setupSession:方法实现

![img](https://p3-sign.toutiaoimg.com/tos-cn-i-qvj2lq49k0/e40438ea757548499723d948784ff164~noop.image?_iz=58558&from=article.pc_detail&x-expires=1679402552&x-signature=SDOIHWQbjSAtHev0lbttFghgBSk%3D)



![img](https://p3-sign.toutiaoimg.com/tos-cn-i-qvj2lq49k0/c8d98d66fb434f89a72c39f87e98496d~noop.image?_iz=58558&from=article.pc_detail&x-expires=1679402552&x-signature=UcFU3oqhz4HjhIllroWdDdPh87E%3D)



![img](https://p3-sign.toutiaoimg.com/tos-cn-i-qvj2lq49k0/8d949f76a00f4be4ab3dd9b83de17e6d~noop.image?_iz=58558&from=article.pc_detail&x-expires=1679402552&x-signature=%2BxBhWlJLIrasEw%2BAtWSBxJnAvfk%3D)



![img](https://p3-sign.toutiaoimg.com/tos-cn-i-qvj2lq49k0/f94e86a092f5407f9efdf75934bfe606~noop.image?_iz=58558&from=article.pc_detail&x-expires=1679402552&x-signature=9ygkpa1RVPiNOsoL%2F3xmUXyWbHw%3D)



### 3.4 启动和停止捕捉会话

![img](https://p3-sign.toutiaoimg.com/tos-cn-i-qvj2lq49k0/661936fd08874be1a03c3d6fd2ffffc7~noop.image?_iz=58558&from=article.pc_detail&x-expires=1679402552&x-signature=TfJNFmT5QXrHOaPBU2vznJk47Ss%3D)



### 3.5 处理隐私需求

在这个项目会涉及到摄像头、相册、麦克风。需要给出用户提醒，处理隐私需求

注意：iOS7版本只有特定地区有法律规定才会询问用户是否可以访问设备的相机。而从iOS8.0之后，所有的地区和用户都要在应用程序中取得授权才可以访问相机。

常用的隐私设置 plist 修改

![img](https://p3-sign.toutiaoimg.com/tos-cn-i-qvj2lq49k0/5600be7b6e6e4d0c89170f69de3f3de4~noop.image?_iz=58558&from=article.pc_detail&x-expires=1679402552&x-signature=hUuY66COJM2WmI6Du1ViwMK7aJU%3D)



## 4.切换摄像头

### 4.1 摄像头的支撑方法

![img](https://p3-sign.toutiaoimg.com/tos-cn-i-qvj2lq49k0/20b77c0aa78745ab9edc67cf06e7f36f~noop.image?_iz=58558&from=article.pc_detail&x-expires=1679402552&x-signature=1S1gNxTAmLU9Ov5r366YPtR%2BWSM%3D)



![img](https://p3-sign.toutiaoimg.com/tos-cn-i-qvj2lq49k0/5529768d56704407bee3a7b55e534952~noop.image?_iz=58558&from=article.pc_detail&x-expires=1679402552&x-signature=6%2F%2FwlQ1uy8%2F65fL3YdhSddvHY7s%3D)



![img](https://p3-sign.toutiaoimg.com/tos-cn-i-qvj2lq49k0/c47178a6641e47558f4df7b3ddb6f8f2~noop.image?_iz=58558&from=article.pc_detail&x-expires=1679402552&x-signature=7mlepY%2Fu0yyHOSt16Xz7dynX9Tk%3D)



### 4.2 切换摄像头

![img](https://p3-sign.toutiaoimg.com/tos-cn-i-qvj2lq49k0/731d6bde611a48409d5b918a471205f4~noop.image?_iz=58558&from=article.pc_detail&x-expires=1679402552&x-signature=guZRPhpTFBSEi3zIa7Pw8TMMO64%3D)



![img](https://p3-sign.toutiaoimg.com/tos-cn-i-qvj2lq49k0/ff086142e3394cc6aea384a4aa572108~noop.image?_iz=58558&from=article.pc_detail&x-expires=1679402552&x-signature=I2QrflvR1TfTqVaN0lXqU5bhi9s%3D)



![img](https://p3-sign.toutiaoimg.com/tos-cn-i-qvj2lq49k0/bcce814c0b7e40ff9089e1449f97e439~noop.image?_iz=58558&from=article.pc_detail&x-expires=1679402552&x-signature=gZDdPyUYgSf35JhZyaf2DCWbTiE%3D)

原文https://www.toutiao.com/article/7130582816781369894/?channel=&source=search_tab