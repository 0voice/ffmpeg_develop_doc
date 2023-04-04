# iOS逆向 MachO文件

## 1、MachO初探

#### 1.1定义

`MachO`其实是`Mach Object`文件格式的缩写，是mac以及iOS上可执行文件的格式，类似于Windows上的PE格式（Portable Executable）、Linux上的elf格式（Executable and Linking Format）

它是一种用于可执行文件、目标代码、动态库的文件格式，作为.out格式的替代，MachO提供了更强的扩展性

#### 1.2常见的MachO文件

- 目标文件.o
- 库文件
  - .a
  - .dylib
  - .Framework
- 可执行文件
- dyld（动态链接器）
- .dsym（符号表：Relese环境运行生成）

#### 1.3查看文件类型

```
$ file xxx.xx
```

![img](https://p1-jj.byteimg.com/tos-cn-i-t2oaga2asx/gold-user-assets/2019/12/14/16f03b938a746084~tplv-t2oaga2asx-zoom-in-crop-mark:4536:0:0:0.image)



## 2、关于架构

#### 2.1架构表

其实iPhone不同的型号对应的架构是不一样的

| 架构   | 手机型号                                        |
| ------ | ----------------------------------------------- |
| i386   | 32位模拟器                                      |
| x86_64 | 64位模拟器                                      |
| armv7  | iPhone4、iPhone4S                               |
| armv7s | iPhone5、iPhone5C                               |
| arm64  | iPhone5s——iPhoneX                               |
| arm64e | iPhone XS、iPhone XS Max、iPhoneXR、iPhone11... |

#### 2.2生成多种架构

新建一个工程，真机运行，查看可执行文件仅仅是一个arm64架构的

![img](https://p1-jj.byteimg.com/tos-cn-i-t2oaga2asx/gold-user-assets/2019/12/15/16f07f9f9126dda9~tplv-t2oaga2asx-zoom-in-crop-mark:4536:0:0:0.image)

将项目最低适配系统调为iOS9.0，真机运行`Relese环境`

![img](https://p1-jj.byteimg.com/tos-cn-i-t2oaga2asx/gold-user-assets/2019/12/15/16f08008f3d55aab~tplv-t2oaga2asx-zoom-in-crop-mark:4536:0:0:0.image)



**为什么要改为iOS9.0呢**？是因为iPhone5c等armv7、armv7s架构不支持iOS11.0

**为什么要Relese环境运行呢**？因为Xcode默认Debug只生成单一架构

![img](https://p1-jj.byteimg.com/tos-cn-i-t2oaga2asx/gold-user-assets/2019/12/15/16f080382e437831~tplv-t2oaga2asx-zoom-in-crop-mark:4536:0:0:0.image)



**怎么生成所有架构**？Xcode10中只包含了v7和64，需要在`Architectures`中添加

![img](https://p1-jj.byteimg.com/tos-cn-i-t2oaga2asx/gold-user-assets/2019/12/15/16f080ececc0f693~tplv-t2oaga2asx-zoom-in-crop-mark:4536:0:0:0.image)

![img](https://p1-jj.byteimg.com/tos-cn-i-t2oaga2asx/gold-user-assets/2019/12/15/16f080d661eb4112~tplv-t2oaga2asx-zoom-in-crop-mark:4536:0:0:0.image)



## 3、通用二进制文件

#### 3.1定义

通用二进制文件（Universal binary）也被叫做`胖二进制（Fat binary）`

- 苹果公司提出的一种程序代码，能同时适用多种架构的二进制文件
- 同一个程序包中同时为多种架构提供最理想的性能
- 因为需要储存多种代码，通用二进制应用程序通常比单一平台二进制的程序要大
- 但是由于两种架构有共通的非执行资源，所以并不会达到单一版本的两倍之多
- 而且由于执行中只调用一部分代码，运行起来也不需要额外的内存

#### 3.2拆分/合并架构

架构拆分

![img](https://p1-jj.byteimg.com/tos-cn-i-t2oaga2asx/gold-user-assets/2019/12/15/16f08240102dc4d4~tplv-t2oaga2asx-zoom-in-crop-mark:4536:0:0:0.image)



合并架构

![img](https://p1-jj.byteimg.com/tos-cn-i-t2oaga2asx/gold-user-assets/2019/12/15/16f082abf93c1aab~tplv-t2oaga2asx-zoom-in-crop-mark:4536:0:0:0.image)

`通用二进制`大小为342kb，四个架构大小为80+80+80+81=321kb



What！为什么不是单纯的1+1=2？

因为不同架构之间代码部分是不共用的 (因为代码的二进制文件不同的组合在不同的 cpu 上可能会是不同的意义)，而公共资源文件是公用的

> 利用上述方法可以给我们的app瘦身

**结论：**

①`胖二进制`拆分后再重组会得到原始`胖二进制`

②`通用二进制`的大小可能大于子架构大小之和，也可能小于，也可能等于，取决于`公共资源文件`的多少

#### 3.3终端命令行

```scss
// 查看二进制文件
$ lipo -info xx 
// 通用二进制文件
// 拆分二进制文件
lipo xxx -thin armv7 -output xxx
// 组合二进制文件
lipo -create x1 x2 x3 x4 -output xxx
复制代码
```

## 4、MachO文件

#### 4.1整体结构

用`MachOView`打开会看到`通用二进制文件`由`Fat Header`和`四个可执行文件`组成

![img](https://p1-jj.byteimg.com/tos-cn-i-t2oaga2asx/gold-user-assets/2019/12/15/16f084564833fe48~tplv-t2oaga2asx-zoom-in-crop-mark:4536:0:0:0.image)

`可执行文件`是由`Header`、`Load commands`和`Data`组成

![img](https://p1-jj.byteimg.com/tos-cn-i-t2oaga2asx/gold-user-assets/2019/12/15/16f0845ec5968b12~tplv-t2oaga2asx-zoom-in-crop-mark:4536:0:0:0.image)

我们可以这么理解，把`通用二进制文件`看作四本翻译语言不同的书，每本书有`标题（header）`、`目录（load commands）`、`内容（data）`



- header：
- load commands：
- data：

另外我们也可以通过`otool`命令行查看MachO文件结构

```ruby
$ otool -f universe
```



![img](https://p1-jj.byteimg.com/tos-cn-i-t2oaga2asx/gold-user-assets/2019/12/15/16f08502c785de7b~tplv-t2oaga2asx-zoom-in-crop-mark:4536:0:0:0.image)



#### 4.2header

`header`包含了该二进制文件的字节顺序、架构类型、加载指令的数量等，使得可以快速确认一些信息，比如当前文件用于`32 位`还是`64 位`，对应的处理器是什么、文件类型是什么

Xcode中 `shift+command+O`->`load.h`->如下信息

```arduino
struct mach_header_64 {
    uint32_t	magic;		/* 魔数,快速定位64位/32位 */
    cpu_type_t	cputype;	/* cpu 类型 比如 ARM */
    cpu_subtype_t	cpusubtype;	/* cpu 具体类型 比如arm64 , armv7 */
    uint32_t	filetype;	/* 文件类型 例如可执行文件 .. */
    uint32_t	ncmds;		/* load commands 加载命令条数 */
    uint32_t	sizeofcmds;	/* load commands 加载命令大小*/
    uint32_t	flags;		/* 标志位标识二进制文件支持的功能 , 主要是和系统加载、链接有关*/
    uint32_t	reserved;	/* reserved , 保留字段 */
};
```

> mach_header_64（64位）对比mach_header（32位）只多了一个保留字段

#### 4.3load commands

`load commands`是一张包括区域的位置、符号表、动态符号表等内容的表。 它详细保存着加载指令的内容，告诉链接器如何去加载这个 Mach-O 文件。 通过查看内存地址我们发现，在内存中`load commands`是紧跟在`header`之后的

| 名称                  | 内容                                           |
| --------------------- | ---------------------------------------------- |
| LC_SEGMENT_64         | 将文件中（32位或64位）的段映射到进程地址空间中 |
| LC_DYLD_INFO_ONLY     | 动态链接相关信息                               |
| LC_SYMTAB             | 符号地址                                       |
| LC_DYSYMTAB           | 动态链接相关信息                               |
| LC_LOAD_DYLINKER      | 动态链接相关信息                               |
| LC_UUID               | 动态链接相关信息                               |
| LC_VERSION_MIN_MACOSX | 支持最低的操作系统版本                         |
| LC_SOURCE_VERSION     | 源代码版本                                     |
| LC_MAIN               | 设置程序主线程的入口地址和栈大小               |
| LC_LOAD_DYLIB         | 依赖库的路径，包含三方库                       |
| LC_FUNCTION_STARTS    | 函数起始地址表                                 |
| LC_CODE_SIGNATURE     | 代码签名                                       |

#### 4.4data

`data`是MachO文件中最大的部分，其中`_TEXT段`、`_DATA段`能给到很多信息

`load commands`和`data`之间还留有不少空间，给我们留下了注入代码的冲破口

![img](https://p1-jj.byteimg.com/tos-cn-i-t2oaga2asx/gold-user-assets/2019/12/15/16f0949ed460a46a~tplv-t2oaga2asx-zoom-in-crop-mark:4536:0:0:0.image)

**_TEXT段**



| 名称                 | 作用           |
| -------------------- | -------------- |
| _text                | 主程序代码     |
| _stubs、_stub_helper | 动态链接       |
| _objc_methodname     | 方法名称       |
| _objc_classname      | 类名称         |
| _objc_methtype       | 方法类型(v@:)  |
| _cstring             | 静态字符串常量 |

**_DATA段**

| 名称                                 | 作用           |
| ------------------------------------ | -------------- |
| _got=>Non-Lazy Symbol Pointers       | 非懒加载符号表 |
| _la_symbol_ptr=>Lazy Symbol Pointers | 懒加载符号表   |
| _objc_classlist                      | 方法名称       |
| ...                                  | ...            |

## 5、dyld

dyld（the dynamic link editor）是苹果的动态链接器，是苹果操作系统的一个重要组成部分，在系统内容做好程序准备工作之后，交由dyld负责余下的工作

系统库的方法由于是公用的，存放在共享缓存中，那么我们的MachO在调用系统方法时，dyld会将MachO里调用存放在共享缓存中的方法进行符号绑定。这个符号在`release环境` 是会被自动去掉的，这也是我们经常使用收集 bug 工具时需要恢复符号表的原因


原文链接：https://juejin.cn/post/6844904021010939918