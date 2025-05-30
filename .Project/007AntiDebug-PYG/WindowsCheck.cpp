#include <windows.h>
#include <string>
#include <vector>
#include <algorithm>

#include "WindowsCheck.h"
#include "Misc.h"


// 全局结果容器
std::vector<WindowList> g_WindowList;

// 检查字符串是否包含任意关键词
bool CheckKeyWords(const std::wstring& text,
    const std::vector<std::wstring>& keywords) {
    std::wstring lower_text = Str2Lower(text);

    for (const auto& kw : keywords) {
        if (lower_text.find(Str2Lower(kw)) != std::wstring::npos) {
            return true;
        }
    }
    return false;
}

// 递归枚举菜单项
void CheckMenus(HMENU hMenu, float& confidence) {
    if (!hMenu) return;

    int count = GetMenuItemCount(hMenu);
    for (int i = 0; i < count; ++i) {
        MENUITEMINFOW mii = { sizeof(MENUITEMINFOW) };
        mii.fMask = MIIM_STRING | MIIM_SUBMENU;
        wchar_t buf[256] = { 0 };
        mii.dwTypeData = buf;
        mii.cch = 255;

        if (GetMenuItemInfoW(hMenu, i, TRUE, &mii)) {
            // 检查菜单项文本
            if (CheckKeyWords(buf, FEATURE_KEYWORDS)) {
                confidence += 1.0f;
            }

            // 递归检查子菜单
            if (mii.hSubMenu) {
                CheckMenus(mii.hSubMenu, confidence);
            }
        }
    }
}

// 子窗口枚举回调
BOOL CALLBACK EnumSubWndProc(HWND hwnd, LPARAM lParam) {
    float* pConfidence = reinterpret_cast<float*>(lParam);

    // 检查窗口文本
    wchar_t text[256] = { 0 };
    GetWindowTextW(hwnd, text, 255);
    if (CheckKeyWords(text, FEATURE_KEYWORDS)) {
        *pConfidence += .5f;
    }

    // 检查子窗口的菜单
    HMENU hMenu = GetMenu(hwnd);
    CheckMenus(hMenu, *pConfidence);

    // 继续枚举子窗口的子窗口
    EnumChildWindows(hwnd, EnumSubWndProc, lParam);
    return TRUE;
}

// 主窗口枚举回调
BOOL CALLBACK EnumWndProc(HWND hwnd, LPARAM lParam) {
    // 获取窗口标题
    wchar_t title[256] = { 0 };
    GetWindowTextW(hwnd, title, 255);

    // 初始化检测结果
    WindowList result;
    result.hWnd = hwnd;
    result.confidence = 0.0f;
    result.title = title;

    // 主窗口名称检测
    if (CheckKeyWords(title, MAIN_WINDOW_KEYWORDS)) {
        result.confidence += 60.0f;
    }

    // 子窗口检测
    EnumChildWindows(hwnd, EnumSubWndProc, (LPARAM)&result.confidence);

    // 主窗口菜单检测
    HMENU hMainMenu = GetMenu(hwnd);
    CheckMenus(hMainMenu, result.confidence);

    // 置信度上限控制（可选）
    //result.confidence = min(result.confidence, 100.0f);

    // 记录可疑窗口（阈值可调整）
    if (result.confidence > 10.0f) {
        g_WindowList.push_back(result);
    }

    return TRUE;
}

// 主检测函数
std::vector<WindowList> CheckDbgWnd() {
    g_WindowList.clear();
    EnumWindows(EnumWndProc, 0);
    // 按置信度降序排序
    std::sort(g_WindowList.begin(), g_WindowList.end(),
        [](const WindowList& a, const WindowList& b) {
        return a.confidence > b.confidence;
    });
    return g_WindowList;
}

