# iOS短视频篇：音视频编辑之音视频合成，添加水印及音视频导出

## 1.基本介绍

音视频编辑主要依靠AVFoundation框架，首先要有一个AVMutableComposition对象composition，一个composition可以简单的认为是一组轨道（tracks）的集合，这些轨道可以是来自不同媒体资源AVAsset，AVMutableComposition提供了接口来插入或者删除轨道，也可以调整这些轨道的顺序。

下面这张图反映了一个新的 mixComposition 是怎么从已有的 AVAsset 中获取对应的 track 并进行拼接形成新的 AVAsset

![img](https://img-blog.csdnimg.cn/20190511151431700.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3dlaXhpbl80MjQzMzQ4MA==,size_16,color_FFFFFF,t_70)

如下图所示，我们还可以使用 AVMutableVideoComposition 来直接处理 composition 中的视频轨道。处理一个单独的 video composition 时，你可以指定它的渲染尺寸、缩放比例、帧率等参数并输出最终的视频文件。通过一些针对 video composition 的指令（AVMutableVideoCompositionInstruction 等），我们可以修改视频的背景颜色、应用 layer instructions。这些 layer instructions（AVMutableVideoCompositionLayerInstruction 等）可以用来对 composition 中的视频轨道实施图形变换、添加图形渐变、透明度变换、增加透明度渐变。此外，你还能通过设置 video composition 的 animationTool 属性来应用 Core Animation Framework 框架中的动画效果。

![img](https://img-blog.csdnimg.cn/20190511152526982.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3dlaXhpbl80MjQzMzQ4MA==,size_16,color_FFFFFF,t_70)

如下图所示，你可以使用 AVAssetExportSession 相关的接口来合并你的 composition 中的 audio mix 和 video composition。你只需要初始化一个 AVAssetExportSession 对象，然后将其 audioMix 和 videoComposition 属性分别设置为你的 audio mix 和 video composition 即可。

![img](https://img-blog.csdnimg.cn/20190511152942596.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3dlaXhpbl80MjQzMzQ4MA==,size_16,color_FFFFFF,t_70)

## 2.创建Composition对象

当使用 AVMutableComposition 创建自己的 composition 时，最典型的，我们可以使用 AVMutableCompositionTrack 来向 composition 中添加一个或多个 composition tracks，比如下面这个简单的例子便是向一个 composition 中添加一个音频轨道和一个视频轨道：

    //音频视频合成体
    AVMutableComposition *mixComposition = [[AVMutableComposition alloc] init];
    
    //创建音频通道容器
    AVMutableCompositionTrack *audioTrack = [mixComposition addMutableTrackWithMediaType:AVMediaTypeAudio
                                                                        preferredTrackID:kCMPersistentTrackID_Invalid];
    //创建视频通道容器
    AVMutableCompositionTrack *videoTrack = [mixComposition addMutableTrackWithMediaType:AVMediaTypeVideo
                                                                        preferredTrackID:kCMPersistentTrackID_Invalid];

当为 composition 添加一个新的 track 的时候，需要设置其媒体类型（media type）和 track ID，主要的媒体类型包括：音频、视频、字幕、文本等等。

这里需要注意的是，每个 track 都需要一个唯一的 track ID，比较方便的做法是：设置 track ID 为 kCMPersistentTrackID_Invalid 来为对应的 track 获得一个自动生成的唯一 ID。

## 3.向 Composition 添加音视频数据

要将音视频数据添加到一个 composition track 中需要访问媒体数据所在的 AVAsset，可以使用 AVMutableCompositionTrack 的接口将具有相同媒体类型的多个 track 添加到同一个 composition track 中。下面的例子便是从多个 AVAsset 中各取出一份 video asset track，再添加到一个新的 composition track 中去

CMTime totalDuration = kCMTimeZero;
    for (int i = 0; i < videosPathArray.count; i++) {
        //        AVURLAsset *asset = [AVURLAsset assetWithURL:[NSURL URLWithString:videosPathArray[i]]];
        //如果创建AVURLAsset时传入的AVURLAssetPreferPreciseDurationAndTimingKey值为NO(不传默认为NO)，duration会取一个估计值，计算量比较小。反之如果为YES，duration需要返回一个精确值，计算量会比较大，耗时比较长
        NSDictionary* options = @{AVURLAssetPreferPreciseDurationAndTimingKey:@YES};
        AVAsset* asset = [AVURLAsset URLAssetWithURL:videosPathArray[i] options:options];
        

        NSError *erroraudio = nil;
        //获取AVAsset中的音频 或者视频
        AVAssetTrack *assetAudioTrack = [[asset tracksWithMediaType:AVMediaTypeAudio] firstObject];
        //向通道内加入音频或者视频
        BOOL ba = [audioTrack insertTimeRange:CMTimeRangeMake(kCMTimeZero, asset.duration)
                                      ofTrack:assetAudioTrack
                                       atTime:totalDuration
                                        error:&erroraudio];
        
        NSLog(@"erroraudio:%@%d",erroraudio,ba);
        NSError *errorVideo = nil;
        AVAssetTrack *assetVideoTrack = [[asset tracksWithMediaType:AVMediaTypeVideo]firstObject];
        BOOL bl = [videoTrack insertTimeRange:CMTimeRangeMake(kCMTimeZero, asset.duration)
                                      ofTrack:assetVideoTrack
                                       atTime:totalDuration
                                        error:&errorVideo];
        
        NSLog(@"errorVideo:%@%d",errorVideo,bl);
        totalDuration = CMTimeAdd(totalDuration, asset.duration);
    }

这里需要注意：如果创建AVURLAsset时传入的AVURLAssetPreferPreciseDurationAndTimingKey值为NO(不传默认为NO)，duration会取一个估计值，计算量比较小。反之如果为YES，duration需要返回一个精确值，计算量会比较大，耗时比较长，一般我们对短视频的音视频编辑时都是将这个属性设置为YES，来获取准确的duration。

## 4.视频水印处理

处理音频是我们使用 AVMutableAudioMix，那么处理视频时，我们就使用 AVMutableVideoComposition，只需要一个 AVMutableVideoComposition 实例就可以为 composition 中所有的 video track 做处理，比如设置渲染尺寸、缩放、播放帧率等等。所有的 video composition 也必然对应一组 AVVideoCompositionInstruction 实例，每个 AVVideoCompositionInstruction 中至少包含一条 video composition instruction。我们可以使用 AVMutableVideoCompositionInstruction 来创建我们自己的 video composition instruction，通过这些指令，我们可以修改 composition 的背景颜色、后处理、layer instruction 等等。

我们也可以用 video composition instructions 来应用 video composition layer instructions。AVMutableVideoCompositionLayerInstruction 可以用来设置 video track 的图形变换、图形渐变、透明度、透明度渐变等等。一个 video composition instruction 的 layerInstructions 属性中所存储的 layer instructions 的顺序决定了 tracks 中的视频帧是如何被放置和组合的。

我们还能通过设置 video composition 的 animationTool 属性来使用 Core Animation Framework 框架的强大能力。比如：设置视频水印、视频标题、动画浮层等。

下面的代码展示了直接使用 Core Animation Layer 在视频帧中渲染动画效果的一个例子，在视频右上角添加水印：

    CGSize videoSize = [videoTrack naturalSize];
    CALayer* aLayer = [CALayer layer];
    aLayer.contents = (id)waterImg.CGImage;
    aLayer.frame = CGRectMake(videoSize.width - waterImg.size.width - 30, videoSize.height - waterImg.size.height*3, waterImg.size.width, waterImg.size.height);
    aLayer.opacity = 0.9;
    
    CALayer *parentLayer = [CALayer layer];
    CALayer *videoLayer = [CALayer layer];
    parentLayer.frame = CGRectMake(0, 0, videoSize.width, videoSize.height);
    videoLayer.frame = CGRectMake(0, 0, videoSize.width, videoSize.height);
    [parentLayer addSublayer:videoLayer];
    [parentLayer addSublayer:aLayer];
    AVMutableVideoComposition* videoComp = [AVMutableVideoComposition videoComposition];
    videoComp.renderSize = videoSize;
    
    //表示 30 帧每秒
    videoComp.frameDuration = CMTimeMake(1, 30);
    //应用 Core Animation Framework 框架中的动画效果。
    videoComp.animationTool = [AVVideoCompositionCoreAnimationTool videoCompositionCoreAnimationToolWithPostProcessingAsVideoLayer:videoLayer inLayer:parentLayer];
    AVMutableVideoCompositionInstruction* instruction = [AVMutableVideoCompositionInstruction videoCompositionInstruction];
    
    instruction.timeRange = CMTimeRangeMake(kCMTimeZero, [mixComposition duration]);
    AVAssetTrack* mixVideoTrack = [[mixComposition tracksWithMediaType:AVMediaTypeVideo] objectAtIndex:0];
    AVMutableVideoCompositionLayerInstruction* layerInstruction = [AVMutableVideoCompositionLayerInstruction videoCompositionLayerInstructionWithAssetTrack:mixVideoTrack];
    //layerInstructions 属性中所存储的 layer instructions 的顺序决定了 tracks 中的视频帧是如何被放置和组合的。
    instruction.layerInstructions = [NSArray arrayWithObject:layerInstruction];
    videoComp.instructions = [NSArray arrayWithObject: instruction];

## 5.音视频导出

主要是通过AVAssetExportSession这个类来导出音视频，它可以用来合并你添加了水印等的videoComposition和audioMix。

下面是使用AVAssetExportSession导出音视频的例子：

NSURL *mergeFileURL = [NSURL fileURLWithPath:outpath];
    

    //视频导出工具
    AVAssetExportSession *exporter = [[AVAssetExportSession alloc] initWithAsset:mixComposition
                                                                      presetName:AVAssetExportPreset1280x720];
    exporter.videoComposition = videoComp;
    /*
     exporter.progress
     导出进度
     This property is not key-value observable.
     不支持kvo 监听
     只能用定时器监听了  NStimer
     */
    exporter.outputURL = mergeFileURL;
    exporter.outputFileType = AVFileTypeQuickTimeMovie;
    exporter.shouldOptimizeForNetworkUse = YES;
    [exporter exportAsynchronouslyWithCompletionHandler:^{
        dispatch_async(dispatch_get_main_queue(), ^{
            
            [videoCamera stopCameraCapture];
            
            EditVideoViewController* view = [[EditVideoViewController alloc]init];
            view.width = _width;
            view.hight = _hight;
            view.bit = _bit;
            view.frameRate = _frameRate;
            view.videoURL = [NSURL fileURLWithPath:outpath];;
            [[NSNotificationCenter defaultCenter] removeObserver:self];

//            [[AppDelegate sharedAppDelegate] pushViewController:view animated:YES];
          if (self.delegate&&[self.delegate respondsToSelector:@selector(pushCor:)]) {
            [self.delegate pushCor:view];
          }
            [self removeFromSuperview];
        });
        

原文链接：https://blog.csdn.net/weixin_42433480/article/details/90109873