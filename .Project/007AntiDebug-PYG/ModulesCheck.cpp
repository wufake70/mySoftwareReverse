#include "ModulesCheck.h"
#include "Misc.h"
#include <TlHelp32.h>
#include <algorithm>

// 进程检测实现
std::vector<ProcessInfo> CheckProcessModules() {
    std::vector<ProcessInfo> results;

    // 获取进程快照
    HANDLE hProcessSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnapshot == INVALID_HANDLE_VALUE) {
        return results;
    }

    PROCESSENTRY32W pe32 = { sizeof(PROCESSENTRY32W) };
    if (!Process32FirstW(hProcessSnapshot, &pe32)) {
        CloseHandle(hProcessSnapshot);
        return results;
    }

    do {
        ProcessInfo info;
        info.pid = pe32.th32ProcessID;
        info.mainModule = pe32.szExeFile;
        info.matchedDLLs.clear();
        info.matchedCount = 0;

        // 获取模块快照（包含32位模块）
        HANDLE hModuleSnapshot = CreateToolhelp32Snapshot(
            TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, info.pid
        );
        if (hModuleSnapshot == INVALID_HANDLE_VALUE) {
            continue;
        }

        MODULEENTRY32W me32 = { sizeof(MODULEENTRY32W) };
        if (Module32FirstW(hModuleSnapshot, &me32)) {
            do {
                std::wstring moduleName = me32.szModule;
                std::wstring lowerName = Str2Lower(moduleName);

                // 检查是否匹配调试器DLL
                for (const auto& dll : DEBUGGER_DLLS) {
                    std::wstring lowerDLL = Str2Lower(dll);
                    //if (lowerName.find(lowerDLL) != std::wstring::npos) {
                    if (lowerName == lowerDLL) {
                        info.matchedDLLs.push_back(moduleName);
                        break; // 每个模块只匹配一次
                    }
                }
            } while (Module32NextW(hModuleSnapshot, &me32));
        }
        CloseHandle(hModuleSnapshot);

        // 更新匹配总数
        info.matchedCount = static_cast<int>(info.matchedDLLs.size());

        // 只记录有匹配项的进程
        if (info.matchedCount > 0) {
            results.push_back(std::move(info));
        }
    } while (Process32NextW(hProcessSnapshot, &pe32));

    CloseHandle(hProcessSnapshot);


    std::sort(results.begin(), results.end(),
        [](const ProcessInfo& a, const ProcessInfo& b) {
        return a.matchedCount > b.matchedCount;
    });
    return results;
}