## **写在前面**

最近在做和转码有关的项目，接触到ffmpeg这个神器。从一开始简单的写脚本直接调用ffmpeg的可执行文件做些转码的工作，到后来需要写程序调用ffmpeg的API。虽然上网搜了别人的demo稍微改改顺利完成了工作，但是对于ffmpeg这个黑盒子，还是有些好奇心和担心（项目中使用不了解的代码总是不那么放心），于是抽空翻了翻ffmpeg的源码，整理成文章给大家分享分享。

由于我并非做音频出身，对于音频一窍不通。ffmpeg整个也非常庞大，所以这篇文章从ffmpeg提供的转码的demo开始，侧重于讲清楚整个输入->转码->输出的流程，并学习ffmpeg如何做到通用和可扩展性。

注：本文基于ffmpeg提供的transcode_aac.c样例。

## **三个重点**

转码的过程是怎么样的？简单来说就是从输入读取数据，解析原来的数据格式，转成目标数据格式，再将最终数据输出。这里就涉及到三个**点**：**数据输入和输出方式**，**数据的编码方式**及**数据的容器格式**（容器是用来区分不同文件的数据类型的，而编码格式则由音视频的压缩算法决定，一般所说的文件格式或者后缀名指的就是文件的容器。对于一种容器，可以包含不同编码格式的一种视频和音频）。

ffmpeg是一个非常非常通用的工具，支持非常广的数据输入和输出，包括：hls流，文件，内存等，支持各类数据编码格式，包括：aac，mp3等等，同时支持多种容器格式，包括ts，aac等。另外ffmpeg是通过C语言实现的，如果是C++，我们可以通过继承和多态来实现。定义一个IO的基类，一个Format的基类和一个Codec的基类，具体的输入输出协议继承IO基类实现各自的输入输出方法，具体的容器格式继承Format基类，具体的编码格式继承Codec基类。这篇文章也会简单讲解ffmpeg如何用C语言实现类似C++的继承和多态。

## **基本数据结构**

ffmpeg转码中最基本的结构为AVFormatContext和AVCodecContext。AVCodecContext负责编码，AVFormatContext负责IO和容器格式。

我从AVFormatContext类抽离出三个基本的成员iformat，oformat，pb。分别属于AVInputFormat，AVOutputFormat，AVIOContext类。iformat为输入的数据格式，oformat为输出的数据格式，pb则负责输入输出。

![img](https://pic4.zhimg.com/v2-2632063d41df7a34fb60304434a75f1b_b.png)



我把这三个类的定义抽离了出来简化了下，可以看出AVInputFormat声明了read_packet方法，AVOutputFormat声明了write_packet方法，AVIOContext声明了read_packet, write_packet方法。同时AVInputFormat和AVOutputFormat还有一个成员变量name用以标识该格式的后缀名。

![img](https://pic3.zhimg.com/v2-58239fb65a540dc8b19241d70f7c9e7a_b.png)



下一节我们会看到Input/OutputForm的read/write packet方法和IOContext的关系。

## **输入函数调用图**

下面是初始化输入的整个过程的函数调用图。

![img](https://pic3.zhimg.com/v2-4b021abefdc7ab53c82833f722bf6e7a_b.png)



首先从调用open_input_file开始，首先**解析输入的protocol**。avio_open2函数会调用一系列helper函数（ffurl_open，ffio_fdopen）分析输入的协议，设置AVFormatContext的pb变量的read_packet方法。而av_probe_input_buffer2函数则会**分析输入文件的格式**（从文件名解析或输入数据做判断），设置AVFormatContext的iformat的read_packet方法。

![img](https://pic2.zhimg.com/v2-e9a529f86175c4cb2f21aa7f190e1769_b.png)



两个read_packet有什么关系呢？第二个函数调用图可以看出，iformat的read_packet最终会调用pb的read_packet方法。意思就是**数据本身由pb的read_packet方法来读取，而iformat则会在输入的数据上做些格式相关的解析操作**（比如解析输入数据的头部，提取出输入数据中真正的音频/视频数据，再加以转码）。

## **IO相关代码**

直接看上面的图不太直观，这一节我把源码中各个步骤截图下来进行分析。

转码开始步骤，调用open_input_file函数，传入文件名。

![img](https://pic3.zhimg.com/v2-31edf2063a8c04656557b946311f7492_b.png)

avformat_open_input函数会调用init_input()来处理输入文件。

![img](https://pic2.zhimg.com/v2-71305dac46d81e0e44fff17439bbabe9_b.png)

init_input函数主要做两个事情，一是解析输入协议（如何读取数据？hls流？文件？内存？），二是解析输入数据的格式（输入数据为aac？ts？m4a？）

![img](https://pic4.zhimg.com/v2-d37bf73363d41ea844ac698758a0b353_b.png)

avio_open2函数首先调用ffurl_open函数，根据文件名来推断所属的输入协议（URLProtocol）。之后再调用ffio_fdopen设置pb的read_packet方法。

![img](https://pic3.zhimg.com/v2-89e21351616f4b8c65dd386b8a41dc9e_b.png)



![img](https://pic1.zhimg.com/v2-1e33906ff3c6f718970753d4eda2aaf0_b.png)



![img](https://pic4.zhimg.com/v2-aeb0dbfcf245564406274ff644d63123_b.png)



上面几段代码的逻辑为：根据文件名查找对应的URLProtocol->把该URLProtocol赋值给URLContext的prot成员变量->创建AVIOContext实例，赋值给AVFormatContext的pb成员变量。

![img](https://pic3.zhimg.com/v2-2620110211da572988203229d29d0416_b.png)



![img](https://pic4.zhimg.com/v2-fa4a5834ba7b3709ca9c5af2e1349b37_b.png)

这里设置了AVIOContext实例的read_packet为ffurl_read方法。

![img](https://pic4.zhimg.com/v2-520147188f5f044fed71dd64776a75ff_b.png)

ffurl_read方法其实就是调用URLContext的prot（上面赋值的）的url_read方法。通过函数指针去调用具体的URLContext对象的prot成员变量的url_read方法。

![img](https://pic2.zhimg.com/v2-c250ed674ddac54fd9d3ef80b070a729_b.png)

接下来看看解析输入数据格式的代码。av_probe_input_buffer2函数调用av_probe_input_format2函数来推断数据数据的格式。从之前的图我们知道*fmt其实就是&s->iformat。因此这里设置了AVFormatContext的iformat成员变量。

![img](https://pic1.zhimg.com/v2-bfcaa2cb999687df6ddb09c1165f5310_b.png)



至此AVFormatContext对象的iformat和pb成员变量就设置好了。接下来看看如何读取输入开始转码。

av_read_frame函数调用read_frame_internal函数开始读取数据。

![img](https://pic2.zhimg.com/v2-8dfe519e59eab253c3360e4eed36eef5_b.png)



read_frame_internal会调用ff_read_packet，后者最终调用的是iformat成员变量的read_packet方法。

![img](https://pic2.zhimg.com/v2-98b6b38ae8793c193d97ea70c1e78e39_b.png)



![img](https://pic2.zhimg.com/v2-b47fb24bdbf9657b3ac1921dab6d4749_b.png)



拿aac举例，aac的read_packet方法实际上是ff_raw_read_partial_packet函数。

![img](https://pic3.zhimg.com/v2-bfd1eeaa1f4cd1c5733dc1392782f5d2_b.png)



ff_raw_read_partial_packet会调用ffio_read_partial，后者最终调用的是AVFormatContext的pb成员变量的read_packet方法。而我们知道pb成员的read_packet其实就是ffurl_read，也就是具体输入URLProtocl的read_packet方法。

![img](https://pic2.zhimg.com/v2-e529b8f31ed3f9c136c38197a6f896e5_b.png)



![img](https://pic2.zhimg.com/v2-c6f53da467f8278936ed779d2182e3dd_b.png)



至此已经走完了整个输入的流程，输出也是类似的代码，这里就不再赘述。

## **转码函数调用图**

上面关于IO的介绍我从输入的角度进行分析。接下来的转码过程我则从输出的角度进行分析。下图是转码过程的函数调用图（做了简化）。load_encode_and_write调用encode_audio_frame, encode_audio_frame调用avcodec_encode_audio2来做实际的编码工作，最后调用av_write_frame将编码完的数据写入输出。

![img](https://pic4.zhimg.com/v2-673c2b1a1d1db45ad83e93e4149f487f_b.png)

## **转码相关代码**

首先需要设置输出目标编码格式，下面的代码为设置编码格式（aac）的片段：

![img](https://pic1.zhimg.com/v2-b7db4b56403fca052660be8b437e1df4_b.png)



在这里设置了output_codec_context（AVCodecContext类对象）之后，从前面的函数调用图，我们知道是avcodec_encode_audio2函数执行的转码过程：

![img](https://pic3.zhimg.com/v2-c24e2a95d172b8b5a6006097b565d48e_b.png)



这里看到调用了avctx（AVCodecContext类对象）的codec（AVCodec类对象）成员变量的encode2方法去做编码操作。

转码这里专业性比较强，我并没有细读，因此这里简单带过。

## **总结**

可以看出ffmpeg大量使用函数指针来实现类似C++的继承/多态的效果。并且ffmpeg具有非常好的扩展性。如果我需要自定义一个新的输入协议，只需要自己定义一个新的URLProtocol对象，实现read_packet方法即可。如果需要自定义一个新的容器格式，只需要定义一个新的AVInputFormat对象，实现read_packet方法即可。如果需要自定义一个新的编码格式，只需要定义一个新的AVCodec对象，实现encode2方法即可。真是非常赞的代码架构设计！

