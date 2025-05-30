#include "Misc.h"
#include <algorithm>
#include <Windows.h>

// 字符串工具函数：转换为小写
std::wstring Str2Lower(std::wstring s) {
    std::transform(s.begin(), s.end(), s.begin(),
        [](wchar_t c) { return towlower(c); });
    return s;
}

// 将 std::wstring 转换为 UTF-8 编码的 char*
std::string ConvertToUtf8(const std::wstring& wstr) {
    // 获取所需字节数（包含终止符）
    int size_needed = WideCharToMultiByte(
        936, 0, wstr.data(), (int)wstr.length(),
        nullptr, 0, nullptr, nullptr
    );
    if (size_needed == 0) return ""; // 转换失败

    // 分配缓冲区
    std::string result(size_needed, 0);
    WideCharToMultiByte(
        936, 0, wstr.data(), (int)wstr.length(),
        &result[0], size_needed, nullptr, nullptr
    );
    return result;
}