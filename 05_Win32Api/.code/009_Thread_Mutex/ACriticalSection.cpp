

// 临界区实现 

DWORD WINAPI ThreadProc1(
    LPVOID lpParameter   // thread data		
)
{
    hThread2 = CreateThread(0, 0, ::ThreadProc2, (LPVOID)lpParameter, 0, 0);
    hThread3 = CreateThread(0, 0, ::ThreadProc3, (LPVOID)lpParameter, 0, 0);
    hThread4 = CreateThread(0, 0, ::ThreadProc4, (LPVOID)lpParameter, 0, 0);
    HANDLE handleList[] = {hThread2,hThread3,hThread4 };
    WaitForMultipleObjects(3,handleList,TRUE,INFINITE);
    CloseHandle(hThread1);
    CloseHandle(hThread2);
    CloseHandle(hThread3);
    CloseHandle(hThread4);
        
    return 0;
}


DWORD WINAPI ThreadProc2(
    LPVOID lpParameter   // thread data		
)
{
    HWND handEdit = GetDlgItem((HWND)lpParameter, IDC_EDIT_MONEY);
    TCHAR szEditText[0x20] = { 0 };
    HWND hStaticTextA = GetDlgItem((HWND)lpParameter, IDC_STATIC_A);
    TCHAR szStaticTextA[0x20] = { 0 };
    DWORD dwNum = 0;

    do {

        EnterCriticalSection(&cs);
        if (dwMoney >= 50)
        {
            dwMoney -= 50;
            dwNum += 50;
        }
        else if (dwMoney)
        {
            dwMoney--;
            dwNum++;
        }
        else {
            LeaveCriticalSection(&cs);
            break;
        }
        memset(szEditText,0,0x20*sizeof(TCHAR));
        wsprintf(szEditText,TEXT("%d"),dwMoney);
        SetWindowText(handEdit, szEditText);
        LeaveCriticalSection(&cs);

        memset(szStaticTextA,0,0x20*sizeof(TCHAR));
        wsprintf(szStaticTextA, TEXT("%d"), dwNum);
        SetWindowText(hStaticTextA,szStaticTextA);
        
        Sleep(20);

    } while (1);

    memset(szEditText, 0, 0x20 * sizeof(TCHAR));
    wsprintf(szEditText, TEXT("%d"), dwMoney);
    SetWindowText(handEdit, szEditText);

    memset(szStaticTextA, 0, 0x20 * sizeof(TCHAR));
    wsprintf(szStaticTextA, TEXT("%d"), dwNum);
    SetWindowText(hStaticTextA, szStaticTextA);

    return 0;
}


DWORD WINAPI ThreadProc3(
    LPVOID lpParameter   // thread data		
)
{
    HWND handEdit = GetDlgItem((HWND)lpParameter, IDC_EDIT_MONEY);
    TCHAR szEditText[0x20] = { 0 };
    HWND hStaticTextB = GetDlgItem((HWND)lpParameter, IDC_STATIC_B);
    TCHAR szStaticTextB[0x20] = { 0 };
    DWORD dwNum = 0;

    do {

        EnterCriticalSection(&cs);
        if (dwMoney >= 50)
        {
            dwMoney -= 50;
            dwNum += 50;
        }
        else if (dwMoney)
        {
            dwMoney--;
            dwNum++;
        }
        else {

            LeaveCriticalSection(&cs);
            break;
        }
        memset(szEditText, 0, 0x20 * sizeof(TCHAR));
        wsprintf(szEditText, TEXT("%d"), dwMoney);
        SetWindowText(handEdit, szEditText);
        LeaveCriticalSection(&cs);

        memset(szStaticTextB, 0, 0x20 * sizeof(TCHAR));
        wsprintf(szStaticTextB, TEXT("%d"), dwNum);
        SetWindowText(hStaticTextB, szStaticTextB);

        Sleep(20);

    } while (1);

    memset(szEditText, 0, 0x20 * sizeof(TCHAR));
    wsprintf(szEditText, TEXT("%d"), dwMoney);
    SetWindowText(handEdit, szEditText);

    memset(szStaticTextB, 0, 0x20 * sizeof(TCHAR));
    wsprintf(szStaticTextB, TEXT("%d"), dwNum);
    SetWindowText(hStaticTextB, szStaticTextB);

    return 0;
}


DWORD WINAPI ThreadProc4(
    LPVOID lpParameter   // thread data		
)
{
    HWND handEdit = GetDlgItem((HWND)lpParameter, IDC_EDIT_MONEY);
    TCHAR szEditText[0x20] = { 0 };
    HWND hStaticTextC = GetDlgItem((HWND)lpParameter, IDC_STATIC_C);
    TCHAR szStaticTextC[0x20] = { 0 };
    DWORD dwNum = 0;

    do {
        EnterCriticalSection(&cs);
        if (dwMoney >= 50)
        {
            dwMoney -= 50;
            dwNum += 50;
        }
        else if (dwMoney)
        {
            dwMoney--;
            dwNum++;
        }
        else {
            LeaveCriticalSection(&cs);
            break;
        }
        memset(szEditText, 0, 0x20 * sizeof(TCHAR));
        wsprintf(szEditText, TEXT("%d"), dwMoney);
        SetWindowText(handEdit, szEditText);
        LeaveCriticalSection(&cs);

        memset(szStaticTextC, 0, 0x20 * sizeof(TCHAR));
        wsprintf(szStaticTextC, TEXT("%d"), dwNum);
        SetWindowText(hStaticTextC, szStaticTextC);

        Sleep(20);

    } while (1);

    memset(szEditText, 0, 0x20 * sizeof(TCHAR));
    wsprintf(szEditText, TEXT("%d"), dwMoney);
    SetWindowText(handEdit, szEditText);

    memset(szStaticTextC, 0, 0x20 * sizeof(TCHAR));
    wsprintf(szStaticTextC, TEXT("%d"), dwNum);
    SetWindowText(hStaticTextC, szStaticTextC);

    return 0;
}