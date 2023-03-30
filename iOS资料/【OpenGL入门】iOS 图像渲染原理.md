# 【OpenGL入门】iOS 图像渲染原理

- CPU（Central Processing Unit）：现代计算机的三大核心部分之一，作为整个系统的运算和控制单元。CPU 内部的流水线结构使其拥有一定程度的并行计算能力。
- GPU（Graphics Processing Unit）：一种可进行绘图运算工作的专用微处理器。GPU 能够生成 2D/3D 的图形图像和视频，从而能够支持基于窗口的操作系统、图形用户界面、视频游戏、可视化图像应用和视频播放。GPU 具有非常强的并行计算能力。

使用 GPU 渲染图形的根本原因就是速度问题。GPU 优秀的并行计算能力使其能够快速将图形结果计算出来并在屏幕的所有像素中进行显示。

**屏幕图像的显示原理**

介绍屏幕图像显示的原理，需要先从 CRT 显示器原理说起，如下图所示。CRT 的电子枪从上到下逐行扫描，扫描完成后显示器就呈现一帧画面。然后电子枪回到初始位置进行下一次扫描。为了同步显示器的显示过程和系统的显示控制器，显示器会用硬件时钟产生一系列的定时信号。当电子枪换行进行扫描时，显示器会发出一个水平同步信号（horizonal synchronization），简称 HSync；而当一帧画面绘制完成后，电子枪回复到原位，准备画下一帧前，显示器会发出一个垂直同步信号（vertical synchronization），简称 VSync。显示器通常以固定频率进行刷新，这个刷新率就是 VSync 信号产生的频率。虽然现在的显示器基本都是液晶显示屏了，但其原理基本一致。

![img](https://pic1.zhimg.com/80/v2-c567191f1ddac6c7906a4ec6fe886730_720w.webp)

下图所示为常见的 CPU、GPU、显示器工作方式。CPU 计算好显示内容提交至 GPU，GPU 渲染完成后将渲染结果存入帧缓冲区，显示控制器会按照 `VSync` 信号逐帧读取帧缓冲区的数据，经过数据转换后最终由显示器进行显示。

![img](https://pic3.zhimg.com/80/v2-94d08b851b852471ea44957f77e2d27a_720w.webp)

**双缓冲机制**

所以，显示一个画面需要两步完成：

- CPU把需要显示的画面数据计算出来
- 显示器把这些数据显示出来

这两步工作都需要时间，并且可以并行执行，因为具体执行这两个过程的硬件是相互独立的（ `CPU/显卡` 和 `显示控制器` ）。但是这两个工作的耗时是不同的。 CPU 以及显卡每秒能计算出的画面数量是根据硬件性能决定的。 但是显示器每秒刷新频率是固定的（一般是 `60hz` ，所以每隔16.667ms就会刷新一次）。

由于存在两边速率不统一的问题，所以引入了 `帧缓冲区（FrameBuffer）` 的概念。

最简单的情况下，帧缓冲区只有一个。此时，帧缓冲区的读取和刷新都都会有比较大的效率问题。为了解决效率问题，GPU 通常会引入两个缓冲区，即 `双缓冲机制` 。在这种情况下，GPU 会预先渲染一帧放入一个缓冲区中，用于显示控制器的读取。当下一帧渲染完毕后，GPU 会直接把显示控制器的指针指向第二个缓冲器。

根据苹果的官方文档描述，iOS 设备会始终使用 `Vsync + Double Buffering` （垂直同步+双缓冲） 的策略。

![img](https://pic1.zhimg.com/80/v2-0a195e17869e5af5cfa61115d3df726c_720w.webp)

**屏幕撕裂**

双缓冲虽然能解决效率问题，但会引入一个新的问题。当显示控制器还未读取完成时，即屏幕内容刚显示一半时，GPU 将新的一帧内容提交到帧缓冲区并把两个缓冲区进行交换后，显示控制器就会把新的一帧数据的下半段显示到屏幕上，造成画面撕裂现象，如下图：

![img](https://pic3.zhimg.com/80/v2-40de91254067227770d4db83a84fc26e_720w.webp)

为了解决这个问题，GPU 通常有一个机制叫做 `垂直同步` （简写也是 `V-Sync` ），当开启 `垂直同步` 后，GPU 会等待显示器的 `VSync` 信号发出后，才进行新的一帧渲染和缓冲区更新。这样能解决画面撕裂现象，也增加了画面流畅度，但需要消费更多的计算资源，也会带来部分延迟。

![img](https://pic4.zhimg.com/80/v2-c6a164f9341ed757595d4b5cc941119f_720w.webp)

![img](https://pic3.zhimg.com/80/v2-06ea73b0aa1a4cdc964d1ef93138fa36_720w.webp)

**掉帧**

开启了 `垂直同步` 后，理想状况下 CPU 和 GPU 可以在16ms内处理完每一帧的渲染。但是如果显卡的帧率小于屏幕的刷新率，CPU 和 GPU 处理完一帧的渲染的时间超过了16ms，就会发生掉帧的情况。那一帧会被丢弃，等待下一次机会再显示，而这时显示屏会保留之前的内容不变。这就是界面卡顿的原因。

此时显示控制器占用一个 Buffer ，GPU 占用一个 Buffer 。两个Buffer都被占用，导致 CPU 空闲下来浪费了资源，因为垂直同步的原因，只有到了 `VSync` 信号的时间点， CPU 才能触发绘制工作。

![img](https://pic4.zhimg.com/80/v2-2a46ad2c01866efe376dfcce09f753e7_720w.webp)

**三缓冲机制**

在Android4.1系统开始，引入了 `三缓冲+垂直同步` 的机制。由于多加了一个 Buffer，实现了 CPU 跟 GPU 并行，便可以做到了只在开始掉一帧，后续却不掉帧， `双缓冲` 充分利用16ms做到低延时， `三缓冲` 保障了其稳定性。

![img](https://pic4.zhimg.com/80/v2-e0f42cfb324b793488341d9913406897_720w.webp)

**iOS的渲染框架**

iOS App 的图形渲染使用了 `Core Graphics` 、 `Core Animation` 、 `Core Image` 等框架来绘制可视化内容，这些软件框架相互之间也有着依赖关系。这些框架都需要通过 `OpenGL` 来调用 GPU 进行绘制，最终将内容显示到屏幕之上。

![img](https://pic2.zhimg.com/80/v2-6e6254a99fca6d11733da229c87f35c5_720w.webp)

**UIKit**

`UIKit` 是 iOS 开发者最常用的框架，可以通过设置 `UIKit` 组件的布局以及相关属性来绘制界面。

事实上， `UIKit` 自身并不具备在屏幕成像的能力，其主要负责对用户操作事件的响应（ `UIView` 继承自 `UIResponder` ），事件响应的传递大体是经过逐层的 `视图树` 遍历实现的。

**Core Animation**

`Core Animation` 源自于 `Layer Kit` ，动画只是 `Core Animation` 特性的冰山一角。

`Core Animation` 是一个复合引擎，其职责是 尽可能快地组合屏幕上不同的可视内容，这些可视内容可被分解成独立的图层（即 `CALayer` ），这些图层会被存储在一个叫做 `图层树` 的体系之中。从本质上而言， `CALayer` 是用户所能在屏幕上看见的一切的基础。

**Core Graphics**

`Core Graphics` 基于 `Quartz` 高级绘图引擎，主要用于运行时绘制图像。开发者可以使用此框架来处理基于路径的绘图，转换，颜色管理，离屏渲染，图案，渐变和阴影，图像数据管理，图像创建和图像遮罩以及 PDF 文档创建，显示和分析。

当开发者需要在 `运行时创建图像` 时，可以使用 `Core Graphics` 去绘制。与之相对的是 `运行前创建图像` ，例如用 Photoshop 提前做好图片素材直接导入应用。相比之下，我们更需要 `Core Graphics` 去在运行时实时计算、绘制一系列图像帧来实现动画。

**Core Image**

`Core Image` 与 `Core Graphics` 恰恰相反， `Core Graphics` 用于在 `运行时创建图像` ，而 `Core Image` 是用来处理 `运行前创建的图像` 的。 `Core Image` 框架拥有一系列现成的图像过滤器，能对已存在的图像进行高效的处理。

大部分情况下， `Core Image` 会在 GPU 中完成工作，但如果 GPU 忙，会使用 CPU 进行处理。

**OpenGL ES**

`OpenGL ES（OpenGL for Embedded Systems，简称 GLES）` ，是 `OpenGL` 的子集。 `OpenGL` 是一套第三方标准，函数的内部实现由对应的 GPU 厂商开发实现。

**Metal**

`Metal` 类似于 `OpenGL ES` ，也是一套第三方标准，具体实现由苹果实现。大多数开发者都没有直接使用过 `Metal` ，但其实所有开发者都在间接地使用 `Metal` 。 `Core Animation` 、 `Core Image` 、 `SceneKit` 、 `SpriteKit` 等等渲染框架都是构建于 `Metal` 之上的。

当在真机上调试 `OpenGL` 程序时，控制台会打印出启用 `Metal` 的日志。根据这一点可以猜测， `Apple` 已经实现了一套机制将 `OpenGL` 命令无缝桥接到 `Metal` 上，由 `Metal` 担任真正于硬件交互的工作。

**UIView 与 CALayer**

`CALayer` 是用户所能在屏幕上看见的一切的基础，用来存放 `位图（Bitmap）` 。 `UIKit` 中的每一个 UI 视图控件（ `UIView` ）其实内部都有一个关联的 `CALayer` ，即 `backing layer` 。

由于这种一一对应的关系，视图（ `UIView` ）层级拥有 `视图树` 的树形结构，对应 `CALayer` 层级也拥有 `图层树` 的树形结构。

视图（ `UIView` ）的职责是 `创建并管理` 图层，以确保当子视图在层级关系中 `添加或被移除` 时，其关联的图层在图层树中也有相同的操作，即保证视图树和图层树在结构上的一致性。

那么为什么 iOS 要基于 UIView 和 CALayer 提供两个平行的层级关系呢？

其原因在于要做 职责分离，这样也能避免很多重复代码。在 `iOS` 和 `Mac OS X` 两个平台上，事件和用户交互有很多地方的不同，基于多点触控的用户界面和基于鼠标键盘的交互有着本质的区别，这就是为什么 `iOS` 有 `UIKit` 和 `UIView` ，对应 `Mac OS X` 有 `AppKit` 和 `NSView` 的原因。它们在功能上很相似，但是在实现上有着显著的区别。

**CALayer**

在 `CALayer.h` 中， `CALayer` 有这样一个属性 `contents`

```text
/** Layer content properties and methods. **/

/* An object providing the contents of the layer, typically a CGImageRef,
 * but may be something else. (For example, NSImage objects are
 * supported on Mac OS X 10.6 and later.) Default value is nil.
 * Animatable. */

@property(nullable, strong) id contents;
复制代码
```

`contents` 提供了 layer 的内容，是一个指针类型，在 `iOS` 中的类型就是 `CGImageRef` （在 `OS X` 中还可以是 `NSImage` ）。 `CALayer` 中的 `contents` 属性保存了由设备渲染流水线渲染好的位图 `bitmap` （通常也被称为 `backing store` ），而当设备屏幕进行刷新时，会从 `CALayer` 中读取生成好的 `bitmap` ，进而呈现到屏幕上。

图形渲染流水线支持从顶点开始进行绘制（在流水线中，顶点会被处理生成 `纹理` ），也支持直接使用 `纹理（图片）` 进行渲染。相应地，在实际开发中，绘制界面也有两种方式：一种是 `手动绘制` ；另一种是 `使用图片` 。

- 使用图片： **contents image**
- 手动绘制： **custom drawing**

Contents Image `Contents Image` 是指通过 `CALayer` 的 `contents` 属性来配置图片。然而， `contents` 属性的类型为 `id` 。在这种情况下，可以给 `contents` 属性赋予任何值，app 仍可以编译通过。但是在实践中，如果 `content` 的值不是 `CGImage` ，得到的图层将是空白的。

本质上， `contents` 属性指向的一块缓存区域，称为 `backing store` ，可以存放 `bitmap` 数据。

Custom Drawing `Custom Drawing` 是指使用 `Core Graphics` 直接绘制 `寄宿图` 。实际开发中，一般通过继承 `UIView` 并实现 `-drawRect:` 方法来自定义绘制。

虽然 `-drawRect:` 是一个 `UIView` 方法，但事实上都是底层的 `CALayer` 完成了重绘工作并保存了产生的图片。下图所示为 `-drawRect:` 绘制定义 `寄宿图` 的基本原理。

![img](https://pic2.zhimg.com/80/v2-d4d6661fa6f681255a6640870249bfb9_720w.webp)

- `UIView` 有一个关联图层，即 `CALayer` 。
- `CALayer` 有一个可选的 `delegate` 属性，实现了 `CALayerDelegate` 协议。 `UIView` 作为 `CALayer` 的代理实现了 `CALayerDelegae` 协议。
- 当需要重绘时，即调用 `-drawRect:` ， `CALayer` 请求其代理给予一个寄宿图来显示。
- `CALayer` 首先会尝试调用 `-displayLayer:` 方法，此时代理可以直接设置 `contents` 属性。

```text
- (void)displayLayer:(CALayer *)layer;
复制代码
```

- 如果代理没有实现 `-displayLayer:` 方法， `CALayer` 则会尝试调用 `-drawLayer:inContext:` 方法。在调用该方法前， `CALayer` 会创建一个空的寄宿图（尺寸由 `bounds` 和 `contentScale` 决定）和一个 `Core Graphics` 的绘制上下文，为绘制寄宿图做准备，作为 `context` 参数传入。

```text
- (void)drawLayer:(CALayer *)layer inContext:(CGContextRef)ctx;
复制代码
```

- 最后，由 `Core Graphics` 绘制生成的寄宿图会存入 `backing store` 。

**UIView**

`UIView` 是 app 中的基本组成结构，定义了一些统一的规范。它会负责内容的渲染以及，处理交互事件。

- Drawing and animation：绘制与动画
- Layout and subview management：布局与子 view 的管理
- Event handling：点击事件处理

`CALayer` 是 `UIView` 的属性之一，负责渲染和动画，提供可视内容的呈现。 `UIView` 提供了对 `CALayer` 部分功能的封装，同时也另外负责了交互事件的处理。

- 相同的层级结构：我们对 `UIView` 的层级结构非常熟悉，由于每个 `UIView` 都对应 `CALayer` 负责页面的绘制，所以 `CALayer` 也具有相应的层级结构。
- 部分效果的设置：因为 `UIView` 只对 `CALayer` 的部分功能进行了封装，而另一部分如圆角、阴影、边框等特效都需要通过调用 layer 属性来设置。
- 是否响应点击事件： `CALayer` 不负责点击事件，所以不响应点击事件，而 `UIView` 会响应。
- 不同继承关系： `CALayer` 继承自 `NSObject` ， `UIView` 由于要负责交互事件，所以继承自 `UIResponder` 。

**Core Animation**

**Core Animation 流水线**

事实上，app 本身并不负责渲染，渲染则是由一个独立的进程负责，即 Render Server 进程。

![img](https://pic3.zhimg.com/80/v2-b856501368b5232d47c4280947db8b7a_720w.webp)

App 通过 IPC 将渲染任务及相关数据提交给 `Render Server` 。 `Render Server` 处理完数据后，再传递至 GPU。最后由 GPU 调用 iOS 的图像设备进行显示。

- 首先，由 app 处理事件（Handle Events），如：用户的点击操作，在此过程中 app 可能需要更新 视图树，相应地，图层树 也会被更新。
- 其次，app 通过 CPU 完成对显示内容的计算，如：视图的创建、布局计算、图片解码、文本绘制等。在完成对显示内容的计算之后，app 对图层进行打包，并在下一次 `RunLoop` 时将其发送至 `Render Server` ，即完成了一次 `Commit Transaction` 操作。
- `Render Server` 主要执行 `Open GL` 、 `Core Graphics` 相关程序，并调用 GPU。
- GPU 则在物理层上完成了对图像的渲染。
- 最终，GPU 通过 `Frame Buffer` 、 `视频控制器` 等相关部件，将图像显示在屏幕上。

对上述步骤进行串联，它们执行所消耗的时间远远超过 16.67 ms，因此为了满足对屏幕的 60 FPS 刷新率的支持，需要将这些步骤进行分解，通过流水线的方式进行并行执行，如下图所示。

![img](https://pic1.zhimg.com/80/v2-522c0975e22523eb16b024a4204053a0_720w.webp)

**图层树**

`CoreAnimation` 作为一个复合引擎，将不同的视图层组合在屏幕中，并且存储在 `图层树` 中，向我们展示了所有屏幕上的一切。

整个过程其实经历了三个树状结构，才显示到了屏幕上： `模型树-->呈现树-->渲染树`

层级关系树中除了 `视图树` 和 `图层树` ，还有 `呈现树` 和 `渲染树` 。他们各自都有各自的职责。

- **呈现树** ：我们可以通过 `CALayer` 的 `-presentationLayer` 方法来访问对应的呈现树图层。注意呈现图层仅仅当图层首次被提交（就是首次第一次在屏幕上显示）的时候创建，所以在那之前调用 `-presentationLayer` 将会返回nil。

```text
- (nullable instancetype)presentationLayer;
复制代码
```

- **模型树** ：在呈现图层上调用 `–modelLayer` 将会返回它正在呈现所依赖的 `CALayer` 。通常在一个图层上调用 `-modelLayer` 会返回 `self` （实际上我们已经创建的原始图层就是一种数据模型）。

```text
- (instancetype)modelLayer;
复制代码
```

通常，我们操作的是模型树 `modelLayer` ，在重绘周期最后，我们会将模型树相关内容（层次结构、图层属性和动画）序列化，通过IPC传递给专门负责屏幕渲染的渲染进程。渲染进程拿到数据并反序列化出树状结构--呈现树。这个呈现图层实际上是模型图层的复制，但是它的属性值代表了在任何指定时刻当前外观效果。换句话说，可以通过呈现图层的值来获取当前屏幕上真正显示出来的值。

当模型树 `modelLayer` 上带有动画特征时，提交到渲染进程后，渲染进程会根据动画特征，不断修改呈现树 `presentationLayer` 上的图层属性，并同时不断的在屏幕上渲染出来，这样我们就看到了动画。

如果想让动画的图层响应用户输入，可以使用 `-hitTest:` 方法来判断指定图层是否被触摸，这时候对呈现图层而不是模型图层调用 `-hitTest:` 会显得更有意义，因为呈现图层代表了用户当前看到的图层位置，而不是当前动画结束之后的位置。

可以理解为 `modelLayer` 负责数据的存储和获取， `presentationLayer` 负责显示。每次屏幕刷新的时候， `presentationLayer` 会与 `modelLayer` 状态同步。

当 `CAAnimation` 加到layer上之后， `presentationLayer` 每次刷新的时候会去 `CAAnimation` 询问并同步状态， `CAAnimation` 控制 `presentationLayer` 从 `fromValue` 到 `toValue` 来改变值，而动画结束之后， `CAAnimation` 会从layer上被移除，此时屏幕刷新的时候 `presentationLayer` 又会同步 `modelLayer` 的状态， `modelLayer` 没有改变，所以又回到了起点。当然我们可以通过设置，继续影响 `presentationLayer` 的状态。

**Core Animation 动画**

`Core Animation` 动画，即基于事务的动画，是最常见的动画实现方式。动画执行者是专门负责渲染的渲染进程，操作的是呈现树。我们应该尽量使用 `Core Animation` 来控制动画，因为 `Core Animation` 是充分优化过的：

基于 `Layer` 的绘图过程中， `Core Animation` 通过硬件操作位图（变换、组合等），产生动画的速度比软件操作的方式快很多。

基于 `View` 的绘图过程中， `view` 被改动时会触发的 `drawRect:` 方法来重新绘制位图，但是这种方式需要CPU在主线程执行，比较耗时。而 `Core Animation` 则尽可能的操作硬件中已缓存的位图，来实现相同的效果，从而减少了资源损耗。

**非 Core Animation 动画**

非 `CoreA nimation` 动画执行者是当前进程，操作的是模型树。常见的有定时器动画和手势动画。定时器动画是在定时周期触发时修改模型树的图层属性；手势动画是手势事件触发时修改模型树的图层属性。两者都能达到视图随着时间不断变化的效果，即实现了动画。

非 `Core Animation` 动画动画过程中实际上不断改动的是模型树，而呈现树仅仅成了模型树的复制品，状态与模型树保持一致。整个过程中，主要是CPU在主线程不断调整图层属性、布局计算、提交数据，没有充分利用到 `Core Animation` 强大的动画控制功能。

原文https://zhuanlan.zhihu.com/p/307909741