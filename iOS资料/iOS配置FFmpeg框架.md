# iOS配置FFmpeg框架

一、下载一个Perl写的脚本文件，该脚本在执行编译脚本的时候需要依赖。https://github.com/libav/gas-preprocessor

解压下载好的文件，里面有一个 gas-preprocessor.pl 脚本文件，需要将该文件copy到 /usr/bin 目录下。终端执行：

sudo cp -f /Users/lotheve/Desktop/gas-preprocessor-master/gas-preprocessor.pl /usr/bin

第一个路径为本机中gas-preprocessor.pl所在路径，请自行修改。


注意：Capitan系统开始，苹果在EI Capitan系统中加入了Rootless机制，即使root权限下也无法对 /usr/bin 目录进行读写。解决办法见 http://www.jianshu.com/p/22b89f19afd6

确认文件已经copy到 /usr/bin 之后，修改该文件的权限为可读可写可执行：

chmod a+rwx gas-preprocessor.pl

二、下载脚本文件 https://github.com/kewlbear/FFmpeg-iOS-build-script

解压下载好的文件，里面有一个 build-ffmpeg.sh 文件，这个就是我们要用的编译脚本文件，打开可以查看编译配置信息。实际使用FFmpeg时我们往往只需要库中的部分功能，例如实现播放器，仅仅需要FFmpeg的解码功能，如果将整个库都编译，编译得到的静态库体积非常大，造成不必要的浪费。因此往往只对相关的模块进行编译，而就需要在配置文件里做一些设置，具体将在后面跟进。

执行编译。进入 build-ffmpeg.sh 文件所在的目录,执行脚本文件。在编译的时候会用到yasm汇编器，如果你的环境没有安装yasm，终端会给出提示。如果你安装了Homebrew包管理工具，那么在执行编译的时候会自动安装yasm（当然也可以事先用Homebrew安装: brew install ffmpeg）。

等必要环境准备完毕之后，终端开始下载FFmpeg库，之后开始编译，编译结束之前全程无需干预，可以来杯咖啡压压惊。要是不出意外，一杯咖啡的工夫差不多就编译完毕了。然而我一开始在编译的时候采到一个坑，每次编译到armv7的时候，总是提示 ffmpeg GNU assembler not found, install/update gas-preprocessor报错。检查该支持库的位置，已经在 /usr/bin 中乖乖呆着了，权限也已经设置好，这弄得我一头雾水。折腾了好久最后在网上翻别人博客，才知道自己采了个坑。原因在于之前下载的 gas-preprocessor.sh 版本太老了！我一看last commit 4 years ago！关键是该项目有近200的星，也一直没更新，太坑了。最后换了一个较新的版本，顺利编译。

看一下编译完毕后文件夹的内容：


ffmpeg-3.0：FFmpeg库的源码，当前版本为3.0
FFmpeg-iOS：编译好的iOS下使用的FFmpeg Library库，费尽周折全为它！里面的include文件夹里是相关接口，lib是编译出来的几个静态库。可以用如下命令查看指定静态库的支持架构，发现均支持armv7 i386 x86_64 arm64。

lipo -info libavcodec.a

三、将FFmpeg-iOS库导入工程

直接将FFmpeg-iOS拖到工程里，在一个m文件中 #include "avformat.h" 引入该头文件测试，添加一条执行语句 av_register_all();。
设置头文件搜索路径 在 Header Search Paths 中添加路径 $(PROJECT_DIR)/FFmpeg-iOS/include：

引入依赖库

libiconv.tbd
libbz2.tbd
libz.tbd
本以为完事具备，运行一看，报错如下：

网上一查，还需要添加几个系统库：

CoreMedia.framework
VideoToolbox.framework
再次运行，Build Succeeded！

原文链接：https://blog.csdn.net/lotheve/article/details/51517875