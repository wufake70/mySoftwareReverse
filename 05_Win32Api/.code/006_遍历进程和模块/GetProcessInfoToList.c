// Test.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#include <windows.h>
#include <tlhelp32.h>
#include <iostream>
#include <tchar.h>
#include <list>

using namespace std;

struct ModuleInfo {
    TCHAR name[0x50];
    ULONGLONG imagebase;
    ULONGLONG imagesize;
};

struct ProcessInfo {
    ULONGLONG pid;
    TCHAR name[0x50];
    ULONGLONG imagebase;
    ULONGLONG imagesize;
    list<ModuleInfo>* moduleListRtr;
    ProcessInfo()
    {
        pid = 0;
        memset(name, 0, sizeof name);
        imagebase = 0;
        imagesize = 0;
        moduleListRtr = nullptr;
    }
    ~ProcessInfo()
    {
        delete moduleListRtr;
        moduleListRtr = nullptr;
    }
};

void ListModulesOfProcess(DWORD dwPID, ProcessInfo& pInfo) {
    HANDLE hModuleSnap = INVALID_HANDLE_VALUE;
    MODULEENTRY32 me32;
    ModuleInfo mInfo = { 0 };

    // 创建模块快照
    hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, dwPID);
    if (hModuleSnap == INVALID_HANDLE_VALUE) {
        return;
    }

    // 设置 MODULEENTRY32 结构体的大小
    me32.dwSize = sizeof(MODULEENTRY32);

    // 遍历第一个模块
    if (!Module32First(hModuleSnap, &me32)) {
        CloseHandle(hModuleSnap);
        return;
    }

    pInfo.moduleListRtr = new list<ModuleInfo>;

    // 输出模块详细信息
    do {

        memset(&mInfo, 0, sizeof mInfo);

        memcpy(mInfo.name, me32.szModule, (wcslen(me32.szModule) + 1) * 2);

        mInfo.imagebase = (ULONG64)me32.modBaseAddr;
        mInfo.imagesize = (ULONG64)me32.modBaseSize;
        pInfo.moduleListRtr->push_back(mInfo);
        // pInfo 基址、镜像大小
        if (_wcsicmp(pInfo.name, mInfo.name) == 0)  // 忽略大小写
        {
            pInfo.imagebase = mInfo.imagebase;
            pInfo.imagesize = mInfo.imagesize;
            //wcout << mInfo.name << endl;
        }
    } while (Module32Next(hModuleSnap, &me32));

    CloseHandle(hModuleSnap);
}
void ListProcessesWithModules(list< ProcessInfo>& v) {
    HANDLE hProcessSnap;
    PROCESSENTRY32 pe32;
    ProcessInfo pInfo;

    // 获取进程快照
    hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap == INVALID_HANDLE_VALUE) {
        return;
    }

    // 设置 PROCESSENTRY32 结构体的大小
    pe32.dwSize = sizeof(PROCESSENTRY32);

    // 遍历第一个进程
    if (!Process32First(hProcessSnap, &pe32)) {
        CloseHandle(hProcessSnap);
        return;
    }

    // 输出进程详细信息
    do {
        memset(&pInfo, 0, sizeof pInfo);
        pInfo.pid = (ULONGLONG)pe32.th32ProcessID;
        memcpy(pInfo.name, pe32.szExeFile, (wcslen(pe32.szExeFile) + 1) * 2);

        // 获取进程句柄
        HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pe32.th32ProcessID);
        if (hProcess == NULL) {
            continue;
        }

        // 列出该进程的所有模块
        ListModulesOfProcess(pe32.th32ProcessID, pInfo);
        CloseHandle(hProcess);

        v.push_back(pInfo);
    } while (Process32Next(hProcessSnap, &pe32));

    CloseHandle(hProcessSnap);

    // *** 避免调用结束后 自动析构 pInfo时，delete 掉 moduleListPtr ***
    pInfo.moduleListRtr = nullptr;
}

int main() {
    list<ProcessInfo>* processList = new list<ProcessInfo>;
    list<ProcessInfo>& processListRef = processList[0];

    ListProcessesWithModules(processListRef);
    cout << processList->size() << endl;
    delete processList;
    system("pause>nul");
    return 0;
}


