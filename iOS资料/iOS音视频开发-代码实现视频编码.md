# iOS音视频开发-代码实现视频编码

**硬编码的优点**

- 提高编码性能(使用CPU的使用率大大降低,倾向使用GPU)
- 增加编码效率(将编码一帧的时间缩短)
- 延长电量使用(耗电量大大降低)

**VideoToolBox框架的流程**

- 创建session
- 设置编码相关参数
- 开始编码
- 循环获取采集数据
- 获取编码后数据
- 将数据写入H264文件

编码的输入和输出

![img](https://pic3.zhimg.com/80/v2-a234942364dbe630f66a1f095f5d57e6_720w.webp)

如图所示,左边的三帧视频帧是发送給编码器之前的数据,开发者必须将原始图像数据封装为CVPixelBuufer的数据结构.该数据结构是使用VideoToolBox的核心.

CVPixelBuffer 解析

在这个官方文档的介绍中,CVPixelBuffer 给的官方解释,是其主内存存储所有像素点数据的一个对象.那么什么是主内存了?
其实它并不是我们平常所操作的内存,它指的是存储区域存在于缓存之中. 我们在访问这个块内存区域,需要先锁定这块内存区域

```text
 //1.锁定内存区域:
    CVPixelBufferLockBaseAddress(pixel_buffer,0);
 //2.读取该内存区域数据到NSData对象中
    Void *data = CVPixelBufferGetBaseAddress(pixel_buffer);
 //3.数据读取完毕后,需要释放锁定区域
    CVPixelBufferRelease(pixel_buffer); 
```

单纯从它的使用方式,我们就可以知道这一块内存区域不是普通内存区域.它需要加锁,解锁等一系列操作.
**作为视频开发,尽量减少进行显存和内存的交换.所以在iOS开发过程中也要尽量减少对它的内存区域访问**.建议使用iOS平台提供的对应的API来完成相应的一系列操作.

在AVFoundation 回调方法中,它有提供我们的数据其实就是CVPixelBuffer.只不过当时使用的是引用类型CVImageBufferRef,其实就是CVPixelBuffer的另外一个定义.

Camera 返回的CVImageBuffer 中存储的数据是一个CVPixelBuffer,而经过VideoToolBox编码输出的CMSampleBuffer中存储的数据是一个CMBlockBuffer的引用.

![img](https://pic2.zhimg.com/80/v2-ebaf9479545cbe34e9033a966a9019a1_720w.webp)

在**iOS**中,会经常使用到session的方式.比如我们使用任何硬件设备都要使用对应的session,麦克风就要使用AudioSession,使用Camera就要使用AVCaptureSession,使用编码则需要使用VTCompressionSession.解码时,要使用VTDecompressionSessionRef.

视频编码步骤分解

第一步: 使用VTCompressionSessionCreate方法,创建编码会话;

```text
   //1.调用VTCompressionSessionCreate创建编码session
        //参数1：NULL 分配器,设置NULL为默认分配
        //参数2：width
        //参数3：height
        //参数4：编码类型,如kCMVideoCodecType_H264
        //参数5：NULL encoderSpecification: 编码规范。设置NULL由videoToolbox自己选择
        //参数6：NULL sourceImageBufferAttributes: 源像素缓冲区属性.设置NULL不让videToolbox创建,而自己创建
        //参数7：NULL compressedDataAllocator: 压缩数据分配器.设置NULL,默认的分配
        //参数8：回调  当VTCompressionSessionEncodeFrame被调用压缩一次后会被异步调用.注:当你设置NULL的时候,你需要调用VTCompressionSessionEncodeFrameWithOutputHandler方法进行压缩帧处理,支持iOS9.0以上
        //参数9：outputCallbackRefCon: 回调客户定义的参考值
        //参数10：compressionSessionOut: 编码会话变量
        OSStatus status = VTCompressionSessionCreate(NULL, width, height, kCMVideoCodecType_H264, NULL, NULL, NULL, didCompressH264, (__bridge void *)(self), &cEncodeingSession);     
```

第二步:设置相关的参数

```text
/*
    session: 会话
    propertyKey: 属性名称
    propertyValue: 属性值
*/
VT_EXPORT OSStatus 
VTSessionSetProperty(
  CM_NONNULL VTSessionRef       session,
  CM_NONNULL CFStringRef        propertyKey,
  CM_NULLABLE CFTypeRef         propertyValue ) API_AVAILABLE(macosx(10.8), ios(8.0), tvos(10.2));
```

**kVTCompressionPropertyKey_RealTime**:设置是否实时编码

**kVTProfileLevel_H264_Baseline_AutoLevel**:表示使用H264的Profile规格,可以设置Hight的AutoLevel规格.

**kVTCompressionPropertyKey_AllowFrameReordering**:表示是否使用产生B帧数据(因为B帧在解码是非必要数据,所以开发者可以抛弃B帧数据)

**kVTCompressionPropertyKey_MaxKeyFrameInterval** : 表示关键帧的间隔,也就是我们常说的gop size.

**kVTCompressionPropertyKey_ExpectedFrameRate** : 表示设置帧率

**kVTCompressionPropertyKey_AverageBitRate**/**kVTCompressionPropertyKey_DataRateLimits** 设置编码输出的码率.

第三步: 准备编码

```text
//开始编码
VTCompressionSessionPrepareToEncodeFrames(cEncodeingSession);
```

第四步: 捕获编码数据

- 通过AVFoundation 捕获的视频,这个时候我们会走到AVFoundation捕获结果代理方法:

```text
#pragma mark - AVCaptureVideoDataOutputSampleBufferDelegate
//AV Foundation 获取到视频流
-(void)captureOutput:(AVCaptureOutput *)captureOutput didOutputSampleBuffer:(CMSampleBufferRef)sampleBuffer fromConnection:(AVCaptureConnection *)connection
{
    //开始视频录制，获取到摄像头的视频帧，传入encode 方法中
    dispatch_sync(cEncodeQueue, ^{
        [self encode:sampleBuffer];
    });
```

第五步:数据编码

- 将获取的视频数据编码

```text
- (void) encode:(CMSampleBufferRef )sampleBuffer
{
    //拿到每一帧未编码数据
    CVImageBufferRef imageBuffer = (CVImageBufferRef)CMSampleBufferGetImageBuffer(sampleBuffer);
    
    //设置帧时间，如果不设置会导致时间轴过长。时间戳以ms为单位
    CMTime presentationTimeStamp = CMTimeMake(frameID++, 1000);
    
    VTEncodeInfoFlags flags;
    
    //参数1：编码会话变量
    //参数2：未编码数据
    //参数3：获取到的这个sample buffer数据的展示时间戳。每一个传给这个session的时间戳都要大于前一个展示时间戳.
    //参数4：对于获取到sample buffer数据,这个帧的展示时间.如果没有时间信息,可设置kCMTimeInvalid.
    //参数5：frameProperties: 包含这个帧的属性.帧的改变会影响后边的编码帧.
    //参数6：ourceFrameRefCon: 回调函数会引用你设置的这个帧的参考值.
    //参数7：infoFlagsOut: 指向一个VTEncodeInfoFlags来接受一个编码操作.如果使用异步运行,kVTEncodeInfo_Asynchronous被设置；同步运行,kVTEncodeInfo_FrameDropped被设置；设置NULL为不想接受这个信息.
    
    OSStatus statusCode = VTCompressionSessionEncodeFrame(cEncodeingSession, imageBuffer, presentationTimeStamp, kCMTimeInvalid, NULL, NULL, &flags);
    
    if (statusCode != noErr) {
        
        NSLog(@"H.264:VTCompressionSessionEncodeFrame faild with %d",(int)statusCode);
        
        VTCompressionSessionInvalidate(cEncodeingSession);
        CFRelease(cEncodeingSession);
        cEncodeingSession = NULL;
        return
    
    NSLog(@"H264:VTCompressionSessionEncodeFrame Success");
```

第六步: 编码数据处理-获取SPS/PPS

当编码成功后,就会回调到最开始初始化编码器会话时传入的回调函数,回调函数的原型如下:

**void didCompressH264(void \*outputCallbackRefCon, void \*sourceFrameRefCon, OSStatus status, VTEncodeInfoFlags infoFlags, CMSampleBufferRef sampleBuffer)**

- **判断status,如果成功则返回**0(noErr) **;成功则继续处理,不成功则不处理.**

- **判断是否关键帧**

- - **为什么要判断关键帧呢?**
  - 因为VideoToolBox编码器在每一个关键帧前面都会输出SPS/PPS信息.所以如果本帧未关键帧,则可以取出对应的SPS/PPS信息.

```text
 //判断当前帧是否为关键帧
    //获取sps & pps 数据 只获取1次，保存在h264文件开头的第一帧中
    //sps(sample per second 采样次数/s),是衡量模数转换（ADC）时采样速率的单位
    //pps()
    if (keyFrame) {
        
        //图像存储方式，编码器等格式描述
        CMFormatDescriptionRef format = CMSampleBufferGetFormatDescription(sampleBuffer);
        
        //sps
        size_t sparameterSetSize,sparameterSetCount;
        const uint8_t *sparameterSet;
        OSStatus statusCode = CMVideoFormatDescriptionGetH264ParameterSetAtIndex(format, 0, &sparameterSet, &sparameterSetSize, &sparameterSetCount, 0);
        
        if (statusCode == noErr) {
            
            //获取pps
            size_t pparameterSetSize,pparameterSetCount;
            const uint8_t *pparameterSet;
            
            //从第一个关键帧获取sps & pps
            OSStatus statusCode = CMVideoFormatDescriptionGetH264ParameterSetAtIndex(format, 1, &pparameterSet, &pparameterSetSize, &pparameterSetCount, 0);
            
            //获取H264参数集合中的SPS和PPS
            if (statusCode == noErr)
            {
                //Found pps & sps
                NSData *sps = [NSData dataWithBytes:sparameterSet length:sparameterSetSize];
                NSData *pps = [NSData dataWithBytes:pparameterSet length:pparameterSetSize];
                
                if(encoder)
                {
                    [encoder gotSpsPps:sps pps:pps];
```

第七步 编码压缩数据并写入H264文件

当我们获取了SPS/PPS信息之后,我们就获取实际的内容来进行处理了

```text
 CMBlockBufferRef dataBuffer = CMSampleBufferGetDataBuffer(sampleBuffer);
    size_t length,totalLength;
    char *dataPointer;
    OSStatus statusCodeRet = CMBlockBufferGetDataPointer(dataBuffer, 0, &length, &totalLength, &dataPointer);
    if (statusCodeRet == noErr) {
        size_t bufferOffset = 0;
        static const int AVCCHeaderLength = 4;//返回的nalu数据前4个字节不是001的startcode,而是大端模式的帧长度length
        
        //循环获取nalu数据
        while (bufferOffset < totalLength - AVCCHeaderLength) {
            
            uint32_t NALUnitLength = 0;
            
            //读取 一单元长度的 nalu
            memcpy(&NALUnitLength, dataPointer + bufferOffset, AVCCHeaderLength);
            
            //从大端模式转换为系统端模式
            NALUnitLength = CFSwapInt32BigToHost(NALUnitLength);
            
            //获取nalu数据
            NSData *data = [[NSData alloc]initWithBytes:(dataPointer + bufferOffset + AVCCHeaderLength) length:NALUnitLength];
            
            //将nalu数据写入到文件
            [encoder gotEncodedData:data isKeyFrame:keyFrame];
            
            //move to the next NAL unit in the block buffer
            //读取下一个nalu 一次回调可能包含多个nalu数据
            bufferOffset += AVCCHeaderLength + NALUnitLength;

//第一帧写入 sps & pps
- (void)gotSpsPps:(NSData*)sps pps:(NSData*)pps
{
    NSLog(@"gotSpsPp %d %d",(int)[sps length],(int)[pps length]);
    
    const char bytes[] = "\x00\x00\x00\x01";
    
    size_t length = (sizeof bytes) - 1;
    
    NSData *ByteHeader = [NSData dataWithBytes:bytes length:length];
    
    [fileHandele writeData:ByteHeader];
    [fileHandele writeData:sps];
    [fileHandele writeData:ByteHeader];
    [fileHandele writeData:pps];
- (void)gotEncodedData:(NSData*)data isKeyFrame:(BOOL)isKeyFrame
{
    NSLog(@"gotEncodeData %d",(int)[data length]);
    
    if (fileHandele != NULL) {
        
        //添加4个字节的H264 协议 start code 分割符
        //一般来说编码器编出的首帧数据为PPS & SPS
        //H264编码时，在每个NAL前添加起始码 0x000001,解码器在码流中检测起始码，当前NAL结束。
        /*
         为了防止NAL内部出现0x000001的数据，h.264又提出'防止竞争 emulation prevention"机制，在编码完一个NAL时，如果检测出有连续两个0x00字节，就在后面插入一个0x03。当解码器在NAL内部检测到0x000003的数据，就把0x03抛弃，恢复原始数据。
         
         总的来说H264的码流的打包方式有两种,一种为annex-b byte stream format 的格式，这个是绝大部分编码器的默认输出格式，就是每个帧的开头的3~4个字节是H264的start_code,0x00000001或者0x000001。
         另一种是原始的NAL打包格式，就是开始的若干字节（1，2，4字节）是NAL的长度，而不是start_code,此时必须借助某个全局的数据来获得编 码器的profile,level,PPS,SPS等信息才可以解码。
         
         */
        const char bytes[] ="\x00\x00\x00\x01";
        
        //长度
        size_t length = (sizeof bytes) - 1;
        
        //头字节
        NSData *ByteHeader = [NSData dataWithBytes:bytes length:length];
        
        //写入头字节
        [fileHandele writeData:ByteHeader];
        
        //写入H264数据
        [fileHandele writeData:data];
```

原文https://zhuanlan.zhihu.com/p/552724772