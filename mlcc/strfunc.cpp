#include "strfunc.h"
#include <algorithm>
#include <cstdio>

#ifdef _WIN32
#define popen _popen
#define pclose _pclose
#endif

void strfunc::replaceOneSubStringRef(std::string& s, const std::string& oldstring, const std::string& newstring, int pos0 /*=0*/)
{
    if (oldstring.empty() || oldstring == newstring)
    {
        return;
    }
    auto pos = s.find(oldstring, pos0);
    if (pos != std::string::npos)
    {
        s.replace(pos, oldstring.length(), newstring);
    }
}

void strfunc::replaceAllSubStringRef(std::string& s, const std::string& oldstring, const std::string& newstring)
{
    if (oldstring.empty() || oldstring == newstring)
    {
        return;
    }
    auto pos = s.find(oldstring);
    while (pos != std::string::npos)
    {
        s.replace(pos, oldstring.length(), newstring);
        pos = s.find(oldstring, pos + newstring.length());
    }
}

std::string strfunc::replaceOneSubString(const std::string& s, const std::string& oldstring, const std::string& newstring, int pos0 /*= 0*/)
{
    std::string s1 = s;
    replaceOneSubStringRef(s1, oldstring, newstring, pos0);
    return s1;
}

std::string strfunc::replaceAllSubString(const std::string& s, const std::string& oldstring, const std::string& newstring)
{
    std::string s1 = s;
    replaceAllSubStringRef(s1, oldstring, newstring);
    return s1;
}

std::string strfunc::findANumber(const std::string& s)
{
    bool findPoint = false;
    bool findNumber = false;
    bool findE = false;
    std::string n;
    for (int i = 0; i < s.length(); i++)
    {
        char c = s[i];
        if ((c >= '0' && c <= '9') || c == '-' || c == '.' || c == 'e' || c == 'E')
        {
            if ((c >= '0' && c <= '9') || c == '-')
            {
                findNumber = true;
                n += c;
            }
            if (c == '.')
            {
                if (!findPoint)
                {
                    n += c;
                }
                findPoint = true;
            }
            if (c == 'e' || c == 'E')
            {
                if (findNumber && !(findE))
                {
                    n += c;
                    findE = true;
                }
            }
        }
        else
        {
            if (findNumber)
            {
                break;
            }
        }
    }
    return n;
}

unsigned strfunc::findTheLast(const std::string& s, const std::string& content)
{
    size_t pos = 0, prepos = 0;
    while (pos != std::string::npos)
    {
        prepos = pos;
        pos = s.find(content, prepos + 1);
        //printf("%d\n",pos);
    }
    return prepos;
}

std::vector<std::string> strfunc::splitString(std::string str, std::string pattern, bool trim_space, bool quote)
{
    std::vector<std::string> result;
    if (str.empty())
    {
        return result;
    }
    if (pattern.empty())
    {
        pattern = ",;| ";
    }

    auto trim = [trim_space](const std::string& s) -> std::string
    {
        if (trim_space)
        {
            std::string s1 = s;
            s1.erase(0, s1.find_first_not_of(" "));
            s1.erase(s1.find_last_not_of(" ") + 1);
            return s1;
        }
        return s;
    };

    str += pattern[0];
    bool have_space = pattern.find(" ") != std::string::npos;
    auto size = str.size();

    if (quote)
    {
        bool inquote = false;
        char quotechar = '\"';
        std::string::size_type pos = 0;
        if (have_space)
        {
            pos = str.find_first_not_of(" ");
        }
        for (int i = pos; i < size; i++)
        {
            if (have_space && !inquote)
            {
                //当空格作为分隔符时，连续空格视为一个
                while (str[i + 1] == ' ')
                {
                    i++;
                }
            }
            if (!inquote && (str[i] == '\"' || str[i] == '\''))
            {
                inquote = true;
                quotechar = str[i];
            }
            else if (inquote && str[i] == quotechar)
            {
                inquote = false;
            }
            if (!inquote)
            {
                if (pattern.find_first_of(str[i]) != std::string::npos)
                {
                    result.push_back(trim(str.substr(pos, i - pos)));
                    pos = i + 1;
                }
            }
        }
    }
    else
    {
        std::string::size_type pos = 0;
        for (int i = 0; i < size; i++)
        {
            if (have_space)
            {
                //当空格作为分隔符时，连续空格视为一个
                while (str[i] == ' ')
                {
                    i++;
                }
            }
            pos = str.find_first_of(pattern, i);
            if (pos < size)
            {
                result.push_back(trim(str.substr(i, pos - i)));
                i = pos;
            }
        }
    }

    return result;
}

bool strfunc::isProChar(char c)
{
    return (c >= '0' && c <= '9') || (c >= 'A' && c <= 'z') || (c >= '(' && c <= ')');
}

std::string strfunc::toLowerCase(const std::string& s)
{
    std::string s1 = s;
    std::transform(s1.begin(), s1.end(), s1.begin(), ::tolower);
    return s1;
}

std::string strfunc::toUpperCase(const std::string& s)
{
    std::string s1 = s;
    std::transform(s1.begin(), s1.end(), s1.begin(), ::toupper);
    return s1;
}

std::string strfunc::ltrim(const std::string& s)
{
    size_t start = s.find_first_not_of(" \n\r\t\f\v");
    return (start == std::string::npos) ? "" : s.substr(start);
}

std::string strfunc::rtrim(const std::string& s)
{
    size_t end = s.find_last_not_of(" \n\r\t\f\v");
    return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}

std::string strfunc::trim(const std::string& s)
{
    return rtrim(ltrim(s));
}

bool strfunc::meet_utf8(const std::string& str)
{
    unsigned int n = 0;
    bool all_ascii = true;
    for (const unsigned char c : str)
    {
        if (all_ascii && c < 0x20 || c >= 0x80)
        {
            all_ascii = false;
        }
        if (n == 0)
        {
            //the first of multi byte
            if (c >= 0x80)
            {
                if (c >= 0xFC && c <= 0xFD)
                {
                    n = 6;
                }
                else if (c >= 0xF8)
                {
                    n = 5;
                }
                else if (c >= 0xF0)
                {
                    n = 4;
                }
                else if (c >= 0xE0)
                {
                    n = 3;
                }
                else if (c >= 0xC0)
                {
                    n = 2;
                }
                else
                {
                    return false;
                }
                n--;
            }
        }
        else
        {
            //it should be 10xxxxxx
            if ((c & 0xC0) != 0x80)
            {
                return false;
            }
            n--;
        }
    }
    if (n != 0)
    {
        return false;
    }
    if (all_ascii)
    {
        return true;
    }
    return true;
}

bool strfunc::meet_gbk(const std::string& str)
{
    unsigned int n = 0;
    bool all_ascii = true;
    for (const unsigned char c : str)
    {
        if (all_ascii && c < 0x20 || c >= 0x80)
        {
            all_ascii = false;
        }
        if (n == 0)
        {
            if (c >= 0x80)
            {
                if (c >= 0x81 && c <= 0xFE)
                {
                    n = +2;
                }
                else
                {
                    return false;
                }
                n--;
            }
        }
        else
        {
            if (c < 0x40 || c > 0xFE)
            {
                return false;
            }
            n--;
        }
    }
    if (n != 0)
    {
        return false;
    }
    if (all_ascii)
    {
        return true;
    }
    return true;
}

std::string strfunc::get_cmd_output(const std::string& cmdstring)
{
    std::string str;
    FILE* p_file = NULL;
    const int BUF_SIZE = 1024;
    char buf[BUF_SIZE];
    p_file = popen(cmdstring.c_str(), "r");
    if (!p_file)
    {
        return "";
    }
    while (fgets(buf, BUF_SIZE, p_file) != NULL)
    {
        str += buf;
    }
    pclose(p_file);
    return str;
}
