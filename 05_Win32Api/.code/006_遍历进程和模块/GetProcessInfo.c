#include <windows.h>
#include <tlhelp32.h>
#include <iostream>
#include <tchar.h>

void ListModulesOfProcess(DWORD dwPID) {
    HANDLE hModuleSnap = INVALID_HANDLE_VALUE;
    MODULEENTRY32 me32;

    // 创建模块快照
    hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, dwPID);
    if (hModuleSnap == INVALID_HANDLE_VALUE) {
        std::cerr << "CreateToolhelp32Snapshot for modules failed!" << std::endl;
        return;
    }

    // 设置 MODULEENTRY32 结构体的大小
    me32.dwSize = sizeof(MODULEENTRY32);

    // 遍历第一个模块
    if (!Module32First(hModuleSnap, &me32)) {
        std::cerr << "Module32First failed!" << std::endl;
        CloseHandle(hModuleSnap);
        return;
    }

    // 输出模块详细信息
    do {
        _tprintf(TEXT("\tModule Name: %s\n"), me32.szModule);
        _tprintf(TEXT("\t\tImage Base: 0x%p\n"), me32.modBaseAddr);
        _tprintf(TEXT("\t\tImage Size: %d bytes\n"), me32.modBaseSize);

    } while (Module32Next(hModuleSnap, &me32));

    CloseHandle(hModuleSnap);
}

void ListProcessesWithModules() {
    HANDLE hProcessSnap;
    PROCESSENTRY32 pe32;

    // 获取进程快照
    hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap == INVALID_HANDLE_VALUE) {
        std::cerr << "CreateToolhelp32Snapshot failed!" << std::endl;
        return;
    }

    // 设置 PROCESSENTRY32 结构体的大小
    pe32.dwSize = sizeof(PROCESSENTRY32);

    // 遍历第一个进程
    if (!Process32First(hProcessSnap, &pe32)) {
        std::cerr << "Process32First failed!" << std::endl;
        CloseHandle(hProcessSnap);
        return;
    }

    // 输出进程详细信息
    do {
        _tprintf(TEXT("Process ID: %d, Process Name: %s\n"), pe32.th32ProcessID, pe32.szExeFile);

        // 获取进程句柄
        HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pe32.th32ProcessID);
        if (hProcess == NULL) {
            std::cerr << "OpenProcess failed!" << std::endl;
            continue;
        }

        // 列出该进程的所有模块
        ListModulesOfProcess(pe32.th32ProcessID);

        CloseHandle(hProcess);

    } while (Process32Next(hProcessSnap, &pe32));

    CloseHandle(hProcessSnap);
}

int main() {
    ListProcessesWithModules();
    return 0;
}
