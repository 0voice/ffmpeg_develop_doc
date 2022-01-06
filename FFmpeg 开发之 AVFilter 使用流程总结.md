在使用FFmpeg开发时，使用AVFilter的流程较为复杂，涉及到的数据结构和函数也比较多，那么使用FFmpeg AVFilter的整体流程是什么样，在其执行过程中都有哪些步骤，需要注意哪些细节？这些都是需要我们整理和总结的。

首先，我们需要引入三个概念结构体：AVFilterGraph 、AVFilterContext、AVFilter。



## 一、AVFilterGraph 、AVFilterContext、AVFilter

在 FFmpeg 中有多种多样的滤镜，你可以把他们当成一个个小工具，专门用于处理视频和音频数据，以便实现一定的目的。如 overlay 这个滤镜，可以将一个图画覆盖到另一个图画上；transport 这个滤镜可以将图画做旋转等等。

一个 filter 的输出可以作为另一个 filter 的输入，因此多个 filter 可以组织成为一个网状的 filter graph，从而实现更加复杂或者综合的任务。

在 libavfilter 中，我们用类型 AVFilter 来表示一个 filter，每一个 filter 都是经过注册的，其特性是相对固定的。而 AVFilterContext 则表示一个真正的 filter 实例，这和 AVCodec 以及 AVCodecContext 的关系是类似的。

AVFilter 中最重要的特征就是其所需的输入和输出。

AVFilterContext 表示一个 AVFilter 的实例，我们在实际使用 filter 时，就是使用这个结构体。AVFilterContext 在被使用前，它必须是 被初始化的，就是需要对 filter 进行一些选项上的设置，通过初始化告诉 FFmpeg 我们已经做了相关的配置。

AVFilterGraph 表示一个 filter graph，当然它也包含了 filter chain的概念。graph 包含了诸多 filter context 实例，并负责它们之间的 link，graph 会负责创建，保存，释放 这些相关的 filter context 和 link，一般不需要用户进行管理。除此之外，它还有线程特性和最大线程数量的字段，和filter context类似。graph 的操作有：分配一个graph，往graph中添加一个filter context，添加一个 filter graph，对 filter 进行 link 操作，检查内部的link和format是否有效，释放graph等。



## 二、AVFilter 相关Api使用方法整理



### 1. AVFilterContext 初始化方法

AVFilterContext 的初始化方式有三种，avfilter_init_str() 和 avfilter_init_dict()、avfilter_graph_create_filter(). 



```
/*
 使用提供的参数初始化 filter。
 参数args：表示用于初始化 filter 的 options。该字符串必须使用 ":" 来分割各个键值对， 而键值对的形式为 'key=value'。如果不需要设置选项，args为空。 
 除了这种方式设置选项之外，还可以利用 AVOptions API 直接对 filter 设置选项。
 返回值：成功返回0，失败返回一个负的错误值
*/
int avfilter_init_str(AVFilterContext *ctx, const char *args);
```





```
/*
 使用提供的参数初始化filter。
 参数 options：以 dict 形式提供的 options。
 返回值：成功返回0，失败返回一个负的错误值
 注意：这个函数和 avfilter_init_str 函数的功能是一样的，只不过传递的参数形式不同。 但是当传入的 options 中有不被 filter 所支持的参数时，这两个函数的行为是不同： avfilter_init_str 调用会失败，而这个函数则不会失败，它会将不能应用于指定 filter 的 option 通过参数 options 返回，然后继续执行任务。
*/
int avfilter_init_dict(AVFilterContext *ctx, AVDictionary **options);
```





```
/**
 * 创建一个Filter实例（根据args和opaque的参数），并添加到已存在的AVFilterGraph. 
 * 如果创建成功*filt_ctx会指向一个创建好的Filter实例，否则会指向NULL. 
 * @return 失败返回负数，否则返回大于等于0的数
 */
int avfilter_graph_create_filter(AVFilterContext **filt_ctx, const AVFilter *filt, const char *name,  　　　　　　　　　　　　　　　　　　const char *args, void *opaque, AVFilterGraph *graph_ctx);
```





### 2. AVFilterGraph 相关的Api

AVFilterGraph 表示一个 filter graph，当然它也包含了 filter chain的概念。graph 包含了诸多 filter context 实例，并负责它们之间的 link，graph 会负责创建，保存，释放 这些相关的 filter context 和 link，一般不需要用户进行管理。

graph 的操作有：分配一个graph，往graph中添加一个filter context，添加一个 filter graph，对 filter 进行 link 操作，检查内部的link和format是否有效，释放graph等。

根据上述操作，可以列举的方法分别为：

**分配空的filter graph：**

```
/*
  分配一个空的 filter graph.
  成功返回一个 filter graph，失败返回 NULL
*/
AVFilterGraph *avfilter_graph_alloc(void);
```

 **创建一个新的filter实例：**



```
/*
    在 filter graph 中创建一个新的 filter 实例。这个创建的实例尚未初始化。
    详细描述：在 graph 中创建一个名称为 name 的 filter类型的实例。
    创建失败，返回NULL。创建成功，返回 filter context实例。创建成功后的实例会加入到graph中，
    可以通过 AVFilterGraph.filters 或者 avfilter_graph_get_filter() 获取。
*/
AVFilterContext *avfilter_graph_alloc_filter(AVFilterGraph *graph, const AVFilter *filter, const char *name);                                          
```



**返回名字为name的filter context：**

```
/*
   返回 graph 中的名为 name 的 filter context。
*/
AVFilterContext *avfilter_graph_get_filter(AVFilterGraph *graph, const char *name);
 
```

**在 filter graph 中创建一个新的 filter context 实例，并使用args和opaque初始化这个filter context：**



```
/*
 在 filter graph 中创建一个新的 filter context 实例，并使用 args 和 opaque 初始化这个实例。
    
 参数 filt_ctx：返回成功创建的 filter context
    
 返回值：成功返回正数，失败返回负的错误值。
*/
int avfilter_graph_create_filter(AVFilterContext **filt_ctx, const AVFilter *filt, const char *name, 　　 　　　　　　　　　　　　　　　　　　　 const char *args, void *opaque,  AVFilterGraph *graph_ctx);     
```



 **配置 AVFilterGraph 的链接和格式：**

```
/*
  检查 graph 的有效性，并配置其中所有的连接和格式。
  有效则返回 >= 0 的数，否则返回一个负值的 AVERROR.
 */
int avfilter_graph_config(AVFilterGraph *graphctx, void *log_ctx);
```

**释放AVFilterGraph：**

```
/*
   释放graph，摧毁内部的连接，并将其置为NULL。
*/
void avfilter_graph_free(AVFilterGraph **graph);
```

**在一个已经存在的link中插入一个FilterContext：**



```
/*
  在一个已经存在的 link 中间插入一个 filter context。
  参数filt_srcpad_idx和filt_dstpad_idx：指定filt要连接的输入和输出pad的index。
  成功返回0.
*/
int avfilter_insert_filter(AVFilterLink *link, AVFilterContext *filt, 　　　　　　　　　　　　　　　　　　unsigned filt_srcpad_idx, unsigned filt_dstpad_idx);         
```



将字符串描述的filter graph 加入到一个已存在的graph中：



```
/*
    将一个字符串描述的 filter graph 加入到一个已经存在的 graph 中。
    
    注意：调用者必须提供 inputs 列表和 outputs 列表。它们在调用这个函数之前必须是已知的。
    
    注意：inputs 参数用于描述已经存在的 graph 的输入 pad 列表，也就是说，从新的被创建的 graph 来讲，它们是 output。
        outputs 参数用于已经存在的 graph 的输出 pad 列表，从新的被创建的 graph 来说，它们是 input。
        
    成功返回 >= 0，失败返回负的错误值。
*/
int avfilter_graph_parse(AVFilterGraph *graph, const char *filters,
                         AVFilterInOut *inputs, AVFilterInOut *outputs,
                         void *log_ctx);                   
```





```
/*
    和 avfilter_graph_parse 类似。不同的是 inputs 和 outputs 参数，即做输入参数，也做输出参数。
        在函数返回时，它们将会保存 graph 中所有的处于 open 状态的 pad。返回的 inout 应该使用 avfilter_inout_free() 释放掉。
    
    注意：在字符串描述的 graph 中，第一个 filter 的输入如果没有被一个字符串标识，默认其标识为"in"，最后一个 filter 的输出如果没有被标识，默认为"output"。
    
    intpus：作为输入参数是，用于保存已经存在的graph的open inputs，可以为NULL。
        作为输出参数，用于保存这个parse函数之后，仍然处于open的inputs，当然如果传入为NULL，则并不输出。
    outputs：同上。
*/
int avfilter_graph_parse_ptr(AVFilterGraph *graph, const char *filters,
                             AVFilterInOut **inputs, AVFilterInOut **outputs, void *log_ctx);
```





```
/*
    和 avfilter_graph_parse_ptr 函数类似，不同的是，inputs 和 outputs 函数不作为输入参数，
    仅作为输出参数，返回字符串描述的新的被解析的graph在这个parse函数后，仍然处于open状态的inputs和outputs。
    返回的 inout 应该使用 avfilter_inout_free() 释放掉。
    
    成功返回0，失败返回负的错误值。
*/
int avfilter_graph_parse2(AVFilterGraph *graph, const char *filters,
                          AVFilterInOut **inputs, AVFilterInOut **outputs);
```



**将graph转换为可读取的字符串描述：**

```
/*
  将 graph 转化为可读的字符串描述。
  参数options：未使用，忽略它。
*/
char *avfilter_graph_dump(AVFilterGraph *graph, const char *options);
```



## 三、FFmpeg Filter Buffer 和 BufferSink 相关APi的使用方法整理

 Buffer 和 BufferSink 作为 graph 的输入点和输出点来和我们交互，我们仅需要和其进行数据交互即可。其API如下：



```
//buffersrc flag
enum {
    //不去检测 format 的变化
    AV_BUFFERSRC_FLAG_NO_CHECK_FORMAT = 1,
 
    //立刻将 frame 推送到 output
    AV_BUFFERSRC_FLAG_PUSH = 4,
 
    //对输入的frame新建一个引用，而非接管引用
    //如果 frame 是引用计数的，那么对它创建一个新的引用；否则拷贝frame中的数据
    AV_BUFFERSRC_FLAG_KEEP_REF = 8,
};
```



**向 buffer_src 添加一个Frame：**



```
/*
    向 buffer_src 添加一个 frame。
    
    默认情况下，如果 frame 是引用计数的，那么这个函数将会接管其引用并重新设置 frame。
        但这个行为可以由 flags 来控制。如果 frame 不是引用计数的，那么拷贝该 frame。
    
    如果函数返回一个 error，那么 frame 并未被使用。frame为NULL时，表示 EOF。
    成功返回 >= 0，失败返回负的AVERROR。
*/
int av_buffersrc_add_frame_flags(AVFilterContext *buffer_src, AVFrame *frame, int flags);
```



**添加一个frame到 src filter：**

```
/*
    添加一个 frame 到 src filter。
    这个函数等同于没有 AV_BUFFERSRC_FLAG_KEEP_REF 的 av_buffersrc_add_frame_flags() 函数。
 */
int av_buffersrc_add_frame(AVFilterContext *ctx, AVFrame *frame)； 
/*
   添加一个 frame 到 src filter。
   这个函数等同于设置了 AV_BUFFERSRC_FLAG_KEEP_REF 的av_buffersrc_add_frame_flags() 函数。
*/
int av_buffersrc_write_frame(AVFilterContext *ctx, const AVFrame *frame);
```

**从sink获取已filtered处理的帧，并放到参数frame中：**



```
/*
    从 sink 中获取已进行 filtered 处理的帧，并将其放到参数 frame 中。
    
    参数ctx：指向 buffersink 或 abuffersink 类型的 filter context
    参数frame：获取到的被处理后的frame，使用后必须使用av_frame_unref() / av_frame_free()释放掉它
    
    成功返回非负数，失败返回负的错误值，如 EAGAIN（表示需要新的输入数据来产生filter后的数据）,
        AVERROR_EOF（表示不会再有新的输入数据）
 */
int av_buffersink_get_frame_flags(AVFilterContext *ctx, AVFrame *frame, int flags);
```



```
/*
     同 av_buffersink_get_frame_flags ，不过不能指定 flag。
 */
int av_buffersink_get_frame(AVFilterContext *ctx, AVFrame *frame)
/*
 和 av_buffersink_get_frame 相同，不过这个函数是针对音频的，而且可以指定读取的取样数。此时 ctx 只能指向 abuffersink 类型的 filter context。
*/
int av_buffersink_get_samples(AVFilterContext *ctx, AVFrame *frame, int nb_samples);
```

 



## 四、FFmpeg AVFilter 使用整体流程 

下图就是FFmpeg AVFilter在使用过程中的流程图：

![img](https://img2020.cnblogs.com/blog/682616/202104/682616-20210415141146493-1433896880.jpg)

我们对上图先做下说明，理解下图中每个步骤的关系，然后，才从代码的角度来给出其使用的步骤。

1. 最顶端的AVFilterGraph，这个结构前面介绍过，主要管理加入的过滤器，其中加入的过滤器就是通过函数avfilter_graph_create_filter来创建并加入，这个函数返回是AVFilterContext（其封装了AVFilter的详细参数信息）。

2. buffer和buffersink这两个过滤器是FFMpeg为我们实现好的，buffer表示源，用来向后面的过滤器提供数据输入（其实就是原始的AVFrame）；buffersink过滤器是最终输出的（经过过滤器链处理后的数据AVFrame），其它的诸如filter 1 等过滤器是由avfilter_graph_parse_ptr函数解析外部传入的过滤器描述字符串自动生成的，内部也是通过avfilter_graph_create_filter来创建过滤器的。

3. 上面的buffer、filter 1、filter 2、filter n、buffersink之间是通过avfilter_link函数来进行关联的（通过AVFilterLink结构），这样子过滤器和过滤器之间就通过AVFilterLink进行关联上了，前一个过滤器的输出就是下一个过滤器的输入，注意，除了源和接收过滤器之外，其它的过滤器至少有一个输入和输出，这很好理解，中间的过滤器处理完AVFrame后，得到新的处理后的AVFrame数据，然后把新的AVFrame数据作为下一个过滤器的输入。

4. 过滤器建立完成后，首先我们通过av_buffersrc_add_frame把最原始的AVFrame（没有经过任何过滤器处理的）加入到buffer过滤器的fifo队列。

5. 然后调用buffersink过滤器的av_buffersink_get_frame_flags来获取处理完后的数据帧（这个最终放入buffersink过滤器的AVFrame是通过之前创建的一系列过滤器处理后的数据）。

 使用流程图就介绍到这里，下面结合上面的使用流程图详细说下FFMpeg中使用过滤器的步骤，这个过程我们分为三个部分：过滤器构建、数据加工、资源释放。



### 1. 过滤器构建：

1）分配AVFilterGraph

```
AVFilterGraph* graph = avfilter_graph_alloc();
```

 2）创建过滤器源

```
char srcArgs[256] = {0};
AVFilterContext *srcFilterCtx;
AVFilter* srcFilter = avfilter_get_by_name("buffer");
avfilter_graph_create_filter(&srcFilterCtx, srcFilter ,"out_buffer", srcArgs, NULL, graph);
```

3）创建接收过滤器

```
AVFilterContext *sinkFilterCtx;
AVFilter* sinkFilter = avfilter_get_by_name("buffersink");
avfilter_graph_create_filter(&sinkFilterCtx, sinkFilter,"in_buffersink", NULL, NULL, graph);
```

4）生成源和接收过滤器的输入输出

这里主要是把源和接收过滤器封装给AVFilterInOut结构，使用这个中间结构来把过滤器字符串解析并链接进graph，主要代码如下：



```
AVFilterInOut *inputs = avfilter_inout_alloc();
AVFilterInOut *outputs = avfilter_inout_alloc();
outputs->name       = av_strdup("in");
outputs->filter_ctx = srcFilterCtx;
outputs->pad_idx    = 0;
outputs->next       = NULL;
inputs->name        = av_strdup("out");
inputs->filter_ctx  = sinkFilterCtx;
inputs->pad_idx     = 0;
inputs->next        = NULL;
```



 这里源对应的AVFilterInOut的name最好定义为in，接收对应的name为out，因为FFMpeg源码里默认会通过这样个name来对默认的输出和输入进行查找。

5）通过解析过滤器字符串添加过滤器

```
const *char filtergraph = "[in1]过滤器名称=参数1:参数2[out1]";
int ret = avfilter_graph_parse_ptr(graph, filtergraph, &inputs, &outputs, NULL);
```

这里过滤器是以字符串形式描述的，其格式为：[in]过滤器名称=参数[out]，过滤器之间用,或;分割，如果过滤器有多个参数，则参数之间用:分割，其中[in]和[out]分别为过滤器的输入和输出，可以有多个。

6）检查过滤器的完整性

```
avfilter_graph_config(graph, NULL);
```



###  2. 数据加工

1）向源过滤器加入AVFrame 

```
AVFrame* frame; // 这是解码后获取的数据帧
int ret = av_buffersrc_add_frame(srcFilterCtx, frame);
```

2）从buffersink接收处理后的AVFrame

```
int ret = av_buffersink_get_frame_flags(sinkFilterCtx, frame, 0);
```

 现在我们就可以使用处理后的AVFrame，比如显示或播放出来。



### 3.资源释放

使用结束后，调用avfilter_graph_free(&graph);释放掉AVFilterGraph类型的graph。
