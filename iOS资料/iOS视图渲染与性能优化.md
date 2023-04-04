# iOS视图渲染与性能优化

## 1、视图渲染

视图渲染的处理层级图如下：
 ![img](https://p3-juejin.byteimg.com/tos-cn-i-k3u1fbpfcp/34ae35cf609343fc96cc102b64c579de~tplv-k3u1fbpfcp-zoom-in-crop-mark:4536:0:0:0.awebp)
 UIKit是常用的框架，显示、动画都通过CoreAnimation；
 CoreAnimation是核心动画，依赖于OpenGL ES做GPU渲染（目前最新的iPhone已经都使用Metal，**为了和图文一致，本文后面继续使用OpenGL ES来描述**），CoreGraphics做CPU渲染；
 最底层的GraphicsHardWare是图形硬件。

视图渲染的整体流程如下： ![img](https://p3-juejin.byteimg.com/tos-cn-i-k3u1fbpfcp/3f7686424cb748159594d73a3c967e45~tplv-k3u1fbpfcp-zoom-in-crop-mark:4536:0:0:0.awebp)
 视图渲染到屏幕上需要CPU和GPU一起协作。App将一部分数据通过CoreGraphics、CoreImage调用CPU进行预处理，最终通过OpenGL ES将数据传送到 GPU，最终显示到屏幕。

## 2、渲染过程

![img](https://p3-juejin.byteimg.com/tos-cn-i-k3u1fbpfcp/90c29bf8d22e466087f14d04d484b05f~tplv-k3u1fbpfcp-zoom-in-crop-mark:4536:0:0:0.awebp)
 渲染的具体过程可以用上图来描述：

- 1、CoreAnimation提交会话，包括自己和子树（view hierarchy）的layout状态等；
- 2、RenderServer解析提交的子树状态，生成绘制指令；
- 3、GPU执行绘制指令；
- 4、显示渲染后的数据；

其中App的Commit流程又可以分为Layout、Display、Prepare、Commit四个步骤。

![img](https://p3-juejin.byteimg.com/tos-cn-i-k3u1fbpfcp/c53940fec0624136a53d36d4e2688eee~tplv-k3u1fbpfcp-zoom-in-crop-mark:4536:0:0:0.awebp)

### 2.1布局（Layout）

调用layoutSubviews方法；
 调用addSubview:方法；

> 会造成CPU和I/O瓶颈；

### 2.2显示（Display）

通过drawRect绘制视图；
 绘制string（字符串）；

> 会造成CPU和内存瓶颈；

每个UIView都有CALayer，同时图层有一个像素存储空间，存放视图；调用`-setNeedsDisplay`的时候，仅会设置图层为dirty。
 当渲染系统准备就绪时会调用视图的`-display`方法，同时装配像素存储空间，建立一个CoreGraphics上下文（CGContextRef），将上下文push进上下文堆栈，绘图程序进入对应的内存存储空间。

```ini
UIBezierPath *path = [UIBezierPath bezierPath];
[path moveToPoint:CGPointMake(10, 10)];
[path addLineToPoint:CGPointMake(20, 20)];
[path closePath];
path.lineWidth = 1;
[[UIColor redColor] setStroke];
[path stroke];
复制代码
```

在-drawRect方法中实现如上代码，UIKit会将自动生成的CGContextRef 放入上下文堆栈。
 当绘制完成后，视图的像素会被渲染到屏幕上；当下次再次调用视图的-setNeedsDisplay，将会再次调用-drawRect方法。

### 2.3准备提交（Prepare）

解码图片；
 图片格式转换；

> 当我们使用UIImage、CGImage时，图片并没有真正解码。iOS会先用一些基础的图像信息创建对象，等到真正使用时再创建bitmap并进行解码。尽量避免使用不支持硬解的图片格式，比如说webp；

### 2.4提交（Commit）

打包layers并发送到渲染server；
 递归提交子树的layers；
 如果子树太复杂，会消耗很大，对性能造成影响；

> 尽可能简化viewTree；

当显示一个UIImageView时，Core Animation会创建一个OpenGL ES纹理，并确保在这个图层中的位图被上传到对应的纹理中。当你重写` -drawInContext`方法时，Core Animation会请求分配一个纹理，同时确保Core Graphics会将你在`-drawInContext`中绘制的东西放入到纹理的位图数据中。

## 3、Tile-Based 渲染

Tiled-Based 渲染是移动设备的主流。整个屏幕会分解成N*Npixels组成的瓦片（Tiles），tiles存储于SoC 缓存（SoC=system on chip，片上系统，是在整块芯片上实现一个复杂系统功能，如intel cpu，整合了集显，内存控制器，cpu运核心，缓存，队列、非核心和I/O控制器）。 几何形状会分解成若干个tiles，对于每一块tile，把必须的几何体提交到OpenGL ES，然后进行渲染（光栅化）。完毕后，将tile的数据发送回cpu。

![img](https://p3-juejin.byteimg.com/tos-cn-i-k3u1fbpfcp/b4c94a4466f346d6a134588b514a0017~tplv-k3u1fbpfcp-zoom-in-crop-mark:4536:0:0:0.awebp)

> 传送数据是非常消耗性能的。相对来说，多次计算比多次发送数据更加经济高效，但是额外的计算也会产生一些性能损耗。
> PS：在移动平台控制帧率在一个合适的水平可以节省电能，会有效的延长电池寿命，同时会相对的提高用户体验。

### 3.1渲染流程

普通的Tile-Based渲染流程如下： ![img](https://p3-juejin.byteimg.com/tos-cn-i-k3u1fbpfcp/eaf4ae319bcc45bdb15e6f7baea7e479~tplv-k3u1fbpfcp-zoom-in-crop-mark:4536:0:0:0.awebp)
 1、CommandBuffer，接受OpenGL ES处理完毕的渲染指令；
 2、Tiler，调用顶点着色器，把顶点数据进行分块（Tiling）；
 3、ParameterBuffer，接受分块完毕的tile和对应的渲染参数；
 4、Renderer，调用片元着色器，进行像素渲染；
 5、RenderBuffer，存储渲染完毕的像素；

### 3.2离屏渲染 —— 遮罩（Mask）

![img](https://p3-juejin.byteimg.com/tos-cn-i-k3u1fbpfcp/a7a2f02c88674a9197ec4be63a6efe6d~tplv-k3u1fbpfcp-zoom-in-crop-mark:4536:0:0:0.awebp)
 1、渲染layer的mask纹理，同Tile-Based的基本渲染逻辑；
 2、渲染layer的content纹理，同Tile-Based的基本渲染逻辑；
 3、Compositing操作，合并1、2的纹理；

### 3.3离屏渲染 ——UIVisiualEffectView

![img](https://p3-juejin.byteimg.com/tos-cn-i-k3u1fbpfcp/0accc7bfd39143618c3461bb0b6eeccd~tplv-k3u1fbpfcp-zoom-in-crop-mark:4536:0:0:0.awebp) 使用UIBlurEffect，应该是尽可能小的view，因为性能消耗巨大。

60FPS的设备，每帧只有16.67ms的时间进行处理。 ![img](https://p3-juejin.byteimg.com/tos-cn-i-k3u1fbpfcp/1102fb148b3b4e96af06491d65fa069c~tplv-k3u1fbpfcp-zoom-in-crop-mark:4536:0:0:0.awebp)

### 3.4渲染等待

由于每一帧的顶点和像素处理相对独立，iOS会将CPU处理，顶点处理，像素处理安排在相邻的三帧中。如图，当一个渲染命令提交后，要在当帧之后的第三帧，渲染结果才会显示出来。
 ![img](https://p3-juejin.byteimg.com/tos-cn-i-k3u1fbpfcp/558ee835ce2e4b7fbe5656e097a3f0c8~tplv-k3u1fbpfcp-zoom-in-crop-mark:4536:0:0:0.awebp)

### 3.5光栅化

把视图的内容渲染成纹理并缓存，可以通过CALayer的shouldRasterize属性开启光栅化。 注意，光栅化的元素，总大小限制为2.5倍的屏幕。 更新内容时，会启用离屏渲染，所以更新代价较大，只能用于静态内容；而且如果光栅化的元素100ms没有被使用将被移除，故而不常用元素的光栅化并不会优化显示。

### 3.6组透明度

CALayer的allowsGroupOpacity属性，UIView 的alpha属性等同于 CALayer opacity属性。
 当`GroupOpacity=YES`时，会先不考虑透明度，等绘制完成所有layer（自身+子layers），再统一计算透明。
 假设某个视图A有一个字视图B，他们的alpha都是0.5（根视图是黑色，A和B都是白色），当我们绘制视图的时候：
 如果未开启组透明，首先是绘制视图A（0.5白色），然后再绘制视图B，绘制视图B的时候是在父视图0.5白色和根视图0.5黑色的基础上叠加视图B的0.5白色，最终就是0.75白色。 ![img](https://p9-juejin.byteimg.com/tos-cn-i-k3u1fbpfcp/29ef7e8ee11c4bc9a516f29c0a50e5f8~tplv-k3u1fbpfcp-zoom-in-crop-mark:4536:0:0:0.awebp)

如果开启了组透明，首先是绘制视图A（白色），然后在A的基础上直接绘制视图B（白色），最终再统一计算透明0.5，所以A和B的颜色保持一致。（边界是特意加的，为了区分视图B）
 ![img](https://p3-juejin.byteimg.com/tos-cn-i-k3u1fbpfcp/e45e3486092240b09aa71e0a88ee9a48~tplv-k3u1fbpfcp-zoom-in-crop-mark:4536:0:0:0.awebp)

> The default value is read from the boolean UIViewGroupOpacity property in the main bundle’s Info.plist file. If no value is found, the default value is YES for apps linked against the iOS 7 SDK or later and NO for apps linked against an earlier SDK.
> 为了让子视图与父视图保持同样的透明度和优化性能，从 iOS 7 以后默认全局开启了这个功能。对现在的开发者来说，几乎可以不用关注。

## 4、性能优化

这个是WWDC推荐的检查项目：
 ![img](https://p3-juejin.byteimg.com/tos-cn-i-k3u1fbpfcp/0f13020c1dce4d98978be2f50e99ee1b~tplv-k3u1fbpfcp-zoom-in-crop-mark:4536:0:0:0.awebp)

1、帧率一般在多少？

> 60帧每秒；（TimeProfiler工具可以查看耗时）

2、是否存在CPU和GPU瓶颈？  （查看占有率）

> 更少的使用CPU和GPU可以有效的保存电量；

3、是否额外使用CPU来进行渲染？

> 重写了drawRect会导致CPU渲染；在CPU进行渲染时，GPU大多数情况是处于等待状态；

4、是否存在过多离屏渲染？

> 越少越好；离屏渲染会导致上下文切换，GPU产生idle；

5、是否渲染过多视图？

> 视图越少越好；透明度为1的视图更受欢迎；

6、使用奇怪的图片格式和大小？

> 避免格式转换和调整图片大小；一个图片如果不被GPU支持，那么需要CPU来转换。（Xcode有对PNG图片进行特殊的算法优化）

7、是否使用昂贵的特效？

> 视图特效存在消耗，调整合适的大小；例如前面提到的UIBlurEffect；

8、是否视图树上存在不必要的元素？

> 理解视图树上所有点的必要性，去掉不必要的元素；忘记remove视图是很常见的事情，特别是当View的类比较大的时候。

![img](https://p3-juejin.byteimg.com/tos-cn-i-k3u1fbpfcp/de7725099d01477c91df8a402c07d328~tplv-k3u1fbpfcp-zoom-in-crop-mark:4536:0:0:0.awebp)
 以上，是8个问题对应的工具。遇到性能问题，先**分析、定位问题所在**，而不是埋头钻进代码的海洋。

![img](https://p3-juejin.byteimg.com/tos-cn-i-k3u1fbpfcp/ea5287bed6b0482d9f98d62021d51cf8~tplv-k3u1fbpfcp-zoom-in-crop-mark:4536:0:0:0.awebp)

## 5、性能优化实例

### 5.1阴影

![img](https://p3-juejin.byteimg.com/tos-cn-i-k3u1fbpfcp/8202dc6d12334d6292c2d4fc5f2924a6~tplv-k3u1fbpfcp-zoom-in-crop-mark:4536:0:0:0.awebp)
 上面的做法，会导致离屏渲染；下面的做法是正确的做法。

### 5.2圆角

![img](https://p3-juejin.byteimg.com/tos-cn-i-k3u1fbpfcp/f77b427dbea342a5bdd8e6824ff51c4f~tplv-k3u1fbpfcp-zoom-in-crop-mark:4536:0:0:0.awebp)
 不要使用不必要的mask，可以预处理图片为圆形；或者添加中间为圆形透明的白色背景视图。即使添加额外的视图，会导致额外的计算；但仍然会快一点，因为相对于切换上下文，GPU更擅长渲染。
 离屏渲染会导致GPU利用率不到100%，帧率却很低。（切换上下文会产生idle time）

### 5.3工具

使用instruments的CoreAnimation工具来检查离屏渲染，黄色是我们不希望看到的颜色。
 ![img](https://p3-juejin.byteimg.com/tos-cn-i-k3u1fbpfcp/4deff2d45f6347a0bcc525e1a6f2d434~tplv-k3u1fbpfcp-zoom-in-crop-mark:4536:0:0:0.awebp)

> 使用真机来调试，因为模拟器使用的CALayer是OSX的CALayer，不是iOS的CALayer。如果用模拟器调试，会发现所有的视图都是黄色。


原文链接：https://juejin.cn/post/6960516630975774734