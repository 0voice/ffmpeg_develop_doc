# iOS音视频开发-采集、编码、滤镜

## 1.OpenGL(绘制点 线 三角形)

**OpenGL 是⼀种图形编程接口(Application Programming Interface, API).简单理解就是开发的图形库，可以进行一些视频,图形,图⽚的渲染(顶点着色 光栅化(连线 完成定点之间的像素点) 片元着色)处理、2D/3D 游戏引擎开发等。**

### **1.1 管线(渲染流程)**

**管线分为2个部分,上半部分是客户端(C/C++,以及OpenGL API), 下半部分为服务器端(接受Attributies属性、Uniforms、TextureData纹理数据，传递到顶点着色器(处理坐标)，顶点着色器的输出最终会传递到片元着色器(注意片元着色器不接受Attributies，但可以接受Uniforms和TextureData)).**

![img](https://pic2.zhimg.com/80/v2-ce98a91ee2c7d68fc8c829a30cec0429_720w.webp)

- **固定管线：**提供一个渲染流程的管线；
- **可编程管线(**GLSL(着色语言): 专⻔为图形开发设计的编程语⾔**)：只有定点着色器(旋转平移缩放投影)和片元着色器部分来进行编程；**

**其中：**

- **Attributies：**就是对⼀个顶点都要作出改变的数据元素.实际上,顶点位置本身就是⼀个属性。
- **Uniform：**通过设置Uniform 变量就紧接着发送⼀个图元批次处理命令. Uniform 变量实际上可以⽆限次的使⽤. 设置⼀个应⽤于整个表⾯的单个颜⾊值,还可也是⼀个时间值.
- **Texture Data**: 一般在片元着色器中处理，给图形填充/添加一些颜色/质感；

### 1.2 iOS实现OpenGL ES(Embedded Systems)的两种方式

- **GLSL**
- **GLKit**

## 2. GLSL语法

### 2.1 基本数据类型

- 整型(有符号/无符号) uint a= 32u;
- 浮点数(单精度) float fValue = 3.1f;
- 布尔值 bool isDisplay= false;
- 向量、分量类型/矩阵类型

![img](https://pic3.zhimg.com/80/v2-f12e44feca3486c61ae16be91e5dc64a_720w.webp)

![img](https://pic1.zhimg.com/80/v2-d47398af2078833c3a0d9199a1ad66d8_720w.webp)

```text
v1 = vec4(10,10,10,10);
//឴ 通过x y z w来获取向量中的元素
 v1.x = 3.0f;
 v1.y = 4.0f;
 v1.z = 5.0f;
 v1.w = 1.0f;
v1.xyz = vec3(1,2,3);

//឴ 也可通过r g b a来获取向量中的元素
v1.r = 1.0f;

//឴ 也可通过s t p q来获取向量中的元素
v1.st = vec2(1.0,2.0);


m1 = mat4(
 1.0,1.0,1.0,1.0,
 1.0,1.0,1.0,1.0,
 1.0,1.0,1.0,1.0,
 1.0,1.0,1.0,1.0,
 )
m1 = mat4(1.0f);
```

### 2.2 存储限定符(着色器变量(输入输出变量)声明过程中的修饰符)

- 输入变量: 从外部(客户端/上一个阶段着色器传递的属性/Uniform等).
- 输出变量: 从任何着色器阶段进行写入的变量。

![img](https://pic1.zhimg.com/80/v2-bf31dee882fe2b08c8700450fd81e4e0_720w.webp)

常用const varying attribute uniform

- **Const**：⽤来修饰任何基本数据类型，不能⽤来修饰包含数组的数组、结构体；声明的变量在其所属的着⾊器器中均是只读的。
- **其中varying**: 传递变量的作用，表示从顶点着色器传递到片元着色器.一般会传递顶点坐标/顶点颜色/纹理值(在.vsh中定义varying修饰的变量，在.fsh中定义varying修饰的同名变量，实现将一个变量从定点着色器传递到片元着色器)。
- 被**uniform**(glUniform**())修饰的变量，只能被shader使用，不能被修改；可理解问vertext和fragment的全局变量。比如变换矩阵/材质/光照/颜色。
- 被**attribute**修饰的变量只能永在vertex shader中。使用场景：顶点坐标/法线/纹理坐标/顶点颜色。

**GLSL渲染思路**：

- -> 创建图层(CAEAGLayer 针对OpenGL ES的渲染的图层)
- -> 创建图形上下文（EAGLContext）
- -> 清空缓冲区
- -> 设置RenderBuffer （glGenRenderbuffers）
- -> 设置FrameBuffer （glGenFrameBuffers）
- -> 开始绘制 loadShaders

## 3. GLKit（无法处理3个以上的光源 和2个以上的纹理）

- -> 新建上下文 并配置GLKView

```text
self.context = [[EAFGContext all] init... APIOpenGLES2];
GLKView *view = self.view;
view.context = self,context;
然后配置颜色深度...
// 设置上下文
[EAGLContext setCurrentContext： self。context];
// 开启深度测试
glEnable(GL_DEPTH_TEST)
```

- -> 实现glkView：drawInRect代理方法

```text
//在这个代理方法中准备绘制
[baseEffect prepareToDraw..];
//索引绘图
glDrawElements(...)
```

- -> 渲染图形

```text
//确定顶点数据GLFloat 绘制索引GLuint 索引数组个数 sizeOf(indices)/sizeOf(GLunit)
//绑定缓冲区：将顶点数组数据载入数组缓冲区 将索引数据存储到索引数组缓冲区
//给着色器中传入顶点数据 颜色数据 纹理数据（用GLKBaseEffect(着色器对象)加载纹理数据）
```

- -> 实现update代理方法

## 4. 滤镜（GPUImage（底层是OpenGL ES、AV Foundation采集）、CoreImage）

### 4.1 用OpenGL ES的片元着色器来实现滤镜

**原理**：利用片元着色器读取每一个像素，对其灰度进行处理(饱和度、曝光)，计算出一个新的颜色。

### 4.2 GPUImage实现滤镜(100多种滤镜，也可以自定义滤镜)

**原理**：把图片或者视频的每一帧图片进行图形变化(饱和度/色温)处理之后，再显示到屏幕上。本质就是像素点颜色的变化。

**流程**：数据源Source -> 滤镜Filter -> Final

**Source(数据源环节)**：

- GPUImageVideoCamera :摄像头（用于拍摄视频）；
- GPUImageStillCamera：摄像头（用于拍摄照片）；
- GPUImagePicture : 用于处理拍摄完成的图片；
- GPUImageMovie : 用于处理已拍摄好的视频；

**Filter(滤镜环节)**：

GPUImageFilter：用来接受图形源，通过(自定义)顶点/片源着色器来渲染新的图像。

**Final(输出Outputs)：**

- GPUImageView
- GPUImageMovieWrite

## 5. Matal

MTKView：在MetalKit中提供了一个视图类`MTKView`，类似于GLKit中`GLKView` ，用于处理metal绘制并显示到屏幕过程中的细节。即首先需要先创建`MTKView`对象。

MTLDevice：由于metal是操作GPU的，所以Metal中提供了`MTLDevice`协议表示GPU接口，通过式`MTLCreateSystemDefaultDevice()`获取GPU。

MTLCommandQueue：在获取了GPU后，创建渲染队列，队列中存储的是将要渲染的命令。`MTLCommandBuffer`。

渲染流程：

- 先用MTLCommandBuffer创建渲染缓存区。
- 其次通过MTLRenderPassDescriptor创建渲染描述符。
- 然后再通过**渲染缓存区**和**渲染描述符**创建命令编辑器MTLRenderCommandEncoder进行编码。
- 最后是结束编码 -> 提交渲染命令 -> 在完成渲染后，将命令缓存区提交至GPU。

## 6. 音视频开发

**音视频采集 -> 视频滤镜(**GPUImage**) -> 音视频编码 -> 推流 -> 流媒体服务器处理 -> 拉流 -> 音视频解码 -> 音视频播放**

### 6.1 AVFoundation(主要是音视频的采集)

- 捕捉会话: AVCaptureSession.
- 捕捉设备: AVCaptureDevice（获取摄像头设备，以及调节摄像头一些属性 聚焦、曝光、闪光灯）.
- 捕捉设备输入: AVCaptureDeviceInput （音频输入、视频输入）
- 捕捉设备输出: AVCaptureOutput抽象类.
- ->AVCaptureStillImageOutput
- ->AVCaputureMovieFileOutput
- ->AVCaputureAudioDataOutput
- ->AVCaputureVideoDataOutput
- 捕捉链接 : AVCaptureConnection
- 捕捉预览: AVCaptureVideoPreviewLayer （显示摄像头实时捕捉的内容）

### 6.2 音视频编码

**硬编码（GPU编码**）：VideoToolBox和AudioToolBox

**软编码（CPU编码）**：

- 视频：用FFmpeg，x264算法把视频原数据YUV/RGB编码成H264编码格式，码率比较低，压缩比高
- 音频：使用fdk_aac 将音频数据PCM转成AAC

### 6.3 推流（将音视频数据通过流媒体协议发送到流媒体服务器）

流媒体协议：RTMP\RTSP\HLS\FLV

- 视频流式封装格式：TS\FLV
- 音频封装格式：Mp3\AAC

### 6.3 流媒体服务器

- 数据分发
- 实时转码
- 内容检测

### 6.4 播放

ijkplayer 播放框架

原文https://zhuanlan.zhihu.com/p/584447691