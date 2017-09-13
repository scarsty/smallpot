#pragma once

#include "PotBase.h"
#include "PotMedia.h"
#include "PotUI.h"
#include "PotConv.h"
#include "PotSubtitle.h"
#include "File.h"
#include <functional>
#include <memory>

class PotPlayer : public PotBase
{
public:
    PotPlayer();
    PotPlayer(void* handle, int handle_type = 0) : PotPlayer()
    {
        _handle = handle;
        _handle_type = handle_type;
    }
    PotPlayer(char* s);
    virtual ~PotPlayer();
private:
    std::string _drop_filename = "", _filepath;
    int _cur_time = 0;

    PotMedia* _media = nullptr;
    std::unique_ptr<PotUI> _UI = nullptr;
    PotSubtitle* _subtitle = nullptr;

    int _w, _h;
    bool _run = true;

    std::string _sys_encode = "cp936", _BP_encode = "utf-8";
    int _cur_volume;
    int eventLoop();
    void* _handle;
    int _handle_type;
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



