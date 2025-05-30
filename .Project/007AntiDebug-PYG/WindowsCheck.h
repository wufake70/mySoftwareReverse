#pragma once
#include <windows.h>
#include <string>
#include <vector>
#include <algorithm>
#include <memory>


// 调试器特征关键词配置（已去重并优化）
const std::vector<std::wstring> MAIN_WINDOW_KEYWORDS = {
    // 主流调试器
    L"x64dbg", L"x32dbg", L"ollydbg", L"ida", L"windbg",
    L"cheat engine", L"ghidra", L"dnspy", L"hiew", L"scylla",

    // 逆向分析工具
    L"cutter", L"radare2", L"binary ninja", L"ilspy", L"dotpeek",
    L"reflector", L"pestudio", L"detours", L"process explorer",

    // 系统工具套件
    L"api monitor", L"procmon", L"debugview", L"sysinternals",
    L"dependency walker", L"resource hacker", L"stud_pe",

    // 常见变体/进程名
    L"ida pro", L"idaq", L"idaq64", L"idaq32", L"idag",
    L"idag64", L"idag32", L"reshacker", L"imprec"
};

const std::vector<std::wstring> FEATURE_KEYWORDS = {
    // 调试核心功能
    L"断点", L"breakpoint", L"调试", L"debug", L"单步", L"step",
    L"逐行", L"into", L"逐过程", L"over", L"跳出", L"out",
    L"运行", L"run", L"继续", L"continue", L"暂停", L"pause",

    // 内存分析
    L"内存", L"memory", L"寄存器", L"registers", L"堆栈", L"stack",
    L"调用栈", L"call stack", L"变量", L"variables", L"数据", L"data",

    // 反汇编相关
    L"反汇编", L"disassembly", L"汇编", L"assembly", L"符号", L"symbols",

    // 调试界面元素
    L"监视", L"watch", L"表达式", L"expression", L"条件", L"condition",
    L"窗口", L"window", L"模块", L"modules", L"导入", L"imports",
    L"导出", L"exports", L"线程", L"threads", L"进程", L"process",

    // 高级功能
    L"调试选项", L"debug options", L"异常", L"exceptions",
    L"内存编辑器", L"memory editor", L"寄存器窗口", L"registers window",
    L"反汇编窗口", L"disassembly window", L"源代码", L"source code",
    L"符号文件", L"symbol file", L"内存映射", L"memory map",

    // 特殊功能
    L"硬件断点", L"hardware breakpoint", L"内存断点", L"memory breakpoint",
    L"调试控制台", L"debug console", L"命令窗口", L"command window"
};

// 窗口检测结果结构体
struct WindowList {
    HWND hWnd;
    float confidence;
    std::wstring title;
};

// 主检测函数
std::vector<WindowList> CheckDbgWnd();
