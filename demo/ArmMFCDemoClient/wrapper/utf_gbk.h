#ifndef _UTF_GBK_H
#define _UTF_GBK_H

#include <string>
#include <windows.h>

namespace utf_gbk  {

/** @brief Convert gbk string to utf8 string */
static std::string toUtf8(const std::string& strGbk) {
    std::string strOutUTF8 = "";
    WCHAR* str1;
    int n = MultiByteToWideChar(CP_ACP, 0, strGbk.c_str(), -1, NULL, 0);
    str1 = new WCHAR[n];
    MultiByteToWideChar(CP_ACP, 0, strGbk.c_str(), -1, str1, n);
    n = WideCharToMultiByte(CP_UTF8, 0, str1, -1, NULL, 0, NULL, NULL);
    char* str2 = new char[n];
    WideCharToMultiByte(CP_UTF8, 0, str1, -1, str2, n, NULL, NULL);
    strOutUTF8 = str2;
    delete[] str1;
    delete[] str2;
    return strOutUTF8;
}

/** @brief Convert utf8 string to gbk string */
static std::string toGbk(const std::string& strUtf8) {
    int len = MultiByteToWideChar(CP_ACP, 0, strUtf8.c_str(), -1, NULL, 0);
    unsigned short* wszGBK = new unsigned short[len + 1];
    memset(wszGBK, 0, len * 2 + 2);
    MultiByteToWideChar(CP_UTF8, 0, (LPCCH)strUtf8.c_str(), -1, (LPWSTR)wszGBK, len);
    len = WideCharToMultiByte(CP_ACP, 0, (LPWSTR)wszGBK, -1, NULL, 0, NULL, NULL);
    char* szGBK = new char[len + 1];
    memset(szGBK, 0, len + 1);
    WideCharToMultiByte(CP_ACP, 0, (LPWSTR)wszGBK, -1, szGBK, len, NULL, NULL);
    std::string strTemp(szGBK);
    delete[] szGBK;
    delete[] wszGBK;
    return strTemp;
}
}

#endif // _UTF_GBK_H
