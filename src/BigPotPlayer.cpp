#include "BigPotPlayer.h"
#include "BigPotSubtitleManager.h"
#ifdef _WIN32
#include <shlobj.h>
#endif

BigPotPlayer::BigPotPlayer()
{
    _UI.reset(new BigPotUI);
    //_config = new BigPotConfig;
    //_subtitle = new BigPotSubtitle;
    //config_->init();
    _w = 320;
    _h = 150;
    _handle = nullptr;
}

BigPotPlayer::BigPotPlayer(char* s) : BigPotPlayer()
{
    _filepath = File::getFilePath(s);
#if defined(_WIN32) //&& defined(_SINGLE_FILE)
    char szPath[MAX_PATH];
    SHGetFolderPathA(NULL, CSIDL_LOCAL_APPDATA, NULL, 0, szPath);
    //std::wstring ws(szPath);
    std::string str(szPath);
    _filepath = str + "/bigpot";
    WIN32_FIND_DATAA wfd;
    if (FindFirstFileA(_filepath.c_str(), &wfd) == INVALID_HANDLE_VALUE)
    { CreateDirectoryA(_filepath.c_str(), NULL); }
#endif

}

BigPotPlayer::~BigPotPlayer()
{
    //delete _UI;
    //delete _config;
    //delete _subtitle;
    //delete media;
}

int BigPotPlayer::beginWithFile(const std::string& filename)
{
    int count = 0;
    if (init() != 0) { return -1; }

    int start_time = engine_->getTicks();

    //首次运行拖拽的文件也认为是同一个
    _drop_filename = filename;
#ifdef _DEBUG
    _drop_filename = BigPotConv::conv(_drop_filename, _sys_encode, _BP_encode);
#endif
    printf("Begin with file: %s\n", filename.c_str());
    auto play_filename = _drop_filename;
    _run = true;

    //_subtitle->init();

    while (_run)
    {
        /*if (count <= 1)
        {
        //_drop_filename = "";
        //play_filename = "";
        }*/

        openMedia(play_filename);
        //首次打开文件窗口居中

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
            engine_->setWindowPosition(BP_WINDOWPOS_CENTERED, BP_WINDOWPOS_CENTERED);
        }
        else
        {
            engine_->resetWindowsPosition();
        }
        this->eventLoop();

        closeMedia(play_filename);
        if (play_filename != "") { count++; }
        play_filename = _drop_filename;

    }
    destroy();
    return 0;
}

int BigPotPlayer::eventLoop()
{
    BP_Event e;

    bool loop = true, pause = false, seeking = false;
    int ui_alpha = 128;
    int finished, i = 0, x, y;
    int seek_step = 5000;
    int volume_step = 4;
    bool havevideo = _media->getVideo()->exist();
    bool havemedia = _media->getAudio()->exist() || havevideo;
    int totalTime = _media->getTotalTime();
    std::string open_filename;
    printf("Total time is %1.3fs or %dmin %1.3fs\n", totalTime / 1e3, totalTime / 60000, totalTime % 60000 / 1e3);

    int maxDelay = 0; //统计使用
    int prev_show_time = 0;  //上一次显示的时间

    while (loop && engine_->pollEvent(e) >= 0)
    {
        seeking = false;

        engine_->getMouseState(x, y);
        if (ui_alpha > 0)
        { ui_alpha--; }
        if (_h - y < 50
            || ((_w - x) < 200 && y < 150))
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
                if (_h - e.button.y < 50)
                {
                    double pos = 1.0 * e.button.x / _w;
                    _media->seekPos(pos, 1, 1);
                    seeking = true;
                }

                if (e.button.y < 50 && e.button.x > _w - 100)
                {
                    pause = !pause;
                    _media->setPause(pause);
                }
            }
            ui_alpha = 128;
            break;
        case BP_MOUSEWHEEL:
        {
            if (e.wheel.y > 0)
            {
                _media->getAudio()->changeVolume(volume_step);
            }
            else if (e.wheel.y < 0)
            {
                _media->getAudio()->changeVolume(-volume_step);
            }
            ui_alpha = 128;
            break;
        }
        case BP_KEYDOWN:
        {
            switch (e.key.keysym.sym)
            {
            case BPK_LEFT:
                _media->seekTime(_media->getTime() - seek_step, -1);
                seeking = true;
                break;
            case BPK_RIGHT:
                _media->seekTime(_media->getTime() + seek_step, 1);
                seeking = true;
                break;
            case BPK_UP:
                _media->getAudio()->changeVolume(volume_step);
                break;
            case BPK_DOWN:
                _media->getAudio()->changeVolume(-volume_step);
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
                _media->setPause(pause);
                break;
            case BPK_RETURN:
                engine_->toggleFullscreen();
                break;
            case BPK_ESCAPE:
                if (engine_->isFullScreen())
                {
                    engine_->toggleFullscreen();
                }
                else
                {
                    loop = false;
                    _run = false;
                }
                break;
            case BPK_DELETE:
                config_->clearRecord();
                break;
            case BPK_BACKSPACE:
                _media->seekTime(0);
                seeking = true;
                break;
            }
            ui_alpha = 128;
            break;
        }
        case BP_QUIT:
            loop = false;
            _run = false;
            break;
        case BP_WINDOWEVENT:
            if (e.window.event == BP_WINDOWEVENT_RESIZED)
            {
                //需要计算显示和字幕的位置
                _w = e.window.data1;
                _h = e.window.data2;
                engine_->setPresentPosition();
                _subtitle->setFrameSize(engine_->getPresentWidth(), engine_->getPresentHeight());
            }
            else if (e.window.event == BP_WINDOWEVENT_LEAVE)
            {
                ui_alpha = 0;
            }
            break;
        case BP_DROPFILE:
            //有文件拖入先检查是不是字幕，不是字幕则当作媒体文件，打开失败活该
            //若将媒体文件当成字幕打开会非常慢，故限制字幕文件的扩展名
            open_filename = BigPotConv::conv(e.drop.file, _BP_encode, _sys_encode);
            printf("Change file: %s\n", open_filename.c_str());
            //检查是不是字幕，如果是则打开
            if (BigPotSubtitleManager::isSubtitle(open_filename))
            {
                BigPotSubtitleManager::destroySubtitle(_subtitle);
                _subtitle = BigPotSubtitleManager::createSubtitle(open_filename);
                _subtitle->setFrameSize(engine_->getPresentWidth(), engine_->getPresentHeight());
            }
            else
            {
                _drop_filename = e.drop.file;
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
        _media->decodeFrame();
        //尝试以音频为基准显示视频
        int audioTime = _media->getTime();  //注意优先为音频时间，若音频不存在使用视频时间
        /*if (seeking)
        {
            cout << audioTime << " " <<_media->getAudioStream()->getTimedts() << endl<<endl;;
        }*/

        int time_s = audioTime;
        if (pause)
        {
            time_s = 0; //pause时不刷新视频时间轴，而依赖后面显示静止图像的语句
        }
        int videostate = _media->getVideo()->showTexture(time_s);

        //播放回调
        if (play_callback)
        { play_callback(audioTime); }
        //结束回调
        if (stop_callback && videostate == BigPotStreamVideo::NoVideoFrame)
        {
            char s[1024];
            stop_callback(&loop, s);
            if (!loop) { _drop_filename = s; }
        }

        //printf("\nvideostate%d", videostate);
        //依据解视频的结果判断是否显示
        bool show = false;
        //有视频显示成功，或者有静态视频，或者只有音频，均刷新
        if (videostate == BigPotStreamVideo::VideoFrameShowed)
        {
            show = true;
            //以下均是为了显示信息，可以去掉
#ifdef _DEBUG
            int videoTime = (_media->getVideo()->getTimedts());
            int delay = -videoTime + audioTime;
            printf("\rvolume %d, audio %4.3f, video %4.3f, diff %1.3f in loop %d\t",
                   _media->getAudio()->changeVolume(0), audioTime / 1e3, videoTime / 1e3, delay / 1e3, i);
#endif
        }
        //静止时，无视频时，视频已放完时40毫秒显示一次
        //有视频未暂停且未到时间不会进入此判断
        else if ((pause || videostate == BigPotStreamVideo::NoVideo || videostate == BigPotStreamVideo::NoVideoFrame)
                 && engine_->getTicks() - prev_show_time > 100)
        {
            show = true;
            if (havevideo)
            { engine_->renderCopy(); }
            else
            { engine_->showLogo(); }
        }
        if (show)
        {
            if (_subtitle->exist())
            { _subtitle->show(audioTime); }
            _UI->drawUI(ui_alpha, audioTime, totalTime, _media->getAudio()->getVolume());
            engine_->renderPresent();
            prev_show_time = engine_->getTicks();
        }
        i++;
        engine_->delay(1);
        //if (audioTime >= totalTime)
        //_media->seekTime(0);
    }
    engine_->renderClear();
    engine_->renderPresent();

    auto s = File::formatString("%d", i);
    //engine_->showMessage(s);
    return 0;
}

int BigPotPlayer::init()
{
    if (engine_->init(_handle)) { return -1; }
    config_->init(_filepath);
#ifdef _MSC_VER
    _sys_encode = config_->getString("sys_encode", "cp936");
#else
    _sys_encode = config_->getString("sys_encode", "utf-8");
#endif
    _cur_volume = config_->getInteger("volume", BP_AUDIO_MIX_MAXVOLUME / 2);
    _UI->init();
    return 0;
}

void BigPotPlayer::destroy()
{
    config_->setString(_sys_encode, "sys_encode");
    config_->setInteger(_cur_volume, "volume");
    _UI->destory();
    engine_->destroy();
    config_->write();
}


//参数为utf8编码
void BigPotPlayer::openMedia(const std::string& filename)
{
    _media = nullptr;
    _media = new BigPotMedia;

    File::changePath(File::getFilePath(filename));

    //如果是控制台程序，通过参数传入的是ansi
    //如果是窗口程序，通过参数传入的是utf-8
    //所有通过拖拽传入的都是utf-8
    //播放器应以窗口程序为主

    engine_->setWindowTitle(filename);

    //打开文件, 需要进行转换
    auto open_filename = BigPotConv::conv(filename, _BP_encode, _sys_encode); //这个需要ansi
    _media->openFile(open_filename);

    //窗口尺寸，时间
    _w = _media->getVideo()->getWidth();
    _h = _media->getVideo()->getHeight();
    engine_->setRatio(_media->getVideo()->getRatioX(), _media->getVideo()->getRatioY());
    engine_->setWindowSize(_w, _h);
    engine_->createMainTexture(_w, _h);

    engine_->setRotation(_media->getVideo()->getRotation());

    //重新获取尺寸，有可能与之前不同
    _w = engine_->getWindowsWidth();
    _h = engine_->getWindowsHeight();

    //音量
    _media->getAudio()->setVolume(_cur_volume);

    //试图载入字幕
    auto open_subfilename = BigPotSubtitleManager::lookForSubtitle(open_filename);
    _subtitle = BigPotSubtitleManager::createSubtitle(open_subfilename);
    _subtitle->setFrameSize(engine_->getPresentWidth(), engine_->getPresentHeight());

    //读取记录中的文件时间并跳转
    if (_media->isMedia())
    {
        _cur_time = 0;
        _cur_time = config_->getRecord(filename.c_str());
        printf("Play from %1.3fs\n", _cur_time / 1000.0);
        if (_cur_time > 0 && _cur_time < _media->getTotalTime())
        { _media->seekTime(_cur_time, -1); }
    }
}

void BigPotPlayer::closeMedia(const std::string& filename)
{
    engine_->destroyMainTexture();

    //记录播放时间
    _cur_time = _media->getTime();
    _cur_volume = _media->getAudio()->getVolume();

    //关闭字幕
    BigPotSubtitleManager::destroySubtitle(_subtitle);
    //_subtitle->closeSubtitle();

    //如果是媒体文件就记录时间
    if (_media->isMedia()
        && _cur_time < _media->getTotalTime()
        && _cur_time > 0)
    {
        config_->setRecord(_cur_time, filename.c_str());
    }
    else
    {
        config_->removeRecord(filename.c_str());
    }

    delete _media;
}



