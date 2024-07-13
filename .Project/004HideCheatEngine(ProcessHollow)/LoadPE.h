#include "StdAfx.h"

#if !defined(AFX_PETOOL_H__BC9AE6DA_8D52_4356_9802_921968C7E3DA__INCLUDED_)
#define AFX_PETOOL_H__BC9AE6DA_8D52_4356_9802_921968C7E3DA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//判断是否是PE文件
BOOL IsPE(TBYTE* m_strFilePath)
{
	// TODO: Add your control notification handler code here

	HANDLE    hFile = INVALID_HANDLE_VALUE; //打开的PE文件句柄
	IMAGE_DOS_HEADER DosHeader = { 0 };
	DWORD    dwReadLen = 0;
	DWORD    dwNTSignature = 0;

	//创建文件
	hFile = CreateFile(m_strFilePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		//MessageBox("打开文件失败");
		return FALSE;
	}

	//读取文件
	if (!ReadFile(hFile, &DosHeader, sizeof(DosHeader), &dwReadLen, NULL))
	{
		CloseHandle(hFile);
		//MessageBox("读取文件失败");
		return FALSE;
	}
	//判断DOS头部是否为MZ
	if (DosHeader.e_magic != IMAGE_DOS_SIGNATURE)
	{
		CloseHandle(hFile);
		//MessageBox("不是PE文件");
		return FALSE;
	}


	SetFilePointer(hFile, DosHeader.e_lfanew, NULL, FILE_BEGIN);
	//读取PE的NT头字节
	if (!ReadFile(hFile, &dwNTSignature, sizeof(dwNTSignature), &dwReadLen, NULL))
	{
		return FALSE;
	}
	//判断PE标志
	if (dwNTSignature != IMAGE_NT_SIGNATURE)
	{
		//MessageBox("不是PE文件");
		CloseHandle(hFile);
		return FALSE;
	}

	CloseHandle(hFile);
	hFile = INVALID_HANDLE_VALUE;

	//MessageBox("是PE文件");
	return TRUE;
}
BOOL IsPE32(TBYTE* m_strFilePath)
{
	// TODO: Add your control notification handler code here

	HANDLE    hFile = INVALID_HANDLE_VALUE; //打开的PE文件句柄
	IMAGE_DOS_HEADER DosHeader = { 0 };
	DWORD    dwReadLen = 0;
	DWORD    dwNTSignature = 0;
	WORD	 dwMachine = 0;

	//创建文件
	hFile = CreateFile(m_strFilePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		//MessageBox("打开文件失败");
		return FALSE;
	}

	//读取文件
	if (!ReadFile(hFile, &DosHeader, sizeof(DosHeader), &dwReadLen, NULL))
	{
		CloseHandle(hFile);
		//MessageBox("读取文件失败");
		return FALSE;
	}
	//判断DOS头部是否为MZ
	if (DosHeader.e_magic != IMAGE_DOS_SIGNATURE)
	{
		CloseHandle(hFile);
		//MessageBox("不是PE文件");
		return FALSE;
	}


	SetFilePointer(hFile, DosHeader.e_lfanew, NULL, FILE_BEGIN);
	//读取PE的NT头字节
	if (!ReadFile(hFile, &dwNTSignature, sizeof(dwNTSignature), &dwReadLen, NULL))
	{
		return FALSE;
	}
	//判断PE标志
	if (dwNTSignature != IMAGE_NT_SIGNATURE)
	{
		//MessageBox("不是PE文件");
		CloseHandle(hFile);
		return FALSE;
	}
	// 判断Machine x86
	SetFilePointer(hFile, DosHeader.e_lfanew+4, NULL, FILE_BEGIN);
	if (!ReadFile(hFile, &dwMachine, sizeof(dwMachine), &dwReadLen, NULL)) return FALSE;
	if (dwMachine != 0x14C)
	{
		CloseHandle(hFile);
		return FALSE;
	}

	CloseHandle(hFile);
	hFile = INVALID_HANDLE_VALUE;

	//MessageBox("是PE文件");
	return TRUE;
}


DWORD AutoAlign(DWORD x,DWORD alignNum)
{
	return x%alignNum==0?(x!=0?x:alignNum):(x/alignNum+1)*alignNum;
}

//DWORD GetFileSize(LPBYTE fPath) {
//	FILE* file = nullptr;
//	fopen_s(&file,fPath, "rb");
//    if (file != nullptr) 
//	{
//		fseek(file, 0, SEEK_END);
//		DWORD size = ftell(file);
//		fclose(file);
//
//		return size;
//	}else{
//        //DbgPrintf(TEXT("Failed to open file.\n\n"));
//		
//        return -1;
//    }
//}


//void WriteToFile(LPBYTE data, LPBYTE fPath, DWORD fileSize) {
//	FILE* file = nullptr;
//	fopen_s(&file,fPath, "wb");
//    if (file != nullptr) {
//       		
//		fseek(file,0,SEEK_SET);
//		DWORD result=fwrite(data, 1, fileSize, file);
//
//		if(result==fileSize)
//		{
//			//DbgPrintf(TEXT("Data Successfully written to %s.\n\n"),fPath);
//		}else{
//			//DbgPrintf(TEXT("Data Unsuccessfully written to %s.\n\n"),fPath);
//		}
//		fclose(file);
//    }else{
//		//DbgPrintf(TEXT("Failed to open %s.\n\n"),fPath);
//	}
//}


//LPBYTE ReadPE(LPBYTE pePath)
//{
//	LPBYTE buffer=nullptr;
//	int i=0;
//	FILE* fp=nullptr;
//	fopen_s(&fp, pePath, "rb");
//	if(fp!=nullptr)
//	{
//		i++;
//		fseek(fp,0,SEEK_END);
//		int size=ftell(fp);
//		rewind(fp);
//		buffer=(LPBYTE)malloc(sizeof(BYTE)*size);
//		if(buffer)
//		{
//			i++;
//			DWORD result=fread(buffer,1,size,fp);
//			if(result==(unsigned int)size){
//				i++;
//			}
//		}
//	}
//
//	switch(i)
//	{
//		case 0:
//			//DbgPrintf(TEXT("%s file do't exist.\n"),pePath);
//			break;
//		case 1:
//			//DbgPrintf(TEXT("memory not enough.\n"));
//			fclose(fp);
//			break;
//		case 2:
//			//DbgPrintf(TEXT("read pe fail.\n"));
//			free(buffer);
//			fclose(fp);
//			break;
//
//		default:
//			fclose(fp);
//			return buffer;
//	}
//
//	return nullptr;
//}


DOS_HEADER* GetDosHeadersPtr(LPBYTE buffer)
{
	struct DOS_HEADER* p=( DOS_HEADER*)buffer;
	if(p)
	{
		return p;
	}else{
		return nullptr;
	}
}


NT_HEADERS* GetNtHeadersPtr(LPBYTE buffer)
{
	struct NT_HEADERS* p=( NT_HEADERS*)&buffer[(( DOS_HEADER*)buffer)->e_lfanew];
	if(p)
	{
		return p;
	}else{
		return nullptr;
	}
}


FILE_HEADER* GetFileHeadersPtr(LPBYTE buffer)
{

	struct FILE_HEADER* p=( FILE_HEADER*)&(( NT_HEADERS*)&buffer[(( DOS_HEADER*)buffer)->e_lfanew])->FileHeader;
	if(p)
	{
		return p;
	}else{
		return nullptr;
	}
}

OPTIONAL_HEADER* GetOptionalHeadersPtr(LPBYTE buffer)
{
	struct OPTIONAL_HEADER* p=( OPTIONAL_HEADER*)&(( NT_HEADERS*)&buffer[(( DOS_HEADER*)buffer)->e_lfanew])->OptionalHeader;

	if(p)
	{
		return p;
	}else{
		return nullptr;
	}
}


OPTIONAL_HEADER64* GetOptionalHeaders64Ptr(LPBYTE buffer)
{
	struct OPTIONAL_HEADER64* p=( OPTIONAL_HEADER64*)&(( NT_HEADERS64*)&buffer[(( DOS_HEADER*)buffer)->e_lfanew])->OptionalHeader;
	if(p)
	{
		return p;
	}else{
		return nullptr;
	}
}



// SECTION_HEADER
SECTION_HEADER* GetSectionHeadersPtr(LPBYTE buffer,int index)
{
	int section_num=GetFileHeadersPtr(buffer)->NumberOfSections;	
	int section_size=40;
	if(index<section_num)
	{
		return ( SECTION_HEADER*)&buffer[GetDosHeadersPtr(buffer)->e_lfanew+(4+20+GetFileHeadersPtr(buffer)->SizeOfOptionalHeader)+index*section_size];
	}else{
		return nullptr;
	}
}

// 模拟pe拉伸
DWORD FileBufferToImageBuffer(IN LPBYTE lpFilebuffer,OUT LPBYTE& lpImageBuffer)
{
	DWORD log_=2;
	DWORD i=0;
	DWORD j=0;
	DWORD img_size=0;
	DWORD headers_size=0;
	
	FILE_HEADER* p_stdpe=GetFileHeadersPtr(lpFilebuffer);
	DWORD section_num=p_stdpe->NumberOfSections;
	SECTION_HEADER* p_section=nullptr;
	DWORD virtual_address=0;
	DWORD rawdata_size=0;
	DWORD pointer_rawdata=0;

	//LPBYTE lpImageBuffer=nullptr;
	if(lpFilebuffer)
	{
		log_--;
		// x86,x86-64
		if(p_stdpe->Machine==0x14c)
		{
			img_size=GetOptionalHeadersPtr(lpFilebuffer)->SizeOfImage;
			headers_size=GetOptionalHeadersPtr(lpFilebuffer)->SizeOfHeaders;
		}else{
			img_size=GetOptionalHeaders64Ptr(lpFilebuffer)->SizeOfImage;
			headers_size=GetOptionalHeaders64Ptr(lpFilebuffer)->SizeOfHeaders;
		}
			lpImageBuffer=(LPBYTE)HeapAlloc(GetProcessHeap(),0, sizeof(BYTE) * img_size);

		if(lpImageBuffer)
		{
			log_--;
			memset(lpImageBuffer,0,img_size);
			// headers
			for(i=0;i<headers_size;i++)
			{
				lpImageBuffer[i]=lpFilebuffer[i];
			}
			
			// sections
			for(i=0;i<section_num;i++)
			{
				p_section=GetSectionHeadersPtr(lpFilebuffer,i);
				virtual_address=p_section->VirtualAddress;
				rawdata_size=p_section->SizeOfRawData;
				pointer_rawdata=p_section->PointerToRawData;

				for(j=0;j<rawdata_size;j++)
				{
					lpImageBuffer[virtual_address+j]=lpFilebuffer[pointer_rawdata+j];
				}
			}
		}

	}
	switch(log_)
	{
		case 2:
			//DbgPrintf(TEXT("get a nullptr.\n"));
			break;
		case 1:
			//DbgPrintf(TEXT("memory not enough.\n"));
			break;
		default:
			return log_;
	}

	return log_;
}

LPBYTE ImageBufferToFileBuffer(LPBYTE p)
{
	int log_=0;
	int i=0;
	int j=0;
	int headers_size=0;
	
	FILE_HEADER* p_stdpe=GetFileHeadersPtr(p);
	int section_num=p_stdpe->NumberOfSections;
	SECTION_HEADER* p_section=nullptr;
	int virtual_address=0;
	int rawdata_size=0;
	int pointer_rawdata=0;
	int file_size=0;

	LPBYTE file_buffer=nullptr;
	if(p)
	{
		log_++;
		// x86,x86-64
		if(p_stdpe->Machine==0x14c)
		{

			headers_size=GetOptionalHeadersPtr(p)->SizeOfHeaders;
		}else{
			headers_size=GetOptionalHeaders64Ptr(p)->SizeOfHeaders;
		}
		
		p_section=GetSectionHeadersPtr(p,section_num-1);
		file_size=p_section->PointerToRawData+p_section->SizeOfRawData;
		file_buffer=(LPBYTE)malloc(sizeof(BYTE)*file_size);		

		if(file_buffer)
		{
			log_++;
			memset(file_buffer,0,file_size);
			// headers
			for(i=0;i<headers_size;i++)
			{
				file_buffer[i]=p[i];
			}
			
			// sections
			for(i=0;i<section_num;i++)
			{
				p_section=GetSectionHeadersPtr(p,i);
				virtual_address=p_section->VirtualAddress;
				rawdata_size=p_section->SizeOfRawData;
				pointer_rawdata=p_section->PointerToRawData;

				for(j=0;j<rawdata_size;j++)
				{
					file_buffer[pointer_rawdata+j]=p[virtual_address+j];
				}
			}
		}


	}
	switch(log_)
	{
		case 0:
			//DbgPrintf(TEXT("get a nullptr.\n"));
			break;
		case 1:
			//DbgPrintf(TEXT("memory not enough.\n"));
			break;
		default:
			return file_buffer;
	}

	return nullptr;
}


// rva=内存中相对偏地址
ULONG64 RVAtoFOA(ULONG64 rva,LPBYTE p)
{
	if(p==nullptr)
	{
		//DbgPrintf(TEXT("get a nullptr.\n"));
		return 0;
	}
	ULONG64 img_base;
	int virtual_address;
	int file_address;
	ULONG64 roa;

	int section_num=GetFileHeadersPtr(p)->NumberOfSections;
	int i=0;

	// rva< VirtualAddress of first section 
	if(rva<GetSectionHeadersPtr(p,i)->VirtualAddress)
	{
		return rva;
	}

	if(GetFileHeadersPtr(p)->Machine==0x14c)
	{
		img_base=(ULONG64)GetOptionalHeadersPtr(p)->ImageBase;
	}else{
		img_base=GetOptionalHeaders64Ptr(p)->ImageBase;
	}

	// 确定内存中偏移
	//roa=rva-img_base;
	roa=rva;

	// 确定所在节
	for(i=0;i<section_num;i++)
	{
		// vc++6.0 ULONG64类型变量作比较存在问题
		if((DWORD)roa>=(DWORD)GetSectionHeadersPtr(p,i)->VirtualAddress && 
			(DWORD)roa<(DWORD)(GetSectionHeadersPtr(p,i)->VirtualAddress+GetSectionHeadersPtr(p,i)->Misc.VirtualSize))
		{
			virtual_address=GetSectionHeadersPtr(p,i)->VirtualAddress;
			file_address=GetSectionHeadersPtr(p,i)->PointerToRawData;
			
			//  确定相对于节的偏移
			roa=roa-virtual_address;
			return roa+file_address;		
		}
	}
	return 0;
}

ULONG64 FOAtoRVA(ULONG64 rva,LPBYTE p)
{
	if(p==nullptr)
	{
		//DbgPrintf(TEXT("get a nullptr.\n"));
		return 0;
	}
	ULONG64 img_base;
	int virtual_address;
	int file_address;
	ULONG64 roa;

	int section_num=GetFileHeadersPtr(p)->NumberOfSections;
	int i=0;

	if(GetFileHeadersPtr(p)->Machine==0x14c)
	{
		img_base=(ULONG64)GetOptionalHeadersPtr(p)->ImageBase;
	}else{
		img_base=GetOptionalHeaders64Ptr(p)->ImageBase;
	}

	// 确定内存中偏移
	//roa=rva-img_base;
	roa=rva;

	// 确定所在节
	for(i=0;i<section_num;i++)
	{
		// vc++6.0 ULONG64类型变量作比较存在问题
		if((DWORD)roa>=(DWORD)GetSectionHeadersPtr(p,i)->PointerToRawData && (DWORD)roa<(DWORD)(GetSectionHeadersPtr(p,i)->PointerToRawData+GetSectionHeadersPtr(p,i)->SizeOfRawData))
		{
			virtual_address=GetSectionHeadersPtr(p,i)->VirtualAddress;
			file_address=GetSectionHeadersPtr(p,i)->PointerToRawData;
			
			//  确定相对于节的偏移
			roa=roa-file_address;
			return roa+virtual_address;		
		}
	}
	return 0;
}



DWORD GetFileBufferSize(LPBYTE file_buffer)
{
	if(file_buffer==nullptr)
	{
		//DbgPrintf(TEXT("Get a nullptr.\n"));
		return 0;
	}

	FILE_HEADER* p_stdpe=GetFileHeadersPtr(file_buffer);
	int section_num=p_stdpe->NumberOfSections;
	SECTION_HEADER* p_section=GetSectionHeadersPtr(file_buffer,section_num-1);

	return p_section->PointerToRawData+p_section->SizeOfRawData;	// 可能不准确

}

// Directory Entry
DATA_DIRECTORY* GetDirectoryEntryPtr(LPBYTE file_buffer, int index)
{
	if (file_buffer == nullptr)
	{
		return nullptr;
	}

	FILE_HEADER* p_stdpe = nullptr;
	OPTIONAL_HEADER* p_optpe = nullptr;
	OPTIONAL_HEADER64* p_optpe64 = nullptr;

	p_stdpe = GetFileHeadersPtr(file_buffer);
	if (p_stdpe->Machine == 0x14c)
	{
		p_optpe = GetOptionalHeadersPtr(file_buffer);
		return &p_optpe->DataDirectory[index];
	}
	else {
		p_optpe64 = GetOptionalHeaders64Ptr(file_buffer);
		return &p_optpe64->DataDirectory[index];
	}
}


BOOL AddNewSection(LPBYTE* fbuffer, 
					DWORD file_size, 
					SECTION_HEADER* new_section_head, 
					LPBYTE new_section_buffer,
					DWORD new_section_size)
{
	if (!fbuffer[0])
	{
		return FALSE;
	}

	LPBYTE new_fbuffer = nullptr;
	DOS_HEADER* p_dos = GetDosHeadersPtr(fbuffer[0]);
	DWORD dos_e_lfanew = p_dos->e_lfanew;
	SECTION_HEADER* p_section = nullptr;
	DWORD section_num = GetFileHeadersPtr(fbuffer[0])->NumberOfSections;
	DWORD img_size = 0;
	DWORD headers_size = 0;
	DWORD i = 0;
	DWORD file_align = 0;
	DWORD section_align = 0;
	if (GetFileHeadersPtr(fbuffer[0])->Machine == 0x14c)
	{
		img_size = GetOptionalHeadersPtr(fbuffer[0])->SizeOfImage;
		headers_size = GetOptionalHeadersPtr(fbuffer[0])->SizeOfHeaders;
		file_align = GetOptionalHeadersPtr(fbuffer[0])->FileAlignment;
		section_align = GetOptionalHeadersPtr(fbuffer[0])->SectionAlignment;
	}
	else {
		img_size = GetOptionalHeaders64Ptr(fbuffer[0])->SizeOfImage;
		headers_size = GetOptionalHeaders64Ptr(fbuffer[0])->SizeOfHeaders;
		file_align = GetOptionalHeaders64Ptr(fbuffer[0])->FileAlignment;
		section_align = GetOptionalHeaders64Ptr(fbuffer[0])->SectionAlignment;
	}
	p_section = GetSectionHeadersPtr(fbuffer[0], section_num - 1) + 1;

	// pe头+节表后的空间 是否充足
#ifdef _WIN64
	if (headers_size - ((ULONG64)p_section - (ULONG64)fbuffer[0]) < 80) {
#else
	if (headers_size - ((DWORD)p_section - (DWORD)fbuffer[0]) < 80) {
#endif // _WIN64

		// 检查dos 后面垃圾数据
		if (dos_e_lfanew - 64 > 80)
		{
			for (i = 0; i < ((LPBYTE)p_section - fbuffer[0]) - dos_e_lfanew; i++)
			{
				fbuffer[0][i + 64] = fbuffer[0][i + dos_e_lfanew];
				fbuffer[0][i + dos_e_lfanew] = 0;
			}

			p_dos->e_lfanew = 64;
			AddNewSection(fbuffer, file_size, new_section_head, new_section_buffer, new_section_size);

			return TRUE;
		}
		return FALSE;
	}
	
	// 80 空数据 00
	for (i = 0; i < 80; i++)
	{
		if (((char*)p_section)[i] != 0) {
			break;
		}
	}
	if (i != 80)
	{
		// 检查dos 后面垃圾数据
		if (dos_e_lfanew - 64 > 80)
		{
			for (i = 0; i < ((LPBYTE)p_section - fbuffer[0]) - dos_e_lfanew; i++)
			{
				fbuffer[0][i + 64] = fbuffer[0][i + dos_e_lfanew];
				fbuffer[0][i + dos_e_lfanew] = 0;
			}

			p_dos->e_lfanew = 64;
			AddNewSection(fbuffer, file_size, new_section_head, new_section_buffer, new_section_size);

			return TRUE;
		}
		return FALSE;
	}

	p_section -= 1;	

	if(!new_section_head->Name[0]) strcpy_s((char*)new_section_head->Name, 8, ".AAA");
	new_section_head->Misc.VirtualSize = AutoAlign(new_section_size, file_align);
	new_section_head->VirtualAddress = img_size;
	new_section_head->SizeOfRawData = AutoAlign(new_section_size, file_align);
	// 最后一个节大小+其foa可能 < file_size，节与节之间可以不连续
	new_section_head->PointerToRawData = file_size;
	new_section_head->Characteristics = 0xE0000020;

	p_section += 1;
	memcpy_s((char*)p_section, sizeof(SECTION_HEADER), (char*)new_section_head, sizeof(SECTION_HEADER));

	GetFileHeadersPtr(fbuffer[0])->NumberOfSections += 1;
	if (GetFileHeadersPtr(fbuffer[0])->Machine == 0x14c)
	{
		GetOptionalHeadersPtr(fbuffer[0])->SizeOfImage = img_size + AutoAlign(new_section_size, section_align);
	}
	else {
		GetOptionalHeaders64Ptr(fbuffer[0])->SizeOfImage = img_size + AutoAlign(new_section_size, section_align);
	}

	new_fbuffer = (LPBYTE)HeapAlloc(GetProcessHeap(), 0,file_size + new_section_head->SizeOfRawData);
	if (!new_fbuffer) return FALSE;
	memcpy_s(new_fbuffer, file_size + new_section_size, fbuffer[0], file_size);
	memcpy_s(&new_fbuffer[file_size], new_section_size, new_section_buffer, new_section_size);
	HeapFree(GetProcessHeap(),0, fbuffer[0]);
	fbuffer[0] = new_fbuffer;
	
	return TRUE;
}


//DWORD AddNewSection(LPBYTE* fbuffer, 
//					DWORD file_size, 
//					SECTION_HEADER* new_section_head, 
//					LPBYTE new_section_buffer,
//					DWORD new_section_size)
//{
//	if (!fbuffer[0])
//	{
//		DbgPrintf(TEXT("Get a nullptr.\n"));
//		return 1;
//	}
//
//	LPBYTE new_fbuffer = nullptr;
//	DWORD log_code = 0;
//	DOS_HEADER* p_dos = GetDosHeadersPtr(fbuffer[0]);
//	DWORD dos_e_lfanew = p_dos->e_lfanew;
//	SECTION_HEADER* p_section = nullptr;
//	DWORD section_num = GetFileHeadersPtr(fbuffer[0])->NumberOfSections;
//	DWORD img_size = 0;
//	DWORD headers_size = 0;
//	DWORD i = 0;
//	DWORD file_align = 0;
//	DWORD section_align = 0;
//	if (GetFileHeadersPtr(fbuffer[0])->Machine == 0x14c)
//	{
//		img_size = GetOptionalHeadersPtr(fbuffer[0])->SizeOfImage;
//		headers_size = GetOptionalHeadersPtr(fbuffer[0])->SizeOfHeaders;
//		file_align = GetOptionalHeadersPtr(fbuffer[0])->FileAlignment;
//		section_align = GetOptionalHeadersPtr(fbuffer[0])->SectionAlignment;
//	}
//	else {
//		img_size = GetOptionalHeaders64Ptr(fbuffer[0])->SizeOfImage;
//		headers_size = GetOptionalHeaders64Ptr(fbuffer[0])->SizeOfHeaders;
//		file_align = GetOptionalHeaders64Ptr(fbuffer[0])->FileAlignment;
//		section_align = GetOptionalHeaders64Ptr(fbuffer[0])->SectionAlignment;
//	}
//	p_section = GetSectionHeadersPtr(fbuffer[0], section_num - 1) + 1;
//
//	// pe头+节表后的空间 是否充足
//	if (headers_size - ((DWORD)p_section - (DWORD)fbuffer[0]) > 80)
//	{
//		log_code++;
//		for (i = 0; i < 80; i++)
//		{
//			if (((char*)p_section)[i] != 0) {
//				break;
//			}
//		}
//		if (i == 80)
//		{
//			log_code++;
//			p_section -= 1;
//
//			if(!new_section_head->Name[0]) strcpy_s((char*)new_section_head->Name, 3, ".AAA");
//			new_section_head->Misc.VirtualSize = AutoAlign(new_section_size, file_align);
//			new_section_head->VirtualAddress = img_size;
//			new_section_head->SizeOfRawData = AutoAlign(new_section_size, file_align);
//			// 最后一个节大小+其foa可能 < file_size，节与节之间可以不连续
//			new_section_head->PointerToRawData = file_size;
//			new_section_head->Characteristics = 0xE0000020;
//
//			p_section += 1;
//			memcpy_s((char*)p_section, sizeof(SECTION_HEADER), (char*)new_section_head, sizeof(SECTION_HEADER));
//
//			GetFileHeadersPtr(fbuffer[0])->NumberOfSections += 1;
//			if (GetFileHeadersPtr(fbuffer[0])->Machine == 0x14c)
//			{
//				GetOptionalHeadersPtr(fbuffer[0])->SizeOfImage = img_size + AutoAlign(new_section_size, section_align);
//			}
//			else {
//				GetOptionalHeaders64Ptr(fbuffer[0])->SizeOfImage = img_size + AutoAlign(new_section_size, section_align);
//			}
//
//			new_fbuffer = (LPBYTE)HeapAlloc(GetProcessHeap(), 0,file_size + new_section_head->SizeOfRawData);
//			if (new_fbuffer)
//			{
//				log_code++;
//				memcpy_s(new_fbuffer, file_size + new_section_size, fbuffer[0], file_size);
//				memcpy_s(&new_fbuffer[file_size], new_section_size, new_section_buffer, new_section_size);
//				HeapFree(GetProcessHeap(),0, fbuffer[0]);
//				fbuffer[0] = new_fbuffer;
//			}
//		}
//	}
//
//	switch (log_code)
//	{
//	case 0:
//		DbgPrintf(TEXT("PE headers not 80 Byte.\n"));
//		return 1;
//	case 1:
//		DbgPrintf(TEXT("Not all 0 in 80 Byte.\n"));
//		// 判断 pe头的dos_stub(垃圾数据) 是否充足，是后面的数据整体向前移动 腾出空间新增节
//		if (dos_e_lfanew - 64 > 80)
//		{
//			for (i = 0; i < ((LPBYTE)p_section - fbuffer[0]) - dos_e_lfanew; i++)
//			{
//				fbuffer[0][i + 64] = fbuffer[0][i + dos_e_lfanew];
//				fbuffer[0][i + dos_e_lfanew] = 0;
//			}
//
//			p_dos->e_lfanew = 64;
//			DbgPrintf(TEXT("\nRecursive call Add_NewSection().\n"));
//			AddNewSection(fbuffer, file_size, new_section_head, new_section_buffer, new_section_size);
//			DbgPrintf(TEXT("\n"));
//			DbgPrintf(TEXT("Dos_stub is greater than 80 Byte.\n"));
//			
//			return 1;
//		}
//		DbgPrintf(TEXT("\tFail add section header\n"));
//		break;
//	case 2:
//		DbgPrintf(TEXT("new_fbuffer Don't get enough memory.\n"));
//		break;
//	default:
//		DbgPrintf(TEXT("Written new section into FileBuffer.\n"));
//		return 1;
//	}
//	return 0;
//}



BOOL RepairByRelocTable(LPBYTE lpbuffer, SIZE_T NewImgBase)
{
	if (lpbuffer == NULL)
	{
		// printf("Get a NULL.\n");
		return FALSE;
	}

	SIZE_T ImgBase = 0;
	SIZE_T DataOfNeedToRepair = 0;
	UINT RVA = 0;
	UINT FOA = 0;


	if (GetFileHeadersPtr(lpbuffer)->Machine == 0x14c)
	{
		ImgBase = GetOptionalHeadersPtr(lpbuffer)->ImageBase;
		GetOptionalHeadersPtr(lpbuffer)->ImageBase = NewImgBase;
	}
	else
	{
		ImgBase = GetOptionalHeaders64Ptr(lpbuffer)->ImageBase;
		GetOptionalHeaders64Ptr(lpbuffer)->ImageBase = NewImgBase;
	}
	UINT i = 0;
	UINT j = 0;

	DATA_DIRECTORY* p_directory = NULL;

	BASE_RELOCATION* p_relocation = NULL;
	BASE_RELOCATION* p_relocation_ = NULL;
	UINT virtual_addr = 0;
	UINT block_size = 0;
	//char str[100];
	UINT block_item = 0;
	UINT block_item_num = 0;
	UINT block_item_type = 0;

	p_directory = GetDirectoryEntryPtr((LPBYTE)lpbuffer, 5);  // 重定位表位于第6目录项	
	if (p_directory->VirtualAddress == 0) return FALSE; // 没有导入表
	p_relocation = (BASE_RELOCATION*)(&lpbuffer[RVAtoFOA(p_directory->VirtualAddress, (LPBYTE)lpbuffer)]);
	p_relocation_ = p_relocation;
	while (1)//virtual_addr!=0&&block_size!=0											
	{
		//sprintf_s(str, "[%dth Block]\n\n", i + 1);
		//printf(str);

		// 每一个重定向表结构体并不连续，										
		p_relocation_ = (BASE_RELOCATION*)((char*)p_relocation_ + block_size);
		virtual_addr = p_relocation_->VirtualAddress;
		block_size = p_relocation_->SizeOfBlock;

		if (virtual_addr != 0 && block_size != 0)
		{
			block_item_num = (block_size - 8) / 2;

			for (j = 0; j < block_item_num; j++)
			{
				block_item = ((WORD*)(&p_relocation_[1]))[j] & 4095; // 获取低12位								
				block_item_type = ((WORD*)(&p_relocation_[1]))[j] >> 12;  //  获取高4位		
				if (GetFileHeadersPtr(lpbuffer)->Machine != 0x14c && block_item_type != 10) break;
				if (GetFileHeadersPtr(lpbuffer)->Machine == 0x14c && block_item_type != 3) break;
				// __int64输出问题		
				RVA = (block_item + virtual_addr);
				FOA = RVAtoFOA((ULONG64)(block_item + virtual_addr), (LPBYTE)lpbuffer);
				DataOfNeedToRepair = ((SIZE_T*)(&lpbuffer[FOA]))[0];
				((SIZE_T*)(&lpbuffer[FOA]))[0] = DataOfNeedToRepair - ImgBase + NewImgBase;
				//DataOfNeedToRepair = ((SIZE_T*)(&lpbuffer[FOA]))[0];
				//printf("%d  %08X(rva)  %08X(foa)  %d(type)	0x%016x\n", j,RVA,FOA, block_item_type,DataOfNeedToRepair);
			}
			//system("pause>nul");
		}
		else {
			break;
		}
		i++;
	}
	return TRUE;
}



#endif // !defined(AFX_PETOOL_H__BC9AE6DA_8D52_4356_9802_921968C7E3DA__INCLUDED_)