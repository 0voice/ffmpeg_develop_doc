首先需要去FFmpeg的官网http://www.ffmpeg.org/去下载FFmpeg的源码，目前的版本号为FFmpeg3.3(Hilbert)。

下载的文件为压缩包，解压后得到ffmpeg-3.3目录。

**修改ffmpeg-3.3的configure文件：**

```
# 原来的配置内容：
SLIBNAME_WITH_MAJOR='$(SLIBNAME).$(LIBMAJOR)'

LIB_INSTALL_EXTRA_CMD='$$(RANLIB)"$(LIBDIR)/$(LIBNAME)"'

SLIB_INSTALL_NAME='$(SLIBNAME_WITH_VERSION)'

SLIB_INSTALL_LINKS='$(SLIBNAME_WITH_MAJOR)$(SLIBNAME)'

#替换后的内容：

SLIBNAME_WITH_MAJOR='$(SLIBPREF)$(FULLNAME)-$(LIBMAJOR)$(SLIBSUF)'

LIB_INSTALL_EXTRA_CMD='$$(RANLIB)"$(LIBDIR)/$(LIBNAME)"'

SLIB_INSTALL_NAME='$(SLIBNAME_WITH_MAJOR)'

SLIB_INSTALL_LINKS='$(SLIBNAME)'
```

原因：如果不修改配置，直接进行编译出来的so文件类似libavcodec.so.55.39.101，文件的版本号位于so之后，这样在Android上无法加载，所以需要修改！

**编写build_android.sh脚本文件:**

在编译FFmpeg之前需要进行配置，设置相应的环境变量等。所有的配置选项都在ffmpeg-3.3/configure这个脚本文件中，执行如下命令可查看所有的配置选项：

$ ./configure –help

下面将配置项和环境变量设置写成一个sh脚本文件来运行以便编译出Android平台需要的so文件出来。

build_android.sh的内容如下：

```
#!/bin/bash
NDK=/Users/renhui/framework/android-ndk-r14b
SYSROOT=$NDK/platforms/android-9/arch-arm/
TOOLCHAIN=$NDK/toolchains/arm-linux-androideabi-4.9/prebuilt/darwin-x86_64

function build_one
{
./configure \
    --prefix=$PREFIX \
    --enable-shared \
    --disable-static \
    --disable-doc \--enable-cross-compile \
    --cross-prefix=$TOOLCHAIN/bin/arm-linux-androideabi- \
    --target-os=linux \
    --arch=arm \
    --sysroot=$SYSROOT \
    --extra-cflags="-Os -fpic $ADDI_CFLAGS" \
    --extra-ldflags="$ADDI_LDFLAGS" \
    $ADDITIONAL_CONFIGURE_FLAG
}
CPU=arm
PREFIX=$(pwd)/android/$CPU
ADDI_CFLAGS="-marm"
build_one
```

需要确定的是NDK,SYSROOT和TOOLCHAIN是否是本地的环境，并确定cross-prefix指向的路径存在。

保存脚本文件后，将脚本的权限提升：

```
chmod 777 build_android.sh  
```

然后执行脚本，该脚本会完成对ffmpeg的配置，并生成config.h等配置文件，后面的编译会用到。如果未经过配置直接进行编译会提示无法找到config.h文件等错误。

然后执行下面两个命令：

```
$make  
$make install  
```

至此，会在ffmpeg-3.3目录下生成一个android目录，其/android/arm/lib目录下的so库文件就是能够在Android上运行的so库。

 

创建Demo工程，测试上面生成的so文件能否正常使用：

1. 创建一个新的Android工程

2. 在工程根目录下创建jni文件夹 

3. 在jni下创建prebuilt目录，然后：将上面编译成功的so文件放入到该目录下

4. 创建包含native方法的类，先在src下创建cn.renhui包，然后创建FFmpegNative.java类文件。主要包括加载so库文件和一个native测试方法两部分，其内容如下： 

   ```
   package cn.renhui;
   
   public class FFmpegNative {
   
       static {
           System.loadLibrary("avutil-55");
           System.loadLibrary("avcodec-57");
           System.loadLibrary("swresample-2");
           System.loadLibrary("avformat-57");
           System.loadLibrary("swscale-4");
           System.loadLibrary("avfilter-6");
           System.loadLibrary("avdevice-57");
           System.loadLibrary("ffmpeg_codec");
       }
   
       public native int avcodec_find_decoder(int codecID);
   } 
   ```

1. 用javah创建.头文件: classes目录，执行：javah-jni cn.renhui.FFmpegNative，会在当前目录产生cn_renhui_FFmpegNative.h的C头文件;

1. 根据头文件名，建立相同名字c文件cn_renhui_FFmpegNative.c，在这个源文件中实现头文件中定义的方法，代码如下：

   ```
   #include "cn_renhui_FFmpegNative.h"
   
   #ifdef __cplusplus
   extern "C" {
   #endif
   
   JNIEXPORT jint JNICALL Java_cn_renhui_FFmpegNative_avcodec_1find_1decoder
     (JNIEnv *env, jobject obj, jint codecID)
   {
       AVCodec *codec = NULL;
   
       /* register all formats and codecs */
       av_register_all();
   
       codec = avcodec_find_decoder(codecID);
   
       if (codec != NULL)
       {
           return 0;
       }
       else
       {
           return -1;
       }
   }
   
   #ifdef __cplusplus
   }
   #endif
   ```

1. 编写Android.mk，内容如下：

   ```
   LOCAL_PATH := $(call my-dir)
   
   include $(CLEAR_VARS)
   LOCAL_MODULE := avcodec-57-prebuilt
   LOCAL_SRC_FILES := prebuilt/libavcodec-57.so
   include $(PREBUILT_SHARED_LIBRARY)
   
   include $(CLEAR_VARS)
   LOCAL_MODULE := avdevice-57-prebuilt
   LOCAL_SRC_FILES := prebuilt/libavdevice-57.so
   include $(PREBUILT_SHARED_LIBRARY)
   
   include $(CLEAR_VARS)
   LOCAL_MODULE := avfilter-6-prebuilt
   LOCAL_SRC_FILES := prebuilt/libavfilter-6.so
   include $(PREBUILT_SHARED_LIBRARY)
   
   include $(CLEAR_VARS)
   LOCAL_MODULE := avformat-57-prebuilt
   LOCAL_SRC_FILES := prebuilt/libavformat-57.so
   include $(PREBUILT_SHARED_LIBRARY)
   
   include $(CLEAR_VARS)
   LOCAL_MODULE :=  avutil-55-prebuilt
   LOCAL_SRC_FILES := prebuilt/libavutil-55.so
   include $(PREBUILT_SHARED_LIBRARY)
   
   include $(CLEAR_VARS)
   LOCAL_MODULE :=  avswresample-2-prebuilt
   LOCAL_SRC_FILES := prebuilt/libswresample-2.so
   include $(PREBUILT_SHARED_LIBRARY)
   
   include $(CLEAR_VARS)
   LOCAL_MODULE :=  swscale-4-prebuilt
   LOCAL_SRC_FILES := prebuilt/libswscale-4.so
   include $(PREBUILT_SHARED_LIBRARY)
   
   include $(CLEAR_VARS)
   
   LOCAL_MODULE := ffmpeg_codec
   LOCAL_SRC_FILES := cn_dennishucd_FFmpegNative.c
   
   LOCAL_LDLIBS := -llog -ljnigraphics -lz -landroid
   LOCAL_SHARED_LIBRARIES := avcodec-57-prebuilt avdevice-57-prebuilt avfilter-6-prebuilt avformat-57-prebuilt avutil-55-prebuilt
   
   include $(BUILD_SHARED_LIBRARY)
   ```

1. 编译so文件，执行ndk-build

2. 新建一个Activity，进行测试，测试核心代码：

   ```
   FFmpegNative ffmpeg = new FFmpegNative();
   
   int codecID = 28;
   
   int res = ffmpeg.avcodec_find_decoder(codecID);
   
   if (res == 0) {
        tv.setText("Success!");
   } else {
        tv.setText("Failed!");
   }
   ```

​    28是H264的编解码ID，可以在ffmpeg的源代码中找到，它是枚举类型定义的。在C语言中，可以换算为整型值。这里测试能否找到H264编解码，如果能找到，说明调用ffmpeg的库函数是成功的，这也表明我们编译的so文件是基本可用。

