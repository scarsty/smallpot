# smallpot
<img src="https://raw.githubusercontent.com/scarsty/bigpot/master/logo.png" width = "20%" />

## 简介

SmallPot是一个轻量级播放器。

该播放器的前身是金庸水浒传的片头动画播放子程。在整个游戏过程中该子程仅被调用了一次，但是为了做好这个部分，开发组使用了FFmpeg进行解码，BASS进行播放，SDL2进行输出，并成功将其移植到了其他平台。因此，金庸水浒传的片头实际支持相当多的格式。而大水壶播放器在设计阶段，也是使用类似的架构，但是在开发阶段发现音频难以控制，因此改为了使用SDL2播放。

而水浒的谐音是水壶，同时新论坛叫大武侠，有个“大”字，所以该播放器起名“大水壶”，英文BigPot。至于跟著名的播放器PotPlayer有没有关系，答案是一点都没有，而且PotPlayer的功能远远强于大水壶，名字有点像只是巧合。

另外据说叫大太嚣张，现在改叫小水壶了。

## 架构

程序语言是C++，使用FFmpeg进行解码，SDL2硬件输出，还有SDL_image、SDL_ttf等库。字幕部分使用libass，该库又依赖Fontconfig、freetype和fribidi。配置文件使用的是ini。

该播放器的架构并未参考其他主流播放器，而是重新设计的单线程预解，原理如下图。在跳转的时候可能会稍慢于其他的主流播放器，但是相差并不明显。

<img src="https://raw.githubusercontent.com/scarsty/smallpot/master/pic/ac.png" width = "50%" />

## 编译

首先需要取得mlcc工程。

```shell
git clone https://github.com/scarsty/mlcc mlcc
```

Engine.h和Engine.cpp在kys-cpp中。

其余依赖库包括iconv，ffmpeg，libass，SDL2，SDL2-image，SDL2-ttf等，推荐使用系统的包管理工具获取这些库，Windows下推荐使用vcpkg。其中SDL2_image仅有一处使用，且并不是必须的，可以简单修改后去除。

Windows下也可以从<https://github.com/scarsty/lib-collection>取得头文件和导入库。

<https://github.com/AutoItConsulting/text-encoding-detect>直接包含代码到工程中。

### Windows

请使用Visual Studio编译。
若编译为dll，可以嵌入其他程序的窗口播放。特别是基于SDL2的游戏，用法非常简单。
建议使用vcpkg安装依赖库。

### MacOS

推荐使用homebrew安装依赖库。

使用CMake生成Makefile。

脚本a.sh可以自动编译和处理动态库的依赖修正。

**由于Mac上App目录的参数传递非常SB，目前无法直接打开文件。**

### Linux

与上面方法类似，但是通常不需要打包为app，因此比Mac要简单。

### 单文件版

如果需要编译单文件（全静态链接）版，导入库比动态链接版要多出很多，建议使用vcpkg之类解决（vcpkg生成的fribidi静态库不正确，需手动修正）。

以下为参考（fribidi及以下是动态链接不需要的）：

```
sdl2.lib
sdl2_ttf.lib
sdl2_image.lib
sdl2main.lib
ass.lib
libiconv.lib
avutil.lib
avcodec.lib
avformat.lib
swresample.lib
swscale.lib
fribidi.lib
harfbuzz.lib
freetype.lib
bz2.lib
libpng16.lib
zlib.lib
libcharset.lib
winmm.lib
version.lib
imm32.lib
bcrypt.lib
secur32.lib
ws2_32.Lib
```

若是需要编译dll文件，用于在其他基于SDL2的游戏中播放视频时，则SmallPot和游戏均不应静态链接SDL。因为SDL的动态库中含有全局变量，多次静态链接后该变量会有多个副本，其中一个很可能是不正确的。

## 使用方法

因为没有制作配置的图形界面，所以仅能将文件拖到图标或者窗口上进行播放，或者设置为文件类型默认的打开方式。

### 支持的格式

FFmpeg能解什么格式它就能放什么格式，FFmpeg不能解的，它也放不出来。而且也不考虑调用其他的解码器，因为作者不会。

特别地，不能播放WAV，以及WAV为音频流的视频文件，因为WAV是没有压缩的，谈不上解码。也不推荐用它播放纯音频，因为它的音频没有经过处理，只是把解码的结果原样放出来，远不及专门的播放器。

### 字幕

打开文件的时候，会先判断有没有字幕，有的话会自动载入。或者播放的时候拖一个字幕进去也会载入字幕，而字幕的扩展名必须是ass、ssa、srt、txt其中之一。其他文件都会当成媒体文件处理，能否播放看解码器的。

查找字幕的方式是先依次将媒体文件的扩展名替换为ass、ssa、srt，并在媒体所在目录下以及subs子目录中寻找，即可以将字幕集中放到subs子目录。

### 功能键

| 按键               | 功能                   |
| ------------------ | ---------------------- |
| 方向左右，鼠标滚轮  | 跳过几秒               |
| 方向上下             | 音量                   |
| 空格               | 暂停                   |
| 回车               | 全屏切换               |
| 退格               | 回到视频开头           |
| Delete             | 删除播放记录           |
| 1                  | 切换音频流             |
| 2                  | 切换字幕流             |
| ,(<)               | 上一个文件             |
| .(>)               | 下一个文件             |
| 0                  | 窗口大小调整为视频尺寸 |
| -                  | 减小窗口               |
| =(+)               | 增大窗口               |

鼠标放在音量时，滚轮可以控制音量。

直接点击音量部分也可以控制音量。

### ini中的设置

| 设置             | 功能                     |
| ---------------- | ------------------------ |
| volume           | 音量                     |
| auto_play_recent | 自动播放上次关闭时的文件 |
| record_name      | 是否记录文件名           |
| sys_encode       | 系统字串编码             |
| ui_font          | 显示界面的字体           |
| sub_font         | 显示字幕的默认字体       |

## 遗留问题

因为是单线程架构，所以在一些文件跳转时会出现马赛克。一般来说这个可以通过清除解码器状态来解决，但是单线程架构下这个操作会导致后面一帧的解码卡顿，故没有这么做。

通常RM和RMVB，以及从流媒体服务器直接下载的MP4可能有此问题。

## 播放效果

<img src="https://raw.githubusercontent.com/scarsty/smallpot/master/pic/1.png" width = "80%" />
