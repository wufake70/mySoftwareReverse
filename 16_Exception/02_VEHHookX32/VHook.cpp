#include "VHook.h"
#include <map>
#include <vector>
#include <ImageHlp.h>
#include "AsmCode.h"

typedef struct _vehNode
{
	LIST_ENTRY entry;
	ULONG isEncode;
	ULONG encodeFunction;

}vehNode;

typedef PVOID(WINAPI *RtlPcToFileHeaderProc)(PVOID PcValue, PVOID *BaseOfImage);

RtlPcToFileHeaderProc RtlPcToFileHeaderX = NULL;

typedef struct _HookInfo
{
	ULONG_PTR HookAddr;
	ULONG_PTR newAddr;
	ULONG_PTR OrgMouduleBase;
	ULONG_PTR OrgMouduleSize;
	ULONG_PTR NewMouduleBase;
	char OrgiCode[20];
	ULONG OrgiLen;
	ULONG_PTR DispatchFuncCall; // shellcode
	bool isHook;

}HookInfo,*PHookInfo;


// 键为模块的基址，值为 该模块的HookInfo列表
std::map<ULONG_PTR, std::vector<HookInfo>> gHookinfoMaps;

int GetInsLen(ULONG_PTR hookAddr, int minLen)
{
	int len = 0;
	do 
	{
		int temp =insn_len_x86_32((PVOID)hookAddr);
		hookAddr = temp + hookAddr;
		len += temp;
	} while (len < minLen);

	return len;
}

LONG NTAPI VHookException(PEXCEPTION_POINTERS ExceptionInfo)
{
	if(ExceptionInfo->ExceptionRecord->ExceptionCode == 0xC0000005)
	{
		PVOID imageBase = NULL;
		RtlPcToFileHeaderX(ExceptionInfo->ExceptionRecord->ExceptionAddress,&imageBase);
		if (imageBase)
		{
			std::vector<HookInfo> & infos = gHookinfoMaps[(ULONG)imageBase];
			HookInfo findInfo = {0};
			if (!infos.empty())
			{
				for (int i = 0; i < infos.size(); i++)
				{
					HookInfo& info =infos[i];
					ULONG startAddr = ((ULONG)ExceptionInfo->ExceptionRecord->ExceptionAddress & (~0xFFF));
					ULONG startAddr2  = ((ULONG)info.HookAddr & (~0xFFF));
					if (startAddr == startAddr2)
					{
						findInfo = info;
						break;
					}
				}
	
				//找到了
				if (findInfo.HookAddr)
				{
					// eip修改为 新复制的区段的地址
					ExceptionInfo->ContextRecord->Eip =  
						(ULONG)ExceptionInfo->ExceptionRecord->ExceptionAddress - 
						findInfo.OrgMouduleBase + findInfo.NewMouduleBase;
					return EXCEPTION_CONTINUE_EXECUTION;
				}
			}
		}
	}

	return EXCEPTION_CONTINUE_SEARCH;
}

bool InitVEHook()
{

	vehNode * node = (vehNode *)AddVectoredExceptionHandler(TRUE, VHookException);
	DestroyVEHook();
	gHookinfoMaps.clear();

	HMODULE hModule = GetModuleHandleA("ntdll.dll");
	
	if (!hModule) return false;
	RtlPcToFileHeaderX = (RtlPcToFileHeaderProc)GetProcAddress(hModule, "RtlPcToFileHeader");
	if (!RtlPcToFileHeaderX) return false;
	return true;
}

void DestroyVEHook()
{
	for (const auto& hookInfoList : gHookinfoMaps) {
		for (const auto& hookInfo : hookInfoList.second)
		{
			ULONG page = hookInfo.HookAddr & (~0xFFF);
			ULONG oldProtectFlag = 0;
			VirtualProtect((PVOID)page, USN_PAGE_SIZE, PAGE_EXECUTE_READ, &oldProtectFlag);
		}
	}
	gHookinfoMaps.clear();

}

bool AddHook(ULONG_PTR HookAddr, HookCallback newAddr)
{
	bool isRet = false;

	HookInfo hookInfo;
	hookInfo.HookAddr = HookAddr;
	hookInfo.newAddr = (ULONG_PTR)newAddr;
	ULONG_PTR imageBase = NULL;
	
	// 读取HookAddr所在模块的pe头信息
	if (RtlPcToFileHeaderX((PVOID)HookAddr, (PVOID *)&imageBase))
	{
		std::vector<HookInfo> hokInfoList = gHookinfoMaps[imageBase];

		PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)imageBase;
		PIMAGE_NT_HEADERS pNts = (PIMAGE_NT_HEADERS)(imageBase + pDos->e_lfanew);

		hookInfo.OrgMouduleBase = imageBase;
		hookInfo.OrgMouduleSize = pNts->OptionalHeader.SizeOfImage;

		// 判断该模块的区段是否复制，空的既没有被复制，
  		if (hokInfoList.empty())
		{
         	hookInfo.NewMouduleBase = (ULONG_PTR)malloc(hookInfo.OrgMouduleSize);

			// >> 12 获取内存页（4k）的个数
			int count = hookInfo.OrgMouduleSize >> 12;
			int i = 0;
			do 
			{
				ULONG size = 0;
				ReadProcessMemory(GetCurrentProcess(),
					(PVOID)(hookInfo.OrgMouduleBase + i * USN_PAGE_SIZE), (PVOID)(hookInfo.NewMouduleBase + i * USN_PAGE_SIZE), USN_PAGE_SIZE,&size);
				i++;
				count--;
			} while (count != 0);
			

			ULONG pro = 0;
			// 新的模块赋予执行权限
			VirtualProtect((PVOID)hookInfo.NewMouduleBase, hookInfo.OrgMouduleSize, PAGE_EXECUTE_READWRITE, &pro);

		}
		else 
		{
			hookInfo.NewMouduleBase = hokInfoList[0].NewMouduleBase;
		}

		for (auto hook : hokInfoList)
		{
			if (hook.HookAddr == HookAddr)
			{
				//printf("已经hook过了\n");
				return false;
			}
		}

		//修改页属性
		ULONG pageOffset = HookAddr & 0xFFF;
		ULONG page = HookAddr & (~0xFFF);
		ULONG moduleOffset = HookAddr - imageBase;

		//求得模块HOOK地址
		ULONG NewHookAddr = hookInfo.NewMouduleBase + moduleOffset;

		//获取长度 保存原字节
		int insLen = GetInsLen(NewHookAddr, 5);

		// 备份原来字节码
		memcpy(hookInfo.OrgiCode, (PUCHAR)NewHookAddr, insLen);
		hookInfo.OrgiLen = insLen;


		// 构造shellcode
		
		/*
			0175E6AF  <模块入口点> 60             pushad                                                                     
			0175E6B0               9C             pushfd                                                                     
			0175E6B1               8D 04 24       lea  eax,dword ptr ss:[esp]                                                
			0175E6B4               B9 78 56 34 12 mov  ecx,0x12345678                                                        
			0175E6B9               50             push  eax                                                                  
			0175E6BA               FF D1          call  ecx                                                                  
			0175E6BC               9D             popfd                                                                      
			0175E6BD               61             popad                                                                      
			0175E6BE               90             nop                                                                        
			0175E6BF               90             nop                                                                        
			0175E6C0               90             nop                                                                        
			0175E6C1               90             nop                                                                        
			0175E6C2               90             nop                                                                        
			0175E6C3               90             nop                                                                        
			0175E6C4               90             nop                                                                        
			0175E6C5               90             nop                                                                        
			0175E6C6               90             nop                                                                        
			0175E6C7               90             nop                                                                        
			0175E6C8               90             nop                                                                        
			0175E6C9               90             nop                                                                        
			0175E6CA               68 78 45 23 01 push  0x01234578                                                           
			0175E6CF               C3             retn                                                                       

		*/

		char bufcode[] = 
		{
			0x60,
			0x9C,
			0x8D,0x04,0x24,
			0xB9,0x78,0x56,0x34,0x12,
			0x50,
			0xFF,0xD1,
			0x9D,
			0x61,
			0x90,0x90,0x90,0x90,0x90,0x90,0x90,0x90,0x90,0x90,0x90,0x90,
			0x68,0x78,0x45,0x23,0x01,
			0xC3
		};

		ULONG oldProtectFlag = 0;
		//制作派发流程
		hookInfo.DispatchFuncCall = (ULONG_PTR)malloc(sizeof(bufcode));
		VirtualProtect((PVOID)hookInfo.DispatchFuncCall, sizeof(bufcode), PAGE_EXECUTE_READWRITE, &oldProtectFlag);

		// 替换 bufcode中的 第一个0x12345678 为 HookCallback，执行指定回调函数
		*(PULONG_PTR)&bufcode[6] = (ULONG_PTR)newAddr; 
		// NewHookAddr + insLen，NewHookAddr的下一行代码
		*(PULONG_PTR)&bufcode[28] = NewHookAddr + insLen;
		// 写入NewHookAddr 的原字节码
		memcpy(&bufcode[15], hookInfo.OrgiCode, insLen);
		memcpy((PVOID)hookInfo.DispatchFuncCall, bufcode, sizeof(bufcode));

		// 在复制的区段中 进行hook，实现无痕hook
		// 在NewHookAddr点进行jmp，跳转到 hookInfo.DispatchFuncCall
		char jmpCode[5] = {0xe9,0};
		*(PULONG_PTR)&jmpCode[1] = hookInfo.DispatchFuncCall - NewHookAddr - 5;

		//开始挂页
		memcpy((PVOID)NewHookAddr, jmpCode, 5);
		// 将hook的信息添加到列表中
		hokInfoList.push_back(hookInfo); 
		isRet = true;
		hookInfo.isHook = isRet;
		gHookinfoMaps[imageBase] = hokInfoList;
		
		// 设置 HookAddr所在的内存页为不可执行，用于 VEH
		bool x = VirtualProtect((PVOID)page, USN_PAGE_SIZE, PAGE_READONLY, &oldProtectFlag);
		
	}


	return isRet;
	
}