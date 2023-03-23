# iOS下 WebRTC 视频渲染

## 1、前言

今天为大家介绍一下 iOS 下 WebRTC是如何渲染视频的。在iOS中有两种加速渲染视频的方法。一种是使用OpenGL；另一种是使用 Metal。

OpenGL的好处是跨平台，推出时间比较长，因此比较稳定。兼容性也比较好。而Metal是iOS最近才推出的技术，理论上来说比OpenGL ES效率更高。

WebRTC中这两种渲染方式都支持。它首先会判断当前iOS系统是否支持Metal，如果支持的话，优先使用Metal。如果不支持的话，就使用 OpenGL ES。

我们今天介绍的是 OpenGL ES的方案。

![图片](https://mmbiz.qpic.cn/mmbiz/lf9n56ou2IDAKqhqw3uKVicy6tEBjv5vdIiahpIDjEg3Lb1Ric4AKOPPT2TaLZF74n61UG1ooTg9KXlqeTw2XQpQA/640?wx_fmt=other&wxfrom=5&wx_lazy=1&wx_co=1)

## 2、创建 OpenGL 上下文

在iOS中使用OpenGL ES做视频渲染时，首先要创建EAGLContext对象。这是因为，EAGLContext管理着 OpengGL ES 渲染上下文。该上下文中，包括了状态信息，渲染命令以及OpenGL ES绘制资源（如纹理和renderbuffers）。为了执行OpenGL ES命令，你需要将创建的EAGLContext设置为当前渲染上下文。

EAGLContext并不直接管理绘制资源，它通过与上下文相关的EAGLSharegroup对象来管理。当创建EAGLContext时，你可以选择创建一个新的sharegroup或与之前创建的EAGLContext共享EAGLSharegroup。

EAGLContext与EAGLSharegroup的关系如下图所示：

![图片](https://mmbiz.qpic.cn/mmbiz/lf9n56ou2IDAKqhqw3uKVicy6tEBjv5vdZsH3otqEQQ24Be4APupysvf6h55RUdJSNFk5bZyH5HhibeBVrLDm2ng/640?wx_fmt=other&wxfrom=5&wx_lazy=1&wx_co=1)



WebRTC中并没有使用共享EAGLSharegroup的情况，所以对于这种情况我们这里就不做特别讲解了。有兴趣的同学可以在网上查找相关资料。

目前，OpenGL ES有3个版本，主要使用版本2和版本3 。所以我们在创建时要对其作判断。首先看是否支持版本3，如果不支持我们就使用版本2。

**代码如下：**

![图片](https://mmbiz.qpic.cn/mmbiz_png/lf9n56ou2IDAKqhqw3uKVicy6tEBjv5vdXRLhL2v4rOsCKTbrV4bdw8pkWld3vLTiaOnrzU8fXslnHd7TXcSzalw/640?wx_fmt=png&tp=wxpic&wxfrom=5&wx_lazy=1&wx_co=1)

创建完上下文后，我们还要将它设置为当前上下文，这样它才能真正起作用。

**代码如下**：

![图片](https://mmbiz.qpic.cn/mmbiz_png/lf9n56ou2IDAKqhqw3uKVicy6tEBjv5vdsWgeCOSwuYY2sPWicb3dGcBRu8jZzLulh5ZBHKerib4MCQUVDXR8rY9Q/640?wx_fmt=png&tp=wxpic&wxfrom=5&wx_lazy=1&wx_co=1)

需要注意的是，由于应用切换到后台后，上下文就发生了切换。所以当它切换到前台时，也要做上面那个判断。

OpenGL ES上下文创建好后，下面我们看一下如何创建View。

## 3、创建 OpenGL View

在iOS中，有两种展示层，一种是 GLKView，另一种是 CAEAGLLayer。WebRTC中使用GLKView进行展示。CAEAGLLayer暂不做介绍。

GLKit框架提供了View和View Controller类以减少建立和维护绘制 OpenGL ES 内容的代码。GLKView类用于管理展示部分；GLKViewController类用于管理绘制的内容。它们都是继承自UIKit。GLKView的好处是，开发人员可以将自己的精力聚焦在OpenGL ES渲染的工作上。

GLKView展示的基本流程如下：

![图片](https://mmbiz.qpic.cn/mmbiz/lf9n56ou2IDAKqhqw3uKVicy6tEBjv5vdmY0kLVMhXUGmOT3LMiakUQ159fufrfu6eKHDWy0JkVtdEibrgpm1SUCQ/640?wx_fmt=other&wxfrom=5&wx_lazy=1&wx_co=1)

如上图所示，绘制 OpenGL ES 内容有三步：

- 准备 OpenGL ES 环境；
- 发送绘制命令；
- 展示渲染内容。

GLKView类自己实现了第一步和第三步。第二步由开发人员来完成，也就是要实现drawRect函数。GLKView之所以能为OpenGL ES提供简单的绘制接口，是因为它管理了OpenGL ES渲染过程的标准部分：

- 在调用绘制方法之前：

- - 使用 EAGLContext 作为当前上下文。
  - 根据size, 缩放因子和绘制属性，创建 FBO 和 renderbuffer。
  - 绑定 FBO，作为绘制命令的当前目的地。
  - 匹配 OpenGL ES viewport与 framebuffer size 。

- 在绘制方法返回之后：

- - 解决多采样 buffers(如果开启了多采样)。
  - 当内容不在需要时，丢掉 renderbuffers。
  - 展示renderbuffer内容。

使用GLKView有两种方法，一种是实现一个类，直接继承自GLKView，并实现drawRect方法。另一种是实现GLKView的代理，也就是GLKViewDelegate，并实现drawInRect方法。

在WebRTC中，使用的是第二种方法。RTCEAGLVideoView 是GLKView的包裹类，并且继承自GLKViewDelegate。

首先，创建GLKView.

![图片](https://mmbiz.qpic.cn/mmbiz_png/lf9n56ou2IDAKqhqw3uKVicy6tEBjv5vd88ym9FFJP53IzINibzxicYLdIUvGmeaswLuRGSW5xe6JTgibarl7ChGsA/640?wx_fmt=png&tp=wxpic&wxfrom=5&wx_lazy=1&wx_co=1)

创建好GLKView后，需要将glkView.delegate设置为RTCEAGLVideoView，这样就可以将绘制工作交由RTCEAGLVideoView来完成了。另外，glkView.enableSetNeedsDisplay 设置为 NO，由我们自己来控制何时进行绘制。

然后，实现drawInRect方法。

![图片](https://mmbiz.qpic.cn/mmbiz_png/lf9n56ou2IDAKqhqw3uKVicy6tEBjv5vdw42PymUibWQxGj9GdNDOibtSv9uowaR4qSFtQtwEuibF2ecKaYRkEyfVQ/640?wx_fmt=png&tp=wxpic&wxfrom=5&wx_lazy=1&wx_co=1)

上面的代码就是通过Shader来绘制NV12的YUV数据到View中。这段代码的基本意思是将一个解码后的视频帧分解成Y数据纹理，UV数据纹理。然后调用Shader程序将纹理转成rgb数据，最终渲染到View中。

## 4、Shader程序

OpenGL ES 有两种 Shader。一种是顶点(Vetex)Shader; 另一种是片元(fragment )Shader。

- Vetex Shader: 用于绘制顶点。
- Fragment Shader:用于绘制像素点。

### 4.1 Vetex Shader

Vetex Shader用于绘制图形的顶点。我们都知道，无论是2D还是3D图形，它们都是由顶点构成的。

在OpenGL ES中，有三种基本图元，分别是点，线，三角形。由它们再构成更复杂的图形。而点、线、三角形又都是由点组成的。

视频是在一个矩形里显示，所以我们要通过基本图元构建一个矩形。理论上，矩形可以通过点、线绘制出来，但这样做的话，OpenGL ES就要绘制四次。而通过三角形绘制只需要两次，所以使用三角形执行速度更快。

下面的代码就是 WebRTC 中的Vetex Shader程序。该程序的作用是每个顶点执行一次，将用户输入的顶点输出到 gl_Position中，并将顶点的纹理作标点转作为 Fragment Shader 的输入。

1. OpenGL坐标原点是屏幕的中心。纹理坐标的原点是左下角。

2. gl_Position是Shader的内部变量，存放一个项点的坐标。

![图片](https://mmbiz.qpic.cn/mmbiz_png/lf9n56ou2IDAKqhqw3uKVicy6tEBjv5vdRERDUX1HiaSxuicic3PqD9iaEF8OjUBZS3pGD39UPagsGhyE6YqRkmmw0A/640?wx_fmt=png&tp=wxpic&wxfrom=5&wx_lazy=1&wx_co=1)

OpenGL ES Shader语法请见我的另一篇文章着色器

### 4.2 fragment Shader

fragment Shader程序是对片元着色，每个片元执行一次。片元与像素差不多。可以简单的把片元理解为像素。

下面的代码是WebRTC中的 fragment Shader程序。WebRTC收到远端传来的H264视频帧后，解码成YUV数据。之后，对YUV数据进行分解，如移动端使用的YUV数据格式为NV12, 所以就被分成了两部分，一部分是Y数据纹理，另一部分是UV数据纹理。

YUV有多种格式，可以参见我的另一篇文章YUV。

在代码中，使用FRAGMENT_SHADER_TEXTURE命令，也就是OpenGL ES中的 texture2D 函数，分别从 Y 数据纹理中取出 y值，从 UV 数据纹理中取出 uv值，然后通过公式计算出每个像素(实际是片元)的 rgb值。

![图片](https://mmbiz.qpic.cn/mmbiz_png/lf9n56ou2IDAKqhqw3uKVicy6tEBjv5vdsjYWbrNc4Jf0cxOLbJjB2ic0SIWG1DsU5dMuKXAiarvO32ZDmpVHbL1A/640?wx_fmt=png&tp=wxpic&wxfrom=5&wx_lazy=1&wx_co=1)

有了顶点数据和片元的RGB值后，就可以调用OpenGL ES的 draw 方法进行视频的绘制了。

## 5、Shader的编译、链接与使用

上面介绍了 WebRTC下 Vetex Shader 和 Fragment Shader程序。要想让程序运行起来，还要额外做一些工作。

OpenGL ES的 shader程序与C程序差不多。想像一下C程序，要想让一个C程序运行起来，要有以下几个步骤：

- 1.写好程序代码
- 2.编译
- 3.链接
- 4.执行

Shader程序的运行也是如此。我们看看 WebRTC是如何做的。

![图片](https://mmbiz.qpic.cn/mmbiz_png/lf9n56ou2IDAKqhqw3uKVicy6tEBjv5vdHHzaHWLTfGn1dBa89CtvRviamLiaIPfM1ssX7x1oqjtk1M2YBPicP7Xog/640?wx_fmt=png&tp=wxpic&wxfrom=5&wx_lazy=1&wx_co=1)

它首先创建一个 Shader, 然后将上面的 Shader 程序与 Shader 绑定。之后编译 Shader。

![图片](https://mmbiz.qpic.cn/mmbiz_png/lf9n56ou2IDAKqhqw3uKVicy6tEBjv5vdJib52B34h6AMNVsHvExCPJXDYpIzTKm7lu8JEHTVb2jeKce3VC7GDBA/640?wx_fmt=png&tp=wxpic&wxfrom=5&wx_lazy=1&wx_co=1)

编译成功后，创建 program 对象。将之前创建的 Shader 与program绑定到一起。之后做链接工作。一切准备就绪后，就可以使用Shader程序绘制视频了。

![图片](https://mmbiz.qpic.cn/mmbiz_png/lf9n56ou2IDAKqhqw3uKVicy6tEBjv5vdyC0pUxEHjqReQDtYE33pCIxIH4Sx7KGhTib2jbzDjnBic92AvSrkhTgg/640?wx_fmt=png&tp=wxpic&wxfrom=5&wx_lazy=1&wx_co=1)

## 6、WebRTC中视频渲染相关文件

- RTCEAGLVideoView.m/h：创建 EAGLContext及OpenGL ES View，并将视频数据显示出来。
- RTCShader.mm/h：OpenGL ES Shader 程序的创建，编译与链接相关的代码。
- RTCDefaultShader.mm/h: Shader 程序，绘制相关的代码。
- RTCNV12TextureCache.mm/h: 用于生成 YUV NV12 相关纹理的代码。
- RTCI420TexutreCache.mm/h: 用于生成 I420 相关纹理的代码。

## 7、小结

本文对 WebRTC 中 OpenGL ES 渲染做了介绍。通过本篇文章大家可以了解到WebRTC是如何将视频渲染出来的。包括：

- 上下文的创建与初始化。
- GLKView的创建。
- 绘制方法的实现。
- Shader代码的分析。
- Shader的编译与执行。



对于 OpenGL ES 是一个相当大的主题，如果没有相应的基础，看本篇文章还是比较困难的。大家可以参考我前面写的几篇关于 OpenGL 的文章。



原文链接：https://mp.weixin.qq.com/s/IYJSYw4-o5IYyoKdgYs4wg

