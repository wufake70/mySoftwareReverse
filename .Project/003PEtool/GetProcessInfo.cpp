//#include "GetProcessInfo.h"


void ListModulesOfProcess(DWORD dwPID, ProcessInfo& pInfo) {

    HANDLE hModuleSnap = INVALID_HANDLE_VALUE;
    MODULEENTRY32 me32 = { 0 };
    ModuleInfo mInfo = { 0 };
    pInfo.moduleListPtr = new vector<ModuleInfo>();
    pInfo.moduleListPtr->reserve(200);

    // ����ģ�����
    hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, dwPID);
    if (hModuleSnap == INVALID_HANDLE_VALUE) {
        return;
    }

    // ���� MODULEENTRY32 �ṹ��Ĵ�С
    me32.dwSize = sizeof(MODULEENTRY32);

    // ������һ��ģ��
    if (!Module32First(hModuleSnap, &me32)) {
        CloseHandle(hModuleSnap);
        return;
    }

    // ���ģ����ϸ��Ϣ
    do {
        memset(&mInfo, 0, sizeof mInfo);

        memcpy(mInfo.szModuleName, me32.szModule, (wcslen(me32.szModule) + 1) * sizeof(TCHAR));
        memcpy(mInfo.szModulePath, me32.szExePath, (wcslen(me32.szExePath) + 1) * sizeof(TCHAR));
        mInfo.ulImagebase = (ULONG64)me32.modBaseAddr;
        mInfo.ulImagesize = (ULONG64)me32.modBaseSize;
        pInfo.moduleListPtr->push_back(mInfo);
        if (_wcsicmp(pInfo.szProName, mInfo.szModuleName) == 0)
        {
            pInfo.ulImagebase = mInfo.ulImagebase;
            pInfo.ulImagesize = mInfo.ulImagesize;
            memcpy(pInfo.szExePath, me32.szExePath, (wcslen(me32.szExePath) + 1) * sizeof(TCHAR));
            //wcout << pInfo.name << endl;
        }
    } while (Module32Next(hModuleSnap, &me32));

    CloseHandle(hModuleSnap);
}

void ListProcessesWithModules(vector< ProcessInfo>& v) {
    HANDLE hProcessSnap = { 0 };
    PROCESSENTRY32 pe32 = { 0 };
    ProcessInfo pInfo;

    // ��ȡ���̿���
    hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap == INVALID_HANDLE_VALUE) {
        return;
    }

    // ���� PROCESSENTRY32 �ṹ��Ĵ�С
    pe32.dwSize = sizeof(PROCESSENTRY32);

    // ������һ������
    if (!Process32First(hProcessSnap, &pe32)) {
        CloseHandle(hProcessSnap);
        return;
    }

    // ���������ϸ��Ϣ
    do {
        memset(&pInfo, 0, sizeof pInfo);
        pInfo.pid = (ULONGLONG)pe32.th32ProcessID;
        memcpy(pInfo.szProName, pe32.szExeFile, (wcslen(pe32.szExeFile) + 1) * 2);

        // ��ȡ���̾��
        HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pe32.th32ProcessID);
        if (hProcess == NULL) {
            continue;
        }

        // �г��ý��̵�����ģ��
        ListModulesOfProcess(pe32.th32ProcessID, pInfo);
        CloseHandle(hProcess);

        v.push_back(pInfo);
    } while (Process32Next(hProcessSnap, &pe32));

    CloseHandle(hProcessSnap);

    // *** ������ý����� �Զ����� pInfoʱ��delete �� moduleListPtr ***
    pInfo.moduleListPtr = nullptr;
}
