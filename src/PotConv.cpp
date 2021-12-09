#include "PotConv.h"

PotConv::PotConv()
{
}

PotConv::~PotConv()
{
}

std::string PotConv::conv(const std::string& src, const char* from, const char* to)
{
    //const char *from_charset, const char *to_charset, const char *inbuf, size_t inlen, char *outbuf;
    iconv_t cd;
    cd = iconv_open(to, from);
    if (cd == nullptr)
    {
        return "";
    }
    size_t inlen = src.length();
    size_t outlen = src.length() * 2;
    auto in = new char[inlen + 1];
    auto out = new char[outlen + 1];
    memset(in, 0, inlen + 1);
    memcpy(in, src.c_str(), inlen);
    memset(out, 0, outlen + 1);
    char* pin = in, * pout = out;
    if (iconv(cd, &pin, &inlen, &pout, &outlen) == -1)
    {
        out[0] = '\0';
    }
    std::string result(out);
    delete[] in;
    delete[] out;
    iconv_close(cd);
    return result;
}

std::string PotConv::conv(const std::string& src, const std::string& from, const std::string& to)
{
    return conv(src, from.c_str(), to.c_str());
}
