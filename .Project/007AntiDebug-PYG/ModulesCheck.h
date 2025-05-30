#pragma once
#include <Windows.h>
#include <string>
#include <vector>

// 调试器相关常见DLL列表（示例）
const std::vector<std::wstring> DEBUGGER_DLLS = {
    // ollydbg
    L"Ollydbg.exe",L"loaddll.exe",L"Udd Cleaner.exe",L"吾爱破解.exe",
    L"+BP-OLLY v2.0 beta4.dll",L"advancedolly.dll",L"ApiBreak.dll",L"DeJunk.dll",
    L"ollydll.dll",L"ExCounter.dll",L"fkvmp.dll",L"IDAficator.dll",L"ODbgScript.dll",
    L"OllyDump.dll",L"OllyMachine.dll",L"SOD 隐藏插件.dll",L"中文搜索.dll"

    // x32dbg
    L"x32dbg.exe",L"x32dbg-unsigned.exe",L"x32_dbg.dll",L"x32gui.dll",L"TitanEngine.dll",
    L"asmjit.dll",L"HookLibraryx86.dll",L"x32bridge.dll",
    L"x32dbg.dll",L"x32_bridge.dll", L"x32_dbg.dll", 

    // x64dbg
    L"x64dbg.exe",L"x64_dbg.dll",L"x64gui.dll",L"TitanEngine.dll",
    L"HookLibraryx64.dll",L"Scylla.dll",L"TitanHideTE.dll"
    L"x64dbg-unsigned.exe",L"x64bridge.dll",L"x64dbg.dll",
    L"x64_bridge.dll",L"TitanEngine.dll"

    // IDA Pro
    L"ida.exe",L"ida64.exe",L"ida.dll",L"ida64.dll",L"hexrays.dll",L"idapython3.dll",L"idapython3_64.dll",
    L"Bindiff.dll",L"geos.dll",L"macho.dll",L"macho64.dll"
    L"idapyswitch.exe",L"idat.exe", L"idat64.exe"

    // Cheat Engine
    L"Cheat Engine.exe",L"Cheat Engine.dll",
    L"AutoAssembler.dll",L"LuaScript.dll",L"allochook-i386.dll",L"allochook-x86_64.dll",
    L"d3dhook.dll",L"d3dhook64.dll",L"speedhack-i386.dll",L"speedhack-x86_64.dll",
    L"winhook-i386.dll",L"winhook-x86_64.dll",L"ceregreset.exe",L"cheatengine-i386.exe",
    L"cheatengine-x86_64-SSE4-AVX2.exe", L"cheatengine-x86_64.exe", L"DotNetDataCollector32.exe",
    L"DotNetDataCollector64.exe", L"ced3d10hook.dll", L"ced3d10hook64.dll",L"ced3d11hook.dll",
    L"ced3d11hook64.dll",L"ced3d9hook.dll", L"ced3d9hook64.dll",L"symsrv.dll",L"lua53-32.dll",
    L"lua53-64.dll",L"tcc32-32.dll",L"tcc64-32.dll",L"tcc64-64.dll"

};

// 进程检测结果结构体
struct ProcessInfo {
    DWORD pid;
    std::wstring mainModule;
    std::vector<std::wstring> matchedDLLs;
    int matchedCount; // 新增匹配总数字段
};

// 新增进程检测函数声明
std::vector<ProcessInfo> CheckProcessModules();

