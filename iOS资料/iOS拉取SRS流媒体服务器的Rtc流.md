# iOS拉取SRS流媒体服务器的Rtc流

## 1、搭建flutter环境

已经有的可以跳过
安装教程：https://flutter.cn/docs/get-started/install/macos
1.下载以下安装包 stable Flutter SDK：
下载地址：https://flutter.dev/docs/development/tools/sdk/releases
2.将文件解压到目标路径

```text
cd ~/Desktop
mkdir development
cd ~/development
 unzip ~/Downloads/flutter_macos_3.0.3-stable.zip  // 这里要替换成你下载的文件名
```

3.永久配置 flutter 的 PATH 环境变量：

macOS Catalina 操作系统默认使用 Z Shell，所以需要修改 $HOME/.zshrc 文件。

```text
vim $HOME/.zshrc
```

文件中增加下列这行命令

```text
export PATH="$PATH:/Users/zyb/Desktop/development/flutter/bin"
```

关闭终端然后重新打开

4.验证 flutter 命令是否可用，可以执行下面的命令检测：

```text
 which flutter
```

5.运行 flutter doctor 命令

```text
flutter doctor
```

## 2、设置 iOS 开发环境 

1. 通过 直接下载 或者通过 Mac App Store 来安装最新稳定版 Xcode；
2. 配置 Xcode 命令行工具以使用新安装的 Xcode 版本。从命令行中运行以下命令：

```text
$ sudo xcode-select --switch /Applications/Xcode.app/Contents/Developer
$ sudo xcodebuild -runFirstLaunch
```

## 3、运行flutter_live项目

1.下载flutter_live和配置项目

```text
git clone https://github.com/ossrs/flutter_live.git
cd flutter_live
cd example
git checkout -b ios origin/ios
flutter run
```

2.安装iOS依赖项目

```text
flutter packages get
flutter pub get
pod update
```

这里会报错，把flutter_live/example/android/app/src/main/AndroidManifest.xml改成

```text
android:name="${applicationName}"
```

![img](https://pic4.zhimg.com/80/v2-363e1378353c0dbfee645a405c3e8f2b_720w.webp)

3.配置编译证书（个人就行）

![img](https://pic3.zhimg.com/80/v2-d7d0d238efa7d1651cdc7b349d5f5a36_720w.webp)

## 4、配置环境进行拉流

![img](https://pic4.zhimg.com/80/v2-9dcb02c7bab127433ca1e5460f68a593_720w.webp)

配置环境界面

![img](https://pic1.zhimg.com/80/v2-7ab56ba26204d5a48e95cdcd63b8b514_720w.webp)

拉流成功

原文链接：[iOS拉取SRS流媒体服务器的Rtc流 - 资料 - 我爱音视频网 - 构建全国最权威的音视频技术交流分享论坛](