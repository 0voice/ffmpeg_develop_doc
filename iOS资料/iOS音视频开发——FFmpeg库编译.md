# iOS音视频开发——FFmpeg库编译

## 1.安装 Homebrew

**Homebrew：**是 Mac 平台上的一个包管理工具，提供了许多 Mac 下没有的 Linux工具等，
而且安装工具极其简单，一句命令行的事。

1. 检查是否已安装 `Homebrew`，只需在终端输入：

```text
$ brew
```

2.得到如图结果，说明已经安装，否则需要安装。

![img](https://pic2.zhimg.com/80/v2-84818233bf0e17574fa9adbb86033c89_720w.webp)

3.安装 `Homebrew` ，在终端输入：

```text
$ ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
```

## 2.编译 iOS 版可用的 FFmpeg库

> 需要用到的工具：

1. gas-preprocessor
2. yasm
3. FFmpeg-iOS-build-script

## 3.下载 gas-preprocessor

> gas-preprocessor 就是我们要编译 FFmpeg 的所需脚本文件。

1. 将其解压后，其内部只有简单的 4 个文件，如下图：

![img](https://pic1.zhimg.com/80/v2-4d1c8fac8a887c38be36172ff4df5f18_720w.webp)

2.将 **gas-preprocessor.pl** 文件复制到 `/usr/local/bin/` 目录下，然后为文件开启可执行权限：

```text
$ chmod 777 /usr/local/bin/gas-preprocessor.pl
```

## 4.安装 yasm

> Yasm是一个完全重写的 NASM 汇编。目前，它支持x86和AMD64指令集，接受NASM和气体汇编语法，产出二进制，ELF32 ， ELF64 ， COFF ， Mach - O的（ 32和64 ），RDOFF2 ，的Win32和Win64对象的格式，并生成STABS 调试信息的来源，DWARF 2 ，CodeView 8格式。

1. 下载 yasm：

```text
$ brew install yasm
```

![img](https://pic3.zhimg.com/80/v2-4ae32b038ed108a2a11dcf9ba1c43b9a_720w.webp)

2.检测是否已安装 yasm

```text
$ brew install yasm
```

![img](https://pic3.zhimg.com/80/v2-98c23d9d3374d2c77a38f23f6fbee082_720w.webp)

3.编译 `FFmpeg` 脚本如图：

![img](https://pic1.zhimg.com/80/v2-68c7746aa926d9391b92d18714bf0d24_720w.webp)

## 5.编译 FFmpeg-iOS-build-script，得到我们需要的 iOS 能用的 ffmpeg 库

> 这个脚本是转为 `iOS` 编译出可用的 `ffmpeg` 的库，有了这个脚本，就不用下载 ffmpeg 了，脚本会自动下载好最新版本的 ffmpeg，并打包成一个 iOS 可用的 ffmpeg 库提供给我们了

1. 下载FFmpeg-iOS-build-script压缩包。
2. 解压 FFmpeg-iOS-build-script 得到的文件如下：

![img](https://pic4.zhimg.com/80/v2-cefee949add18eb65069f937d4cc32c7_720w.webp)

3.终端 cd 到文件的目录，然后执行以下命令，编译脚本，打包出我们需要的 iOS 的 ffmpeg 库：

```text
$ ./build-ffmpeg.sh
```

4.编译完成后，就可以得到 `FFmpeg` 源码以及我们需要的 `lib` :

![img](https://pic3.zhimg.com/80/v2-f6706caed048cd5f89acb77ed4b6dfba_720w.webp)

## 6.集成FFmpeg 库开发工程当中

1. 把 FFmpeg-iOS 直接复制到你的工程目录下，如图：

![img](https://pic1.zhimg.com/80/v2-e7682effea8758e33155c42cd81692a4_720w.webp)

2.设置环境：进入 `Build Setting` ，修改 `header search Path` 链接到工程的 include 文件当中 操作如下：

![img](https://pic2.zhimg.com/80/v2-ec1b7ad9a5600ec52af045c2dc30257d_720w.webp)

3.至此，已经成功编译并集成了 ffmpeg 了。

原文链接：[iOS音视频开发--FFmpeg库编译 - 资料 - 我爱音视频网 - 构建全国最权威的音视频技术交流分享论坛](