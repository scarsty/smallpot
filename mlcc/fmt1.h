#pragma once

#include <map>
#include <string>
#include <vector>

#ifndef FMT1_USE_STD_FORMAT
#define FMT1_USE_STD_FORMAT 1
#endif

#if !FMT1_USE_STD_FORMAT
#include <cctype>
#include <iostream>
#include <typeinfo>

namespace fmt1
{

inline std::string getfmt(const std::string& fmt, const std::string& fmt_s)
{
    std::string res = fmt_s;
    if (fmt.find_first_of(':') == 0)
    {
        res = "%" + fmt.substr(1);
        if (!isalpha(fmt.back()))
        {
            res += fmt_s.substr(1);
        }
    }
    return res;
}

template <typename T>
std::string sprintf1(const std::string& fmt, const T& t)
{
    int len = snprintf(nullptr, 0, fmt.c_str(), t);
    std::string res(len + 1, '\0');
    snprintf((char*)res.data(), len + 1, fmt.c_str(), t);    //C++11中尾部含\0的空间，可以不处理此处
    res.pop_back();
    return res;
}

template <typename T>
std::string sprintf2(const std::string& fmt, const std::string& fmt_s, const T& t)
{
    return sprintf1(getfmt(fmt, fmt_s), t);
}

//template <typename T>
//inline std::string to_string(const std::string& fmt, const T& t)
//{
//    std::cerr << std::stacktrace::current() << std::endl;
//    return typeid(T).name();
//}

template <typename T>
std::string to_string(const std::string& fmt, const T t)
{
    return sprintf2(fmt, "%p", t);
}

inline std::string to_string(const std::string& fmt, const char* t)
{
    return sprintf2(fmt, "%s", t);
}

inline std::string to_string(const std::string& fmt, const char t)
{
    return sprintf2(fmt, "%c", t);
}

inline std::string to_string(const std::string& fmt, const signed char t)
{
    return sprintf2(fmt, "%hhd", t);
}

inline std::string to_string(const std::string& fmt, const unsigned char t)
{
    return sprintf2(fmt, "%hhu", t);
}

inline std::string to_string(const std::string& fmt, const short t)
{
    return sprintf2(fmt, "%hd", t);
}

inline std::string to_string(const std::string& fmt, const unsigned short t)
{
    return sprintf2(fmt, "%hu", t);
}

inline std::string to_string(const std::string& fmt, const int t)
{
    return sprintf2(fmt, "%d", t);
}

inline std::string to_string(const std::string& fmt, const unsigned t)
{
    return sprintf2(fmt, "%u", t);
}

inline std::string to_string(const std::string& fmt, const long t)
{
    return sprintf2(fmt, "%ld", t);
}

inline std::string to_string(const std::string& fmt, const unsigned long t)
{
    return sprintf2(fmt, "%lu", t);
}

inline std::string to_string(const std::string& fmt, const long long t)
{
    return sprintf2(fmt, "%lld", t);
}

inline std::string to_string(const std::string& fmt, const unsigned long long t)
{
    return sprintf2(fmt, "%llu", t);
}

inline std::string to_string(const std::string& fmt, const double t)
{
    return sprintf2(fmt, "%g", t);
}

inline std::string to_string(const std::string& fmt, const float t)
{
    return sprintf2(fmt, "%g", t);
}

inline std::string to_string(const std::string& fmt, const bool t)
{
    return sprintf2(fmt, "%d", t);
}

inline std::string to_string(const std::string& fmt, const std::string& t)
{
    return sprintf2(fmt, "%s", t.c_str());
}

template <typename T>
std::string to_string(const std::string& fmt, const std::vector<T>& t)
{
    if (t.empty())
    {
        return "[]";
    }
    std::string res = "[";
    for (auto& i : t)
    {
        res += to_string(fmt, i) + ", ";
    }
    res.pop_back();
    res.back() = ']';
    return res;
}

template <typename T1, typename T2>
std::string to_string(const std::string& fmt, const std::map<T1, T2>& t)
{
    if (t.empty())
    {
        return "[]";
    }
    std::string res = "[";
    for (auto& i : t)
    {
        res += to_string(fmt, i.first) + ": " + to_string(fmt, i.second) + ", ";
    }
    res.pop_back();
    res.back() = ']';
    return res;
}

// array is conflict with pointer
template <typename T, size_t N>
std::string to_string(const std::string& fmt, const T (&t)[N])
{
    if (N == 0)
    {
        return "[]";
    }
    std::string res = "[";
    for (int i = 0; i < N - 1; i++)
    {
        res += to_string(fmt, t[i]) + ", ";
    }
    res += to_string(fmt, t[N - 1]) + "]";
    return res;
}

inline void format2(size_t pos0, std::string& fmt)
{
}

//template <typename T>
//concept is_printable = requires(T t) { to_string("", t); };

template <typename T, typename... Args>
void format2(size_t pos0, std::string& fmt, const T& t, Args&&... args)
{
    auto pos = fmt.find_first_of('{', pos0);
    if (pos != std::string::npos)
    {
        auto pos1 = fmt.find_first_of('}', pos + 1);
        if (pos1 != std::string::npos)
        {
            pos = fmt.find_last_of('{', pos);
            auto s = to_string(fmt.substr(pos + 1, pos1 - pos - 1), t);
            fmt = fmt.substr(0, pos) + s + fmt.substr(pos1 + 1);
            format2(pos + s.size(), fmt, args...);
        }
    }
}

template <typename... Args>
std::string format(const std::string& fmt, Args&&... args)
{
    auto res = fmt;
    format2(0, res, args...);
    return res;
}

template <typename... Args>
void print(FILE* fout, const std::string& fmt, Args&&... args)
{
    auto res = format(fmt, args...);
    fprintf(fout, "%s", res.c_str());
}

template <typename... Args>
void print(const std::string& fmt, Args&&... args)
{
    print(stdout, fmt, args...);
}
}    // namespace fmt1
#else
#include <format>
#include <print>

#define FMT1_STRING const std::format_string<Args...>

namespace fmt1
{

template <typename T>
concept is_printable = requires { std::formatter<std::remove_cvref_t<T>>(); };

template <is_printable... Args>
std::string vformat(const std::string& fmt, Args&&... args)
{
    return std::vformat(fmt.c_str(), std::make_format_args(args...));
}

//template <is_printable... Args>
//std::string format(FMT1_STRING fmt, Args&&... args)
//{
//    return std::format(fmt, std::forward<Args>(args)...);
//}
//
//template <is_printable... Args>
//void print(FILE* fout, FMT1_STRING fmt, Args&&... args)
//{
//    auto res = format(fmt, std::forward<Args>(args)...);
//    fprintf(fout, "%s", res.c_str());
//}
//
//template <is_printable... Args>
//void print(FMT1_STRING fmt, Args&&... args)
//{
//    print(stdout, fmt, std::forward<Args>(args)...);
//}

template <is_printable... Args>
std::string format(const std::string_view& fmt, Args&&... args)
{
    return std::vformat(fmt, std::make_format_args(args...));
}

template <is_printable... Args>
void print(FILE* fout, std::format_string<Args...> fmt, Args&&... args)
{
    std::print(fout, fmt, std::forward<Args>(args)...);
}

template <is_printable... Args>
void print(const std::format_string<Args...> fmt, Args&&... args)
{
    print(stdout, fmt, std::forward<Args>(args)...);
}

}    // namespace fmt1

template <typename T, typename CharT>
struct std::formatter<T*, CharT>
{
    constexpr auto parse(std::format_parse_context& context)
    {
        return context.begin();
    }

    auto format(const T* t, std::format_context& context) const
    {
        return std::format_to(context.out(), "{}", (uint64_t)t);
    }
};

template <typename T, typename CharT>
struct std::formatter<std::vector<T>, CharT> : std::formatter<T, CharT>
{
    auto format(const std::vector<T>& v, std::format_context& format_context) const
    {
        auto&& out = format_context.out();
        format_to(out, "[");
        bool first = true;
        for (const auto& item : v)
        {
            if (first)
            {
                first = false;
            }
            else
            {
                format_to(out, ", ");
            }
            formatter<T>::format(item, format_context);
        }
        return format_to(out, "]");
    }
};

template <typename T1, typename T2, typename CharT>
struct std::formatter<std::map<T1, T2>, CharT> : std::formatter<const char*, CharT>
{
    auto format(const std::map<T1, T2>& v, std::format_context& format_context) const
    {
        auto&& out = format_context.out();
        format_to(out, "[");
        bool first = true;
        for (const auto& item : v)
        {
            if (first)
            {
                first = false;
            }
            else
            {
                format_to(out, ", ");
            }
            std::formatter<T1, CharT>().format(item.first, format_context);
            format_to(out, ": ");
            std::formatter<T2, CharT>().format(item.second, format_context);
        }
        return format_to(out, "]");
    }
};
#endif