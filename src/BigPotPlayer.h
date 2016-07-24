#pragma once

#include "BigPotBase.h"
#include "BigPotMedia.h"
#include "BigPotUI.h"
#include "BigPotConv.h"
#include "BigPotSubtitle.h"
#include "File.h"
#include <functional>
#include <memory>

class BigPotPlayer : public BigPotBase
{
public:
    BigPotPlayer();
    BigPotPlayer(void* handle) : BigPotPlayer() { _handle = handle; }
    BigPotPlayer(char* s) : BigPotPlayer() { _filepath = File::getFilePath(s); }
    virtual ~BigPotPlayer();
private:
    std::string _drop_filename = "", _filepath;
    int _cur_time = 0;

    BigPotMedia* _media = nullptr;
    std::unique_ptr<BigPotUI> _UI = nullptr;
    BigPotSubtitle* _subtitle = nullptr;

    int _w, _h;
    bool _run = true;

    std::string _sys_encode = "cp936", _BP_encode = "utf-8";
    int _cur_volume;
    int eventLoop();
    void* _handle;
    //int drawTex2();
    //int showTex2();
    //string getSysString(const string& str);
public:
    int beginWithFile(const std::string& filename);
    int init();
    void destroy();
    void openMedia(const std::string& filename);
    void closeMedia(const std::string& filename);

    //std::function<void(bool&, std::string&)> stop_callback = nullptr;
    //std::function<void(int)> play_callback = nullptr;
    void(*stop_callback)(bool*, char*) = nullptr;
    void(*play_callback)(int) = nullptr;
};



