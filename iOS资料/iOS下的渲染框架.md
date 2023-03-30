# iOS下的渲染框架

## 1.图形渲染框架

iOS APP图形渲染框架，APP在显示可视化的图形时，使用到了Core Animation、Core Graphics、Core Image等框架，这些框架在渲染图形时，都需要通过OpenGL ES / Metal来驱动GPU进行渲染与绘制。

![img](https://pic2.zhimg.com/80/v2-b3b623f9e42eec9c52331e524f6b320d_720w.webp)

- **UIKit**

UIKit是iOS开发最常用的框架，可以通过设置UIKit组件的布局以及相关属性来绘制界面。
事实上，UIKit自身并不具备在屏幕成像的能力，其主要负责对用户操作事件的响应（UIView继承自UIResponder），事件响应的传递大体是经过逐层的**视图树**遍历实现的。

- **Core Animation**

Core Animation源自于Layer Kit，动画只是Core Animation的冰山一角。
Core Animation是一个复合引擎，其职责是**尽可能快地组合屏幕上不同的可视内容，这些可视内容可被分解成独立的图层（即CALayer），这些图层会被存储在一个叫做图层树的体系之中**。从本质上而言，CALayer是用户所能在屏幕上看见的一切的基础。

- **Core Graphics**

Core Graphics是基于Quartz 的高级绘图引擎，主要用于运行时绘制图像。开发者可以使用此框架来处理基于路径的绘图，转换，颜色管理，离屏渲染，图案，渐变和阴影，图像数据管理，图像创建和图像遮罩以及PDF文档创建，显示和分析。

- **Core Image**

Core Image与Core Graphics恰恰相反，Core Graphics用于在运行时创建图像，而Core Image用于处理运行前创建的图像。Core Image框架拥有一系列现成的图像过滤器，能对一寸照的图像进行高效的处理。大部分情况下，Core Image会在GPU中完成工作，如果GPU忙，会使用CPU进行处理。

## 2.UIView与CALayer的关系

CALayer事实上是用户所能在屏幕上看见的一切的基础。为什么UIKit中的视图能够呈现可视化内容，就是因为UIKit中的每一个UI视图控件其实内部都有一个关联的CALayer，即backing layer。

由于这种一一对应的关系，视图层级有用**视图树**的树形结构，对应CALayer层级也拥有**图层树**的树形结构。

其中，视图的职责是创建并管理图层，以确保当子视图在层级关系中添加或被移除时，其关联的图层在图层树中也有相同的操作，即保证视图树和图层树在结构上的一致性。

为什么iOS要基于UIView和CALayer提供两个平行的层级关系呢？

其原因在于要做**职责分离**，这样也能避免很多重复代码。在iOS和Mac OSX两个平台上，事件和用户交互有很多地方的不同，基于多点触控的用户界面和基于鼠标键盘的交互有着本质的区别，这就是为什么iOS有UIKit和UIView，对应Mac OSX有AppKit和NSView的原因。它们在功能上很相似，但是在实现上有着显著的区别。实际上，这里并不是两个层级关系，而是四个。每一个都扮演着不同的角色。除了**视图树**和**图层树**，还有**呈现树**和**渲染树**。

那么为什么CALayer可以呈现可视化内容呢？因为CALayer基本等同于一个**纹理**。纹理是GPU进行图像渲染的重要依据。

在[图形渲染原理](https://link.zhihu.com/?target=https%3A//links.jianshu.com/go%3Fto%3Dhttp%3A%2F%2Fchuquan.me%2F2018%2F08%2F26%2Fgraphics-rending-principle-gpu%2F)中提到纹理本质上就是一张图片，因此CALayer也包含一个contents属性指向一块缓存区，称为backing store，可以存放位图(Bitmap)。iOS中将该缓存区保存的图片称为**寄宿图**。

![img](https://pic1.zhimg.com/80/v2-ed1a8236002c33eeace98438d7bec358_720w.webp)

图形渲染流水线支持从顶点开始进行绘制（在流水线中，顶点会被处理生成纹理），也支持直接使用纹理（图片）进行渲染。相应地，在实际开发中，绘制界面也有两种方式： 一种是**手动绘制**；另一种是**使用图片**。

对此，iOS中也有两种相应的实现方式：

- 使用图片：contents image
- 手动绘制：custom drawing

**Contents Image**
Contents Image是指通过CALayer的contents属性来配置图片。然而，contents属性的类型为id，在这种情况下，可以给contents属性赋予任何值，app仍可以编译通过。但是在实践中，如果contents的值不是CGImage，得到的图层将是空白的。
既然如此，为什么要将contents的属性类型定义为id而非CGImage。因为在Mac OS系统中，该属性对CGImage和NSImage类型的值都起作用，而在iOS系统中，该属性只对CGImage起作用。
本质上，contents属性指向的一块缓存区域，称为backing store，可以存放bitmap数据。
**Custom Drawing**
Custom Drawing是指使用Core Graphics直接绘制寄宿图。实际开发中，一般通过继承UIView并实现-drawRect:方法来自定义绘制。
虽然-drawRect:是一个UIView方法，但事实上都是底层的CALayer完成了重绘工作并保存了产生的图片。
下图所示为drawRect：绘制定义寄宿图的基本原理

![img](https://pic2.zhimg.com/80/v2-59ba203ff08d01499f194f0c258da479_720w.webp)

- UIView有一个关联图层，即CALayer。
- CALayer有一个可选的delegate属性，实现了CALayerDelegate协议。UIView作为CALayer的代理实现了CALayerDelegate协议。
- 当需要重绘时，即调用-drawRect:，CALayer请求其代理给予一个寄宿图来显示。
- CALayer首先会尝试调用-displayLayer:方法，此时代理可以直接设置contents属性。

```objective-c
- (void)displayLayer:(CALayer *)layer;
```

- 如果代理没有实现-displayLayer:方法，CALayer则会尝试调用`-`drawLayer:inContext:方法。在调用该方法前，CALayer会创建一个空的寄宿图（尺寸由bounds和contentScale决定）和一个Core Graphics的绘制上下文，为绘制寄宿图做准备，作为ctx参数传入。

```objective-c
- (void)drawLayer:(CALayer *)layer inContext:(CGContextRef)ctx;
```

- 最后，有Core Graphics绘制生成的寄宿图会存入backing store。

**三个框架间的依赖关系**

Core Animation、Core Graphics、Core Image这个三个框架间也存在着依赖关系。

上面提到CALayer是用户所能在屏幕上看到一切的基础。所以Core Graphics、Core Image是需要依赖于CALayer来显示界面的。由于CALayer又是Core Animation框架提供的，所以说Core Graphics、Core Image是依赖于``Core Animation ```的。

上文还提到每一个 UIView 内部都关联一个CALayer图层，即backing layer，每一个CALayer都包含一个content属性指向一块缓存区，即backing store， 里面存放位图（Bitmap）。iOS中将该缓存区保存的图片称为寄宿图。

这个寄宿图有两个设置方式：

直接向content设置CGImage图片，这需要依赖Core Image来提供图片。

通过实现UIView的drawRect方法自定义绘图，这需要借助Core Graphics来绘制图形，再由CALayer生成图片。

![img](https://pic2.zhimg.com/80/v2-765f27162bb7790d79d84a8a8894917d_720w.webp)

## 3.Core Animation 流水线

CALayer是如何调用 GPU 并显示可视化内容的呢？下面我们就需要介绍一下 Core Animation 流水线的工作原理。

![img](https://pic4.zhimg.com/80/v2-90eb26de8df27fc3b3ff74f918af5293_720w.webp)

事实上，app 本身并不负责渲染，渲染则是由一个独立的进程负责，即Render Server进程。

App 通过 IPC 将渲染任务及相关数据提交给Render Server。Render Server处理完数据后，再传递至 GPU。最后由 GPU 调用 iOS 的图像设备进行显示。

Core Animation 流水线的详细过程如下：

首先，由 app 处理事件（Handle Events），如：用户的点击操作，在此过程中 app 可能需要更新**视图树**，相应地，**图层树**也会被更新。

其次，app 通过 CPU 完成对显示内容的计算，如：视图的创建、布局计算、图片解码、文本绘制等。在完成对显示内容的计算之后，app 对图层进行打包，并在下一次 RunLoop 时将其发送至Render Server，即完成了一次Commit Transaction操作。

Render Server主要执行 Open GL、Core Graphics 相关程序，并调用 GPU

GPU 则在物理层上完成了对图像的渲染。

最终，GPU 通过 Frame Buffer、视频控制器等相关部件，将图像显示在屏幕上。

对上述步骤进行串联，它们执行所消耗的时间远远超过 16.67 ms，因此为了满足对屏幕的 60 FPS 刷新率的支持，需要将这些步骤进行分解，通过流水线的方式进行并行执行，如下图所示。

![img](https://pic1.zhimg.com/80/v2-3a9da6baa3cfb091ab559abad14f6da4_720w.webp)

**Commit Transaction**

在 Core Animation 流水线中，app 调用Render Server前的最后一步 Commit Transaction 其实可以细分为 4 个步骤：

Layout：主要进行视图构建，包括：LayoutSubviews方法的重载，addSubview:方法填充子视图等。

Display：视图绘制，这里仅仅是绘制寄宿图，该过程使用CPU和内存

Prepare：阶段属于附加步骤，一般处理图像的解码和转换等操作

Commit：主要将图层进行打包，并将它们发送至 Render Server。该过程会递归执行，因为图层和视图都是以树形结构存在。

原文https://zhuanlan.zhihu.com/p/157556221