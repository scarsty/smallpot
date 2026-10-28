# smallpot
<img src="https://raw.githubusercontent.com/scarsty/bigpot/master/logo.png" width = "20%" />

## 简介

SmallPot是一个轻量级播放器。

该播放器的前身是金庸水浒传的片头动画播放子程。在整个游戏过程中该子程仅被调用了一次，但是为了做好这个部分，开发组使用了FFmpeg进行解码，BASS进行播放，SDL2进行输出，并成功将其移植到了其他平台。因此，金庸水浒传的片头实际支持相当多的格式。而大水壶播放器在设计阶段，也是使用类似的架构，但是在开发阶段发现音频难以控制，因此改为了使用SDL2播放。

而水浒的谐音是水壶，同时新论坛叫大武侠，有个“大”字，所以该播放器起名“大水壶”，英文BigPot。至于跟著名的播放器PotPlayer有没有关系，答案是一点都没有，而且PotPlayer的功能远远强于大水壶，名字有点像只是巧合。

另外据说叫大太嚣张，现在改叫小水壶了。

## 架构

程序语言是 C++23，使用 FFmpeg 进行解码、SDL3 进行音视频输出，并使用 SDL3_ttf 显示界面文字。字幕功能使用 libass；该库还依赖 Fontconfig、FreeType 和 FriBidi。桌面版配置文件为运行目录下的 `smallpot.config.json`。

该播放器最初采用自行设计的单线程预解架构，后续跳转处理参考了 mpv 的请求调度和播放恢复思路。应用层的输入、解复用、解码和渲染仍由同一个主循环驱动：输入事件只提交跳转请求，播放步骤执行定位和解码器刷新，随后恢复视频显示及音频提交。请求式处理并不意味着单次 FFmpeg 调用变成异步操作。

下图为原始预解架构示意；当前跳转流程以以下文字说明为准。

<img src="https://raw.githubusercontent.com/scarsty/smallpot/master/pic/ac.png" width = "50%" />

相对跳转以尚未执行的目标或正在恢复的目标为基准，只累加一次。新的跳转请求可以在下一轮替换尚未完成的恢复，不必等待旧目标播放到达。恢复期间首个新视频帧不受暂停的音频时钟约束；快速跳转在该帧显示后恢复音频，精确跳转则先丢弃目标前的视频帧。到达文件尾且没有新帧时也会退出恢复状态。

### mpv 与 VLC 的跳转实现比较

针对滚轮快速跳转的源码比较，固定参考版本为 mpv commit `baa3e11d42cb7978da634a8a6a3a55fe074d01be`，VLC commit `6ae433cc2c45c30563c4db9fb792ba0d64f8b059`。

结论如下：

- **mpv 不是通过完全不刷新解码器来实现快速跳转。** `player/playloop.c` 中的 `mp_seek()` 仍会执行 demux seek、清理音频输出和 `reset_playback_state()`。
- mpv 的 seek 请求先由 `queue_seek()` 排队，实际在播放循环的 `execute_queued_seek()` 阶段执行；循环中还会处理音视频输出。仅凭调用顺序，不能证明其首个目标帧一定更快显示，也不能认定它靠继续显示旧帧消除了跳转等待。
- mpv 使用 `hrseek_active`、`last_seek_pts` 等状态，先定位到目标附近，再从关键帧解码到目标时间，形成粗定位与精确定位两阶段。
- VLC 也有输入、解复用、解码和音视频输出链路，并非 smallpot 式的单线程播放循环。其 `src/input/decoder.c`、`src/input/es_out.c` 维护 decoder FIFO、PCR/PTS 和 program clock；仅凭这些结构，不能判定它比 mpv 慢的具体原因。
- VLC 的 `src/input/decoder_prevframe.c` 提供上一帧保存与逐帧后退支持，但不能据此断言普通滚轮 seek 一定使用该路径。
- 因此，VLC“卡一下”与 mpv“更跟手”的主要差异，不应简单归因于“VLC flush、mpv 不 flush”或单一缓存开关，更可能来自 seek 执行时机、输出流水线、精确定位状态和音视频时钟恢复方式的组合。

### smallpot 的跳转恢复与实测结果

当前实现通过 `PotMedia::requestSeek()` 提交绝对目标，通过 `requestRelativeSeek()` 累加相对位移，由播放循环调用 `processSeekStep()` 执行。每次实际定位都正常 flush 解码器，不再沿用旧 H.265 参考帧换取速度。

| 操作 | 视频恢复条件 | 取舍 |
| --- | --- | --- |
| 滚轮等快速跳转 | 首个可用新视频帧显示后结束恢复 | 允许落在请求时间附近，不保证每次精确移动一秒 |
| 进度条精确跳转 | 丢弃目标前的视频帧，显示到达目标的帧后结束恢复 | 定位更准确，但可能需要解码更多帧 |

恢复期间暂停音频数据提交。`showVideoFrame()` 使用新视频帧自身时间决定首帧显示，不等待被暂停的音频时钟；`finishSeekAfterFrameShown()` 在符合恢复条件的帧显示后解除 seek 状态，随后允许音频继续提交。保留已有画面只避免显示空白，不能代替新目标帧，也不能降低解码本身的成本。

2026-10-27 用户实测：修正后 H.265 滚轮跳转明显流畅，观感接近 mpv。对应修复包括：

1. 消除“音频等待视频恢复、视频又等待音频时钟”的相互等待。
2. 快速后退落在目标前的关键帧附近时，不再等待播放到精确目标才结束恢复。
3. 新请求可在下一轮替换旧恢复，不必等待已经过时的目标。
4. 相对位移只计算一次，避免正反向滚轮交替时目标重复偏移。

这些结果说明，此次明显停顿主要与状态、时钟及目标计算逻辑有关，不能直接归因于单线程或 H.265 的 flush 成本，更不能据此认定必须增加后台解码线程。尚无分阶段耗时测量，不能量化各项修复的贡献，也不能将这次体验推广到所有媒体。mpv 与 VLC 在同一文件、相同定位精度及解码配置下的性能差异，仍需测量验证。

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

ASS/SSA 字幕通过 libass 的系统字体提供器查找已安装字体；Windows 使用 DirectWrite/GDI，不再把整个系统字体目录额外加载到每个字幕渲染器。缺少字幕指定字体时，可将字体安装进系统。此调整不影响使用 SDL3_ttf 的 SRT 字体配置。

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

当前跳转已正常刷新解码器；此前“为了速度不 flush，从而可能出现马赛克”的实验策略不再适用。单线程本身并不必然导致马赛克。

正常解码时，`avcodec_send_packet()` 返回 `EAGAIN` 表示输入包尚未被接收：必须保留该包，取出可用的输出帧后重试，不能释放并跳过它。音视频输出帧使用自身的 `best_effort_timestamp`（回退到帧 `pts`）定时，而不是使用本次提交包的 DTS；解码缓存和重排序会使二者不对应。否则 H.265 丢失参考包后可能连续报错、跳到后续可解码帧，画面等待其未来时间戳，看起来像自动暂停。

单次 `avformat_seek_file()`、解码或纹理转换调用仍可能阻塞主循环。高分辨率、长 GOP、索引不完整或网络媒体的跳转耗时仍需按样本验证；当前请求式恢复不保证所有文件都能无停顿跳转。

## 播放效果

<img src="https://raw.githubusercontent.com/scarsty/smallpot/master/pic/1.png" width = "80%" />
