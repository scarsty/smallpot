#include "CrashLog.h"

#include <chrono>
#include <cstdio>
#include <exception>
#include <filesystem>
#include <format>
#include <fstream>
#include <mutex>
#include <string>
#include <thread>
#include <cstdlib>
#include <stacktrace>

#if defined(__unix__) || defined(__APPLE__)
#include <csignal>
#include <execinfo.h>
#include <unistd.h>
#endif

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

namespace
{
std::filesystem::path log_path;
std::mutex log_mutex;

std::string timestamp()
{
    const auto now = std::chrono::system_clock::now();
    const auto time = std::chrono::system_clock::to_time_t(now);
    std::tm local{};
#ifdef _WIN32
    localtime_s(&local, &time);
#else
    localtime_r(&time, &local);
#endif
    char buffer[32]{};
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &local);
    return buffer;
}

void append_line(const std::string& line)
{
    std::lock_guard lock(log_mutex);
    std::ofstream out(log_path, std::ios::app);
    if (out)
    {
        out << '[' << timestamp() << "] " << line << '\n';
        out.flush();
    }
}

void write_stacktrace()
{
    try
    {
        const auto trace = std::stacktrace::current();
        for (std::size_t i = 0; i < trace.size(); ++i)
        {
            append_line(std::format("  frame {} {}", i, std::to_string(trace[i])));
        }
    }
    catch (...)
    {
        append_line("Unable to capture C++ stacktrace");
    }
}

#ifdef _WIN32
LONG WINAPI seh_filter(EXCEPTION_POINTERS* exception)
{
    if (exception && exception->ExceptionRecord)
    {
        append_line(std::format("Windows exception code=0x{:08X} address=0x{:016X}",
            static_cast<unsigned int>(exception->ExceptionRecord->ExceptionCode),
            reinterpret_cast<std::uintptr_t>(exception->ExceptionRecord->ExceptionAddress)));
    }
    else
    {
        append_line("Windows exception without exception record");
    }
    write_stacktrace();
    return EXCEPTION_EXECUTE_HANDLER;
}
#endif

#if defined(__unix__) || defined(__APPLE__)
void signal_handler(int signal_number)
{
    append_line(std::format("Unix signal {}", signal_number));
    write_stacktrace();
    void* frames[32]{};
    const int count = backtrace(frames, 32);
    char** symbols = backtrace_symbols(frames, count);
    if (symbols)
    {
        for (int i = 0; i < count; ++i)
        {
            append_line(std::format("  frame {} {}", i, symbols[i]));
        }
        free(symbols);
    }
    std::_Exit(128 + signal_number);
}
#endif

void smallpot_terminate_handler()
{
    try
    {
        const auto exception = std::current_exception();
        if (exception)
        {
            std::rethrow_exception(exception);
        }
        append_line("std::terminate called without an active exception");
        write_stacktrace();
    }
    catch (const std::exception& e)
    {
        append_line(std::format("Unhandled C++ exception: {}", e.what()));
        write_stacktrace();
    }
    catch (...)
    {
        append_line("Unhandled non-standard C++ exception");
        write_stacktrace();
    }
    std::_Exit(EXIT_FAILURE);
}
}

namespace crashlog
{
void install(const char* executable_path)
{
    try
    {
        const auto executable = std::filesystem::absolute(
            executable_path ? std::filesystem::path(executable_path) : std::filesystem::path("smallpot.exe"));
        log_path = executable.parent_path() / "smallpot-crash.log";
        std::error_code error;
        std::filesystem::remove(log_path, error);
        std::set_terminate(smallpot_terminate_handler);
#ifdef _WIN32
        SetUnhandledExceptionFilter(seh_filter);
#elif defined(__unix__) || defined(__APPLE__)
        std::signal(SIGABRT, signal_handler);
        std::signal(SIGBUS, signal_handler);
        std::signal(SIGFPE, signal_handler);
        std::signal(SIGILL, signal_handler);
        std::signal(SIGSEGV, signal_handler);
#endif
    }
    catch (...)
    {
        // Crash logging must never prevent playback startup.
    }
}
}
