#pragma once

#include <chrono>
#include <string>

#ifdef _WIN32
#define localtime _localtime64
#endif

class Timer
{
private:
    std::chrono::time_point<std::chrono::system_clock> t0_, t1_, t_last_;
    bool running_ = false;

public:
    Timer() { start(); }

    // Returns current time as a string
    static std::string getNowAsString(const std::string& format = "%F %a %H:%M:%S")
    {
        //#ifdef __cpp_lib_format
        //        auto t = std::chrono::time_point_cast<std::chrono::seconds>(std::chrono::system_clock::now());
        //        auto t1 = std::chrono::current_zone()->to_local(t);
        //        auto fmt = "{:" + format + "}";
        //        return std::vformat(fmt, std::make_format_args(t1));
        //#else
        auto t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        auto tm = *localtime(&t);
        char buffer[64] = {};
        strftime(buffer, sizeof(buffer), format.c_str(), &tm);
        return std::string(buffer);
        //#endif
    }

    void start()
    {
        running_ = true;
        t0_ = std::chrono::system_clock::now();
        t_last_ = t0_;
    }

    void stop()
    {
        running_ = false;
        t1_ = std::chrono::system_clock::now();
    }

    double getElapsedTime(bool restart = false)
    {
        if (running_)
        {
            t1_ = std::chrono::system_clock::now();
        }
        auto s = std::chrono::duration_cast<std::chrono::duration<double>>(t1_ - t0_);
        if (restart)
        {
            t0_ = std::chrono::system_clock::now();
        }
        return s.count();
    }

    double getLastPeriod()
    {
        return getElapsedTime(true);
    }

    static std::string autoFormatTime(double s)
    {
        const int size = 80;
        char buffer[size];
        int h = s / 3600;
        int m = (s - h * 3600) / 60;
        s = s - h * 3600 - m * 60;
        if (h > 0)
        {
            snprintf(buffer, size, "%d:%02d:%05.2f", h, m, s);
        }
        else if (m > 0)
        {
            snprintf(buffer, size, "%d:%05.2f", m, s);
        }
        else
        {
            snprintf(buffer, size, "%.2f s", s);
        }
        return std::string(buffer);
    }

    static std::string formatTime(double s, const std::string& format_str = "%d:%02d:%05.2f")
    {
        const int size = 80;
        char buffer[size];
        int h = s / 3600;
        int m = (s - h * 3600) / 60;
        s = s - h * 3600 - m * 60;
        snprintf(buffer, size, format_str.c_str(), h, m, s);
        return std::string(buffer);
    }

    static int64_t getNanoTime()
    {
        return std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    }

    // return the time within two callings of getLastPeriod2, no restart
    double getLastPeriod2()
    {
        if (running_)
        {
            t1_ = std::chrono::system_clock::now();
        }
        auto s = std::chrono::duration_cast<std::chrono::duration<double>>(t1_ - t_last_);
        t_last_ = t1_;
        return s.count();
    }
};