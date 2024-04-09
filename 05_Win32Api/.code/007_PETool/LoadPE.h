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


DWORD AutoAlign(DWORD x,DWORD alignNum)
{
	return x%alignNum==0?(x!=0?x:alignNum):(x/alignNum+1)*alignNum;
}

//DWORD GetFileSize(BYTE* fPath) {
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


//void WriteToFile(BYTE* data, BYTE* fPath, DWORD fileSize) {
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


//BYTE* ReadPE(BYTE* pePath)
//{
//	BYTE* buffer=nullptr;
//	int i=0;
//	FILE* fp=nullptr;
//	fopen_s(&fp, pePath, "rb");
//	if(fp!=nullptr)
//	{
//		i++;
//		fseek(fp,0,SEEK_END);
//		int size=ftell(fp);
//		rewind(fp);
//		buffer=(BYTE*)malloc(sizeof(BYTE)*size);
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


DOS_HEADER* GetDosHeadersPtr(BYTE* buffer)
{
	struct DOS_HEADER* p=( DOS_HEADER*)buffer;
	if(p)
	{
		return p;
	}else{
		return nullptr;
	}
}


NT_HEADERS* GetNtHeadersPtr(BYTE* buffer)
{
	struct NT_HEADERS* p=( NT_HEADERS*)&buffer[(( DOS_HEADER*)buffer)->e_lfanew];
	if(p)
	{
		return p;
	}else{
		return nullptr;
	}
}


FILE_HEADER* GetFileHeadersPtr(BYTE* buffer)
{

	struct FILE_HEADER* p=( FILE_HEADER*)&(( NT_HEADERS*)&buffer[(( DOS_HEADER*)buffer)->e_lfanew])->FileHeader;
	if(p)
	{
		return p;
	}else{
		return nullptr;
	}
}

OPTIONAL_HEADER* GetOptionalHeadersPtr(BYTE* buffer)
{
	struct OPTIONAL_HEADER* p=( OPTIONAL_HEADER*)&(( NT_HEADERS*)&buffer[(( DOS_HEADER*)buffer)->e_lfanew])->OptionalHeader;

	if(p)
	{
		return p;
	}else{
		return nullptr;
	}
}


OPTIONAL_HEADER64* GetOptionalHeaders64Ptr(BYTE* buffer)
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
SECTION_HEADER* GetSectionHeadersPtr(BYTE* buffer,int index)
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

// IMAGE_BUFFER
BYTE* FileBufferToImageBuffer(BYTE* p)
{
	int log_=0;
	int i=0;
	int j=0;
	int img_size=0;
	int headers_size=0;
	
	FILE_HEADER* p_stdpe=GetFileHeadersPtr(p);
	int section_num=p_stdpe->NumberOfSections;
	SECTION_HEADER* p_section=nullptr;
	int virtual_address=0;
	int rawdata_size=0;
	int pointer_rawdata=0;

	BYTE* Img_buffer=nullptr;
	if(p)
	{
		log_++;
		// x86,x86-64
		if(p_stdpe->Machine==0x14c)
		{
			img_size=GetOptionalHeadersPtr(p)->SizeOfImage;
			headers_size=GetOptionalHeadersPtr(p)->SizeOfHeaders;
		}else{
			img_size=GetOptionalHeaders64Ptr(p)->SizeOfImage;
			headers_size=GetOptionalHeaders64Ptr(p)->SizeOfHeaders;
		}
			Img_buffer=(BYTE*)malloc(sizeof(BYTE)*img_size);	

		if(Img_buffer)
		{
			log_++;
			memset(Img_buffer,0,img_size);
			// headers
			for(i=0;i<headers_size;i++)
			{
				Img_buffer[i]=p[i];
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
					Img_buffer[virtual_address+j]=p[pointer_rawdata+j];
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
			return Img_buffer;
	}

	return nullptr;

}

BYTE* ImageBufferToFileBuffer(BYTE* p)
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

	BYTE* file_buffer=nullptr;
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
		file_buffer=(BYTE*)malloc(sizeof(BYTE)*file_size);		

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
ULONG64 RVAtoFOA(ULONG64 rva,BYTE* p)
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
		if((DWORD)roa>=(DWORD)GetSectionHeadersPtr(p,i)->VirtualAddress && (DWORD)roa<(DWORD)(GetSectionHeadersPtr(p,i)->VirtualAddress+GetSectionHeadersPtr(p,i)->Misc.VirtualSize))
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

ULONG64 FOAtoRVA(ULONG64 rva,BYTE* p)
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



DWORD GetFileBufferSize(BYTE* file_buffer)
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
DATA_DIRECTORY* GetDirectoryEntryPtr(BYTE* file_buffer, int index)
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




#endif // !defined(AFX_PETOOL_H__BC9AE6DA_8D52_4356_9802_921968C7E3DA__INCLUDED_)