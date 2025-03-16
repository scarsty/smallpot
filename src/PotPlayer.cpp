﻿#include "PotPlayer.h"
#include "Config.h"
#include "Font.h"
#include "PotSubtitleManager.h"
#include "strfunc.h"

#ifdef _WIN32
//#include <shlobj.h>
//#pragma comment(lib,"shfolder.lib")
#endif

PotPlayer::PotPlayer()
{
    //_subtitle = new BigPotSubtitle;
    //Config::getInstance()->init();
    width_ = 320;
    height_ = 150;
    handle_ = nullptr;
    run_path_ = "./";
}

PotPlayer::PotPlayer(char* s) :
    PotPlayer()
{
    run_path_ = filefunc::getParentPath(s);
#if defined(_WIN32) && defined(_SINGLE_FILE)
    char szPath[MAX_PATH];
    //SHGetSpecialFolderPath(NULL, szPath, CSIDL_LOCAL_APPDATA, false);
    SHGetFolderPathA(NULL, CSIDL_LOCAL_APPDATA, NULL, 0, szPath);
    //std::wstring ws(szPath);
    std::string str(szPath);
    run_path_ = str + "/bigpot";
    WIN32_FIND_DATAA wfd;
    if (FindFirstFileA(run_path_.c_str(), &wfd) == INVALID_HANDLE_VALUE)
    {
        CreateDirectoryA(run_path_.c_str(), NULL);
    }
#endif
}

PotPlayer::~PotPlayer()
{
    //delete _UI;
    //delete _config;
    //delete _subtitle;
    //delete media;
}

int PotPlayer::eventLoop()
{
    EngineEvent e;

    bool hold_mouse = false;
    int64_t hold_time = 0;
    bool loop = true, pause = false, seeking = false;
    int finished, i = 0;
    const int seek_step = 1000;
    float volume_step = 1.0 / 128;
    bool havevideo = media_->getVideo()->exist();
    bool havemedia = media_->getAudio()->exist() || havevideo;
    int totalTime = media_->getTotalTime();
    std::string open_filename;
    fmt1::print("Total time is {:1.3}s or {}min {:1.3}s\n", totalTime / 1e3, totalTime / 60000, totalTime % 60000 / 1e3);

    int maxDelay = 0;          //统计使用
    int prev_show_time = 0;    //上一次显示的时间
    exit_type_ = 0;

    int sub_state = 0;    //0不显示，1外部字幕，2及以上内部字幕
    int internal_sub_count = media_->getStreamCount(MEDIA_TYPE_SUBTITLE);
    if (internal_sub_count > 0)
    {
        sub_state = 2;
    }
    if (subtitle_ && subtitle_->exist())
    {
        sub_state = 1;
    }
    int find_direct = 0;

    float touch_start_x = 0, touch_start_y = 0;
    double touch_start_time = 0;

    while (loop && engine_->pollEvent(e) >= 0)
    {
        seeking = false;
        find_direct++;    //连续24天后方向会出现bug，但是不管了
        int last_volume = media_->getAudio()->getVolume();

        auto switchSubtitle = [&]()
        {
            sub_state++;
            if (sub_state == 1)
            {
                if (subtitle_ == nullptr || !subtitle_->exist())
                {
                    sub_state++;
                }
            }
            else if (sub_state > 2)
            {
                media_->switchStream(MEDIA_TYPE_SUBTITLE);
                media_->getSubtitle()->setFrameSize(engine_->getPresentWidth(), engine_->getPresentHeight());
            }
            if (sub_state >= 2 + internal_sub_count)
            {
                sub_state = 0;
            }
            if (sub_state == 0)
            {
                UI_.setText("Hide subtitles");
            }
            else if (sub_state == 1)
            {
                UI_.setText("External subtitles");
            }
            else if (sub_state >= 2 && media_->getSubtitle()->getStreamIndex() >= 0)
            {
                UI_.setText(fmt1::format("Internal subtitles stream {}", media_->getSubtitle()->getStreamIndex()));
            }
        };

        auto seekButton = [&]()
        {
            if (e.button.button == BUTTON_LEFT)
            {
                int button = UI_.inButton();
                if (button == PotUI::ButtonLeft)
                {
                    media_->seekTime(media_->getTime() - seek_step, -1);
                    UI_.setText("");
                    seeking = true;
                }
                else if (button == PotUI::ButtonRight)
                {
                    media_->seekTime(media_->getTime() + seek_step, 1);
                    UI_.setText("");
                    seeking = true;
                }
            }
        };

        if (hold_mouse && engine_->getTicks() - hold_time > 500)
        {
            seekButton();
        }

        switch (e.type)
        {
        case EVENT_MOUSE_MOTION:
            break;
        case EVENT_MOUSE_BUTTON_UP:
            hold_mouse = false;
            if (e.button.button == BUTTON_LEFT)
            {
                double pos = UI_.inProcess();
                int button = UI_.inButton();
                if (pos >= 0)
                {
                    media_->seekPos(pos, 1, 1);
                    seeking = true;
                }
                if (button == PotUI::ButtonPause)
                {
                    pause = !pause;
                    media_->setPause(pause);
                }
                else if (button == PotUI::ButtonNext)
                {
                    find_direct = 1;
                    auto next_file = findNextFile(drop_filename_, find_direct);
                    if (next_file != "")
                    {
                        drop_filename_ = next_file;
                        loop = false;
                    }
                }
                else if (button == PotUI::ButtonFullScreen)
                {
                    engine_->toggleFullscreen();
                    setSubtitleFrameSize();
                }
                else if (button == PotUI::ButtonAudio)
                {
                    media_->switchStream(MEDIA_TYPE_AUDIO);
                    UI_.setText(fmt1::format("Switch audio stream to {}", media_->getAudio()->getStreamIndex()));
                }
                else if (button == PotUI::ButtonSubtitle)
                {
                    switchSubtitle();
                }
                else if (button == PotUI::ButtonVolume)
                {
                    int x, y;
                    engine_->getMouseState(x, y);
                    float v = 1.0 * (x - UI_.getButtonPos(button)) / UI_.getButtonWidth(button);
                    media_->getAudio()->setVolume(v);
                }
            }
#ifdef _WINDLL
            if (e.button.button == BUTTON_RIGHT)
            {
                loop = false;
                running_ = false;
            }
#endif
            break;
        case EVENT_MOUSE_BUTTON_DOWN:
            hold_mouse = true;
            hold_time = engine_->getTicks();
            seekButton();
            break;
        case EVENT_MOUSE_WHEEL:
        {
            if (int(UI_.inButton()) == PotUI::ButtonVolume)
            {
                if (e.wheel.y > 0)
                {
                    media_->getAudio()->changeVolume(volume_step);
                }
                else if (e.wheel.y < 0)
                {
                    media_->getAudio()->changeVolume(-volume_step);
                }
                UI_.setText("v");
            }
            else
            {
                if (e.wheel.y > 0)
                {
                    media_->seekTime(media_->getTime() - seek_step, -1);
                    UI_.setText("");
                    seeking = true;
                    //cur_volume_ = media_->getAudio()->changeVolume(volume_step);
                }
                else if (e.wheel.y < 0)
                {
                    media_->seekTime(media_->getTime() + seek_step, 1);
                    UI_.setText("");
                    seeking = true;
                    //cur_volume_ = media_->getAudio()->changeVolume(-volume_step);
                }
            }
            //UI_.setText("v");
            break;
        }
        case EVENT_KEY_DOWN:
        {
            switch (e.key.key)
            {
            case K_LEFT:
                media_->seekTime(media_->getTime() - seek_step, -1);
                UI_.setText("");
                seeking = true;
                break;
            case K_RIGHT:
                media_->seekTime(media_->getTime() + seek_step, 1);
                UI_.setText("");
                seeking = true;
                break;
            case K_UP:
                media_->getAudio()->changeVolume(volume_step);
                UI_.setText("v");
                break;
            case K_DOWN:
                media_->getAudio()->changeVolume(-volume_step);
                UI_.setText("v");
                break;
            case K_1:
                media_->switchStream(MEDIA_TYPE_AUDIO);
                UI_.setText(fmt1::format("Switch audio stream to {}", media_->getAudio()->getStreamIndex()));
                break;
            case K_2:
                switchSubtitle();
                break;
            }
            break;
        }
        case EVENT_KEY_UP:
        {
            switch (e.key.key)
            {
            case K_ESCAPE:
                if (engine_->isFullScreen())
                {
                    engine_->toggleFullscreen();
                    setSubtitleFrameSize();
                }
                else
                {
                    loop = false;
                    running_ = false;
                }
                break;
            case K_BACKSPACE:
                media_->seekTime(0);
                seeking = true;
                break;
#ifndef _WINDLL
            case K_SPACE:
                pause = !pause;
                media_->setPause(pause);
                break;
            case K_RETURN:
                engine_->toggleFullscreen();
                setSubtitleFrameSize();
                break;
            case K_DELETE:
                Config::getInstance().clearAllRecord();
                break;
            case K_PERIOD:
            {
                find_direct = 1;
                auto next_file = findNextFile(drop_filename_, find_direct);
                if (next_file != "")
                {
                    drop_filename_ = next_file;
                    loop = false;
                }
                break;
            }
            case K_COMMA:
            {
                find_direct = -60 * 1000;
                auto next_file = findNextFile(drop_filename_, find_direct);
                if (next_file != "")
                {
                    drop_filename_ = next_file;
                    loop = false;
                }
                break;
            }
            case K_EQUALS:
            {
                int w, h;
                engine_->getWindowSize(w, h);
                w += width_ / 4;
                h += height_ / 4;
                setWindowSize(w, h);
                engine_->setWindowPosition(WINDOWPOS_CENTERED, WINDOWPOS_CENTERED);
                break;
            }
            case K_MINUS:
            {
                int w, h;
                engine_->getWindowSize(w, h);
                w -= width_ / 4;
                h -= height_ / 4;
                setWindowSize(w, h);
                engine_->setWindowPosition(WINDOWPOS_CENTERED, WINDOWPOS_CENTERED);
                break;
            }
            case K_0:
                setWindowSize(media_->getVideo()->getWidth(), media_->getVideo()->getHeight());
                engine_->setWindowPosition(WINDOWPOS_CENTERED, WINDOWPOS_CENTERED);
                break;
#endif
            }

            break;
        }
        //#ifndef _WINDLL
        case EVENT_QUIT:
            //pause = true;
#ifdef _WINDLL
            engine_->delay(10);
#endif
            //media_->setPause(pause);
            loop = false;
            running_ = false;
            exit_type_ = 1;
            break;
            //#endif
        case EVENT_WINDOW_RESIZED:
            setWindowSize(e.window.data1, e.window.data2);
            break;
        case EVENT_WINDOW_MOUSE_LEAVE:
            break;
        case EVENT_DROP_FILE:
            //有文件拖入先检查是不是字幕，不是字幕则当作媒体文件，打开失败活该
            //若将媒体文件当成字幕打开会非常慢，故限制字幕文件的扩展名
            open_filename = PotConv::conv(e.drop.data, BP_encode_, sys_encode_);
            fmt1::print("Change file: {}\n", open_filename);
            //检查是不是字幕，如果是则打开
            if (PotSubtitleManager::isSubtitle(open_filename))
            {
                PotSubtitleManager::destroySubtitle(subtitle_);
                subtitle_ = PotSubtitleManager::createSubtitle(open_filename);
                setSubtitleFrameSize();
                if (subtitle_ && subtitle_->exist())
                {
                    sub_state = 1;
                }
            }
            else
            {
                drop_filename_ = e.drop.data;
                loop = false;
            }
            break;
        case EVENT_FINGER_DOWN:    // 触摸开始
            touch_start_x = e.tfinger.x;
            touch_start_y = e.tfinger.y;
            touch_start_time = engine_->getTicks();
            break;
        case EVENT_FINGER_MOTION:    // 触摸移动
        {
            float dx = e.tfinger.x - touch_start_x;
            float dy = e.tfinger.y - touch_start_y;

            // 判断主要滑动方向（横向/纵向）
            if (fabsf(dx) > fabsf(dy))
            {
                // 横向滑动处理（进度调节）
                if (fabsf(dx) > 0.05f)
                {    // 滑动阈值
                    if (dx > 0)
                    {
                        media_->seekTime(media_->getTime() + seek_step, 1);
                    }
                    else
                    {
                        media_->seekTime(media_->getTime() - seek_step, -1);
                    }
                    UI_.setText("");
                    seeking = true;
                    // 重置起始位置以便连续滑动
                    touch_start_x = e.tfinger.x;
                    touch_start_y = e.tfinger.y;
                }
            }
            else
            {
                // 纵向滑动处理（音量调节）
                if (fabsf(dy) > 0.05f)
                {
                    if (dy > 0)
                    {
                        media_->getAudio()->changeVolume(-volume_step);
                    }
                    else
                    {
                        media_->getAudio()->changeVolume(volume_step);
                    }
                    UI_.setText("v");
                    // 重置起始位置
                    touch_start_x = e.tfinger.x;
                    touch_start_y = e.tfinger.y;
                }
            }
            break;
        }
        case EVENT_FINGER_UP:    // 触摸结束
        {
            uint32_t duration = engine_->getTicks() - touch_start_time;
            float dx = e.tfinger.x - touch_start_x;
            float dy = e.tfinger.y - touch_start_y;

            // 点击判断（短时间+小距离）
            if (duration < 300 && (dx * dx + dy * dy) < 0.0001f)
            {
                // 检查右上角区域（归一化坐标）
                if (e.tfinger.x > 0.8f && e.tfinger.y < 0.2f)
                {
                    loop = false;
                    running_ = false;
                }
            }
            break;
        }
        default:
            break;
        }
        e.type = EVENT_FIRST;

        if (!loop)
        {
            break;
        }
        //在每个循环均尝试预解压
        media_->decodeFrame();
        //media_->getAudio()->show();
        //尝试以音频为基准显示视频
        int audioTime = media_->getTime();    //注意优先为音频时间，若音频不存在使用视频时间
        //if (seeking)
        //{
        //    cout << audioTime << " " << media_->getAudioStream()->getTimedts() << endl;
        //}

        if (last_volume == media_->getAudio()->getVolume())
        {
            volume_step = 1.0 / 128;
        }
        else
        {
            volume_step = (std::min)(volume_step + 1.0 / 128, 5.0 / 128);
        }

        int time_s = audioTime;
        if (pause)
        {
            time_s = 0;    //pause时不刷新视频时间轴，而依赖后面显示静止图像的语句
        }
        int videostate = media_->getVideo()->show(time_s);

        //播放回调
        if (play_callback)
        {
            play_callback(audioTime);
        }
        //结束回调
        if (stop_callback && videostate == PotStreamVideo::NoVideoFrame)
        {
            char s[1024];
            stop_callback(&loop, s);
            if (!loop)
            {
                drop_filename_ = s;
            }
        }

        //fmt1::print("\nvideostate{}", videostate);
        //依据解视频的结果判断是否显示
        bool show = false;
        //有视频显示成功，或者有静态视频，或者只有音频，均刷新
        if (videostate == PotStreamVideo::VideoFrameShowed)
        {
            show = true;
            //以下均是为了显示信息，可以去掉
#ifdef _DEBUG
            int videoTime = (media_->getVideo()->getTimedts());
            int delay = -videoTime + audioTime;
            fmt1::print("\rvolume {}, audio {:4.3}, video {:4.3}, diff {:1.3} in loop {}\t",
                media_->getAudio()->getVolume(), audioTime / 1e3, videoTime / 1e3, delay / 1e3, i);
#endif
        }
        //静止时，无视频时，视频已放完时40毫秒显示一次
        //有视频未暂停且未到时间不会进入此判断
        else if ((pause || videostate == PotStreamVideo::NoVideo || videostate == PotStreamVideo::NoVideoFrame) && engine_->getTicks() - prev_show_time > 100)
        {
            show = true;
            if (havevideo)
            {
                engine_->renderTexture(engine_->getMainTexture());
            }
            else
            {
                //engine_->showLogo();
            }
        }
        if (show)
        {
            if (sub_state == 1 && subtitle_ && subtitle_->exist())
            {
                subtitle_->show(audioTime);
            }
            if (sub_state >= 2)
            {
                media_->getSubtitle()->show(audioTime);
            }
            UI_.drawUI(audioTime, totalTime, media_->getAudio()->getVolume(), pause);
            engine_->renderPresent();
            prev_show_time = engine_->getTicks();
        }
        i++;
        engine_->delay(1);
        if (audioTime >= totalTime)
        {
        }
#ifdef _WINDLL
        if (videostate == PotStreamVideo::NoVideo || time_s >= totalTime)
        {
            loop = false;
            running_ = false;
        }
#endif
    }
    engine_->renderClear();
    engine_->renderPresent();

    auto s = fmt1::format("{}", i);
    //engine_->showMessage(s);
    return exit_type_;
}

int PotPlayer::init()
{
    Config::getInstance().init(run_path_);
    int maximum = Config::getInstance()["windows_maximized"];
    if (engine_->init(handle_, handle_type_, maximum))
    {
        return -1;
    }
#ifdef _WIN32
    sys_encode_ = Config::getInstance().get("sys_encode", "cp936");
#else
    sys_encode_ = Config::getInstance().get("sys_encode", "utf-8");
#endif
    if (cur_volume_ == -1.00f)
    {
        cur_volume_ = Config::getInstance().get("volume", 64);
    }
    PotStreamAudio::setVolume(cur_volume_);
    UI_.init();
    return 0;
}

void PotPlayer::destroy()
{
    UI_.destory();
    //engine_->destroy();
#ifndef _WINDLL
    if (!media_)
    {
        //从未打开过文件
        Config::getInstance().write();    //关闭媒体时已经保存了
    }
#endif
}

int PotPlayer::beginWithFile(std::string filename)
{
    int count = 0;
    if (init() != 0)
    {
        return -1;
    }
    engine_->resetRenderTarget();
    int start_time = engine_->getTicks();

    if (filename.empty() && Config::getInstance()["auto_play_recent"].toInt())
    {
        filename = Config::getInstance().getNewestRecord();
        if (!filefunc::fileExist(PotConv::conv(filename, BP_encode_, sys_encode_)))
        {
            filename = "";
        }
    } 
    //首次运行拖拽的文件也认为是同一个
    drop_filename_ = Config::getInstance().findSuitableFilename(filename);

    fmt1::print("Begin with file: {}\n", filename);
    auto play_filename = drop_filename_;
    running_ = true;

    while (running_)
    {
        /*if (count <= 1)
        {
        //_drop_filename = "";
        //play_filename = "";
        }*/

        openMedia(play_filename);
        bool add_cond = true;
        //fmt1::print("{}", engine_->getTicks() - start_time);
        add_cond = engine_->getTicks() - start_time < 2000;
#ifndef _WINDLL
        //if (count == 0 && add_cond)
        {
            /*auto w = engine_->getMaxWindowWidth();
            auto h = engine_->getMaxWindowHeight();
            auto x = max(0, (w-_w)/2);
            auto y = max(0, (h-_h)/2);
            fmt1::print("{},{}\n",x,y);
            engine_->setWindowPosition(x, y);*/
            int w, h;
            engine_->getWindowSize(w, h);
            w = Config::getInstance().get("windows_width", w);
            h = Config::getInstance().get("windows_height", h);
            setWindowSize(w, h);
            //首次打开文件窗口居中
            if (engine_->isFullScreen() || engine_->getWindowIsMaximized())
            {
            }
            else
            {
                engine_->setWindowPosition(WINDOWPOS_CENTERED, WINDOWPOS_CENTERED);
            }
        }
#endif
        this->eventLoop();
        closeMedia(play_filename);
        if (play_filename != "")
        {
            count++;
        }
        play_filename = drop_filename_;
    }
    destroy();
    return exit_type_;
}

//参数为utf8编码
void PotPlayer::openMedia(const std::string& filename)
{
    media_ = nullptr;
    media_ = new PotMedia;
    //通过参数传入的字串被SDL转为utf-8
    //打开文件, 需要进行转换
    auto open_filename = PotConv::conv(filename, BP_encode_, sys_encode_);    //windows下打开需要ansi
#ifndef _WINDLL
    //某些格式的媒体是分开为很多个文件，这类文件最好先切换工作目录
    if (filefunc::getFileExt(open_filename) == "m3u8")
    {
        filefunc::changePath(filefunc::getParentPath(open_filename));
    }
#endif
    if (media_->openFile(filename) != 0)
    {
        return;
    }

    //窗口尺寸，时间
    width_ = media_->getVideo()->getWidth();
    height_ = media_->getVideo()->getHeight();
    engine_->setRatio(media_->getVideo()->getRatioX(), media_->getVideo()->getRatioY());
    engine_->setRotation(media_->getVideo()->getRotation());
#ifndef _WINDLL
    engine_->setWindowSize(width_, height_);
    engine_->setWindowTitle(filename);
#endif
    auto pixfmt = media_->getVideo()->getSDLPixFmt();
    engine_->createMainTexture(pixfmt, TEXTUREACCESS_STREAMING, width_, height_);
    //engine_->createAssistTexture(width_, height_);

    //重新获取尺寸，有可能与之前不同
    width_ = engine_->getWindowWidth();
    height_ = engine_->getWindowHeight();

    //音量
    media_->getAudio()->setVolume(cur_volume_);

    //试图载入字幕
    auto open_subfilename = PotSubtitleManager::lookForSubtitle(open_filename);
    subtitle_ = PotSubtitleManager::createSubtitle(open_subfilename);
    setSubtitleFrameSize();

#ifndef _WINDLL
    //读取记录中的文件时间并跳转
    if (media_->isMedia())
    {
        cur_time_ = 0;
        cur_time_ = Config::getInstance().getRecord(filename.c_str());
        std::thread th{ [this]()
            {
                Config::getInstance().autoClearRecord();
                return;
            } };
        th.detach();
        fmt1::print("Play from {:1.3}s\n", cur_time_ / 1000.0);
        if (cur_time_ > 0 && cur_time_ < media_->getTotalTime())
        {
            media_->seekTime(cur_time_, -1);
        }
    }
#endif
}

void PotPlayer::setSubtitleFrameSize()
{
    if (subtitle_)
    {
        subtitle_->setFrameSize(engine_->getPresentWidth(), engine_->getPresentHeight());
    }
    media_->getSubtitle()->setFrameSize(engine_->getPresentWidth(), engine_->getPresentHeight());
}

void PotPlayer::closeMedia(const std::string& filename)
{
    //记录播放时间
    cur_time_ = media_->getTime();
    cur_volume_ = media_->getAudio()->getVolume();

    //关闭字幕
    PotSubtitleManager::destroySubtitle(subtitle_);
    //_subtitle->closeSubtitle();

    //如果是媒体文件就记录时间
#ifndef _WINDLL
    auto& config = Config::getInstance();
    if (media_->isMedia() && cur_time_ < media_->getTotalTime() && cur_time_ > 0)
    {
        config.setRecord(filename, cur_time_);
    }
    else
    {
        config["record"].erase(filename);
    }
    if (media_->isMedia())
    {
        config["sys_encode"] = sys_encode_;
        config["volume"] = cur_volume_;
        int w, h;
        engine_->getWindowSize(w, h);
        //config->setInteger("windows_width", w);
        //config->setInteger("windows_height", h);
        config["windows_maximized"] = int(engine_->getWindowIsMaximized());
        //config->autoClearRecord();

        config.write();
    }
#endif
    delete media_;
    filefunc::changePath(run_path_);
}

std::string PotPlayer::findNextFile(const std::string& filename, int direct)
{
#ifdef _WINDLL
    return "";
#endif
    if (filename == "")
    {
        return "";
    }
    if (direct == 0)
    {
        return filename;
    }
    std::string next_file;
    auto filename1 = PotConv::conv(drop_filename_, BP_encode_, sys_encode_);
    auto path = filefunc::getParentPath(filename1);
    filename1 = filefunc::getFilenameWithoutPath(filename1);
    auto ext = filefunc::getFileExt(filename1);
    auto files = filefunc::getFilesInPath(path);
    //只查找相同扩展名
    for (auto it = files.begin(); it != files.end();)
    {
        if (filefunc::getFileExt(*it) == ext)
        {
            it++;
        }
        else
        {
            it = files.erase(it);
        }
    }
    std::sort(files.begin(), files.end());
    if (direct > 0)
    {
        for (int i = 0; i < int(files.size()) - 1; i++)
        {
            if (files.at(i) == filename1)
            {
                next_file = files.at(i + 1);
            }
        }
    }
    else
    {
        for (int i = int(files.size()) - 1; i > 0; i--)
        {
            if (files.at(i) == filename1)
            {
                next_file = files.at(i - 1);
            }
        }
    }
    if (next_file != "")
    {
#ifdef _WIN32
        return PotConv::conv(path + "\\" + next_file, sys_encode_, BP_encode_);
#else
        return PotConv::conv(path + "/" + next_file, sys_encode_, BP_encode_);
#endif
    }
    else
    {
        return "";
    }
}

void PotPlayer::setWindowSize(int w, int h)
{
    //w = media_->getVideo()->getWidth();
    //h = media_->getVideo()->getHeight();
    engine_->setWindowSize(w, h);
    if (engine_->isFullScreen() || engine_->getWindowIsMaximized())
    {
    }
    else
    {
        //ngine_->setWindowPosition(BP_WINDOWPOS_CENTERED, BP_WINDOWPOS_CENTERED);
    }
    engine_->getWindowSize(width_, height_);
    engine_->setPresentPosition(engine_->getMainTexture());
    setSubtitleFrameSize();
}

void PotPlayer::setVolume(float volume)
{
    cur_volume_ = volume;
}

void PotPlayer::seekTime(int time)
{
    media_->seekTime(time);
}
