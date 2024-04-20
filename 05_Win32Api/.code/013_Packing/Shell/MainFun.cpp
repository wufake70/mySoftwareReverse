#include <WINDOWS.H>
#include <STDLIB.H>
#define XORKEY 0x55

DWORD CopyFileBufferToImageBuffer(PVOID pFileBuffer,PVOID* pImageBuffer){
	PIMAGE_DOS_HEADER pImageDosHeader = NULL;
	PIMAGE_NT_HEADERS pImageNtHeader = NULL;
	PIMAGE_FILE_HEADER pImageFileHeader = NULL;
	PIMAGE_OPTIONAL_HEADER32 pImageOptionalHeader = NULL;
	PIMAGE_SECTION_HEADER pImageSectionHeaderGroup = NULL;
	DWORD ImageBufferSize = 0;
	DWORD i=0;
	
	// DOS头
	pImageDosHeader = (PIMAGE_DOS_HEADER)pFileBuffer;
	
	// 标准PE
	pImageFileHeader = (PIMAGE_FILE_HEADER)((DWORD)pImageDosHeader + pImageDosHeader->e_lfanew + 4);
	
	// 可选PE
	pImageOptionalHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pImageFileHeader + IMAGE_SIZEOF_FILE_HEADER);
	
	//节表组
	pImageSectionHeaderGroup = (PIMAGE_SECTION_HEADER)((DWORD)pImageOptionalHeader + pImageFileHeader->SizeOfOptionalHeader);
	
	//获取ImageBufffer的内存大小
	ImageBufferSize = pImageOptionalHeader->SizeOfImage;

	//为pImageBuffer分配内存空间
	*pImageBuffer = (PVOID)malloc(ImageBufferSize);
	
	if (*pImageBuffer == NULL)
	{
		printf("malloc failed");
		return -1;
	}


	
	//清零
	memset(*pImageBuffer, 0, ImageBufferSize);
	
	// 拷贝头+节表
	memcpy(*pImageBuffer, pFileBuffer, pImageOptionalHeader->SizeOfHeaders);
	
	//循环拷贝节表
	for(i=0;i<pImageFileHeader->NumberOfSections;i++){
		memcpy(
			(PVOID)((DWORD)*pImageBuffer + pImageSectionHeaderGroup[i].VirtualAddress), // 要拷贝的位置 ImageBuffer中的每个节数据的偏移位置
			(PVOID)((DWORD)pFileBuffer + pImageSectionHeaderGroup[i].PointerToRawData), // 被拷贝的位置是 Filebuffer中的每个节数据的偏移位置
			pImageSectionHeaderGroup[i].SizeOfRawData // 被拷贝的大小为 每个节数据的文件对齐大小
			);
	}
	
	return 0;
}	


void MyReadFile(PVOID* pFileBuffer,PDWORD BufferLenth, TCHAR* szFilePath){
	FILE* File;
	File = fopen(szFilePath,"rb");
	
	if(File == NULL){
		printf("文件句柄打开失败");
		return;
	}
	
	//读取文件
	fseek(File,0,SEEK_END);
	*BufferLenth = ftell(File);
	
	//重新把File指针指向文件的开头
	fseek(File,0,SEEK_SET);
	
	//开辟新空间
	*pFileBuffer = (PVOID)malloc(*BufferLenth);
	
	//内存清零
	memset(*pFileBuffer,0,*BufferLenth);
	
	//读取到内存缓冲区
	fread(*pFileBuffer,*BufferLenth,1,File);// 一次读入*bufferlenth个字节，重复1次
	
	//关闭文件句柄
	fclose(File);
}

//FOA_TO_RVA:FOA 转换 RVA							
DWORD FOA_TO_RVA(PVOID FileAddress, DWORD FOA,PDWORD pRVA)
{
	int ret = 0;
	int i;
	
	PIMAGE_DOS_HEADER pDosHeader				= (PIMAGE_DOS_HEADER)(FileAddress);
	PIMAGE_FILE_HEADER pFileHeader				= (PIMAGE_FILE_HEADER)((DWORD)pDosHeader + pDosHeader->e_lfanew + 4);
	PIMAGE_OPTIONAL_HEADER32 pOptionalHeader	= (PIMAGE_OPTIONAL_HEADER32)((DWORD)pFileHeader + sizeof(IMAGE_FILE_HEADER));
	PIMAGE_SECTION_HEADER pSectionGroup			= (PIMAGE_SECTION_HEADER)((DWORD)pOptionalHeader + pFileHeader->SizeOfOptionalHeader);
	
	//RVA在文件头中 或 SectionAlignment 等于 FileAlignment 时RVA等于FOA
	if (FOA < pOptionalHeader->SizeOfHeaders || pOptionalHeader->SectionAlignment == pOptionalHeader->FileAlignment)
	{
		*pRVA = FOA;
		return ret;
	}
	
	//循环判断FOA在节区中
	for (i=0;i < pFileHeader->NumberOfSections; i++)
	{
		if (FOA >= pSectionGroup[i].PointerToRawData && FOA < pSectionGroup[i].PointerToRawData + pSectionGroup[i].SizeOfRawData)
		{
			*pRVA = FOA - pSectionGroup[i].PointerToRawData + pSectionGroup[i].VirtualAddress;
			return *pRVA;
		}
	}
	
	//没有找到地址
	ret = -4;
	printf("func FOA_TO_RVA() Error: %d 地址转换失败！\n", ret);
	return ret;
}

//功能：RVA 转换 FOA
// RVA_TO_FOA(pFileBuffer,pOptionHeader->DataDirectory[5].VirtualAddress,&FOA);
DWORD RVA_TO_FOA(PVOID FileAddress, DWORD RVA, PDWORD pFOA)
{
	int ret = 0;
	int i=0;
	PIMAGE_DOS_HEADER pDosHeader				= (PIMAGE_DOS_HEADER)(FileAddress);
	PIMAGE_FILE_HEADER pFileHeader				= (PIMAGE_FILE_HEADER)((DWORD)pDosHeader + pDosHeader->e_lfanew + 4);
	PIMAGE_OPTIONAL_HEADER32 pOptionalHeader	= (PIMAGE_OPTIONAL_HEADER32)((DWORD)pFileHeader + sizeof(IMAGE_FILE_HEADER));
	PIMAGE_SECTION_HEADER pSectionGroup			= (PIMAGE_SECTION_HEADER)((DWORD)pOptionalHeader + pFileHeader->SizeOfOptionalHeader);
	
	
	//RVA在文件头中 或 SectionAlignment(内存对齐) 等于 FileAlignment(文件对齐) 时 RVA等于FOA
	if (RVA < pOptionalHeader->SizeOfHeaders || pOptionalHeader->SectionAlignment == pOptionalHeader->FileAlignment)
	{
		// 37000
		*pFOA = RVA;
		return ret;
	}
	
	/*
		第一步：指定节.VirtualAddress <= RVA <= 指定节.VirtualAddress + Misc.VirtualSize(当前节内存实际大小)
		第二步：差值 = RVA - 指定节.VirtualAddress
		第三步：FOA = 指定节.PointerToRawData + 差值
	*/

	//循环判断RVA在节区中
	for (i=0;i<pFileHeader->NumberOfSections; i++)
	{
		// RVA > 当前节在内存中的偏移地址 并且 RVA < 当前节的内存偏移地址+文件偏移地址
		if (RVA >= pSectionGroup[i].VirtualAddress && RVA < pSectionGroup[i].VirtualAddress + pSectionGroup[i].Misc.VirtualSize)
		{
			*pFOA =  RVA - pSectionGroup[i].VirtualAddress + pSectionGroup[i].PointerToRawData;
			return ret;
		}
	}
	
	//没有找到地址
	ret = -4;
	printf("func RVA_TO_FOA() Error: %d 地址转换失败！\n", ret);
	return ret;
}

DWORD GetSizeOfImage(PVOID pFileBuffer){
	PIMAGE_DOS_HEADER pImageDosHeader = NULL;
	PIMAGE_FILE_HEADER pImageFileHeader = NULL;
	PIMAGE_OPTIONAL_HEADER32 pImageOptionalHeader = NULL;
	PIMAGE_SECTION_HEADER pImageSectionHeaderGroup = NULL;
	PIMAGE_SECTION_HEADER NewSec = NULL;

	DWORD NewLength=0;
	PVOID LastSection = NULL;
	PVOID CodeSection = NULL;
	PVOID AddressOfSectionTable = NULL;
	
	
	pImageDosHeader = (PIMAGE_DOS_HEADER)pFileBuffer;
	pImageFileHeader = (PIMAGE_FILE_HEADER)((DWORD)pImageDosHeader + pImageDosHeader->e_lfanew + 4);
	pImageOptionalHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pImageFileHeader + sizeof(IMAGE_FILE_HEADER));
	pImageSectionHeaderGroup = (PIMAGE_SECTION_HEADER)((DWORD)pImageOptionalHeader + pImageFileHeader->SizeOfOptionalHeader);

	return pImageOptionalHeader->SizeOfImage;
}


DWORD GetImageBase(PVOID pFileBuffer){
	PIMAGE_DOS_HEADER pImageDosHeader = NULL;
	PIMAGE_FILE_HEADER pImageFileHeader = NULL;
	PIMAGE_OPTIONAL_HEADER32 pImageOptionalHeader = NULL;
	PIMAGE_SECTION_HEADER pImageSectionHeaderGroup = NULL;
	PIMAGE_SECTION_HEADER NewSec = NULL;
	
	DWORD NewLength=0;
	PVOID LastSection = NULL;
	PVOID CodeSection = NULL;
	PVOID AddressOfSectionTable = NULL;
	
	
	pImageDosHeader = (PIMAGE_DOS_HEADER)pFileBuffer;
	pImageFileHeader = (PIMAGE_FILE_HEADER)((DWORD)pImageDosHeader + pImageDosHeader->e_lfanew + 4);
	pImageOptionalHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pImageFileHeader + sizeof(IMAGE_FILE_HEADER));
	pImageSectionHeaderGroup = (PIMAGE_SECTION_HEADER)((DWORD)pImageOptionalHeader + pImageFileHeader->SizeOfOptionalHeader);
	
	return pImageOptionalHeader->ImageBase;
}

DWORD GetRelocationTable(PVOID pFileBuffer){
	PIMAGE_DOS_HEADER pImageDosHeader = NULL;
	PIMAGE_FILE_HEADER pImageFileHeader = NULL;
	PIMAGE_OPTIONAL_HEADER32 pImageOptionalHeader = NULL;
	PIMAGE_SECTION_HEADER pImageSectionHeaderGroup = NULL;
	PIMAGE_SECTION_HEADER NewSec = NULL;
	DWORD res = 0;

	DWORD NewLength=0;
	PVOID LastSection = NULL;
	PVOID CodeSection = NULL;
	PVOID AddressOfSectionTable = NULL;
	DWORD FOA = 0;
	
	pImageDosHeader = (PIMAGE_DOS_HEADER)pFileBuffer;
	pImageFileHeader = (PIMAGE_FILE_HEADER)((DWORD)pImageDosHeader + pImageDosHeader->e_lfanew + 4);
	pImageOptionalHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pImageFileHeader + sizeof(IMAGE_FILE_HEADER));
	pImageSectionHeaderGroup = (PIMAGE_SECTION_HEADER)((DWORD)pImageOptionalHeader + pImageFileHeader->SizeOfOptionalHeader);

	return pImageOptionalHeader->DataDirectory[5].VirtualAddress;
}


DWORD GetOep(PVOID pFileBuffer){
	PIMAGE_DOS_HEADER pImageDosHeader = NULL;
	PIMAGE_FILE_HEADER pImageFileHeader = NULL;
	PIMAGE_OPTIONAL_HEADER32 pImageOptionalHeader = NULL;
	PIMAGE_SECTION_HEADER pImageSectionHeaderGroup = NULL;
	PIMAGE_SECTION_HEADER NewSec = NULL;
	PIMAGE_BASE_RELOCATION pRelocationDirectory = NULL;

	pImageDosHeader = (PIMAGE_DOS_HEADER)pFileBuffer;
	pImageFileHeader = (PIMAGE_FILE_HEADER)((DWORD)pImageDosHeader + pImageDosHeader->e_lfanew + 4);
	pImageOptionalHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pImageFileHeader + sizeof(IMAGE_FILE_HEADER));
	pImageSectionHeaderGroup = (PIMAGE_SECTION_HEADER)((DWORD)pImageOptionalHeader + pImageFileHeader->SizeOfOptionalHeader);

	return pImageOptionalHeader->AddressOfEntryPoint;
}

void ChangesImageBase(PVOID pFileBuffer, DWORD TempImageBase){
	PIMAGE_DOS_HEADER pImageDosHeader = NULL;
	PIMAGE_FILE_HEADER pImageFileHeader = NULL;
	PIMAGE_OPTIONAL_HEADER32 pImageOptionalHeader = NULL;
	PIMAGE_SECTION_HEADER pImageSectionHeaderGroup = NULL;
	PIMAGE_SECTION_HEADER NewSec = NULL;
	
	pImageDosHeader = (PIMAGE_DOS_HEADER)pFileBuffer;
	pImageFileHeader = (PIMAGE_FILE_HEADER)((DWORD)pImageDosHeader + pImageDosHeader->e_lfanew + 4);
	pImageOptionalHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pImageFileHeader + sizeof(IMAGE_FILE_HEADER));
	pImageSectionHeaderGroup = (PIMAGE_SECTION_HEADER)((DWORD)pImageOptionalHeader + pImageFileHeader->SizeOfOptionalHeader);

	pImageOptionalHeader->ImageBase = TempImageBase;
}

void XorDecodeAAA(char* p_data,DWORD DecodeSize)
{
    for(DWORD i = 0; i < DecodeSize; i++)
    {
		p_data[i] = p_data[i] ^ XORKEY;
    }	
}

void GetSrcDataFromShell(PVOID pFileBufferShell, PVOID* FileBufferSrc, PDWORD FileBufferLength, PDWORD FileBufferImageBase){
	PIMAGE_DOS_HEADER pDosHeader = NULL;    
    PIMAGE_NT_HEADERS pNTHeader = NULL; 
    PIMAGE_FILE_HEADER pPEHeader = NULL;    
    PIMAGE_OPTIONAL_HEADER32 pOptionHeader = NULL;  
    PIMAGE_SECTION_HEADER pSectionHeader = NULL;
	
	
	pDosHeader = (PIMAGE_DOS_HEADER)pFileBufferShell;
    pNTHeader = (PIMAGE_NT_HEADERS)((DWORD)pFileBufferShell+pDosHeader->e_lfanew);
    pPEHeader = (PIMAGE_FILE_HEADER)(((DWORD)pNTHeader) + 4);  
    pOptionHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPEHeader+IMAGE_SIZEOF_FILE_HEADER); 
	pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader + IMAGE_SIZEOF_NT_OPTIONAL_HEADER);


	// (1) 定为到SHELL文件的最后一个节	
	*FileBufferSrc = (PVOID)((DWORD)pFileBufferShell + ((PIMAGE_SECTION_HEADER)&pSectionHeader[pPEHeader->NumberOfSections-1])->PointerToRawData);
	XorDecodeAAA((char*)(*FileBufferSrc),((PIMAGE_SECTION_HEADER)&pSectionHeader[pPEHeader->NumberOfSections-1])->SizeOfRawData);
	pDosHeader = (PIMAGE_DOS_HEADER)*FileBufferSrc;
    pNTHeader = (PIMAGE_NT_HEADERS)((DWORD)*FileBufferSrc + pDosHeader->e_lfanew);
    pPEHeader = (PIMAGE_FILE_HEADER)(((DWORD)pNTHeader) + 4);  
    pOptionHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPEHeader+IMAGE_SIZEOF_FILE_HEADER); 
	pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader + IMAGE_SIZEOF_NT_OPTIONAL_HEADER);

	// get SizeOfImage
	*FileBufferLength = pOptionHeader->SizeOfImage;
	
	// get ImageBase
	*FileBufferImageBase = pOptionHeader->ImageBase;
}


// 解密壳子
PIMAGE_DOS_HEADER GetDosHeadersPtr(LPBYTE buffer)
{
	PIMAGE_DOS_HEADER p=(PIMAGE_DOS_HEADER)buffer;
	if(p)
	{
		return PIMAGE_DOS_HEADER(p);
	}else{
		return NULL;
	}
}
PIMAGE_FILE_HEADER GetFileHeadersPtr(LPBYTE buffer)
{

	PIMAGE_FILE_HEADER p=( PIMAGE_FILE_HEADER)&(( PIMAGE_NT_HEADERS)&buffer[(( PIMAGE_DOS_HEADER)buffer)->e_lfanew])->FileHeader;
	if(p)
	{
		return PIMAGE_FILE_HEADER(p);
	}else{
		return NULL;
	}
}

PIMAGE_SECTION_HEADER GetSectionHeadersPtr(LPBYTE buffer,int index)
{
	int section_num=GetFileHeadersPtr(buffer)->NumberOfSections;	
	int section_size=40;
	if(index<section_num)
	{
		return ( PIMAGE_SECTION_HEADER)&buffer[GetDosHeadersPtr(buffer)->e_lfanew+(4+20+GetFileHeadersPtr(buffer)->SizeOfOptionalHeader)+index*section_size];
	}else{
		return NULL;
	}
}
BOOL IsPackingEXE(LPBYTE hInstance)
{
    DWORD sectionNum = GetFileHeadersPtr((LPBYTE)hInstance)->NumberOfSections-1;
    PIMAGE_SECTION_HEADER lastSection = GetSectionHeadersPtr((LPBYTE)hInstance, sectionNum);
    if (strcmp((CHAR*)lastSection[0].Name, ".AAA") == 0)
    {
        return TRUE;
    }
    return FALSE;
}