AVPacket是存储压缩编码数据相关信息的结构体。下面我们来分析一下该结构体里重要变量的含义和作用。

# 一、源码整理

首先我们先看一下结构体AVPacket的定义的结构体源码(位于libavcodec/avcodec.h)：



```
typedef struct AVPacket {
    /**
     * Presentation timestamp in AVStream->time_base units; the time at which
     * the decompressed packet will be presented to the user.
     * Can be AV_NOPTS_VALUE if it is not stored in the file.
     * pts MUST be larger or equal to dts as presentation cannot happen before
     * decompression, unless one wants to view hex dumps. Some formats misuse
     * the terms dts and pts/cts to mean something different. Such timestamps
     * must be converted to true pts/dts before they are stored in AVPacket.
     */
    int64_t pts;
    /**
     * Decompression timestamp in AVStream->time_base units; the time at which
     * the packet is decompressed.
     * Can be AV_NOPTS_VALUE if it is not stored in the file.
     */
    int64_t dts;
    uint8_t *data;
    int   size;
    int   stream_index;
    /**
     * A combination of AV_PKT_FLAG values
     */
    int   flags;
    /**
     * Additional packet data that can be provided by the container.
     * Packet can contain several types of side information.
     */
    struct {
        uint8_t *data;
        int      size;
        enum AVPacketSideDataType type;
    } *side_data;
    int side_data_elems;
 
    /**
     * Duration of this packet in AVStream->time_base units, 0 if unknown.
     * Equals next_pts - this_pts in presentation order.
     */
    int   duration;
    void  (*destruct)(struct AVPacket *);
    void  *priv;
    int64_t pos;                            ///< byte position in stream, -1 if unknown
 
    /**
     * Time difference in AVStream->time_base units from the pts of this
     * packet to the point at which the output from the decoder has converged
     * independent from the availability of previous frames. That is, the
     * frames are virtually identical no matter if decoding started from
     * the very first frame or from this keyframe.
     * Is AV_NOPTS_VALUE if unknown.
     * This field is not the display duration of the current packet.
     * This field has no meaning if the packet does not have AV_PKT_FLAG_KEY
     * set.
     *
     * The purpose of this field is to allow seeking in streams that have no
     * keyframes in the conventional sense. It corresponds to the
     * recovery point SEI in H.264 and match_time_delta in NUT. It is also
     * essential for some types of subtitle streams to ensure that all
     * subtitles are correctly displayed after seeking.
     */
    int64_t convergence_duration;
} AVPacket;
```



# 二、AVPacket 重点字段



```
uint8_t *data：压缩编码的数据。

int   size：data的大小

int64_t pts：显示时间戳

int64_t dts：解码时间戳

int   stream_index：标识该AVPacket所属的视频/音频流。
```



针对data做一下说明：对于H.264格式来说，在使用FFMPEG进行视音频处理的时候，我们常常可以将得到的AVPacket的data数据直接写成文件，从而得到视音频的码流文件。
