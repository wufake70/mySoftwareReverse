#include <windows.h>
#include <tlhelp32.h>
#include <iostream>
#include <tchar.h>
#include <vector>

#ifndef GETPROCESSINFO_H
#define GETPROCESSINFO_H

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
    vector<ModuleInfo>* moduleListPtr;
    ProcessInfo()
    {
        pid = 0;
        memset(&name, 0, 0x50 * sizeof(TCHAR));
        imagebase = 0;
        imagesize = 0;
        moduleListPtr = nullptr;
    }

    ~ProcessInfo()
    {

        delete moduleListPtr;
        moduleListPtr = nullptr;

    }
};
void ListModulesOfProcess(DWORD dwPID, ProcessInfo& pInfo);
void ListProcessesWithModules(vector< ProcessInfo>& v);

#endif // GETPROCESSINFO_H
#include "GetProcessInfo.cpp"