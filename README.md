# smallpot
<img src="https://raw.githubusercontent.com/scarsty/bigpot/master/logo.png" width = "20%" />

## 简介

SmallPot是一个轻量级播放器。

该播放器的前身是金庸水浒传的片头动画播放子程。在整个游戏过程中该子程仅被调用了一次，但是为了做好这个部分，开发组使用了FFmpeg进行解码，BASS进行播放，SDL2进行输出，并成功将其移植到了其他平台。因此，金庸水浒传的片头实际支持相当多的格式。而大水壶播放器在设计阶段，也是使用类似的架构，但是在开发阶段发现音频难以控制，因此改为了使用SDL2播放。

而水浒的谐音是水壶，同时新论坛叫大武侠，有个“大”字，所以该播放器起名“大水壶”，英文BigPot。至于跟著名的播放器PotPlayer有没有关系，答案是一点都没有，而且PotPlayer的功能远远强于大水壶，名字有点像只是巧合。

另外据说叫大太嚣张，现在改叫小水壶了。

## 架构

程序语言是 C++23，使用 FFmpeg 进行解码、SDL3 进行音视频输出，并使用 SDL3_ttf 显示界面文字。字幕功能使用 libass；该库还依赖 Fontconfig、FreeType 和 FriBidi。桌面版配置文件为运行目录下的 `smallpot.config.json`。

该播放器的架构并未参考其他主流播放器，而是重新设计的单线程预解，原理如下图。在跳转的时候可能会稍慢于其他的主流播放器，但是相差并不明显。

<img src="https://raw.githubusercontent.com/scarsty/smallpot/master/pic/ac.png" width = "50%" />

## 编译

主程序源码和 `Engine` 已包含在本仓库的 `src` 目录中。CMake 构建仍会引用同级目录中的 `mlcc`：

```shell
git clone https://github.com/scarsty/mlcc ../mlcc
```

其余依赖包括 iconv、FFmpeg、libass、SDL3 和 SDL3_ttf。推荐通过系统包管理器安装；Windows 下建议使用 vcpkg。CMake 要求 3.20 或更高版本以及支持 C++23 的编译器。

<https://github.com/AutoItConsulting/text-encoding-detect>直接包含代码到工程中。

### Windows

请使用支持 C++23 的 Visual Studio 编译。`src/smallpot.vcxproj` 用于构建桌面播放器，`smallpot.dll/smallpot.dll.vcxproj` 用于构建供游戏嵌入的视频播放 DLL。建议使用 vcpkg 安装依赖库。

### MacOS

推荐使用homebrew安装依赖库。

使用CMake生成Makefile。

脚本a.sh可以自动编译和处理动态库的依赖修正。

**由于Mac上App目录的参数传递非常SB，目前无法直接打开文件。**

### Linux

与上面方法类似，但是通常不需要打包为app，因此比Mac要简单。

### 单文件版

如果需要编译单文件（全静态链接）版，导入库比动态链接版要多出很多，建议使用vcpkg之类解决。

以下为参考。其中fribidi及以下是动态链接不需要的，winmm.lib及以下是Windows自带的库：

```
sdl3.lib
sdl3_ttf.lib
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
mfplat.lib
mfuuid.lib
strmiids.lib
```

### 在游戏中嵌入 DLL 播放视频（Windows）

打开 `SmallPot.sln`，以 **x64** 配置生成 `smallpot.dll` 项目。该项目定义 `WITHOUT_SUBTITLE`，因此 DLL 版本不包含字幕功能。游戏进程、`smallpot.dll` 以及其依赖 DLL 必须使用相同的架构；发布时应将 `smallpot.dll` 及 SDL3、FFmpeg 等运行时依赖放在游戏可搜索的目录中。

`smallpot.dll/PotDll.h` 导出 C ABI，并使用 `__stdcall` 调用约定。文件名参数必须是 UTF-8 编码的、以 `\0` 结尾的可写字符数组。`PotInputVideo` 和 `PotPlayVideo` 会进入播放事件循环，应从适合阻塞播放的游戏流程调用，而非每帧调用。

| 函数 | 说明 |
| --- | --- |
| `PotCreateFromHandle(void* handle)` | 用原生 Win32 `HWND` 创建嵌入式播放器。 |
| `PotCreateFromWindow(void* handle)` | 用已有的 `SDL_Window*` 创建播放器；适合 SDL3 游戏。 |
| `PotInputVideo(void* pot, char* filename)` | 以当前音量同步播放文件。返回播放器退出类型。 |
| `PotPlayVideo(void* pot, char* filename, float volume)` | 设置音量后同步播放文件。音量通常应在 $0.0$ 到 $1.0$ 范围内。 |
| `PotSeek(void* pot, int seek)` | 跳转到从媒体开头算起的毫秒位置。应在播放器仍有效时调用。 |
| `PotDestory(void* pot)` | 释放播放器实例。函数名按当前导出 API 保持 `Destory` 拼写。 |

示例（SDL3 游戏窗口）：

```cpp
#include "PotDll.h"

void PlayOpeningVideo(SDL_Window* gameWindow, char* filename)
{
    void* player = PotCreateFromWindow(gameWindow);
    if (player != nullptr)
    {
        PotPlayVideo(player, filename, 0.8f);
        PotDestory(player);
    }
}
```

当游戏和 SmallPot 都使用 SDL3 时，应让两者共享同一份 SDL3 动态库，避免分别静态链接 SDL。SDL 内部含有全局状态，多份静态副本可能导致行为异常。

## 使用方法

因为没有制作配置的图形界面，所以仅能将文件拖到图标或者窗口上进行播放，或者设置为文件类型默认的打开方式。

### 支持的格式

FFmpeg能解什么格式它就能放什么格式，FFmpeg不能解的，它也放不出来。而且也不考虑调用其他的解码器，因为作者不会。

特别地，不能播放WAV，以及WAV为音频流的视频文件，因为WAV是没有压缩的，谈不上解码。也不推荐用它播放纯音频，因为它的音频没有经过处理，只是把解码的结果原样放出来，远不及专门的播放器。

### 字幕

打开文件的时候，会先判断有没有字幕，有的话会自动载入。或者播放的时候拖一个字幕进去也会载入字幕，而字幕的扩展名必须是ass、ssa、srt、txt其中之一。其他文件都会当成媒体文件处理，能否播放看解码器的。

查找字幕的方式是先依次将媒体文件的扩展名替换为ass、ssa、srt，并在媒体所在目录下以及subs子目录中寻找，即可以将字幕集中放到subs子目录。

### 字幕的字体文件

libass的字体文件只能设置一个目录，所以如果字体显示不正常请将其安装进系统。

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

### `smallpot.config.json` 中的设置

| 设置                | 功能                                      |
| ------------------- | ----------------------------------------- |
| `volume`            | 音量。                                    |
| `auto_play_recent`  | 非零时自动播放上次关闭时的文件。          |
| `sys_encode`        | Windows 文件路径使用的系统编码。           |
| `ui_font`           | 显示界面使用的字体。                      |
| `sub_font`          | 字幕的默认字体。                          |
| `channels`          | 输出音频声道数；负数时使用媒体原始声道数。|
| `windows_maximized` | 非零时以最大化窗口启动。                  |

播放记录保存在同一配置文件的 `record` 节点中；`record_name` 不是当前版本支持的设置。

## 遗留问题

因为是单线程架构，所以在一些文件跳转时会出现马赛克。一般来说这个可以通过清除解码器状态来解决，但是单线程架构下这个操作会导致后面一帧的解码卡顿，故没有这么做。

通常RM和RMVB，以及从流媒体服务器直接下载的MP4可能有此问题。

## 播放效果

<img src="https://raw.githubusercontent.com/scarsty/smallpot/master/pic/1.png" width = "80%" />
