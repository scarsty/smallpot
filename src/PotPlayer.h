﻿#pragma once

#include "PotBase.h"
#include "PotConv.h"
#include "PotMedia.h"
#include "PotSubtitle.h"
#include "PotUI.h"
#include "filefunc.h"
#include <functional>

class PotPlayer : public PotBase
{
public:
    PotPlayer();
    PotPlayer(void* handle, int handle_type = 0) : PotPlayer()
    {
        handle_ = handle;
        handle_type_ = handle_type;
    }
    PotPlayer(char* s);
    virtual ~PotPlayer();

private:
    std::string drop_filename_ = "", run_path_;
    int cur_time_ = 0;

    PotMedia* media_ = nullptr;
    PotUI UI_;
    PotSubtitle* subtitle_ = nullptr;

    int width_, height_;
    bool running_ = true;

    std::string sys_encode_ = "cp936", BP_encode_ = "utf-8";
    float cur_volume_ = -1.00f;
    int eventLoop();
    void* handle_;
    int handle_type_;
    //int drawTex2();
    //int showTex2();
    //string getSysString(const string& str);

public:
    int init();
    void destroy();
    int beginWithFile(std::string filename);
    void openMedia(const std::string& filename);
    void setSubtitleFrameSize();
    void closeMedia(const std::string& filename);

    //std::function<void(bool&, std::string&)> stop_callback = nullptr;
    //std::function<void(int)> play_callback = nullptr;
    void (*stop_callback)(bool*, char*) = nullptr;
    void (*play_callback)(int) = nullptr;

    int exit_type_ = 0;    //0 - 播放完毕或者按esc退出，1 - 按关闭退出

    std::string findNextFile(const std::string& filename, int direct = 1);

    void setWindowSize(int w, int h);

    void setVolume(float volume);
    void seekTime(int time);
};
