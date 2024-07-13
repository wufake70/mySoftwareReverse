#include <windows.h>
#include <tlhelp32.h>
#include <iostream>
#include <tchar.h>
#include <vector>

#ifndef GETPROCESSINFO_H
#define GETPROCESSINFO_H

using namespace std;


struct ModuleInfo {
    TCHAR szModuleName[0x50];
    ULONGLONG ulImagebase;
    ULONGLONG ulImagesize;
    TCHAR szModulePath[MAX_PATH];
};

struct ProcessInfo {
    DWORD pid;
    TCHAR szProName[0x50];
    ULONGLONG ulImagebase;
    ULONGLONG ulImagesize;
    vector<ModuleInfo>* moduleListPtr;
    TCHAR szExePath[MAX_PATH];
    ProcessInfo()
    {
        pid = 0;
        memset(&szProName, 0, 0x50 * sizeof(TCHAR));
        ulImagebase = 0;
        ulImagesize = 0;
        moduleListPtr = nullptr;
        memset(szExePath,0,MAX_PATH);
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