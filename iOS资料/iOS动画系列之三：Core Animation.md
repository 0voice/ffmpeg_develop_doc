# iOS动画系列之三：Core Animation

## 1.  介绍

- Core Animation是一个非常强大的动画处理 API，使用它能做出非常绚丽的动画效果，而且往往是事半功倍，也就是说，使用少量的代码就可以实现非常强大的功能。
- 苹果封装的 UIView 的 block 动画就是对核心动画的封装，使用起来更加简单。
- 绝大多数情况下，使用 UIView 的 block 动画能够满足开发中的日常需求。
- 一些很酷炫的动画，还是需要通过核心动画来完成的。

## 2. 支持的平台

- Core Animation 同时支持 MAC OS 和 iOS 平台
- Core Animation 是直接作用在 CALayer 的，并非 UIView。所以这个系列，咱们是从CALayer开始的。
- Core Animation 的动画执行过程都是在后台操作的，不会阻塞主线程。

## 3. `Core Animation` 的继承结构图

- 是所有动画对象的父类，负责控制动画的持续时间和速度、是个抽象类，不能直接使用，应该使用具体子类。需要注意的是`CAAnimation` 和 `CAPropertyAnimation` 都是抽象类。
- view是负责响应事件的,layer是负责显示的。

下面盗用了一张网络上的图片用来解释继承结构。



![继承结构图](https://p1-jj.byteimg.com/tos-cn-i-t2oaga2asx/gold-user-assets/2017/8/13/700e6bd0a70d6bba9d2233ad1e424930~tplv-t2oaga2asx-zoom-in-crop-mark:4536:0:0:0.image)继承结构图



黄色的区块是常用的属性、方法或者需要遵守的协议，灰色的是名称。

其中CAAnimationGroup、CABasicAnimation、CAKeyFramkeAnimation咱们会在下次更新中写一些小例子。

## 4. 常见属性和使用步骤

### 4.1 使用步骤

通常分成三部完成：
1，创建核心动画对象；
2，设置动画属性；
3，添加到要作用的layer上。
就想把大象放进冰箱需要三步一样。哈哈～

### 4.2 常用属性

就是咱们上面图片中的小黄图显示的。

- `duration`：持续时间，默认值是0.25秒
- `repeatCount`:重复次数，无线循环可以设置HUGE_VALF或者CGFLOAT_MAX
- `repeatDuration`:重复时间
- `removeOnCompletion`: 默认为YES，代表动画执行完毕后就从图层上移除，图形会恢复到执行动画之前的状态。*如果想要图层保持显示动画执行后的状态，那就设置为NO，同时设置fillMode为kCAFillModeForwards*
- `fillMode`：决定当前对象在非active时间段的行为
- `beginTime`：可以用来设置动画延时执行，若想延迟2s，就设置为`CACurrentMediaTIme() + 2`
- `CACurrentMediaTIme()`：图层的当前时间
- `timingFunction`:速度控制函数，控制动画运行节奏
- `delegate`：动画代理

### 4.3 animationWithKeyPath中，常用的keyPath

| 属性名称                | 类型             | 作用                             |
| ----------------------- | ---------------- | -------------------------------- |
| transform.rotation.x    | CGFloat或float   | 绕X轴坐标旋转 角度               |
| transform.rotation.y    | CGFloat或float   | 绕Y轴坐标旋转 角度               |
| transform.rotation.z    | CGFloat或float   | 绕Z轴坐标旋转 角度               |
| transform.rotation      | CGFloat或float   | 作用与transform.tation.z一样     |
| ----                    | ----             | ----                             |
| transform.scale         | CGFloat          | 整个layer的比例                  |
| transform.scale.x       | CGFloat          | x轴坐标比例变化                  |
| transform.scale.y       | CGFloat          | y轴坐标比例变化                  |
| transform.scale.z       | CGFloat          | z轴坐标比例变化                  |
| ----                    | ----             | ----                             |
| transform.translation   | CGMutablePathRef | 整个layer的xyz轴都进行移动       |
| transform.translation.x | CGMutablePathRef | 横向移动                         |
| transform.translation.y | CGMutablePathRef | 纵向移动                         |
| transform.translation.z | CGMutablePathRef | 纵深移动                         |
| ----                    | ----             | ----                             |
| opacity                 | CGFloat          | 透明度,闪烁等动画用 。范围是0～1 |
| backgroundColor         | CGColor          | 背景颜色                         |
| cornerRadius            | CGFloat          | 圆角                             |

### 4.4 动画填充模式

- kCAFillModeForwards:当动画结束后，layer会一直保持着动画最后的状态

- kCAFillModeBackwards：在动画开始前，只需要将动画加入了一个layer，layer便立即进入动画的初始状态并等待动画开始

- kCAFillModeBoth：这个其实就是上面两个合成，动画加入后，开始之前，layer便处于动画初始状态，动画结束后layer保持动画最后的状态

- kCAFillModeRemoved：这个是默认值，也就是说当动画开始前和动画结束后，动画对layer都没有影响，动画结束后，layer会恢复到之前的状态

  ```ini
    keyArc.calculationMode = kCAAnimationPaced
  ```

### 4.5 速度控制函数

- kCAMediaTimingFunctionLinear（线性）：匀速，给你一个相对静态的感觉

- kCAMediaTimingFunctionEaseIn（渐进）：动画缓慢进入，然后加速离开

- kCAMediaTimingFunctionEaseOut（渐出）：动画全速进入，然后减速的到达目的地

- kCAMediaTimingFunctionEaseInEaseOut（渐进渐出）：动画缓慢的进入，中间加速，然后减速的到达目的地。这个是默认的动画行为。

  ```ini
    keyArc.timingFunction = [CAMediaTimingFunction functionWithName:kCAMediaTimingFunctionEaseInEaseOut];
  ```

原文链接：https://juejin.cn/post/6844903490980954119