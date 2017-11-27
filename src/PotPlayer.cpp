#include "PotPlayer.h"
#include "PotSubtitleManager.h"
#ifdef _WIN32
//#include <shlobj.h>
//#pragma comment(lib,"shfolder.lib")
#endif

PotPlayer::PotPlayer()
{
    //_config = new BigPotConfig;
    //_subtitle = new BigPotSubtitle;
    //config_->init();
    width_ = 320;
    height_ = 150;
    handle_ = nullptr;
    _filepath = "./";
}

PotPlayer::PotPlayer(char* s) : PotPlayer()
{
    _filepath = File::getFilePath(s);
#if defined(_WIN32) && defined(_SINGLE_FILE)
    char szPath[MAX_PATH];
    //SHGetSpecialFolderPath(NULL, szPath, CSIDL_LOCAL_APPDATA, false);
    SHGetFolderPathA(NULL, CSIDL_LOCAL_APPDATA, NULL, 0, szPath);
    //std::wstring ws(szPath);
    std::string str(szPath);
    _filepath = str + "/bigpot";
    WIN32_FIND_DATAA wfd;
    if (FindFirstFileA(_filepath.c_str(), &wfd) == INVALID_HANDLE_VALUE)
    {
        CreateDirectoryA(_filepath.c_str(), NULL);
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

int PotPlayer::beginWithFile(const std::string& filename)
{
    int count = 0;
    if (init() != 0) { return -1; }
    engine_->resetRenderTarget();
    int start_time = engine_->getTicks();

    //首次运行拖拽的文件也认为是同一个
    drop_filename_ = filename;
#ifdef _DEBUG
    drop_filename_ = PotConv::conv(drop_filename_, sys_encode_, BP_encode_);
#endif
    printf("Begin with file: %s\n", filename.c_str());
    auto play_filename = drop_filename_;
    run_ = true;

    //_subtitle->init();

    while (run_)
    {
        /*if (count <= 1)
        {
        //_drop_filename = "";
        //play_filename = "";
        }*/

        openMedia(play_filename);

        bool add_cond = true;
        //printf("%d", engine_->getTicks() - start_time);
        add_cond = engine_->getTicks() - start_time < 2000;
        if (count == 0 && add_cond)
        {
            /*auto w = engine_->getMaxWindowWidth();
            auto h = engine_->getMaxWindowHeight();
            auto x = max(0, (w-_w)/2);
            auto y = max(0, (h-_h)/2);
            printf("%d,%d\n",x,y);
            engine_->setWindowPosition(x, y);*/
            //首次打开文件窗口居中
            engine_->setWindowPosition(BP_WINDOWPOS_CENTERED, BP_WINDOWPOS_CENTERED);
        }
        else
        {
            engine_->resetWindowsPosition();
        }
        this->eventLoop();

        closeMedia(play_filename);
        if (play_filename != "") { count++; }
        play_filename = drop_filename_;

    }
    destroy();
    return exit_type_;
}

int PotPlayer::eventLoop()
{
    BP_Event e;

    bool loop = true, pause = false, seeking = false;
    int ui_alpha = 128;
    int finished, i = 0, x, y;
    int seek_step = 5000;
    int volume_step = 4;
    bool havevideo = media_->getVideo()->exist();
    bool havemedia = media_->getAudio()->exist() || havevideo;
    int totalTime = media_->getTotalTime();
    std::string open_filename;
    printf("Total time is %1.3fs or %dmin %1.3fs\n", totalTime / 1e3, totalTime / 60000, totalTime % 60000 / 1e3);

    int maxDelay = 0; //统计使用
    int prev_show_time = 0;  //上一次显示的时间
    exit_type_ = 0;

    while (loop && engine_->pollEvent(e) >= 0)
    {
        seeking = false;

        engine_->getMouseState(x, y);
        if (ui_alpha > 0)
        {
            ui_alpha--;
        }
        if (height_ - y < 50
            || ((width_ - x) < 200 && y < 150))
        {
            ui_alpha = 128;
        }
        switch (e.type)
        {
        case BP_MOUSEMOTION:
            break;
        case BP_MOUSEBUTTONUP:
            if (e.button.button == BP_BUTTON_LEFT)
            {
                if (height_ - e.button.y < 50)
                {
                    double pos = 1.0 * e.button.x / width_;
                    media_->seekPos(pos, 1, 1);
                    seeking = true;
                }

                if (e.button.y < 50 && e.button.x > width_ - 100)
                {
                    pause = !pause;
                    media_->setPause(pause);
                }
            }
#ifdef _LIB
            if (e.button.button == BP_BUTTON_RIGHT)
            {
                loop = false;
                run_ = false;
            }
#endif
            break;
        case BP_MOUSEWHEEL:
        {
            if (e.wheel.y > 0)
            {
                media_->getAudio()->changeVolume(volume_step);
            }
            else if (e.wheel.y < 0)
            {
                media_->getAudio()->changeVolume(-volume_step);
            }
            ui_alpha = 128;
            break;
        }
        case BP_KEYDOWN:
        {
            switch (e.key.keysym.sym)
            {
            case BPK_LEFT:
                media_->seekTime(media_->getTime() - seek_step, -1);
                seeking = true;
                break;
            case BPK_RIGHT:
                media_->seekTime(media_->getTime() + seek_step, 1);
                seeking = true;
                break;
            case BPK_UP:
                media_->getAudio()->changeVolume(volume_step);
                break;
            case BPK_DOWN:
                media_->getAudio()->changeVolume(-volume_step);
                break;
            }
            ui_alpha = 128;
            break;
        }
        case BP_KEYUP:
        {
            switch (e.key.keysym.sym)
            {
            case BPK_SPACE:
                pause = !pause;
                media_->setPause(pause);
                break;
            case BPK_RETURN:
#ifndef _LIB
                engine_->toggleFullscreen();
#endif
                break;
            case BPK_ESCAPE:
                if (engine_->isFullScreen())
                {
                    engine_->toggleFullscreen();
                }
                else
                {
                    loop = false;
                    run_ = false;
                }
                break;
            case BPK_DELETE:
                config_->clearRecord();
                break;
            case BPK_BACKSPACE:
                media_->seekTime(0);
                seeking = true;
                break;
            }
            ui_alpha = 128;
            break;
        }
        //#ifndef _LIB
        case BP_QUIT:
            pause = true;
#ifdef _LIB
            engine_->delay(10);
#endif
            media_->setPause(pause);
            loop = false;
            run_ = false;
            exit_type_ = 1;
            break;
        //#endif
        case BP_WINDOWEVENT:
            if (e.window.event == BP_WINDOWEVENT_RESIZED)
            {
                //需要计算显示和字幕的位置
                width_ = e.window.data1;
                height_ = e.window.data2;
                engine_->setPresentPosition();
                subtitle_->setFrameSize(engine_->getPresentWidth(), engine_->getPresentHeight());
            }
            else if (e.window.event == BP_WINDOWEVENT_LEAVE)
            {
                ui_alpha = 0;
            }
            break;
        case BP_DROPFILE:
            //有文件拖入先检查是不是字幕，不是字幕则当作媒体文件，打开失败活该
            //若将媒体文件当成字幕打开会非常慢，故限制字幕文件的扩展名
            open_filename = PotConv::conv(e.drop.file, BP_encode_, sys_encode_);
            printf("Change file: %s\n", open_filename.c_str());
            //检查是不是字幕，如果是则打开
            if (PotSubtitleManager::isSubtitle(open_filename))
            {
                PotSubtitleManager::destroySubtitle(subtitle_);
                subtitle_ = PotSubtitleManager::createSubtitle(open_filename);
                subtitle_->setFrameSize(engine_->getPresentWidth(), engine_->getPresentHeight());
            }
            else
            {
                drop_filename_ = e.drop.file;
                loop = false;
            }
            engine_->free(e.drop.file);
            break;
        default:
            break;
        }
        e.type = BP_FIRSTEVENT;

        if (!loop) { break; }
        //在每个循环均尝试预解压
        media_->decodeFrame();
        //尝试以音频为基准显示视频
        int audioTime = media_->getTime();  //注意优先为音频时间，若音频不存在使用视频时间
        /*if (seeking)
        {
            cout << audioTime << " " <<_media->getAudioStream()->getTimedts() << endl<<endl;;
        }*/

        int time_s = audioTime;
        if (pause)
        {
            time_s = 0; //pause时不刷新视频时间轴，而依赖后面显示静止图像的语句
        }
        int videostate = media_->getVideo()->showTexture(time_s);

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
            if (!loop) { drop_filename_ = s; }
        }

        //printf("\nvideostate%d", videostate);
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
            printf("\rvolume %d, audio %4.3f, video %4.3f, diff %1.3f in loop %d\t",
                media_->getAudio()->changeVolume(0), audioTime / 1e3, videoTime / 1e3, delay / 1e3, i);
#endif
        }
        //静止时，无视频时，视频已放完时40毫秒显示一次
        //有视频未暂停且未到时间不会进入此判断
        else if ((pause || videostate == PotStreamVideo::NoVideo || videostate == PotStreamVideo::NoVideoFrame)
            && engine_->getTicks() - prev_show_time > 100)
        {
            show = true;
            if (havevideo)
            {
                engine_->renderCopy();
            }
            else
            {
                engine_->showLogo();
            }
        }
        if (show)
        {
            if (subtitle_->exist())
            {
                subtitle_->show(audioTime);
            }
            UI_.drawUI(ui_alpha, audioTime, totalTime, media_->getAudio()->getVolume());
            engine_->renderPresent();
            prev_show_time = engine_->getTicks();
        }
        i++;
        engine_->delay(1);
        //if (audioTime >= totalTime)
        //_media->seekTime(0);
#ifdef _LIB
        if (videostate == PotStreamVideo::NoVideo || time_s >= totalTime)
        {
            loop = false;
            run_ = false;
        }
#endif
    }
    engine_->renderClear();
    engine_->renderPresent();

    auto s = File::formatString("%d", i);
    //engine_->showMessage(s);
    return exit_type_;
}

int PotPlayer::init()
{
    if (engine_->init(handle_, handle_type_)) { return -1; }
    config_->init(_filepath);
#ifdef _WIN32
    sys_encode_ = config_->getString("sys_encode", "cp936");
#else
    sys_encode_ = config_->getString("sys_encode", "utf-8");
#endif
    cur_volume_ = config_->getInteger("volume", BP_AUDIO_MIX_MAXVOLUME / 2);
    UI_.init();
    return 0;
}

void PotPlayer::destroy()
{
    config_->setString(sys_encode_, "sys_encode");
    config_->setInteger(cur_volume_, "volume");
    UI_.destory();
    engine_->destroy();
    config_->write();
}


//参数为utf8编码
void PotPlayer::openMedia(const std::string& filename)
{
    media_ = nullptr;
    media_ = new PotMedia;
#ifndef _LIB
    File::changePath(File::getFilePath(filename));
#endif
    //如果是控制台程序，通过参数传入的是ansi
    //如果是窗口程序，通过参数传入的是utf-8
    //所有通过拖拽传入的都是utf-8
    //播放器应以窗口程序为主

    //打开文件, 需要进行转换
    auto open_filename = PotConv::conv(filename, BP_encode_, sys_encode_); //这个需要ansi
    media_->openFile(open_filename);

    //窗口尺寸，时间
    width_ = media_->getVideo()->getWidth();
    height_ = media_->getVideo()->getHeight();
    engine_->setRatio(media_->getVideo()->getRatioX(), media_->getVideo()->getRatioY());
    engine_->setRotation(media_->getVideo()->getRotation());
#ifndef _LIB
    engine_->setWindowSize(width_, height_);
    engine_->setWindowTitle(filename);
#endif
    engine_->createMainTexture(width_, height_);

    //重新获取尺寸，有可能与之前不同
    width_ = engine_->getWindowsWidth();
    height_ = engine_->getWindowsHeight();

    //音量
    media_->getAudio()->setVolume(cur_volume_);

    //试图载入字幕
    auto open_subfilename = PotSubtitleManager::lookForSubtitle(open_filename);
    subtitle_ = PotSubtitleManager::createSubtitle(open_subfilename);
    subtitle_->setFrameSize(engine_->getPresentWidth(), engine_->getPresentHeight());

#ifndef _LIB
    //读取记录中的文件时间并跳转
    if (media_->isMedia())
    {
        cur_time_ = 0;
        cur_time_ = config_->getRecord(filename.c_str());
        printf("Play from %1.3fs\n", cur_time_ / 1000.0);
        if (cur_time_ > 0 && cur_time_ < media_->getTotalTime())
        {
            media_->seekTime(cur_time_, -1);
        }
    }
#endif
}

void PotPlayer::closeMedia(const std::string& filename)
{
    engine_->destroyMainTexture();

    //记录播放时间
    cur_time_ = media_->getTime();
    cur_volume_ = media_->getAudio()->getVolume();

    //关闭字幕
    PotSubtitleManager::destroySubtitle(subtitle_);
    //_subtitle->closeSubtitle();

    //如果是媒体文件就记录时间
#ifndef _LIB
    if (media_->isMedia()
        && cur_time_ < media_->getTotalTime()
        && cur_time_ > 0)
    {
        config_->setRecord(cur_time_, filename.c_str());
    }
    else
    {
        config_->removeRecord(filename.c_str());
    }
#endif
    delete media_;
}



