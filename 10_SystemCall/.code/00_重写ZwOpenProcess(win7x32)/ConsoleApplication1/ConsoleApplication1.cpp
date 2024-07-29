#include <stdio.h>
#include <Windows.h>

typedef struct CLIENT_ID {
	DWORD UniqueProcess;
	DWORD UniqueThread;

}*PCLIENT_ID;

typedef struct OBJECT_ATTRIBUTES {
	DWORD Length;
	DWORD RootDirectory;
	DWORD ObjectName;
	DWORD Attributes;
	DWORD SecurityDescriptor;
	DWORD SecurityQualityOfService;
}*POBJECT_ATTRIBUTES;

NTSTATUS __declspec(naked) __stdcall AtHookZwOpenProcess(
	PHANDLE ProcessHandle, 
	ACCESS_MASK DesiredAccess, 
	POBJECT_ATTRIBUTES ObjectAtHooktributes, 
	PCLIENT_ID ClientId);

HANDLE WINAPI AtHookOpenProcess(
	_In_ DWORD dwDesiredAccess,
	_In_ BOOL bInheritHandle,
	_In_ DWORD dwProcessId
);

// 声明未导出的函数
extern "C" int __stdcall BaseSetLastNTError(NTSTATUS Status);

int main()
{
	DWORD dwPid = 0;
	printf("input pid: ");
	scanf_s("%d",&dwPid);
	//HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS,FALSE, dwPid);
	HANDLE hProcess = AtHookOpenProcess(PROCESS_ALL_ACCESS,FALSE, dwPid);
	if (!hProcess) {
		printf("open fail.\n");
		printf("error code: 0x%x;\n",GetLastError());
	}
	else {
		printf("open successful.\n");
	}
	system("pause>nul");
}


HANDLE WINAPI AtHookOpenProcess(
	_In_ DWORD dwDesiredAccess,
	_In_ BOOL bInheritHandle,
	_In_ DWORD dwProcessId
) {
	// 两个局部变量
	OBJECT_ATTRIBUTES obj_attr = { 0 };
	CLIENT_ID client_id = { 0 };
	client_id.UniqueProcess = dwProcessId;
	obj_attr.Attributes = bInheritHandle ? 2 : 0;
	NTSTATUS status = AtHookZwOpenProcess((PHANDLE)&dwProcessId,dwDesiredAccess,&obj_attr,&client_id);
	if (status < 0) {
		BaseSetLastNTError(status);
		dwProcessId = 0;
	}

	return (HANDLE)dwProcessId;

}


NTSTATUS __declspec(naked) __stdcall AtHookZwOpenProcess(
	PHANDLE ProcessHandle,
	ACCESS_MASK DesiredAccess,
	POBJECT_ATTRIBUTES ObjectAttributes,
	PCLIENT_ID ClientId) {

	__asm {
		mov     eax, 0BEh; // NtOpenProcess;
		mov     edx, 7FFE0300h
		call    dword ptr[edx];
		retn    10h;
	}

}
