# iOS-WebRTC静态库，framework下载、编译，使用

网上的对于WebRTC的下载和编译的文章其实已经很多，但是有些比较久远，里面很多方法都无法再使用，所以写一篇文章来简单的说下当前的一些使用方法和注意事项。

## 1、编译环境：

硬件：MacBook Pro（Mid 2015）

系统：macOS High Sierra 10.13.3

网络环境：普通的稳定网络就可以

## 2、安装git

这里不做详细介绍，相信做iOS开发的都有，没有的话，百度和Google都是很快可以安装成功。

## 3、安装depot_tools

在你的硬盘上找一个空间专门来做我们的编译工作，保证空间在8G以上

打开终端新建一个文件夹用来存放我们之后所有的相关文件：

> mkdir webrtc_build

文件夹建好后，我们就要确保自己的网络环境是可以用的，当然因为WebRTC的资源是在国外，所以这里要FQ一下，关于FQ真的是因人而异，如果你的工具足够强大下载源码毫无障碍最好了。确定已经FQ，就继续下一步。

确定网络OK后继续：

depot_tools，这是一套Google用来编译Chromium或者WebRTC的构建工具，在我们后续的编译过程中也将使用它。

> a.cd webrtc_build
>
> b.  git clone https://chromium.googlesource.com/chromium/tools/depot_tools.git
>
> c.  把depot_tools 设置到PATH中:
>
> echo "export PATH=$PWD/depot_tools:$PATH" > $HOME/.bash_profile
>
> d.  使PATH设置生效:source $HOME/.bash_profile
>
> e. echo $PATH查看设置是否生效。

### 3.1 安装ninja

**ninja** 是 **WebRTC** 的编译工具，我们需要对其进行编译，步骤如下：

> 1 git clone git://github.com/martine/ninja.git
>
> 2 cd ninja/
>
> 3 ./bootstrap.py

复制到系统目录（也可配置坏境变量）

> 1 sudo cp ninja /usr/local/bin/
>
> 2 sudo chmod a+rx /usr/local/bin/ninja

ps：这个ninja在我编译的时候我并没有手动去调用它，但是应该再后面的编译脚本里会用到它，所以我建议大家还是按照步骤安装下。

## 4、下载WebRTC源码

在我们的编译工作目录webrtc_build下创建一个webtrtc子目录来存放代码：

> mkdir webrtc
>
> cd webrtc

接下来开始下载源码，这个过程会因为不同的网络环境而异，资源一共大约6G多，我的网络FQ环境下，下载速度大概是1M/S，下载部分一共将近2个小时，这一步....下载过程中去做些别的事情吧，时间会非常长：

> a.  设置要编译的平台到环境变量中：export GYP_DEFINES="OS=ios"
>
> b.  fetch --nohooks webrtc_ios
>
> c.  gclient sync

这里要提示下，第一步设置好编译目标的平台以后，b步骤输入执行后，就进入下载步骤了，之前我以为c步骤才是下载的大头，所以被坑了一下(本来要去办事的，结果被b步骤耽误了)，b步骤会下载大概6G多的文件，这个是时间最长的，当漫长的全部下载完毕以后，成功的话，大概是这个样子：

![img](https:////upload-images.jianshu.io/upload_images/2149132-2b828c266a74a9d7.png?imageMogr2/auto-orient/strip|imageView2/2/w/1200/format/webp)

下载过程大部分时间你看到的都是这个样子

![img](https:////upload-images.jianshu.io/upload_images/2149132-3c4b2f97cefaf974.png?imageMogr2/auto-orient/strip|imageView2/2/w/1200/format/webp)

下载成功后是这样的

接下来执行c步骤，我使用的时候直接输入了我需要的版本号去同步，并没有直接执行上面写的c步骤的终端代码，可以使用 gclient sync -r 9f7e2a90da243288657e1802af85168e87daab01 来下载指定版本，9f7e2a90da243288657e1802af85168e87daab01 这个东西是commit id 我下的是18年3月1日的版本貌似，是我写文章的时候最新的，如果你有项目需求要别的版本，你可以自己改一下，具体方式为：

1、进入[WebRTC](https://webrtc.org/native-code/ios/)官网

2、点击上方About->Release Notes

![img](https:////upload-images.jianshu.io/upload_images/2149132-ceb6531c76909461.png?imageMogr2/auto-orient/strip|imageView2/2/w/1200/format/webp)

选择你要的版本，比如M65

![img](https:////upload-images.jianshu.io/upload_images/2149132-091cb1a6f69bb5cf.png?imageMogr2/auto-orient/strip|imageView2/2/w/1018/format/webp)

进入后，再点击WebRTC M65 branch

![img](https:////upload-images.jianshu.io/upload_images/2149132-8fa6c0c12534677c.png?imageMogr2/auto-orient/strip|imageView2/2/w/1200/format/webp)

进入后选择第一条

![img](https:////upload-images.jianshu.io/upload_images/2149132-1de4bdfd32ed9797.png?imageMogr2/auto-orient/strip|imageView2/2/w/1200/format/webp)

点击进入后，你就可以看到你需要的commit id了

![img](https:////upload-images.jianshu.io/upload_images/2149132-567b25df3b4ec544.png?imageMogr2/auto-orient/strip|imageView2/2/w/1200/format/webp)

在gclient sync -r 9f7e2a90da243288657e1802af85168e87daab01这一步之后，终端大概是这样子的

![img](https:////upload-images.jianshu.io/upload_images/2149132-9e0bf6d91239866b.png?imageMogr2/auto-orient/strip|imageView2/2/w/1200/format/webp)

项目的目录大概是这样样子

![img](https:////upload-images.jianshu.io/upload_images/2149132-f63bb99f781dd00a.png?imageMogr2/auto-orient/strip|imageView2/2/w/1200/format/webp)

到这一步，你已经成功下载到了源码。

## 5、编译WebRTC

编译的方式，我看了几个帖子，什么方法都有，这里我根据我的需求，说说我的做法。我的主要目的是因为网上找不到.a模式的webrtc的静态库，都是framework，所以我才自己编译的。我的需求很简单，就是需要.a出来。ios版本的webrtc是有自己的编译脚本文件的，我们在这里面做些修改就可以，脚本文件的路径是：webrtc_build/webrtc/src/tools_webrtc/ios/build_ios_libs.sh

正常情况，如果你不做任何更改，直接执行这个脚本，会编译出WebRTC.framework，默认路径会在：webrtc_build/webrtc/src/out_ios_libs 由于我没有编译framework，所以没有图，但是我猜测的话，应该会有真机的32位和64位的还有模拟器的，因为我编译的.a就是这几个。

所以如果你需要去掉不要的版本，还有你需要编译的.a，在这几个地方更改下就可以，记得更改是在webrtc_build/webrtc/src/tools_webrtc/ios/build_ios_libs.py 

看清楚后缀 看清楚后缀 看清楚后缀是py

![img](https:////upload-images.jianshu.io/upload_images/2149132-3f31cdb92cbb6b2c.png?imageMogr2/auto-orient/strip|imageView2/2/w/1200/format/webp)

arm64真机64位，个人觉得编这个就够了其实.....别的都可以删掉，剩下的是arm是真机32位，x64模拟器64位，x86模拟器32位，根据个人需要删减。建议大家先备份啊！！

之后的default位置后面改成'static_only'这个大家看到help也知道意思了，我这个图是改了后的，之前是default='framework'

脚本对于我来说更改这些就行，如果有其他的需要的话大家可以多研究研究其他的选项。

这里更改完后保存退出。

然后执行

./build_ios_libs.sh

就开始一顿编辑....这个时间不算短，我的机器应该有个20分钟左右。

编译成功以后，你可以在这个地方找到你的.a静态库 

webrtc_build/webrtc/src/out_ios_libs

![img](https:////upload-images.jianshu.io/upload_images/2149132-6b252b6ce042ac44.png?imageMogr2/auto-orient/strip|imageView2/2/w/880/format/webp)

因为我当时忘记删除不要的架构，所以把4个都编出来了，如果你只编一个，时间会节省不少，这里根据你自己的需要，拿去用就好了。还有目前我找到的头文件的位置貌似这个

webrtc_build/webrtc/src/sdk/objc/Framework/Headers/WebRTC

![img](https:////upload-images.jianshu.io/upload_images/2149132-5a0c4b93f452a4a9.png?imageMogr2/auto-orient/strip|imageView2/2/w/1200/format/webp)

之后，把.a静态库和头文件拖进你的项目里去使用就好了，当然是建议建个专门的文件夹来存放.a和头文件，记得在xcode中设置build setting里的Header search path 还有设置bitcode为no，静态库使用的时候，需要引入其他一些依赖：

![img](https:////upload-images.jianshu.io/upload_images/2149132-99d2376add136570.png?imageMogr2/auto-orient/strip|imageView2/2/w/1140/format/webp)

不引用的话，项目编译期就会报错。github上有很多开源的Demo，本篇主要是为了解决需要.a静态库的朋友，以及一些当前下载编译可行简便的方法。后续如果有新的体验会更新，有问题大家可以留言。

写这篇文章的时候，借鉴了如下的文章，有兴趣的可以也去看看：

[iOS下载、编译WebRTC及demo](https://www.jianshu.com/p/64bd7f5b18b1)  //这篇文章最后说了framework的集成，需要freamwork怎么集成到项目里的可以看下

[WebRTC iOS&OSX 库的编译](https://links.jianshu.com/go?to=http%3A%2F%2Fwww.enkichen.com%2F2017%2F05%2F12%2Fwebrtc-ios-build%2F) //我是从这篇文章确定了新的源码库也是可以编译出.a的，才下了决心去下6个G


原文链接：https://www.jianshu.com/p/2ecb9d846b35