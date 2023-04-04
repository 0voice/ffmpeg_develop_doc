# iOS图像渲染及卡顿问题优化

## 1.基本知识

下面来看下GPU和CPU的基本概念：

- **CPU（Central Processing Unit）**：系统的运算和控制单元，是信息处理、程序执行的最终执行单元。CPU内部结构是具有一定程度的并行计算能力。CPU的主要功效是：处理指令、执行操作、控制时间、处理数据。
- **GPU（Graphics Processing Unit）**：进行绘图工作的处理器，GPU可以生成2D/3D图形图像和视频，同时GPU具有超强的并行计算能力。GPU使显卡减少了对CPU的依赖，并进行部分原本CPU的工作，尤其是在3D图形处理时GPU所采用的核心技术有硬件T&L（几何转换和光照处理）、立方环境材质贴图和顶点混合、纹理压缩和凹凸映射贴图、双重纹理四像素256位渲染引擎等，其中GPU的生产商主要有NVIDIA和ATI。

## 2.CPU-GPU工作流

### 2.1工作流

当CPU遇到图像处理时，会调用GPU进行处理，主要流程可以分为如下四步：

1. 将主存的处理数据复制到显存中
2. CPU指令驱动GPU
3. GPU中每个运算单元并行处理
4. GPU将显存结果传回主存

![img](https://p9-juejin.byteimg.com/tos-cn-i-k3u1fbpfcp/5057f71d1c1d4ce68cc8fd799da1730c~tplv-k3u1fbpfcp-zoom-in-crop-mark:4536:0:0:0.awebp)

### 2.2屏幕成像显示原理

如果要研究图片显示原理，需要先从 CRT 显示器原理说起，如下经典图所示。CRT 的电子枪从上到下逐行扫描，扫描完成后显示器就呈现一帧画面。然后电子枪回到初始位置进行下一次扫描。为了同步显示器的显示过程和系统的视频控制器，显示器会用硬件时钟产生一系列的定时信号。当电子枪换行进行扫描时，显示器会发出一个水平同步信号（horizonal synchronization），简称 HSync；而当一帧画面绘制完成后，电子枪回复到原位，准备画下一帧前，显示器会发出一个垂直同步信号（vertical synchronization），简称 VSync。显示器通常以固定频率进行刷新，这个刷新率就是 VSync 信号产生的频率。虽然现在的显示器基本都是液晶显示屏了，但其原理基本一致。

```
拓展：CRT显示器学名为“阴极射线显像管”，是一种使用阴极射线管（Cathode Ray Tube）的显示器。
复制代码
```

![img](https://p9-juejin.byteimg.com/tos-cn-i-k3u1fbpfcp/1db4a9f9d1ee45bdbde9a3a1bba6f9fc~tplv-k3u1fbpfcp-zoom-in-crop-mark:4536:0:0:0.awebp)

下图CPU、GPU、显示器工作方式。CPU计算好显示内容提交到GPU，GPU渲染完成后将渲染结果存入到帧缓冲区，视频控制器会按照VSync信号逐帧读取帧缓冲区的数据，经过数据转换后最终由显示器进行显示。

![img](https://p9-juejin.byteimg.com/tos-cn-i-k3u1fbpfcp/16e9f44698a948ddb40c47f884a0009e~tplv-k3u1fbpfcp-zoom-in-crop-mark:4536:0:0:0.awebp)

最简单的时候，帧缓冲区只有一个。此时，帧缓冲区和读取和刷新都会有比较大的效率问题。为了解决效率问题，GPU通常会引入两个缓冲区，即**双缓冲机制**，即这种情况下，GPU会预先渲染一帧放入缓冲区中，用于视频控制器的读取，当下一帧渲染完毕后，GPU会直接把视频控制器的指针指向第二个缓冲区。

双缓冲虽然能解决效率问题，但会引入一个新的问题。当视频控制器还未读取完成时，即屏幕内容刚显示一半时，GPU 将新的一帧内容提交到帧缓冲区并把两个缓冲区进行交换后，视频控制器就会把新的一帧数据的下半段显示到屏幕上，造成画面撕裂现象，如下图

![img](https://p1-juejin.byteimg.com/tos-cn-i-k3u1fbpfcp/9bd1da7ed73242baadf375a126154331~tplv-k3u1fbpfcp-zoom-in-crop-mark:4536:0:0:0.awebp)

为了解决这个问题，GPU 通常有一个机制叫做垂直同步（简写也是 V-Sync），当开启垂直同步后，GPU 会等待显示器的 VSync 信号发出后，才进行新的一帧渲染和缓冲区更新。这样能解决画面撕裂现象，也增加了画面流畅度，但需要消费更多的计算资源，也会带来部分延迟。

## 3.iOS 渲染框架

iOS为开发者提供了丰富的Framework（UIKit，Core Animation，Core Graphic，OpenGL 等等）来满足开发从上到底层各种需求，下面是iOS渲染视图框架图:

![img](https://p3-juejin.byteimg.com/tos-cn-i-k3u1fbpfcp/5aade2b4426e42b1b868b28baa847748~tplv-k3u1fbpfcp-zoom-in-crop-mark:4536:0:0:0.awebp)

可以看出iOS渲染视图的核心是 Core Animation。从底层到上层依此是 GPU->(OpenGL、Core Graphic) -> Core Animation -> UIKit。

UIKit

UIKit是iOS开发者最常用的框架，通过设置UIKit的控件来实现绘制界面，其实UIKit自身不具备屏幕成像的能力，它的主要职责是对用户操作事件的响应【继承自UIResponder】。

Core Animation

Core Animation源自Layer kit，是一个复合引擎，职责是绘制不同的可视化内容，这些图层都是在图层树的体系之中，从本质上看：CALayer是用户所能在屏幕看见的一切的基础。

Core Graphics

Core Graphics是基于Quartz绘图引擎，主要用于运行时绘制图像。可以使用此框架来处理绘图，转换，离屛渲染，图像创建，和PDF文档创建以及显示和分析。

Core Image

Core Image与Core Graphics恰恰相反，Core Graphics用于运行时创建图像，而Core Image用于处理运行前创建的图像。

大部分情况下，Core Image会在GPU中完成工作，如果GPU忙，会使用CPU进行处理。

OpenGL  ES

`OpenGL ES`是`OpenGL`的子集，函数的内部实现是由厂家GPU开发实现。

Metal

苹果自己推出的图形图像处理框架。Metal类似于OpenGL ES ,也是一套第三方标准,具体实现是由苹果实现.相信大多数开发者没有直接使用过Metal, 但其实所有开发者在间接地使用Metal, Core Animation, Core Image, SceneKit, SpriteKit等等渲染框架都是在构建在Metal之上.

## 4.Core Animation流水线

介绍一下Core Animation工作原理如下: 

![img](https://p6-juejin.byteimg.com/tos-cn-i-k3u1fbpfcp/c4737154047f48ba8abee53c54fa9cd3~tplv-k3u1fbpfcp-zoom-in-crop-mark:4536:0:0:0.awebp)

事实上, APP本身并不负责渲染, 渲染会交给一个独立的进程负责, 即Render Sever进程. 

APP通过IPC将渲染任务及相关数据提交给Render Server. Render Server处理完数据之后,再传至GPU,最后由GPU调用iOS的图像设备进行显示.

Core Animation 流水线的详细过程:

1. 首先,由App处理事件, 如点击操作, 在此过程中app可能需要更新视图树, 相应地,图层也会发生被更新
2. 其次, App通过CPU完成对显示内容的更新, 如: 视图的创建、布局计算, 图片解码,文本绘制等, 在完成对显示内容的计算之后, app会对图层打包, 并在下一次Runloop时将其发送至Render Server, 即完成了一次Commit Transaction操作
3. Render Server主要执行OpenGL, Core Graphics相关程序, 并调用GPU
4. GPU在物理层上完成对图像的渲染
5. GPU通过Frame Buffer,视频控制器等相关部件, 将图像显示在屏幕上.

它们执行所消耗的时间远远超过 16.67 ms，因此为了满足对屏幕的 60 FPS 刷新率的支持，需要将这些步骤进行分解，通过流水线的方式进行并行执行，如下图所示。



![img](https://p6-juejin.byteimg.com/tos-cn-i-k3u1fbpfcp/16cff7985e9a4a679cecacc094d86659~tplv-k3u1fbpfcp-zoom-in-crop-mark:4536:0:0:0.awebp)

### 4.1Commit Transaction

Commit Transaction ,App调用Render Server 前最后一步Commit Transaction其实可以细分4个步骤:

1. Layout
2. Display
3. Prepare
4. Commit

**Layout**

Layout 阶段主要进行视图构建, 包括LayoutSubviews方法的重载, addSubview添加子视图

**Display**

Display主要进行视图绘制, drawRect方法可以自定义UIView的现实,其原理是drawRect方法内部绘制寄宿图,过程使用到了CPU和内存

**Prepare**

Prepare阶段属于附加步骤,一般处理图像的解码和转换操作

**Commit**

Commit 用于对图层进行打包, 将它们发送至Render Server,会递归进行,因为图层和视图都是以树形结构存在

### 4.2动画渲染原理

iOS 动画的渲染也是基于上述 Core Animation 流水线完成的。这里我们重点关注 app 与 `Render Server` 的执行流程。

如果不是特别复杂的动画，一般使用 `UIView` Animation 实现，iOS 将其处理过程分为如下三部阶段：

- Step 1：调用 `animationWithDuration:animations:` 方法
- Step 2：在 Animation Block 中进行 `Layout`，`Display`，`Prepare`，`Commit` 等步骤。
- Step 3：`Render Server` 根据 Animation 逐帧进行渲染。

![img](https://p9-juejin.byteimg.com/tos-cn-i-k3u1fbpfcp/d50822d474084adda2152c67078f33cf~tplv-k3u1fbpfcp-zoom-in-crop-mark:4536:0:0:0.awebp)

## 5.卡顿原因和解决方案

### 5.1卡顿原理

FPS (Frames Per Second) 表示每秒渲染帧数，通常用于衡量画面的流畅度，每秒帧数越多，则表示画面越流畅，60fps 最佳，一般我们的APP的FPS 只要保持在 50-60之间，用户体验都是比较流畅的。

![img](https://p9-juejin.byteimg.com/tos-cn-i-k3u1fbpfcp/c5223e72e1494a6db6864802460590e7~tplv-k3u1fbpfcp-zoom-in-crop-mark:4536:0:0:0.awebp)

在 VSync 信号到来后，系统图形服务会通过 CADisplayLink 等机制通知 App，App 主线程开始在 CPU 中计算显示内容，比如视图的创建、布局计算、图片解码、文本绘制等。随后 CPU 会将计算好的内容提交到 GPU 去，由 GPU 进行变换、合成、渲染。随后 GPU 会把渲染结果提交到帧缓冲区去，等待下一次 VSync 信号到来时显示到屏幕上。由于垂直同步的机制，如果在一个 VSync 时间内，CPU 或者 GPU 没有完成内容提交，则那一帧就会被丢弃，等待下一次机会再显示，而这时显示屏会保留之前的内容不变。这就是界面卡顿的原因。

从上面的图中可以看到，CPU 和 GPU 不论哪个阻碍了显示流程，都会造成掉帧现象。所以开发时，也需要分别对 CPU 和 GPU 压力进行评估和优化。

### 5.2CPU优化

**1. 布局计算**

视图布局的计算是APP最消耗CPU资源的地方，如果在后台线程提前计算好视图布局，并且对视图布局进行缓存，这样就可以解决性能问题啦！一次性调整好对应属性，而不要多次、频繁的计算和调整控件的frame/bounds/center属性。

**2. 文本计算**

如果一个界面中包含大量文本（比如微博微信朋友圈等），文本的宽高计算会占用很大一部分资源，并且不可避免。如果你对文本显示没有特殊要求，可以参考下 UILabel 内部的实现方式：用 [NSAttributedString boundingRectWithSize:options:context:] 来计算文本宽高，用 -[NSAttributedString drawWithRect:options:context:] 来绘制文本。尽管这两个方法性能不错，但仍旧需要放到后台线程进行以避免阻塞主线程。

**3. 图片的绘制**

图像的绘制通常是指用那些以 CG 开头的方法把图像绘制到画布中，然后从画布创建图片并显示这样一个过程。这个最常见的地方就是 [UIView drawRect:] 里面了。由于 CoreGraphic 方法通常都是线程安全的，所以图像的绘制可以很容易的放到后台线程进行。一个简单异步绘制的过程大致如下（实际情况会比这个复杂得多，但原理基本一致）：

```ini
- (void)display {
    dispatch_async(backgroundQueue, ^{
        CGContextRef ctx = CGBitmapContextCreate(...);
        // draw in context...
        CGImageRef img = CGBitmapContextCreateImage(ctx);
        CFRelease(ctx);
        dispatch_async(mainQueue, ^{
            layer.contents = img;
        });
    });
}
复制代码
```

**4. 对象创建**

对象的创建会分配内存、调整属性、甚至还有读取文件等操作，比较消耗 CPU 资源。尽量用轻量的对象代替重量的对象，可以对性能有所优化。比如 CALayer 比 UIView 要轻量许多，那么不需要响应触摸事件的控件，用 CALayer 显示会更加合适。如果对象不涉及 UI 操作，则尽量放到后台线程去创建，但可惜的是包含有 CALayer 的控件，都只能在主线程创建和操作。通过 Storyboard 创建视图对象时，其资源消耗会比直接通过代码创建对象要大非常多，在性能敏感的界面里，Storyboard 并不是一个好的技术选择。

尽量推迟对象创建的时间，并把对象的创建分散到多个任务中去。尽管这实现起来比较麻烦，并且带来的优势并不多，但如果有能力做，还是要尽量尝试一下。如果对象可以复用，并且复用的代价比释放、创建新对象要小，那么这类对象应当尽量放到一个缓存池里复用。

### 5.3GPU优化

相对于 CPU 来说，GPU 能干的事情比较单一：接收提交的纹理（Texture）和顶点描述（三角形），应用变换（transform）、混合并渲染，然后输出到屏幕上。通常你所能看到的内容，主要也就是纹理（图片）和形状（三角模拟的矢量图形）两类。

**1. 纹理的渲染**

所有的Bitmap，包括图片，栅格化等的内容，最终要由内存提交到显存里面，不论是提交到显存的过程，还是渲染Texture过程都是消耗了不少的GPU。当在较短时间显示大量图片时（比如 TableView 存在非常多的图片并且快速滑动时），CPU 占用率很低，GPU 占用非常高，界面仍然会掉帧。避免这种情况的方法只能是尽量减少在短时间内大量图片的显示，尽可能将多张图片合成为一张进行显示。

**2. 视图混合**

当多个视图（或者说 CALayer）重叠在一起显示时，GPU 会首先把他们混合到一起。如果视图结构过于复杂，混合的过程也会消耗很多 GPU 资源。为了减轻这种情况的 GPU 消耗，应用应当尽量减少视图数量和层次，并在不透明的视图里标明 opaque 属性以避免无用的 Alpha 通道合成。当然，这也可以用上面的方法，把多个视图预先渲染为一张图片来显示。

**3. 图形生成**

CALayer的border、圆角、阴影以及遮罩，CASharpLayer的矢量图形显示，这样通常会造成离屏渲染，而离屏渲染通常会发生在GPU中，当一个列表有大量的圆角时候，并且快速欢动，GPU资源已经占满，而CPU资源消耗较少。

最彻底的解法是：把需要显示的图形在后台线程绘制为图片，避免使用圆角、阴影、遮罩属性等。

对于如何去监控卡顿,通过Runloop机制,可以参考掘金里面有很多文章,都是大同小异,在这就不做叙述啦!!!


原文链接：https://juejin.cn/post/6874046143160909838