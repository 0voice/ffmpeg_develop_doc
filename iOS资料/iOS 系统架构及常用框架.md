# iOS 系统架构及常用框架

## 1、ios稳定性

iOS基于UNIX系统，因此从系统的稳定性上来说它要比其他操作系统的产品好很多

## 2、ios系统架构

iOS的系统架构分为四层，由上到下一次为：可触摸层（Cocoa Touch layer）、媒体层（Media layer）、核心服务层（Core Services layer）、核心操作系统层（Core OS layer）如图：

![图片](http://mmbiz.qpic.cn/mmbiz/8RTSPr4mlykpQPkcN0u41BL4DosIMoD3Jq6WmHlicQEAPE0nibB3Oj04iccY7KbM7pE7j70YUNlgWD7Js0zAN0Y6g/640?wx_fmt=jpeg&wxfrom=5&wx_lazy=1&wx_co=1)

(1)触摸层：为应用程序开发提供了各种常用的框架并且大部分框架与界面有关，本质上来说它负责用户在iOS设备上的触摸交互操作。它包括以下这些组件：

```
Multi-Touch Events  Core Motion   Camera

View Hierarchy   Localization   Alerts

Web Views     Image Picker   Multi-Touch Controls
```

(2)媒体层：通过它我们可以在应用程序中使用各种媒体文件，进行音频与视频的录制，图形的绘制，以及制作基础的动画效果。它包括以下这些组件：

```
Core Audio     OpenGL    Audio Mixing

Audio Recording   Video Playback   JPG，PNG，TIFF

PDF     Quartz    Core Animation 

OpenGL ES
```

(3)核心服务层：我们可以通过它来访问iOS的一些服务。它包括以下这些组件：

```
Collections     Address Book   Networking

File Access     SQLite    Core Location

Net Services    Threading    Preferences

URL Utilities
```

(4)核心操作系统层包括：内存管理、文件系统、电源管理以及一些其他的操作系统任务。它可以直接和硬件设备进行交互。核心操作系统层包括以下这些组件：

```
OS X Kernel    Mach 3.0    BSD

Sockets     Power Mgmt  File System

Keychain     Certificates   Security

Bonjour
```

## 3、Cocoa Touch简介

(1)在最上层Cocoa Touch层中的很多技术都是基于Objective-C语言的。Objective-C语言为iOS提供了集合、文件管理、网络操作等支持。比如UIKit框架，它为应用程序提供了各种可视化组件，比如像窗口（Window）、视图（View）和按钮组件（UIButton）。Cocoa Touch层中的其他框架，对我们在应用程序中的开发来说也是非常有用的，如访问用户通信录功能框架、获取照片信息功能的框架、负责加速感应器和三维陀螺仪等硬件支持的框架。

## 4、应用程序的框架

应用程序项目都是从Cocoa Touch层开始的，具体来说就是从UIKit Framework开始的。当在编写程序的过程中需要用到一些特殊功能的时候，我们应该从框架的最顶端技术开始寻找相应的框架，只有在上层结构无法解决时，才能使用其下层的技术。其实，顶层的框架已经涵盖了我们绝大多数需要的功能。

常用的iOS SDK框架：

![图片](http://mmbiz.qpic.cn/mmbiz/8RTSPr4mlykpQPkcN0u41BL4DosIMoD3jPzbXfQZXA3qXtTTyxh7JeNw9wANLWTlGUPv4BkjJ7gIWjkb5yFytA/640?wx_fmt=jpeg&wxfrom=5&wx_lazy=1&wx_co=1)
续表： 　　　　　　　　　　　　      　　　　　　　　　　　　　　　![图片](http://mmbiz.qpic.cn/mmbiz/8RTSPr4mlykpQPkcN0u41BL4DosIMoD3iaf3b40WOqSZ9UdmwwP9UyqtCiapcPQuuQgLkp8ONHx9V5IpG8Gouysg/640?wx_fmt=jpeg&wxfrom=5&wx_lazy=1&wx_co=1)



原文链接：http://www.cnblogs.com/leo_wl/p/3629606.html