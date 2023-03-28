# 基于 AVFoundation 框架开发小视频功能的方案解析

开发视频录制功能最简单的就是使用系统封装的 UIImagePickerController，但是这种方式比较封闭，可自定义东西比较少，所以就需要基于 AVFoundation 框架来开发视频录制功能。基于 AVFoundation 框架来开发，则需要自己手动设置设备音频，视频输入、输出。

AVCaptureSession 是 AVFoundation 的核心类，用于管理捕获对象 AVCaptureInput 的视频和音频的输入，协调捕获的输出 AVCaptureOutput。AVCaptureOutput 的输出有两种方法：一种是直接以 movieFileUrl 方式输出；一种是以原始数据流 data 的方式输出，流程对比图如下：

![img](https://p3-sign.toutiaoimg.com/pgc-image/RxRUAMm47IR6ec~noop.image?_iz=58558&from=article.pc_detail&x-expires=1679402928&x-signature=RgONu1b%2F4Ei41%2FEPFHeQK%2FgHFUA%3D)

下面详细讲解两种录制视频的方案：

（1）AVCaptureSession + AVCaptureMovieFileOutput

\1. 创建 AVCaptureSession

![img](https://p3-sign.toutiaoimg.com/pgc-image/RxRUANXIJMbVmK~noop.image?_iz=58558&from=article.pc_detail&x-expires=1679402928&x-signature=yBCiU2LM%2BWIG4omLjFh8QKPuQ38%3D)

注意：AVCaptureSession 的调用是会阻塞线程的，建议单独开辟子线程处理。

\2. 设置音频、视频输入

![img](https://p3-sign.toutiaoimg.com/pgc-image/RxRUAOC9Pdz2dg~noop.image?_iz=58558&from=article.pc_detail&x-expires=1679402928&x-signature=DSEI%2B5GyPm2WXBogJyX4BtFAMhU%3D)

\3. 设置文件输出源

![img](https://p3-sign.toutiaoimg.com/pgc-image/RxRUAOoFvkrwiK~noop.image?_iz=58558&from=article.pc_detail&x-expires=1679402928&x-signature=BajEnK1168jjekZluniylzEMz3c%3D)

4.添加视频预览层

![img](https://p3-sign.toutiaoimg.com/pgc-image/RxRUAPKEmV4zSM~noop.image?_iz=58558&from=article.pc_detail&x-expires=1679402928&x-signature=Eapgzv5PEm6de%2Bru%2BDj6rHbJ9yc%3D)

5.开始采集

![img](https://p3-sign.toutiaoimg.com/pgc-image/RxRUAjU7v6S49S~noop.image?_iz=58558&from=article.pc_detail&x-expires=1679402928&x-signature=KTF8NOj%2Bqffd3eICGtd9lMVlBME%3D)

6.开始录制

当实际的录制开始或停止时，系统会有代理回调。当开始录制之后，这时可能还没有真正写入，真正开始写入会回调下面代理，停止录制也是如此，所以如果你需要对录制视频起始点操作，建议通过系统的回调代理：

![img](https://p3-sign.toutiaoimg.com/pgc-image/RxRUAlE83mp2bX~noop.image?_iz=58558&from=article.pc_detail&x-expires=1679402928&x-signature=cuTFuZzwvdn0VmF2fjgEekLq45o%3D)

7.停止录制

8.停止采集

![img](https://p3-sign.toutiaoimg.com/pgc-image/RxRUAmQ670uSRF~noop.image?_iz=58558&from=article.pc_detail&x-expires=1679402928&x-signature=llMYqbnPk90D%2FHFyI5obaswDGHA%3D)

（2）AVCaptureSession + AVAssetWriter

1.创建 AVCaptureSession

![img](https://p3-sign.toutiaoimg.com/pgc-image/RxRUB6H9WmgThE~noop.image?_iz=58558&from=article.pc_detail&x-expires=1679402928&x-signature=HZgyugmMYLUyJk%2BEX4OngjfFv54%3D)

2.设置音频、视频输入

![img](https://p3-sign.toutiaoimg.com/pgc-image/RxRUB6u8SUIOh7~noop.image?_iz=58558&from=article.pc_detail&x-expires=1679402928&x-signature=9767NwZVrpAtJZpORThLBn%2FWbDo%3D)

3.设置音频 Data、视频 Data 输出

![img](https://p3-sign.toutiaoimg.com/pgc-image/RxRUB7XEzmSLNE~noop.image?_iz=58558&from=article.pc_detail&x-expires=1679402928&x-signature=9iog%2FMynhccQza8NL%2B7hoOjBfwY%3D)

4.添加视频预览层

![img](https://p3-sign.toutiaoimg.com/pgc-image/RxRUB8HHB4PvBN~noop.image?_iz=58558&from=article.pc_detail&x-expires=1679402928&x-signature=XOn1vzezzebsu4Lzd81AUCQawJU%3D)

5.开始采集

![img](https://p3-sign.toutiaoimg.com/pgc-image/RxRUB8rDzx8b3n~noop.image?_iz=58558&from=article.pc_detail&x-expires=1679402928&x-signature=%2F2OsCj%2FOt6LRBMQWyXzGnZ4GVuM%3D)

和第一种方式不同，第一种方式是开始录制之后，movieFileOutput 的回调才会触发，停止录制回调触发之后也就完成了。AVCaptureSession + AVAssetWriter 方式因为在设置输出源的时候，把输出代理 （setSampleBufferDelegate）已经设置好了，所以一旦开始采集（startRunning），数据流回调也就触发了。

![img](https://p3-sign.toutiaoimg.com/pgc-image/RxRUBTO4HgCQ8k~noop.image?_iz=58558&from=article.pc_detail&x-expires=1679402928&x-signature=AopDKBs4SWkxzvWfzIHZFoGDnfA%3D)

6.开始录制

这里需要创建AVAssetWriter，配置音频、视频录制参数，录制写入过程要单独开辟线程处理，避免阻塞线程，可以和 AVCaptureSession 放在同一线程处理。

![img](https://p3-sign.toutiaoimg.com/pgc-image/RxRUBU84gXFUUO~noop.image?_iz=58558&from=article.pc_detail&x-expires=1679402928&x-signature=1nKhggm5%2BQ7hmvhh0QVFJUF%2FklM%3D)

7.处理数据流

开始采集，数据流就会回调，所以这里用了变量 isRecording 来界定当前是否需要对数据处理，一般是在 startRecord 和 stopRecord 才会去处理数据流。

![img](https://p3-sign.toutiaoimg.com/pgc-image/RxRUBUj3y7d29w~noop.image?_iz=58558&from=article.pc_detail&x-expires=1679402928&x-signature=YTzwSf5vmzx%2FpXzyKOg6iKVtp70%3D)

8.停止录制

![img](https://p3-sign.toutiaoimg.com/pgc-image/RxRUBVCJ9rl2pA~noop.image?_iz=58558&from=article.pc_detail&x-expires=1679402928&x-signature=lIUj3lW%2BQZssYs3Su989eY%2FOmAM%3D)

9.停止采集

![img](https://p3-sign.toutiaoimg.com/pgc-image/RxRUBW8IKWCVyt~noop.image?_iz=58558&from=article.pc_detail&x-expires=1679402928&x-signature=haMWtqJeBEalGVIzYk%2FLwY7SZGg%3D)

两种方案对比：

相同点：他们的数据采集都是通过 AVCaptureSession 处理，音频视频的输入源也是一致的，画面预览一致。

不同点：输出源不一样，前者输出是 fileUrl，也就是说在视频写入完成之前开发者无法操作处理；后者输出是 data，AVAssetWriter 需要拿到 AVCaptureVideoDataOutput 和 AVCaptureAudioDataOutput 两个单独的输出，然后分别处理再写入指定路径。输出方式不同，决定了开发者对视频处理剪裁压缩的方式也就不同，前者如果需要对视频剪裁压缩，就需要从本地取出完整的视频文件，再做处理；而 AVAssetWriter 拿到的是数据流 data，如果需要剪裁压缩，可以直接配置相关参数后处理数据流，这样写入本地的就是已经处理过的视频文件。

其他功能点扩充

1.聚焦处理

![img](https://p3-sign.toutiaoimg.com/pgc-image/RxRUBx2FBnvf3y~noop.image?_iz=58558&from=article.pc_detail&x-expires=1679402928&x-signature=MGRO3ihlKgJH6tE27gl%2F%2FkhdNgE%3D)

2.摄像头切换

![img](https://p3-sign.toutiaoimg.com/pgc-image/RxRUBxYBIBi76o~noop.image?_iz=58558&from=article.pc_detail&x-expires=1679402928&x-signature=LG8CnneIYluAk6VesBSP%2FgKgAUo%3D)

原文https://www.toutiao.com/article/6820684475089814030/?channel=&source=search_tab