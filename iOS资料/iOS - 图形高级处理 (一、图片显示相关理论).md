# iOS - 图形高级处理 (一、图片显示相关理论)

## 1、图片从磁盘中读入到显示到屏幕全过程

### 1.1图片的加载过程：

- 使用 +imageWithContentsOfFile: 方法从磁盘中加载一张图片或 -[UIImage imageNamed:@"xx.JPG"]此时图片并没有解码;（[两种方式的区别](https://link.juejin.cn?target=http%3A%2F%2Fwww.cocoachina.com%2Farticles%2F26556))

- 初始化完成的UITmage 赋值给 UIImageView；

  - 接着一个隐式的 CATransaction 捕获到了 UIImageView 图层树的变化；

  - 在主线程的下一个 runloop 到来时，Core Animation 提交了这个隐式的 transaction ，这个过程可能会对图片进行 copy 操作，而受图片是否字节对齐等因素的影响，这个 copy 操作可能会涉及以下部分或全部步骤：

    - 分配内存缓冲区用于管理文件 IO 和解压缩操作；
    - 将文件数据从磁盘读到内存中；
    - 将压缩的图片数据解码成未压缩的位图形式，这是一个非常耗时的 CPU 操作；
    - 最后 Core Animation 中CALayer使用未压缩的位图数据渲染 UIImageView 的图层。
    - CPU计算好图片的Frame,对图片解压之后.就会交给GPU来做图片渲染。

    由上面的步骤可知，图片的解压缩是一个非常耗时的 CPU 操作，并且它默认是在主线程中执行的。那么当需要加载的图片比较多时，就会对我们应用的响应性造成严重的影响，尤其是在快速滑动的列表上，这个问题会表现得更加突出。

### 1.2渲染图片到屏幕上

- iOS设备给用户视觉反馈其实都是通过QuartzCore框架来进行的，说白了，所有用户最终看到的显示界面都是图层合成的结果，而图层即是QuartzCore中的CALayer。
- 通常我们开发中使用的视图即UIView,他并不是直接显示在屏幕上的，你可以把他想象成一个装有显示层CALayer的容器。我们在在创建视图对象的时候，系统会自动为该视图创建一个CALayer；当然我们也可以自己再往该视图中加入新的CALayer层。等到需要显示的时候，系统硬件将把所有层进行拷贝，然后按Z轴的高低合成最终的合成效果。 ![img](https://p3-juejin.byteimg.com/tos-cn-i-k3u1fbpfcp/5ec12ca4a8084c9785c190fa1f30bcde~tplv-k3u1fbpfcp-zoom-in-crop-mark:4536:0:0:0.awebp)

### 1.3图片渲染大致流程

- 在 VSync 信号到来后，主线程开始在cpu上做计算。
- CPU计算显示内容：视图创建、布局计算、图片解码、文本绘制等。
- GPU进行渲染：CPU将计算好的内容提交给GPU，GPU 进行变换、合成、渲染。
- GPU 会把渲染结果提交到帧缓冲区去，等待下一次 VSync 信号到来时显示到屏幕上。

关于渲染更多知识点，例如离屏渲染等因为篇幅太长不利于学习，这部分放在后面app性能篇继续学习。

## 2、图形处理相关框架

通过以上图片加载显示的理论学习，我们就需要来继续学习一下图形处理的相关理论，毕竟在开发过程中我们无法，性能上也不允许，所有图片的显示都用UIimage从磁盘或内存中读入。同时一些界面显示也或多或少要使用到图形处理框架。 ![img](https://p3-juejin.byteimg.com/tos-cn-i-k3u1fbpfcp/6be2bf274d9e473cbb1364518d20f6d7~tplv-k3u1fbpfcp-zoom-in-crop-mark:4536:0:0:0.awebp)

### 2.1iOS与图形图像处理相关的框架汇总：

- 界面图形框架 -- UIKit
- 核心动画框架 -- Core Animation
- 苹果封装的图形框架 -- Core Graphics & Quartz 2D
- 传统跨平台图形框架 -- OpenGL ES
- 苹果最新力推的图形框架 -- Metal
- 适合图片的苹果滤镜框架 -- Core Image
- `适合视频的第三方滤镜方案 -- GPUImage (第三方不属于系统，这里列出来学习)`
- 游戏引擎 -- Scene Kit (3D) 和 Sprite Kit (2D)
- 计算机视觉在iOS的应用 -- OpenCV for iOS

毫无疑问，开发者们接触得最多的框架是以下几个，UIKit、Core Animation，Core Graphic, Core Image。下面简要介绍这几个框架,顺便介绍下`GPUImage`:

### 2.2界面图形框架 -- UIKit(穿插使用其他图形处理框架)

- UIKit是一组Objective-C API，为线条图形、Quartz图像和颜色操作提供Objective-C 封装，并提供2D绘制、图像处理及用户接口级别的动画。

- UIKit包括UIBezierPath（绘制线、角度、椭圆及其它图形）、UIImage（显示图像）、UIColor（颜色操作）、UIFont和UIScreen（提供字体和屏幕信息）等类以及在位图图形环境、PDF图形环境上进行绘制和 操作的功能等, 也提供对标准视图的支持，也提供对打印功能的支持。

- UIKit与Core Graphics的关系：

  > 在UIKit中，UIView类本身在绘制时自动创建一个图形环境，即Core Graphics层的CGContext类型，作为当前的图形绘制环境。在绘制时可以调用 UIGraphicsGetCurrentContext 函数获得当前的图形环境;

  例如:

  > ```objectivec
  > //这段代码就是在UIView的子类中调用 UIGraphicsGetCurrentContext 函数获得当前的图形环境，然后向该图形环境添加路径，最后绘制。
  > - (void)drawRect:(CGRect)rect {
  >  //1.获取上下文
  >  CGContextRef contextRef = UIGraphicsGetCurrentContext();
  >  //2.描述路径
  >  UIBezierPath * path = [UIBezierPath bezierPath];
  >  //起点
  >  [path moveToPoint:CGPointMake(10, 10)];
  >  //终点
  >  [path addLineToPoint:CGPointMake(100, 100)];
  >  //设置颜色
  >  [[UIColor whiteColor]setStroke];
  >  //3.添加路径
  >  CGContextAddPath(contextRef, path.CGPath);
  >  //显示路径
  >  CGContextStrokePath(contextRef);
  > }
  > 复制代码
  > ```

### 2.3核心动画框架 -- Core Animation

- Core Animation 是常用的框架之一。它比 UIKit 和 AppKit 更底层。正如我们所知，UIView底下封装了一层CALayer树，Core Animation 层是真正的渲染层，我们之所以能在屏幕上看到内容，真正的渲染工作是在 Core Animation 层进行的。
- Core Animation 是一套Objective-C API，实现了一个高性能的复合引擎，并提供一个简单易用的编程接口，给用户UI添加平滑运动和动态反馈能力。
- Core Animation 是 UIKit 实现动画和变换的基础，也负责视图的复合功能。使用Core Animation可以实现定制动画和细粒度的动画控制，创建复杂的、支持动画和变换的layered 2D视图
- OpenGL ES的内容也可以与Core Animation内容进行集成。
- 为了使用Core Animation实现动画，可以修改 层的属性值 来触发一个action对象的执行，不同的action对象实现不同的动画。Core Animation 提供了一组基类及子类，提供对不同动画类型的支持：
  - CAAnimation 是一个抽象公共基类，CAAnimation采用CAMediaTiming 和CAAction协议为动画提供时间（如周期、速度、重复次数等）和action行为（启动、停止等）。
  - CAPropertyAnimation 是 CAAnimation的抽象子类，为动画提供一个由一个key路径规定的层属性的支持；
  - CABasicAnimation 是CAPropertyAnimation的具体子类，为一个层属性提供简单插入能力。
  - CAKeyframeAnimation 也是CAPropertyAnimation的具体子类，提供key帧动画支持。

### 2.4苹果封装的图形框架 -- Core Graphics & Quartz 2D

- Core Graphics（使用Quartz 2D引擎）
  - Core Graphics是一套C-based API， 支持向量图形，线、形状、图案、路径、剃度、位图图像和pdf 内容的绘制
  - Core Graphics 也是常用的框架之一。它用于运行时绘制图像。开发者们可以通过 Core Graphics 绘制路径、颜色。当开发者需要在运行时创建图像时，可以使用 Core Graphics 去绘制，运行时实时计算、绘制一系列图像帧来实现动画。与之相对的是运行前创建图像（例如从磁盘中或内存中已经创建好的UIImage图像）。
- Quartz 2D
  - Quartz 2D是Core Graphics中的2D 绘制呈现引擎。Quartz是资源和设备无关的,提供路径绘制，anti-aliased呈现，剃度填充图案，图像，透明绘制和透明层、遮蔽和阴影、颜色管理，坐标转换，字体、offscreen呈现、pdf文档创建、显示和分析等功能。
  - Quartz 2D能够与所有的图形和动画技术（如Core Animation, OpenGL ES, 和 UIKit 等）一起使用。Quartz 2D采用paint模式进行绘制。
  - Quartz 2D提供的主要类包括：
    - CGContext：表示一个图形环境；
    - CGPath：使用向量图形来创建路径，并能够填充和stroke；
    - CGImage：用来表示位图；
    - CGLayer：用来表示一个能够用于重复绘制和offscreen绘制的绘制层；
    - CGPattern：用来表示Pattern，用于重复绘制；
    - CGShading和 CGGradient：用于绘制剃度；
    - CGColor 和 CGColorSpace；用来进行颜色和颜色空间管理；
    - CGFont, 用于绘制文本；
    - CGPDFContentStream、CGPDFScanner、CGPDFPage、CGPDFObject,CGPDFStream, CGPDFString等用来进行pdf文件的创建、解析和显示。

### 2.5适合图片的苹果滤镜框架 -- Core Image

- Core Image 与 Core Graphics 恰恰相反，Core Graphics 用于在运行时创建图像，而 Core Image 是用来处理已经创建的图像的。Core Image 框架拥有一系列现成的图像过滤器，能对已存在的图像进行高效的处理。

- Core Image 是 iOS5 新加入到 iOS 平台的一个图像处理框架，提供了强大高效的图像处理功能， 用来对基于像素的图像进行操作与分析， 内置了很多强大的滤镜(Filter) (目前数量超过了180种)， 这些Filter 提供了各种各样的效果， 并且还可以通过 滤镜链 将各种效果的 Filter叠加 起来形成强大的自定义效果。

  - 一个 滤镜 是一个对象，有很多输入和输出，并执行一些变换。例如，模糊滤镜可能需要输入图像和一个模糊半径来产生适当的模糊后的输出图像。
  - 一个 滤镜链 是一个链接在一起的滤镜网络，使得一个滤镜的输出可以是另一个滤镜的输入。以这种方式，可以实现精心制作的效果。
  - iOS8 之后更是支持自定义 CIFilter，可以定制满足业务需求的复杂效果。

- Core Image 的优点在于十分高效。大部分情况下，它会在 GPU 中完成工作，但如果 GPU 忙，会使用 CPU 进行处理。如果设备支持 Metal，那么会使用 Metal 处理。这些操作会在底层完成，Apple 的工程师们已经帮助开发者们完成这些操作了。

  - 例如他可以根据需求选择 CPU 或者 GPU 来处理。

    > ```ini
    > // 创建基于 CPU 的 CIContext 对象 (默认是基于 GPU，CPU 需要额外设置参数)
    > context = [CIContext contextWithOptions: [NSDictionary dictionaryWithObject:[NSNumber numberWithBool:YES] forKey:kCIContextUseSoftwareRenderer]];
    > // 创建基于 GPU 的 CIContext 对象
    > context = [CIContext contextWithOptions: nil];
    > // 创建基于 GPU 的 CIContext 对象
    > EAGLContext *eaglctx = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
    > context = [CIContext contextWithEAGLContext:eaglctx];
    > 复制代码
    > ```

- Core Image 的 API 主要就是三类：

  - CIImage 保存图像数据的类，可以通过UIImage，图像文件或者像素数据来创建，包括未处理的像素数据。
  - CIFilter 表示应用的滤镜，这个框架中对图片属性进行细节处理的类。它对所有的像素进行操作，用一些键-值设置来决定具体操作的程度。
  - CIContext 表示上下文，如 Core Graphics 以及 Core Data 中的上下文用于处理绘制渲染以及处理托管对象一样，Core Image 的上下文也是实现对图像处理的具体对象。可以从其中取得图片的信息。

### 2.5适合视频的第三方滤镜方案 -- GPUImage

- GPUImage

  是一个基于OpenGL ES 2.0的开源的图像处理库，优势：

  - 最低支持 iOS 4.0，iOS 5.0 之后就支持自定义滤镜。在低端机型上，GPUImage 有更好的表现。
  - GPUImage 在视频处理上有更好的表现。
  - GPUImage 的代码已经开源。可以根据自己的业务需求，定制更加复杂的管线操作。可定制程度高。


原文链接：https://juejin.cn/post/6847902216238399496