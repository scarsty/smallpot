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

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <dbghelp.h>
#pragma comment(lib, "dbghelp.lib")
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

#ifdef _WIN32
void write_exception_details(EXCEPTION_POINTERS* exception)
{
    if (!exception || !exception->ExceptionRecord)
    {
        append_line("Windows exception without exception record");
        return;
    }
    append_line(std::format("Windows exception code=0x{:08X} address=0x{:016X}",
        static_cast<unsigned int>(exception->ExceptionRecord->ExceptionCode),
        reinterpret_cast<std::uintptr_t>(exception->ExceptionRecord->ExceptionAddress)));

    HANDLE process = GetCurrentProcess();
    HANDLE thread = GetCurrentThread();
    CONTEXT context = *exception->ContextRecord;
    STACKFRAME64 frame{};
#if defined(_M_X64)
    DWORD machine = IMAGE_FILE_MACHINE_AMD64;
    frame.AddrPC.Offset = context.Rip;
    frame.AddrPC.Mode = AddrModeFlat;
    frame.AddrFrame.Offset = context.Rbp;
    frame.AddrFrame.Mode = AddrModeFlat;
    frame.AddrStack.Offset = context.Rsp;
    frame.AddrStack.Mode = AddrModeFlat;
#else
    DWORD machine = IMAGE_FILE_MACHINE_I386;
    frame.AddrPC.Offset = context.Eip;
    frame.AddrPC.Mode = AddrModeFlat;
    frame.AddrFrame.Offset = context.Ebp;
    frame.AddrFrame.Mode = AddrModeFlat;
    frame.AddrStack.Offset = context.Esp;
    frame.AddrStack.Mode = AddrModeFlat;
#endif
    SymInitialize(process, nullptr, TRUE);
    for (int i = 0; i < 32 && StackWalk64(machine, process, thread, &frame, &context,
        nullptr, SymFunctionTableAccess64, SymGetModuleBase64, nullptr); ++i)
    {
        append_line(std::format("  frame {} pc=0x{:016X}", i,
            static_cast<unsigned long long>(frame.AddrPC.Offset)));
    }
    SymCleanup(process);
}

LONG WINAPI seh_filter(EXCEPTION_POINTERS* exception)
{
    write_exception_details(exception);
    return EXCEPTION_EXECUTE_HANDLER;
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
    }
    catch (const std::exception& e)
    {
        append_line(std::format("Unhandled C++ exception: {}", e.what()));
    }
    catch (...)
    {
        append_line("Unhandled non-standard C++ exception");
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
        log_path = std::filesystem::path(executable_path ? executable_path : "smallpot.exe").parent_path()
            / "smallpot-crash.log";
        append_line("Crash logger installed");
        std::set_terminate(smallpot_terminate_handler);
#ifdef _WIN32
        SetUnhandledExceptionFilter(seh_filter);
#endif
    }
    catch (...)
    {
        // Crash logging must never prevent playback startup.
    }
}
}
