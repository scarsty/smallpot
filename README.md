# tinypot
<img src="https://raw.githubusercontent.com/scarsty/bigpot/master/logo.png" width = "20%" />

## 简介

TinyPot是一个轻量级播放器。

该播放器的前身是金庸水浒传的片头动画播放子程。在整个游戏过程中该子程仅被调用了一次，但是为了做好这个部分，开发组使用了FFmpeg进行解码，BASS进行播放，SDL2进行输出，并成功将其移植到了其他平台。因此，金庸水浒传的片头实际支持相当多的格式。而大水壶播放器在设计阶段，也是使用类似的架构，但是在开发阶段发现音频难以控制，因此改为了使用SDL2播放。

而水浒的谐音是水壶，同时新论坛叫大武侠，有个“大”字，所以该播放器起名“大水壶”，英文BigPot。至于跟著名的播放器PotPlayer有没有关系，答案是一点都没有，而且PotPlayer的功能远远强于大水壶，名字有点像只是巧合。

另外现在改叫小水壶了……

## 架构

程序语言是C++，使用FFmpeg进行解码，SDL2硬件输出，还有SDL_image、SDL_ttf等库。字幕部分使用libass，该库又依赖Fontconfig、freetype和fribidi。配置文件使用的是tinyxml2，hash是sha3。

该播放器的架构并未参考其他主流播放器，而是重新设计的单线程预解，原理如下图。在跳转的时候可能会稍慢于其他的主流播放器，但是相差并不明显。

<img src="https://raw.githubusercontent.com/scarsty/bigpot/master/pic/ac.png" width = "50%" />

## 编译

需要首先下载<https://github.com/scarsty/common>并置于适合的位置，或者参考脚本get-common.sh。

### Windows

请使用Visual Studio 2017。已经包含了Windows版本所需的工程文件，以及导入库和dll。主要使用msys2的dll文件。

32位部分默认编译为dll，可以嵌入其他程序的窗口播放。特别是基于SDL2的游戏，用法非常简单。

### macosx

推荐使用homebrew安装libSDL2，libSDL2-image，libSDL2-ttf，以及libass、FFmpeg、iconv等。

使用CMake生成Makefile。

脚本a.sh可以自动做编译和处理库依赖。

### linux

与上面方法类似，但是通常不需要打包为app。

## 使用方法

因为没有制作配置的图形界面，所以仅能将文件拖到图标或者窗口上进行播放，或者设置为文件类型默认的打开方式。

### 支持的格式

FFmpeg能解什么格式它就能放什么格式，FFmpeg不能解的，它也放不出来。而且也不考虑调用其他的解码器，因为作者不会。

特别地，不能播放WAV，以及WAV为音频流的视频文件，因为WAV是没有压缩的，谈不上解码。也不推荐用它播放纯音频，因为它的音频没有经过处理，只是把解码的结果原样放出来，远不及专门的播放器。

### 字幕

打开文件的时候，会先判断有没有字幕，有的话会自动载入。或者播放的时候拖一个字幕进去也会载入字幕，而字幕的扩展名必须是ass、ssa、srt、txt其中之一。其他文件都会当成媒体文件处理，能否播放看解码器的。

查找字幕的方式是先依次将媒体文件的扩展名替换为ass、ssa、srt，并在媒体所在目录的sub子目录下寻找。

### 功能键

方向左右是跳过几秒，上下、加减号或者鼠标滚轮是音量，空格或鼠标点右上是暂停，回车是全屏，退格是返回文件开始，del删除播放记录（打开曾经播放过的文件会自动跳转到上次退出的位置）。

1是切换音频流，2是切换字幕流，3是内部字幕显示/隐藏，4是外部字幕显示/隐藏。

句号（大于号）是下一个文件，逗号（小于号）是上一个文件。

### XML中的设置

```xml
volume：音量
auto_play_recent：自动播放上次关闭时的文件
record_name：是否记录文件名
sys_encode：系统字串编码
ui_font：显示界面的字体
sub_font：显示字幕的默认字体
```
## 遗留问题

因为是单线程架构，所以在一些文件跳转时会出现马赛克。一般来说这个可以通过清除解码器状态来解决，但是单线程架构下这个操作会导致后面一帧的解码卡顿，故没有这么做。

通常RM和RMVB，以及从流媒体服务器直接下载的MP4可能有此问题。

## 预编译版下载

windows版：

<https://www.dawuxia.net/tinypot/tinypot-x64.7z>

Mac版：

<https://www.dawuxia.net/tinypot/tinypot.app.zip>

## 播放效果

<img src="https://raw.githubusercontent.com/scarsty/bigpot/master/pic/1.png" width = "80%" />
