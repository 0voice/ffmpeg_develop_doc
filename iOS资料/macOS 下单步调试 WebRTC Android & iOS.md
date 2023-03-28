# macOS 下单步调试 WebRTC Android & iOS

上一篇文章里有位读者朋友咨询我，如何调试 WebRTC iOS demo。显然这个小问题不值一篇文章，所以这周我就花了大量的精力，解决了长久以来困扰广大 WebRTC 安卓开发者的难题：如何在 Android Studio 里单步调试 WebRTC Android 的 native 代码。

今天我就在这里给大家带来一场盛宴 :)

## 1、WebRTC 代码下载

首先给各位上点冰镇白开，解解暑：

- depot tools 是 chromium 代码库管理工具，包括代码管理、依赖管理、工作流程管理等；
- Android/Linux、Windows、iOS/macOS WebRTC 本身的代码是同一个仓库，但依赖工具不同，所以不可能放到一起，我的移动硬盘里就有 `webrtc_android` 和 `webrtc_ios` 两个目录，一共 46 GB；
- depot tools 的运行基于 python 2.x 环境，且需要是官方 build（`--version` 选项不能输出额外信息）；
- 各个系统的 buildtools 是需要运行 `gclient runhooks` 进行下载的，而且是自动检测运行时的系统，只下载当前系统的；
- gn/clang format 下载地址：https://storage.googleapis.com/chromium-clang-format/0679b295e2ce2fce7919d1e8d003e497475f24a3 , https://storage.googleapis.com/chromium-gn/9be792dd9010ce303a9c3a497a67bcc5ac8c7666 , 替换 hash 值即可，其他 `download_from_google_storage` 的步骤都可以这样解决（替换 bucket 和 hash）；

顺便说说我下载代码的方式：

- 首先准备一台可上网的路由器，我的是华硕 RT-AC68U + koolshare `380.63_0-X7.2`（升级后 format jffs at next boot 并重启） + 离线安装科学上网插件；
- 然后在阿里云香港开一个按量付费的最低配云主机，0.07 元/小时，流量 1 元/GB，速度很快，能到 MB/s 级别，完整下载花费也不会超过 50 元，下完之后释放；
- 把配好 ss 及 ss 自启动的镜像做成自定义镜像，并创建创建实例模板，再需要更新代码时一键启动无需配置，只需到路由器后台改下 IP 即可；
- 有了这个环境之后，也就可以轻松给 WebRTC 提交 patch 了，我都提交了好几个 patch 了；

## 2、Xcode 调试 WebRTC iOS demo

在迎来 Android Studio 这道大菜之前，我们先用 Xcode 开个胃：

- `src/examples/BUILD.gn` 中，搜索 `ios_app_bundle("AppRTCMobile")`，为其中增加以下内容（bundle id 设置为实际使用的独特 id）：

```
extra_substitutions = [
"PRODUCT_BUNDLE_IDENTIFIER=com.github.piasy.AppRTCMobile",
]
```

- `src` 目录下执行 `gn gen out/xcode_ios_arm64 --args='target_os="ios" target_cpu="arm64"' --ide=xcode`；
- 用 Xcode 打开 `src/out/ios/all.xcworkspace`，run target 选择 `AppRTCMobile`，工程文件的设置 target 也选择 `AppRTCMobile`；

![图片](https://mmbiz.qpic.cn/mmbiz_png/gGwQfiaEiaqjib7Xl7bjbIso3kfKHiaicczvSmpm8dzGyTgWMaS1Rno704w8uyJsq2cZZUcsISxaRkeW5CTc7emWd2g/640?wx_fmt=png&wxfrom=5&wx_lazy=1&wx_co=1)

- 修改 `src/examples/objc/AppRTCMobile/ios/Info.plist`，设置同样的 bundle id；
- 在工程文件的 general tab 中，手动选择 `Info.plist` 为 `src/examples/objc/AppRTCMobile/ios/Info.plist`；选择了 plist 之后，可能 Xcode 不会显示 bundle id 等信息，这是 Xcode 的一个小 bug，先查看一个其他文件，再查看工程文
  件，就能看到了；
- 勾选「Automatically manage signing」，选择合适的 team；
- 点击 run 即可，和调试普通 iOS app 一样；
- clean 工程并不会清除 ninja 脚本编译的结果，所以不必担心耗时；
- 更新代码后，可能需要删掉老的 `src/third_party/llvm-build/` 目录，然后执行 `gclient run_hooks` 下载新的 llvm；
- 在 `examples/objc/AppRTCMobile/ARDAppEngineClient.m` 里，修改 `kARDRoomServerHostUrl`, `kARDRoomServerJoinFormat`, `kARDRoomServerJoinFormatLoopback`, `kARDRoomServerMessageFormat`, `kARDRoomServerLeaveFormat` 这四个变量的域名为实际部署的 AppRTC server 域名/地址，即可连接自己的 server；

## 3、Android Studio 调试 WebRTC Android demo

好了，现在开始正餐部分 :)

首先，官方有个 issue，讲的就是 Android Studio 的支持，遗憾的是现在还没有解决方案。

但是，不就是编译么，手写一个 CMakeLists.txt 就好了嘛（在此省略描述编写 CMakeLists.txt 过程中解决各种问题的五千字）。

写好了的 CMakeLists.txt 和 build.gradle 就在这里！就在这里！！就在这里！！！ *超链接文字这么长，应该能点到吧 :)*

注：这个 CMakeLists.txt 基于 `#24277` 提交，且只适配了 arm64-v8a 架构，其他代码版本、架构可能存在问题，不保证可以使用。此外，一定要搭配工程里的 build.gradle 使用，因为还要靠它生成一些代码。

不过 clone 下来之后，别急着用 Android Studio 打开，首先要修改一下 `libjingle_peerconnection/build.gradle`，设置以下变量：

- `webrtc_repo`: WebRTC Android 代码仓库路径；
- `android_jar`: Android SDK 的 android.jar 路径；
- `py2`: Python 2.x 可执行文件的路径；
- `protoc`: protobuf 编译程序的路径，注意它的版本需要和 WebRTC 代码库匹配，我没找到这个程序在代码库里的位置，但我发现用 gn + ninja 编译一次后，会在 `out/dir/clang_x64` 目录下生成这个程序；

修改完了 build.gradle 之后也不要着急，还要修改 Android Studio 启动配置，编辑 `~/Library/Preferences/AndroidStudio3.1/studio.vmoptions`（替换为正确版本），修改 `-Xms`, `-Xmx`, `-XX:MaxPermSize`, `-XX:ReservedCodeCacheSize` 这四个参数：

```
-Xms4096m
-Xmx14336m
-XX:+UseG1GC
-XX:-UseParNewGC
-XX:-UseConcMarkSweepGC
-XX:MaxPermSize=4096m
-XX:ReservedCodeCacheSize=2048m
-XX:+UseCompressedOops
-XX:-OmitStackTraceInFastThrow
-Dsun.io.useCanonCaches=false
```

最后，还要把 CMakeLists.txt 的内容注释了一部分后再打开，sync 成功后解除一部分注释，再 sync，再解除注释，否则 Android Studio build symbols 一整天也不见完。*当然，土豪顶配 MBP 也许可以直接打开。*

对了，我使用的 Android Studio 版本是 3.1.4。

好了，为了表达我激动的心情，放上一个录屏视频：

**视频链接：**[https://blog.piasy.com/2018/08/14/build-webrtc/](https://blog.piasy.com/2018/08/14/build-webrtc/index.html)

## 4、macOS 下 用 gn + ninja 编译 WebRTC Android

Android Studio 的调试毕竟只是为了加断点做一些流程分析，而且写出来的 CMakeLists.txt 和 build.gradle 也只能算是一个临时解决方案，靠谱的编译方式，还得是 gn + ninja，那么接下来我就补上这道餐后甜点。

- 先在 docker 镜像里 sync 好 linux 的版本（直接在 macOS 里下载我猜应该也可以，但我没试过，欢迎大家尝试后分享结果）；
- 创建 `src/third_party/android_tools_mac` 目录，并把 macOS 的 ndk 和 sdk 放入其中；
- 修改 `src/build/config/android/config.gni`:

```
declare_args() {
android_ndk_root = "//third_party/android_tools_mac/ndk"
android_ndk_version = "r16"
android_ndk_major_version = 16

android_sdk_root = "//third_party/android_tools_mac/sdk"
android_sdk_version = 28
android_sdk_build_tools_version = "27.0.3"
android_sdk_tools_version_suffix = "-26.0.0-dev"

lint_android_sdk_root = "//third_party/android_tools_mac/sdk"
lint_android_sdk_version = 26
```

- 修改 `src/build/toolchain/gcc_solink_wrapper.py` 末尾部分：

```
# Finally, strip the linked shared object file (if desired).if args.strip:
    result = subprocess.call(wrapper_utils.CommandToRun(
    [args.readelf[:-7] + "strip", '-o', args.output, args.sofile]))
```

- 下载 `src/buildtools/mac` 下定义版本的 macOS 版的 `clang-format` 和 `gn`，下载方法见上文；
- 本地 build llvm，因为下载的 macOS 版本都没有 llvm-ar 这个程序，build 命令：`env LLVM_FORCE_HEAD_REVISION=1 ./src/tools/clang/scripts/update.py`；
- 把 `$JAVA_HOME/bin` 加到 PATH `/usr/bin` 的前面，这样找到的就会是正确的 jdk 路径，就能找到 rt.jar 了，否则会报错 `No such file or directory: '/System/Library/Frameworks/JavaVM.framework/Versions/A/jre/lib/rt.jar'`；
- 确保在 Python 2.x 的 shell 里执行 gn 和 ninja 即可编译；

## 5、Windows 编译和使用静态库

嗯……最后还有这么个鸡肋，弃之可惜，还是放在最后好了，且将其定义为残羹冷炙 :)

- 首先可能会提示 `winsock.h` 和 `winsock2.h` 里的符号重定义，这是因为 `windows.h` 在 `winsock2.h` 之前被 include 导致的，我们在包含 WebRTC 头文件之前，先 `#include winsock2.h` 即可；
- 对 std min max 的使用导致 `error C2589: '(' : illegal token on right side of '::'` 错误，在项目中增加 `NOMINMAX` 宏定义即可解决；
- 对 `av_err2str()` 的使用会导致 `error C4576: a parenthesized type followed by an initializer list is a non-standard explicit type conversion syntax` 错误，暂时无解，只能去掉；
- ninja 编出来的 `webrtc.lib` 太小（7MB 左右），且 VS 提示文件已损坏……我发誓之前编译成功过一个版本，但后来突然就不行了，已弃疗，不过论坛里的这个帖子也许有用；

## 6、总结

好了，WebRTC 编译和调试的这场盛宴就到此为止了，希望大家喜欢。热心的朋友，可以给这个 GitHub 仓库贡献一个 star，或者 follow 我，再会 :)

点击查看原文链接，获取所有链接。



原文链接：[https://blog.piasy.com/2018/08/14/build-webrtc/](