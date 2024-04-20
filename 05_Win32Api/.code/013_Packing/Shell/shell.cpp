#include "StdAfx.h"

// shell.cpp : Defines the entry point for the console application.
//

//#include "stdafx.h"

int main(int argc, char* argv[])
{
	//--------------------------------------解密过程--------------------------------------
	//获取当前程序运行路径
	char FilePathSelf[MAX_PATH] = {0};
	GetModuleFileName(NULL, FilePathSelf, MAX_PATH);

	// 1、读取当前壳子程序本身 数据
	PVOID pFileBufferShell = NULL;
	DWORD dwBufferLengthShell = 0;
	MyReadFile(&pFileBufferShell,&dwBufferLengthShell,FilePathSelf);
	
	// 判断是否是 加密壳子
	if(!IsPackingEXE((LPBYTE)pFileBufferShell))
	{
		return 0;
	}

	// 2、解密源文件,获取源文件的imagebase sizeofimage数据
	PVOID pFileBufferSrc = NULL;	
	DWORD dwBufferLengthSrc = 0;
	DWORD dwBufferImageBaseSrc = 0;
	// dwBufferLengthSrc = GetSizeOfImage(pFileBufferShell);
	GetSrcDataFromShell(pFileBufferShell, &pFileBufferSrc, &dwBufferLengthSrc,&dwBufferImageBaseSrc);
	
	// 3、拉伸PE  pImageBufferSrc
	PVOID pImageBufferSrc = NULL;
	CopyFileBufferToImageBuffer(pFileBufferSrc,&pImageBufferSrc);

	// 4、以挂起方式运行壳程序进程
	STARTUPINFO si = {0};
	PROCESS_INFORMATION pi;
	si.cb = sizeof(si);
	::CreateProcess(FilePathSelf,NULL,NULL,NULL,NULL,CREATE_SUSPENDED, NULL,NULL,&si,&pi);
	printf("error is %d\n", GetLastError());

	DWORD dwImageBaseShell = GetImageBase(pFileBufferShell); // 获取壳子程序自身的imagebase
	
	//5、卸载外壳程序的文件镜像
	typedef long NTSTATUS;
	typedef NTSTATUS(__stdcall *pfnZwUnmapViewOfSection)(unsigned long ProcessHandle, unsigned long BaseAddress);
	
	pfnZwUnmapViewOfSection ZwUnmapViewOfSection = NULL;
	HMODULE hModule = LoadLibrary("ntdll.dll");
	if(hModule){
		ZwUnmapViewOfSection = (pfnZwUnmapViewOfSection)GetProcAddress(hModule, "ZwUnmapViewOfSection");
		if(ZwUnmapViewOfSection){
			if(ZwUnmapViewOfSection((unsigned long)pi.hProcess, dwImageBaseShell)){ // 卸载掉 壳子程序自身的ImageBase 地址
				printf("ZwUnmapViewOfSection success\n");
			}
		}
		FreeLibrary(hModule);
	}
	
	//6、在指定的位置(src的ImageBase)申请指定大小(src的SizeOfImage)的内存(VirtualAllocEx)
	LPVOID status = NULL;
	status = VirtualAllocEx(pi.hProcess, (LPVOID)dwBufferImageBaseSrc,dwBufferLengthSrc,MEM_RESERVE | MEM_COMMIT,PAGE_EXECUTE_READWRITE);
	printf("VirtualAllocEx: %x\n",status);
	printf("error is %d\n", GetLastError());


	if(status != NULL){
		printf("7777777\n");
		//7、如果成功，将Src的PE文件拉伸 复制到该空间中
		WriteProcessMemory(pi.hProcess, (LPVOID)dwBufferImageBaseSrc, pImageBufferSrc, dwBufferLengthSrc, NULL);

	}else{
		//8、如果申请空间失败，但有重定位表：在任意位置申请空间，然后将PE文件拉伸、复制、修复重定位表。
		printf("8888888\n");
		PIMAGE_BASE_RELOCATION pRelocationDirectory = NULL;
		DWORD pRelocationDirectoryVirtual = 0;
		
		DWORD NumberOfRelocation;
		PWORD Location;
		DWORD RVA_Data;
		WORD reloData;
		DWORD FOA;
		DWORD dwTempImageBaseSrc = dwBufferImageBaseSrc + 0x50000;
		
		pRelocationDirectoryVirtual = GetRelocationTable(pFileBufferSrc); //当前重定位表的虚拟地址
		printf("%x\n",pRelocationDirectoryVirtual);
		if(pRelocationDirectoryVirtual){
			RVA_TO_FOA(pFileBufferSrc, pRelocationDirectoryVirtual, &FOA);
			pRelocationDirectory = (PIMAGE_BASE_RELOCATION)((DWORD)pFileBufferSrc + FOA);
			//申请空间
			status = VirtualAllocEx(pi.hProcess, (LPVOID)dwTempImageBaseSrc,dwBufferLengthSrc,MEM_RESERVE | MEM_COMMIT,PAGE_EXECUTE_READWRITE);
			ChangesImageBase(pFileBufferSrc, dwTempImageBaseSrc);
			WriteProcessMemory(pi.hProcess, (LPVOID)dwTempImageBaseSrc, pImageBufferSrc, dwBufferLengthSrc, NULL);
			while(pRelocationDirectory->SizeOfBlock && pRelocationDirectory->VirtualAddress){				
				NumberOfRelocation = (pRelocationDirectory->SizeOfBlock - 8)/2;// 每个重定位块中的数据项的数量
				Location = (PWORD)((DWORD)pRelocationDirectory + 8); // 加上8个字节
				for(DWORD i=0;i<NumberOfRelocation;i++){
					if(Location[i] >> 12 != 0){ //判断是否是垃圾数据
						// WORD类型的变量进行接收
						reloData = (Location[i] & 0xFFF); //这里进行与操作 只取4字节 二进制的后12位
						RVA_Data = pRelocationDirectory->VirtualAddress + reloData; //这个是RVA的地址
						RVA_TO_FOA(pFileBufferSrc,RVA_Data,&FOA);
						//这里是自增的 进行修复重定位，上面的Imagebase我们改成了TempImageBase,那么改变的值就是 TempImageBase-dwBufferImageBaseSrc
						*(PDWORD)((DWORD)pFileBufferSrc+(DWORD)FOA) = *(PDWORD)((DWORD)pFileBufferSrc+(DWORD)FOA) + dwTempImageBaseSrc - dwBufferImageBaseSrc;	 // 任意位置 - Origin ImageBase			
					}
				}
				pRelocationDirectory = (PIMAGE_BASE_RELOCATION)((DWORD)pRelocationDirectory + (DWORD)pRelocationDirectory->SizeOfBlock); //上面的for循环完成之后，跳转到下个重定位块 继续如上的操作
			}
			
			dwBufferImageBaseSrc = dwTempImageBaseSrc;
		}else{
			// 9、如果第6步申请空间失败，并且还没有重定位表，直接返回：失败.
			printf("999999\n");
			return -1;	
		}
	}


	printf("10000000\n");

	
	// 10、修改外壳程序的Context:
	CONTEXT cont;
	cont.ContextFlags = CONTEXT_FULL; 
	::GetThreadContext(pi.hThread, &cont);

    DWORD dwEntryPoint = GetOep(pFileBufferSrc); // get oep
	cont.Eax = dwEntryPoint + dwBufferImageBaseSrc; // set origin oep

	DWORD theOep = cont.Ebx + 8;
	DWORD dwBytes=0;
	WriteProcessMemory(pi.hProcess, &theOep, &dwBufferImageBaseSrc,4, &dwBytes);

    SetThreadContext(pi.hThread, &cont);
	//记得恢复线程
    ResumeThread(pi.hThread);
	ExitProcess(0);
	return 0;
}
