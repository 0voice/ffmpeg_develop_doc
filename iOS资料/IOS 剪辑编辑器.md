# IOS 剪辑编辑器

最近这两年视频剪辑非常火,很多APP都内置了视频剪辑功能。

IOS视频剪辑主要依赖AVFoundation实现。

## 1、AVMutableComposition 

视频剪辑需要创建一个工程，这个工程只负责把来自不同素材的视频和音频重新组合到自己的轨道track上，等于在内存里把不同素材重新构建成一个新的视频素材。因此你可以直接在AVPlayer上播放这个工程，就像播放一个正常的视频，AVMutableComposition可以等同于AVAsset被使用。

如果你只想把不同的素材拼接起来，仅使用AVMutableComposition就足够了。

![img](https:////upload-images.jianshu.io/upload_images/9906913-2f1246cfa0635caf.png?imageMogr2/auto-orient/strip|imageView2/2/w/444/format/webp)

composition结构

### 1.1 插入视频轨道

```
let composition = AVMutableComposition()

let track = composition.addMutableTrack(withMediaType: .video, preferredTrackID: kCMPersistentTrackID_Invalid)
```

### 1.2 插入素材

```
var path = Bundle.main.path(forResource:"clip", ofType:"mp4")

let asset =AVURLAsset(url:URL(fileURLWithPath: path!))

let assetTrack = asset.tracks(withMediaType: .video).first

var assetTimeRange = CMTimeRangeMake(start:CMTime.zero, duration: assetA.duration)



var startTime = CMTime.zero

trystack.insertTimeRange(assetTimeRange, of: assetTrackA, at: startTime)
```

插入音频的方法大同小异，如果只是为了拼接视频，到这里就足够了。

## 2、AVMutableVideoComposition 

如果你想对合并后的视频素材进行处理，例如转场、特效、滤镜、文本、贴图这样的操作，你需要在AVMutableVideoComposition里完成。



![img](https:////upload-images.jianshu.io/upload_images/9906913-394a710334638c6b.png?imageMogr2/auto-orient/strip|imageView2/2/w/544/format/webp)

video composition结构

### 2.1 直接创建

```
let videoComposition = AVMutableVideoComposition(propertiesOf:composition)
```

当在composition上创建videoComposition之后，会自动创建AVMutableVideoCompositionInstruction和AVMutableVideoCompositionLayerInstruction。

#### 2.1.1 AVMutableVideoCompositionInstruction

一个等待处理的视频剪辑composition会根据插入素材的情况被自动划分为多个可剪辑的区域，例如0-1秒或者2-5秒，你不需要去思考，当你直接创建videoComposition的时候instruction会被自动的创建出来，每一个instruction代表一个可编辑的时间范围。

#### 2.1.2 AVMutableVideoCompositionLayerInstruction 

如果一个视频存在2个视频轨道和2个音频轨道，instruction代表1-3秒的时间范围，那么layerInstruction会自动绑定当前这个时间范围里的所有轨道，有几个轨道就有几个layerInstruction。

#### 2.1.3 利用instruction和layerInstruction转场 

假设你有两个视频，被插入到两个轨道里，第一个视频插入到track 1的0-3秒，第二个视频插入到track 2的2-5秒，那么合并后的视频长度就是5秒。中间叠加的1秒就可以进行转场的操作。



![img](https:////upload-images.jianshu.io/upload_images/9906913-826e379e064b5082.png?imageMogr2/auto-orient/strip|imageView2/2/w/636/format/webp)

Instruction结构

```
let instructions = videoComposition.instructions as! [AVMutableVideoCompositionInstruction]

for instruction in instructions {      

  if instruction.layerInstructions.count < 2 {

          continue 

  }

  var layerA = instruction.layerInstructions.first as! AVMutableVideoCompositionLayerInstruction                 
  let layerB = instruction.layerInstructions.last as! AVMutableVideoCompositionLayerInstruction             

  let fromEndTranform = CGAffineTransform(translationX: composition.naturalSize.width, y: 0)      

  let toStartTranform = CGAffineTransform(translationX: -composition.naturalSize.width, y: 0)      

  let range = CMTimeRangeMake(start: instruction.timeRange.start, duration: CMTimeMake(value: 60, timescale: 30))  
  let identityTransform = CGAffineTransform.identity             

  layerB.setTransformRamp(fromStart: fromEndTranform, toEnd: identityTransform, timeRange: range)      

  layerA.setTransformRamp(fromStart: identityTransform, toEnd: toStartTranform, timeRange: range)      

  layerA.setOpacityRamp(fromStartOpacity: 1.0, toEndOpacity: 0.0, timeRange: range)          

}
```

利用CGAffineTransform进行转场，两个轨道的视频都进行位移进行转场。

## 3、AVAsynchronousCIImageFilteringRequest

这种方式不需要instruction和layer instruction，也是简单的一种方式，只能做滤镜做不了转场，它是把整个composition看成一个。所以你能从里面截取当前的一帧视频，并对这一帧视频做处理。

```
letvideoComposition = AVMutableVideoComposition(asset:composition) { (request)in

  let sourceImage = request.sourceImage.clampedToExtent()

  let outputImage = sourceImage.applyingFilter("CIPhotoEffectProcess")

  request.finish(with: outputImage, context:nil)

}
```

如果你只想给视频套个滤镜，这就足够了。

## 4、customVideoCompositorClass

如果你想给视频添加滤镜又想添加转场，你需要使用第三种，这种模式下可以让你针对每一帧视频做特效，并且可以针对不同的轨道做转场。

```
let videoComposition = AVMutableVideoComposition(propertiesOf: composition)

videoComposition.customVideoCompositorClass = CustomVideoCompositor.self

_ = CMTimeRangeMake(start:CMTimeMake(value:30, timescale:30), duration: CMTimeMake(value:30, timescale:30))

letinstruction =VideoCompositionInstruction(timeRange:assetTimeRange)

// instruction.timeRange = assetTimeRange

videoComposition.instructions= [instruction]
```

和前面的方法不同我们需要自定义两个类CustomVideoCompositor和VideoCompositionInstruction，前者用来处理每一帧的视频。如果你想传递滤镜的名称、参数，转场的参数、类别你还是需要定义后面的这个类，通过它来传递信息。

### 4.1 VideoCompositionInstruction 

作为例子，只传递了一个时间范围

```
import Foundation

import AVFoundation

final class VideoCompositionInstruction: NSObject, AVVideoCompositionInstructionProtocol {

  var timeRange: CMTimeRange

  let enablePostProcessing: Bool = true

  let containsTweening: Bool = false

  var requiredSourceTrackIDs: [NSValue]?

  var passthroughTrackID: CMPersistentTrackID = kCMPersistentTrackID_Invalid

  lazy var transform:CGAffineTransform! = {

    let obj = CGAffineTransform()

    return obj

  }()

  init(timeRange: CMTimeRange) {

    self.timeRange = timeRange

    super.init()

  }

}
```



### 4.2 CustomVideoCompositor 

作为例子，这个视频只有一个轨道，设定了一个转场的状态，对视频增加了一个滤镜，实际情况你还需要开发更多，但方法就是这样的，在这个类里你可以做任何想做的修改，只是比前两种方法要复杂一些。例如转场，你需要自己计算每一帧的状态，而前面你只需要指定几个参数。

```
import AVFoundation

import CoreImage

class CustomVideoCompositor : NSObject, AVVideoCompositing {

  private let queue = DispatchQueue(label: "com.langwan.videoclipeditor.Langwan-VideoClipEditor.render", qos: .default)

  private var renderContext: AVVideoCompositionRenderContext = AVVideoCompositionRenderContext()

  private let colorSpace = CGColorSpaceCreateDeviceRGB()

  private let ciContext: CIContext = {

   if let eaglContext = EAGLContext(api: .openGLES3) ?? EAGLContext(api: .openGLES2) {

      return CIContext(eaglContext: eaglContext)

    }

   return CIContext()

  }()

  private static let pixelFormat = kCVPixelFormatType_32BGRA

  let sourcePixelBufferAttributes: [String : Any]? = [

   kCVPixelBufferPixelFormatTypeKey as String : NSNumber(value: CustomVideoCompositor.pixelFormat),

    kCVPixelBufferOpenGLESCompatibilityKey as String : NSNumber(value: true),

  ]

  let requiredPixelBufferAttributesForRenderContext: [String : Any] = [

    kCVPixelBufferPixelFormatTypeKey as String : NSNumber(value: CustomVideoCompositor.pixelFormat),

   kCVPixelBufferOpenGLESCompatibilityKey as String : NSNumber(value: true),

  ]

  func renderContextChanged(_ newRenderContext: AVVideoCompositionRenderContext) {

    renderContext = newRenderContext

  }

  func startRequest(_ request: AVAsynchronousVideoCompositionRequest) {

    // print("startRequest")

   autoreleasepool {

     queue.async {

       guard let instruction = request.videoCompositionInstruction as? VideoCompositionInstruction else {

          print("instruction is not VideoCompositionInstruction")

          return

       }

        let frameBuffer = self.renderFrame(forRequest: request)

       request.finish(withComposedVideoFrame: frameBuffer)

      }

   }

  }

  private func renderFrame(forRequest request: AVAsynchronousVideoCompositionRequest) -> CVPixelBuffer {

   let stackId = request.sourceTrackIDs[0]

   guard let frameBuffer = request.sourceFrame(byTrackID: CMPersistentTrackID(stackId)) else {

     let blankBuffer = self.renderContext.newPixelBuffer()

      return blankBuffer!

   }

  let sourceImage = CIImage(cvPixelBuffer: frameBuffer)

    //let filter = CIFilter(name: "CIPhotoEffectProcess")!

   let w = CVPixelBufferGetWidth(frameBuffer)

    let transform = CGAffineTransform(translationX: CGFloat(w / 2), y: 0)

   let outputImage = sourceImage.applyingFilter("CIPhotoEffectProcess").transformed(by: transform)

   let renderedBuffer = renderContext.newPixelBuffer()

   ciContext.render(outputImage, to: renderedBuffer!, bounds: outputImage.extent, colorSpace: self.colorSpace)

 return renderedBuffer!

  }

}
```

具体的例子可以参考 [swift_video_clip_editor_exmaple](https://links.jianshu.com/go?to=https%3A%2F%2Fgithub.com%2Flangwan%2Fswift_video_clip_editor_exmaple) 并下载源代码。




原文链接：https://www.jianshu.com/p/24c918d128a4