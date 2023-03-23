# iOS音视频开源框架WebRTC入门-AppRTCMobile

## 1、系列

[iOS音视频开源框架WebRTC入门-编译(前序-授人鱼不如授人以渔)](https://www.jianshu.com/p/435753014d47)
 [iOS音视频开源框架WebRTC入门-编译(后序-带WebRTC源码)](https://www.jianshu.com/p/2473b3ab34ba)
 [iOS音视频开源框架WebRTC入门-AppRTCMobile(WebRTC官网demo)](https://www.jianshu.com/p/f3e89919bd55)
 [iOS音视频开源框架WebRTC入门-简单应用](https://www.jianshu.com/p/67743301d14d)
 [iOS音视频开源框架WebRTC入门-本地/远端图像等比缩放](https://www.jianshu.com/p/f59b4bfd1e10)
 [iOS音视频开源框架WebRTC入门-外网通信](https://www.jianshu.com/p/ddbe5d90332f)

## 2、简介

`AppRTCMobile是WebRTC的一个官方 iOS demo`
 来说说在[获取源码](https://www.jianshu.com/p/435753014d47)的基础上,如何去生成项目并运行项目👇

## 3、生成项目

获取源码以后,根目录是 src, 切换至该目录

```bash
cd /Users/tianjinfeng/Desktop/iOS/WebRTC/webrtcbuilds-master/out/src
```

编译生成 arm64架构的项目:

```rust
gn gen out/iOS_64 --args='target_os="ios" target_cpu="arm64"' --ide=xcode
```

![img](https:////upload-images.jianshu.io/upload_images/732408-0d8fbbda74979784.png?imageMogr2/auto-orient/strip|imageView2/2/w/1200/format/webp)

image.png
 执行完成以后在指定目录(out/iOS_64)中就有一个:`all.xcworkspace`:

![img](https:////upload-images.jianshu.io/upload_images/732408-8ee0d4ceb57ceb14.png?imageMogr2/auto-orient/strip|imageView2/2/w/1200/format/webp)

image.png

## 4、运行项目

![img](https:////upload-images.jianshu.io/upload_images/732408-819ccbf762f370e4.png?imageMogr2/auto-orient/strip|imageView2/2/w/1200/format/webp)

image.png
 直接运行即可:

![img](https:////upload-images.jianshu.io/upload_images/732408-735229a782ac95ba.png?imageMogr2/auto-orient/strip|imageView2/2/w/1200/format/webp)

image.png
 真机界面:

![img](https:////upload-images.jianshu.io/upload_images/732408-1a66abe88873959a.png?imageMogr2/auto-orient/strip|imageView2/2/w/665/format/webp)

image.png

在两台真机(64位)上安装 AppRTCMobile,输入同一个 Room name 之后 Start call 就能互通了(`需要翻墙`)
 在这过程中,我并没有遇到签名问题!!!
 参考链接:
[Webrtc笔记-运行demo到ios真机](https://link.jianshu.com?t=http://www.re2x.com/WebRTC-wiki/zh-CN/#!pages/iOS/Webrtc笔记-运行demo到ios真机.md)
 [Webrtc笔记-运行demo 遇到的签名问题]([http://www.re2x.com/WebRTC-wiki/zh-CN/#!pages/iOS/./Webrtc](https://link.jianshu.com?t=http://www.re2x.com/WebRTC-wiki/zh-CN/#!pages/iOS/./Webrtc)笔记-运行demo 遇到的签名问题.md)




原文链接：https://www.jianshu.com/p/f3e89919bd55