#pragma once
#include <string>

// 字符串工具函数：转换为小写
std::wstring Str2Lower(std::wstring s);

// 将 std::wstring 转换为 UTF-8 编码的 char*
std::string ConvertToUtf8(const std::wstring& wstr);

