# iOS 离屏渲染探究

## 1.为什么要理解离屏渲染

离屏渲染（Offscreen rendering）对iOS开发者来说不是一个陌生的东西，项目中或多或少都会存在离屏渲染，也是面试中经常考察的知识点。一般来说，大多数人都能知道设置圆角、mask、阴影等会触发离屏渲染，但我们深入的探究一下，大家能够很清楚的知道下面几个问题吗？

- 离屏渲染是在哪一步发生的吗？
- 离屏渲染产生的原因是什么呢？
- 设置圆角一定会触发离屏渲染吗？
- 离屏渲染既然会影响性能我们为什么还要使用呢？优化方案又有那些？

今天我就带着这几个问题探究一下离屏渲染。

## 2.ios平台的渲染框架

![img](https://p1-jj.byteimg.com/tos-cn-i-t2oaga2asx/gold-user-assets/2020/7/3/17310948698d1b33~tplv-t2oaga2asx-zoom-in-crop-mark:3024:0:0:0.awebp)

## 3.Core Animation 流水线：

![img](https://p1-jj.byteimg.com/tos-cn-i-t2oaga2asx/gold-user-assets/2020/7/8/1732f1743c0078c7~tplv-t2oaga2asx-zoom-in-crop-mark:3024:0:0:0.awebp)

这是在WWDC的Advanced Graphics and Animations for iOS Apps（WWDC14 419）中有这样一张图，我们可以看到，在Application这一层中主要是CPU在操作，而到了Render Server这一层，CoreAnimation会将具体操作转换成发送给GPU的draw calls（以前是call OpenGL ES，现在慢慢转到了Metal），显然CPU和GPU双方同处于一个流水线中，协作完成整个渲染工作。我们也可以把iOS下的Core Animation可以理解为一个复合引擎，主要职责包含：渲染、构建和实现动画。

## 4.离屏渲染的定义

1. OpenGL中，GPU屏幕渲染有以下两种方式当前屏幕渲染（On-Screen Rendering）：正常情况下，我们在屏幕上显示都是GPU读取帧缓冲区（Frame Buffer）渲染好的的数据，然后显示在屏幕上。流程如图：![img](https://p1-jj.byteimg.com/tos-cn-i-t2oaga2asx/gold-user-assets/2020/7/9/1732f3543028519f~tplv-t2oaga2asx-zoom-in-crop-mark:3024:0:0:0.awebp)
2. （Off-Screen Rendering ）：如果有时因为一些限制，无法把渲染结果直接写入frame buffer，而是先暂存在另外的内存区域，之后再写入frame buffer，那么这个过程被称之为离屏渲染。也就是GPU需要在当前屏幕缓冲区以外新开辟一个缓冲区进行渲染操作。流程如图：![img](https://p1-jj.byteimg.com/tos-cn-i-t2oaga2asx/gold-user-assets/2020/7/9/173313b3aebc4252~tplv-t2oaga2asx-zoom-in-crop-mark:3024:0:0:0.awebp)

在上面的CoreAnimation流水线示意图中，我们可以得知主要的渲染操作是由CoreAnimation的Render Server模块，通过调用显卡驱动提供的OpenGL或Metal接口执行，对于每一层layer，Render Server会遵循“[画家算法](https://link.juejin.cn?target=https%3A%2F%2Flink.zhihu.com%2F%3Ftarget%3Dhttps%3A%2F%2Fen.wikipedia.org%2Fwiki%2FPainter%2527s_algorithm)”（由远及近），按次序输出到frame buffer，然后按照次序绘制到屏幕，当绘制完一层，就会将该层从帧缓存区中移除（以节省空间）如下图，从左至右依次输出，得到最后的显示结果。

![img](https://p1-jj.byteimg.com/tos-cn-i-t2oaga2asx/gold-user-assets/2020/7/9/1732f47f04a367c7~tplv-t2oaga2asx-zoom-in-crop-mark:3024:0:0:0.awebp)

但在某些场景下“画家算法”虽然可以逐层输出，但是无法在某一层渲染完成后，在回过头来擦除/修改某一部分，因为这一层之前的layer像素数据已经被永久覆盖了。这就意味着对于每一层的layer要么能够通过单次遍历就能完成渲染，要么就只能令开辟一块内存作为临时中转区来完成复杂的修改/裁剪等操作。

> 举例说明：对图3进行圆角和裁剪：imageView.clipsToBounds = YES,imageView.layer.cornerRadius=10时，这就不是简单的图层叠加了，图1，图2，图3渲染完成后，还要进行裁减，而且子视图layer因为父视图有圆角，也需要被裁剪，无法在某一层渲染完成之后，再回过头来擦除/改变其中的某个部分。所以不能按照正常的流程，因此苹果会先渲染好每一层，存入一个缓冲区中，即**离屏缓冲区**，然后经过层叠加和处理后，再存储到帧缓存去中，然后绘制到屏幕上，这种处理方式叫做**离屏渲染**

##  5.常见离屏渲染场景分析

使用Simulator检测项目中触发离屏渲染的图层,如下图：

![img](https://p1-jj.byteimg.com/tos-cn-i-t2oaga2asx/gold-user-assets/2020/7/9/1732f593d2a2f32d~tplv-t2oaga2asx-zoom-in-crop-mark:3024:0:0:0.awebp) 

打开 Color Off-screen Rendered,同时我们可以借助Xcode或 [Reveal](https://link.juejin.cn?target=https%3A%2F%2Frevealapp.com%2F) 清楚的看到那些图层触发了离屏渲染。

关于常见的设置圆角触发离屏渲染示例说明：

![img](https://p1-jj.byteimg.com/tos-cn-i-t2oaga2asx/gold-user-assets/2020/7/9/1732f615301291d8~tplv-t2oaga2asx-zoom-in-crop-mark:3024:0:0:0.awebp)

如上图示例代码中（btn.png是一个200x300的本地图片），

- btn1设置了图片，设置了圆角，打开了clipsToBounds = YES，触发了离屏渲染，

- btn2设置了背景颜色，设置了圆角，打开了clipsToBounds = YES，没有触发离屏渲染，

- img1设置了图片，设置了圆角，打开了masksToBounds = YES，触发了离屏渲染，

- img2设置了背景颜色，设置了圆角，打开了masksToBounds = YES，没有触发离屏渲染

> 解释：btn1和img1触发了离屏渲染，原因是btn1是由它的layer和UIImageView的layer混合起来的效果（UIButton有imageView），所以设置圆角的时候会触发离屏渲染。img1设置cornerRadius和masksToBounds是不会触发离屏渲染的，如果再对img1设置背景色，则会触发离屏渲染。

根据示例可以得出只是控件设置了圆角或（圆角+裁剪）并不会触发离屏渲染，同时需要满足父layer需要裁剪时，子layer也因为父layer设置了圆角也需要被裁剪（即视图contents有内容并发生了多图层被裁剪）时才会触发离屏渲染。

苹果官方文档对于`cornerRadius`的描述：

> Setting the radius to a value greater than `0.0` causes the layer to begin drawing rounded corners on its background. By default, the corner radius does not apply to the image in the layer’s `contents` property; it applies only to the background color and border of the layer. However, setting the `masksToBounds` property to `true` causes the content to be clipped to the rounded corners.

设置`cornerRadius`大于0时，只为layer的`backgroundColor`和`border`设置圆角；而不会对layer的`contents`设置圆角，除非同时设置了`layer.masksToBounds`为`true`（对应UIView的`clipsToBounds`属性）。

## 6.圆角触发离屏渲染示意图

![img](https://p1-jj.byteimg.com/tos-cn-i-t2oaga2asx/gold-user-assets/2020/7/9/1732f78b680ed8b0~tplv-t2oaga2asx-zoom-in-crop-mark:3024:0:0:0.awebp)

> 一旦我们 **为contents设置了内容** ，无论是图片、绘制内容、有图像信息的子视图等，再加上圆角+裁剪，就会触发离屏渲染。

## 7.其他触发离屏渲染的场景：

> - 采用了光栅化的 layer (layer.shouldRasterize)
> - 使用了 mask 的 layer (layer.mask)
> - 需要进行裁剪的 layer (layer.masksToBounds /view.clipsToBounds)
> - 设置了组透明度为 YES，并且透明度不为 1 的layer (layer.allowsGroupOpacity/ layer.opacity)
> - 使用了高斯模糊
> - 添加了投影的 layer (layer.shadow*)
> - 绘制了文字的 layer (UILabel, CATextLayer, Core Text 等)

shouldRasterize 光栅化

shouldRasterize开启后，会将layer作为位图保存下来，下次直接与其他内容进行混合。这个保存的位置就是OffscreenBuffer中。这样下次需要再次渲染的时候，就可以直接拿来使用了。

shouldRasterize使用建议：

- layer不复用，没必要打开shouldRasterize
- layer不是静态的，也就是说要频繁的进行修改，没必要使用shouldRasterize
- 离屏渲染缓存内容有100ms时间限制，超过该时间的内容会被丢弃，进而无法复用
- 离屏渲染空间是屏幕像素的2.5倍，如果超过也无法复用

## 8.离屏渲染的优劣

### 8.1劣势

离屏渲染增大了系统的负担，会形象App性能。主要表现在以下几个方面：

- 离屏渲染需要额外的存储空间，渲染空间大小的上限是2.5倍的屏幕像素大小，超过无法使用离屏渲染
- 容易掉帧：一旦因为离屏渲染导致最终存入帧缓存区的时候，已经超过了16.67ms，则会出现掉帧的情况，造成卡顿

### 8.2优势

虽然离屏渲染会需要多开辟出新的临时缓存区来存储中间状态，但是对于多次出现在屏幕上的数据，可以提前渲染好，从而进行复用，这样CPU/GPU就不用做一些重复的计算。

特殊产品需求，为实现一些特殊动效果，需要多图层以及离屏缓存区保存中间状态，这种情况下就不得不使用离屏渲染。比如产品需要实现高斯模糊，无论自定义高斯模糊还是调用系统API都会触发离屏渲染。

## 9.离屏渲染优化方案（关于实现圆角造成的离屏渲染优化）

方案一

```gml
self.view.layer.clipsToBounds = YES;self.view.layer.cornerRadius = 4.f;复制代码
```



> - clipsToBounds：UIView中的属性，其值主要决定了在视图上的子视图，超出父视图的部分是否截取，默认为NO，即不裁剪子视图超出部分。
> - masksToBounds：CALayer中的属性，其值主要决定了视图的图层上的子图层，超出父图层的部分是否需要裁减掉。默认NO。

方案二

> 如果产品设计圆角+阴影的卡片，可以使用切图实现圆角+阴影，避免触发离屏渲染

方案三

> 贝塞尔曲线绘制圆角

```objectivec
- (UIImage *)imageWithCornerRadius:(CGFloat)radius ofSize:(CGSize)size{
    /* 当前UIImage的可见绘制区域 */
    CGRect rect = (CGRect){0.f,0.f,size};
    /* 创建基于位图的上下文 */
    UIGraphicsBeginImageContextWithOptions(size, NO, UIScreen.mainScreen.scale);
    /* 在当前位图上下文添加圆角绘制路径 */
    CGContextAddPath(UIGraphicsGetCurrentContext(), [UIBezierPath bezierPathWithRoundedRect:rect cornerRadius:radius].CGPath);
    /* 当前绘制路径和原绘制路径相交得到最终裁剪绘制路径 */
    CGContextClip(UIGraphicsGetCurrentContext());
    /* 绘制 */
    [self drawInRect:rect];
    /* 取得裁剪后的image */
    UIImage *image = UIGraphicsGetImageFromCurrentImageContext();
    /* 关闭当前位图上下文 */
    UIGraphicsEndImageContext();
    return image;
}复制代码
```

方案四

> CAShapeLayer + UIBezierPath 绘制圆角来实现UITableViewCell圆角并绘制边框颜色（这种方式比直接设置圆角方式好，但也会触发离屏渲染）,代码如下：

```swift
- (void)tableView:(UITableView *)tableView willDisplayCell:(UITableViewCell *)cell forRowAtIndexPath:(NSIndexPath *)indexPath{

    CAShapeLayer *maskLayer = [CAShapeLayer layer];
    maskLayer.frame = CGRectMake(0, 0, cell.width, cell.height);

    CAShapeLayer *borderLayer = [CAShapeLayer layer];
    borderLayer.frame = CGRectMake(0, 0, cell.width, cell.height);
    borderLayer.lineWidth = 1.f;
    borderLayer.strokeColor = COLOR_LINE.CGColor;
    borderLayer.fillColor = [UIColor clearColor].CGColor;

    UIBezierPath *bezierPath = [UIBezierPath bezierPathWithRoundedRect:CGRectMake(0, 0, cell.width, cell.height) cornerRadius:kRadiusCard];
    maskLayer.path = bezierPath.CGPath;
    borderLayer.path = bezierPath.CGPath;

    [cell.contentView.layer insertSublayer:borderLayer atIndex:0];
    [cell.layer setMask:maskLayer];
}
```

> 关于方案四的解释：
>
> - CAShapeLayer继承于CALayer，因而可以使用CALayer的所有属性值；
> - CAShapeLayer需要和贝塞尔曲线配合使用才能够实现效果；
> - CAShapeLayer(属于CoreAnimation)与贝塞尔曲线配合使用可以实现不在view的drawRect（继承于CoreGraphics走的是CPU,消耗的性能较大）方法中画出想要的图形；
> - CAShapeLayer动画渲染是驱动GPU，而view的drawRect方法使用CPU渲染，相比其效率更高，消耗内存更少。
>
> 总的来说使用CAShapeLayer的内存消耗少，渲染速度快。

YYKit是开发中经常用的三方库，YYImage对图片圆角的处理方法是值得推荐的，附上实现源码：

```swift
- (UIImage *)imageByRoundCornerRadius:(CGFloat)radius
                              corners:(UIRectCorner)corners
                          borderWidth:(CGFloat)borderWidth
                          borderColor:(UIColor *)borderColor
                       borderLineJoin:(CGLineJoin)borderLineJoin {
    
    if (corners != UIRectCornerAllCorners) {
        UIRectCorner tmp = 0;
        if (corners & UIRectCornerTopLeft) tmp |= UIRectCornerBottomLeft;
        if (corners & UIRectCornerTopRight) tmp |= UIRectCornerBottomRight;
        if (corners & UIRectCornerBottomLeft) tmp |= UIRectCornerTopLeft;
        if (corners & UIRectCornerBottomRight) tmp |= UIRectCornerTopRight;
        corners = tmp;
    }
    
    UIGraphicsBeginImageContextWithOptions(self.size, NO, self.scale);
    CGContextRef context = UIGraphicsGetCurrentContext();
    CGRect rect = CGRectMake(0, 0, self.size.width, self.size.height);
    CGContextScaleCTM(context, 1, -1);
    CGContextTranslateCTM(context, 0, -rect.size.height);
    
    CGFloat minSize = MIN(self.size.width, self.size.height);
    if (borderWidth < minSize / 2) {
        UIBezierPath *path = [UIBezierPath bezierPathWithRoundedRect:CGRectInset(rect, borderWidth, borderWidth) byRoundingCorners:corners cornerRadii:CGSizeMake(radius, borderWidth)];
        [path closePath];
        
        CGContextSaveGState(context);
        [path addClip];
        CGContextDrawImage(context, rect, self.CGImage);
        CGContextRestoreGState(context);
    }
    
    if (borderColor && borderWidth < minSize / 2 && borderWidth > 0) {
        CGFloat strokeInset = (floor(borderWidth * self.scale) + 0.5) / self.scale;
        CGRect strokeRect = CGRectInset(rect, strokeInset, strokeInset);
        CGFloat strokeRadius = radius > self.scale / 2 ? radius - self.scale / 2 : 0;
        UIBezierPath *path = [UIBezierPath bezierPathWithRoundedRect:strokeRect byRoundingCorners:corners cornerRadii:CGSizeMake(strokeRadius, borderWidth)];
        [path closePath];
        
        path.lineWidth = borderWidth;
        path.lineJoinStyle = borderLineJoin;
        [borderColor setStroke];
        [path stroke];
    }
    
    UIImage *image = UIGraphicsGetImageFromCurrentImageContext();
    UIGraphicsEndImageContext();
    return image;
}
```


原文链接：https://juejin.cn/post/6847902222567604231