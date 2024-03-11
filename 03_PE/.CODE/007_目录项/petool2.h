// petool2.h: interface for the petool2 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PETOOL2_H__5285B99C_CEF4_4CFF_B6E2_6687C068D54A__INCLUDED_)
#define AFX_PETOOL2_H__5285B99C_CEF4_4CFF_B6E2_6687C068D54A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "petool.h"

void GetPEinfo_Dos(dos_head* p_dos)
{
	if(p_dos)
	{
		char* strformat="_IMAGE_DOS_HEADER{\n\t"
						"e_magic: 0x%04x\n\t"
						"e_cblp: 0x%04x\n\t"
						"e_cp: 0x%04x\n\t"
						"e_crlc: 0x%04x\n\t"
						"e_cparhdr: 0x%04x\n\t"
						"e_minalloc: 0x%04x\n\t"
						"e_maxalloc: 0x%04x\n\t"
						"e_ss: 0x%04x\n\te_sp: 0x%04x\n\t"
						"e_csum: 0x%04x\n\t"
						"e_ip: 0x%04x\n\t"
						"e_cs: 0x%04x\n\t"
						"e_lfarlc: 0x%04x\n\t"
						"e_ovno: 0x%04x\n\t"
						"e_res[4]: 0x%04x%04x%04x%04x\n\t"
						"e_oemid: 0x%04x\n\t"
						"e_oeminfo: 0x%04x\n\t"
						"e_res2[10]: 0x%04x%04x%04x%04x%04x%04x%04x%04x%04x%04x\n\t"
						"e_lfanew: 0x%04x\n}";

		printf(strformat,
			p_dos->e_magic,
			p_dos->e_cblp,
			p_dos->e_cp,
			p_dos->e_crlc,
			p_dos->e_cparhdr,
			p_dos->e_minalloc,
			p_dos->e_maxalloc,
			p_dos->e_ss,
			p_dos->e_sp,
			p_dos->e_csum,
			p_dos->e_ip,
			p_dos->e_cs,
			p_dos->e_lfarlc,
			p_dos->e_ovno,
			p_dos->e_res[0],p_dos->e_res[1],p_dos->e_res[2],p_dos->e_res[3],
			p_dos->e_oemid,
			p_dos->e_oeminfo,
			p_dos->e_res2[0],p_dos->e_res2[1],p_dos->e_res2[2],p_dos->e_res2[3],
							p_dos->e_res2[4],p_dos->e_res2[5],p_dos->e_res2[6],
							p_dos->e_res2[7],p_dos->e_res2[8],p_dos->e_res2[9],
			p_dos->e_lfanew);
	}else{
		printf("get a NULL.\n");
	}
}




void GetPEinfo_NT(nt_head* p_nt)
{
	if(p_nt)
	{
		char* strformat="_IMAGE_NT_HEADERS{\n\tSignature: 0x%08x"
						"\n\tFileHeader(struct): 0x%08x"
						"\n\tOptionalHeader(struct): 0x%08x\n}";
		printf(strformat,
				p_nt->Signature,
				&p_nt->FileHeader,
				&p_nt->OptionalHeader);

	}else{
		printf("get a NULL.\n");
	}
}



void GetPEinfo_stdPE(stdpe_head* p_stdpe)
{
	if(p_stdpe)
	{
		char* strformat="_IMAGE_FILE_HEADER{"
						"\n\tMachine: 0x%08x"
						"\n\tNumberOfSections: 0x%08x"
						"\n\tTimeDateStamp: 0x%08x"
						"\n\tPointerToSymbolTable: 0x%08x"
						"\n\tNumberOfSymbols: 0x%08x"
						"\n\tSizeOfOptionalHeader: 0x%08x"
						"\n\tCharacteristics: 0x%08x\n}";
		printf(strformat,
				p_stdpe->Machine,
				p_stdpe->NumberOfSections,
				p_stdpe->TimeDateStamp,
				p_stdpe->PointerToSymbolTable,
				p_stdpe->NumberOfSymbols,
				p_stdpe->SizeOfOptionalHeader,
				p_stdpe->Characteristics);

	}else{
		printf("get a NULL.\n");
	}
}



void GetPEinfo_optPE(optpe_head* p_optpe)
{
	if(p_optpe)
	{
		char* strformat="_IMAGE_OPTIONAL_HEADER32{"
						"\n\tMagic: 0x%08x"
						"\n\tMajorLinkerVersion: 0x%08x"
						"\n\tMinorLinkerVersion: 0x%08x"
						"\n\tSizeOfCode: 0x%08x"
						"\n\tSizeOfInitializedData: 0x%08x"
						"\n\tSizeOfUninitializedData: 0x%08x"
						"\n\tAddressOfEntryPoint: 0x%08x"
						"\n\tBaseOfCode: 0x%08x"
						"\n\tBaseOfData: 0x%08x"
						"\n\tImageBase: 0x%08x"
						"\n\tSectionAlignment: 0x%08x"
						"\n\tFileAlignment: 0x%08x"
						"\n\tMajorOperatingSystemVersion: 0x%08x"
						"\n\tMinorOperatingSystemVersion: 0x%08x"
						"\n\tMajorImageVersion: 0x%08x"
						"\n\tMinorImageVersion: 0x%08x"
						"\n\tMajorSubsystemVersion: 0x%08x"
						"\n\tMinorSubsystemVersion: 0x%08x"
						"\n\tWin32VersionValue: 0x%08x"
						"\n\tSizeOfImage: 0x%08x"
						"\n\tSizeOfHeaders: 0x%08x"
						"\n\tCheckSum: 0x%08x"
						"\n\tSubsystem: 0x%08x"
						"\n\tDllCharacteristics: 0x%08x"
						"\n\tSizeOfStackReserve: 0x%08x"
						"\n\tSizeOfStackCommit: 0x%08x"
						"\n\tSizeOfHeapReserve: 0x%08x"
						"\n\tSizeOfHeapCommit: 0x%08x"
						"\n\tLoaderFlags: 0x%08x"
						"\n\tNumberOfRvaAndSizes: 0x%08x"
						"\n\tDataDirectory[16]: 0x%08x\n}";
		printf(strformat,
				p_optpe->Magic,
				p_optpe->MajorLinkerVersion,
				p_optpe->MinorLinkerVersion,
				p_optpe->SizeOfCode,
				p_optpe->SizeOfInitializedData,
				p_optpe->SizeOfUninitializedData,
				p_optpe->AddressOfEntryPoint,
				p_optpe->BaseOfCode,
				p_optpe->BaseOfData,
				p_optpe->ImageBase,
				p_optpe->SectionAlignment,
				p_optpe->FileAlignment,
				p_optpe->MajorOperatingSystemVersion,
				p_optpe->MinorOperatingSystemVersion,
				p_optpe->MajorImageVersion,
				p_optpe->MinorImageVersion,
				p_optpe->MajorSubsystemVersion,
				p_optpe->MinorSubsystemVersion,
				p_optpe->Win32VersionValue,
				p_optpe->SizeOfImage,
				p_optpe->SizeOfHeaders,
				p_optpe->CheckSum,
				p_optpe->Subsystem,
				p_optpe->DllCharacteristics,
				p_optpe->SizeOfStackReserve,
				p_optpe->SizeOfStackCommit,
				p_optpe->SizeOfHeapReserve,
				p_optpe->SizeOfHeapCommit,
				p_optpe->LoaderFlags,
				p_optpe->NumberOfRvaAndSizes,
				p_optpe->DataDirectory[16]);
	}else{
		printf("get a NULL.\n");
	}
}



void GetPEinfo_optPE64(_IMAGE_OPTIONAL_HEADER64* p_optpe64)
{
	
	if(p_optpe64)
	{
		char* strformat="_IMAGE_OPTIONAL_HEADER64{"
						"\n\tMagic: 0x%08x"
						"\n\tMajorLinkerVersion: 0x%08x"
						"\n\tMinorLinkerVersion: 0x%08x"
						"\n\tSizeOfCode: 0x%08x"
						"\n\tSizeOfInitializedData: 0x%08x"
						"\n\tSizeOfUninitializedData: 0x%08x"
						"\n\tAddressOfEntryPoint: 0x%08x"
						"\n\tBaseOfCode: 0x%08x"
						"\n\tImageBase: 0x%016I64x"	// 数据宽度超过8字节使用%I64d、%I64x(vc++6.0),%llx;
						"\n\tSectionAlignment: 0x%08x"
						"\n\tFileAlignment: 0x%08x"
						"\n\tMajorOperatingSystemVersion: 0x%08x"
						"\n\tMinorOperatingSystemVersion: 0x%08x"
						"\n\tMajorImageVersion: 0x%08x"
						"\n\tMinorImageVersion: 0x%08x"
						"\n\tMajorSubsystemVersion: 0x%08x"
						"\n\tMinorSubsystemVersion: 0x%08x"
						"\n\tWin32VersionValue: 0x%08x"
						"\n\tSizeOfImage: 0x%08x"
						"\n\tSizeOfHeaders: 0x%08x"
						"\n\tCheckSum: 0x%08x"
						"\n\tSubsystem: 0x%08x"
						"\n\tDllCharacteristics: 0x%08x"
						"\n\tSizeOfStackReserve: 0x%016I64x"
						"\n\tSizeOfStackCommit: 0x%016I64x"
						"\n\tSizeOfHeapReserve: 0x%016I64x"
						"\n\tSizeOfHeapCommit: 0x%016I64x"
						"\n\tLoaderFlags: 0x%08x"
						"\n\tNumberOfRvaAndSizes: 0x%08x"
						"\n\tDataDirectory[16]: 0x%08x\n}";
		printf(strformat,
				p_optpe64->Magic,
				p_optpe64->MajorLinkerVersion,
				p_optpe64->MinorLinkerVersion,
				p_optpe64->SizeOfCode,
				p_optpe64->SizeOfInitializedData,
				p_optpe64->SizeOfUninitializedData,
				p_optpe64->AddressOfEntryPoint,
				p_optpe64->BaseOfCode,
				p_optpe64->ImageBase,
				p_optpe64->SectionAlignment,
				p_optpe64->FileAlignment,
				p_optpe64->MajorOperatingSystemVersion,
				p_optpe64->MinorOperatingSystemVersion,
				p_optpe64->MajorImageVersion,
				p_optpe64->MinorImageVersion,
				p_optpe64->MajorSubsystemVersion,
				p_optpe64->MinorSubsystemVersion,
				p_optpe64->Win32VersionValue,
				p_optpe64->SizeOfImage,
				p_optpe64->SizeOfHeaders,
				p_optpe64->CheckSum,
				p_optpe64->Subsystem,
				p_optpe64->DllCharacteristics,
				p_optpe64->SizeOfStackReserve,
				p_optpe64->SizeOfStackCommit,
				p_optpe64->SizeOfHeapReserve,
				p_optpe64->SizeOfHeapCommit,
				p_optpe64->LoaderFlags,
				p_optpe64->NumberOfRvaAndSizes,
				p_optpe64->DataDirectory[16]);
	}else{
		printf("get a NULL.\n");
	}
}

void GetPEinfo_section(section_head* p)
{
	if(p==NULL) printf("get a NULL.\n");
	char* strformat="_IMAGE_SECTION_HEADER{"
					"\n\tName: %s"
					"\n\tMisc: 0x%08x"
					"\n\tVirtualAddress: 0x%08x"
					"\n\tSizeOfRawData: 0x%08x"
					"\n\tPointerToRawData: 0x%08x"
					"\n\tPointerToRelocations: 0x%08x"
					"\n\tPointerToLinenumbers: 0x%08x"
					"\n\tNumberOfRelocations: 0x%08x"
					"\n\tNumberOfLinenumbers: 0x%08x"
					"\n\tCharacteristics: 0x%08x\n}";
	char section_name[9];
	strcpy(section_name,(const char*)p->Name);
	section_name[8]='\0';
	printf(strformat,
			p->Name,
			p->Misc,
			p->VirtualAddress,
			p->SizeOfRawData,
			p->PointerToRawData,
			p->PointerToRelocations,
			p->PointerToLinenumbers,
			p->NumberOfRelocations,
			p->NumberOfLinenumbers,
			p->Characteristics);
}

void printCentered(const char* text, int width) {
	int i=0,j=0,k=0;
    int textLength = strlen(text);
    int padding = (width - textLength) / 2;
	int padding_1=width - textLength-padding;
	char padding_char[100];
	char* new_text=NULL;

	memset(padding_char,(char)'*',100);
	if(new_text=(char*)malloc(width+1))
	{
		//sprintf(new_test,"%s%s%s",);
		for(i=0;i<padding;i++)
		{
			new_text[i]=padding_char[i];
		}
		for(j=0;j<textLength;j++)
		{
			new_text[i++]=text[j];
		}
		for(k=0;k<padding_1;k++)
		{
			new_text[i++]=padding_char[k];
		}
		new_text[i]='\0';
		printf("%s\n", new_text);
	}
}

// Check Directory Entry
void checkDirectoryEntry(char* p)
{
	if(p==NULL) printf("get a NULL.\n");
	char* directory_title_en[16]={
			"Export Directory",
			"Import Directory",
			"Resource Directory",
			"Exception Directory",
			"Security Directory",
			"Base Relocation",
			"Debug Directory",
			"Architecture Specific Data",
			"RVA of GP",
			"TLS Directory",
			"Load Config Directory",
			"Bound Import Directory",
			"Import Address",
			"Delay Import",
			"COM Runtime",
			"Reserved"		
	};
	char* directory_title_zh[16]={
			"导出目录",
			"导入目录",
			"资源目录",
			"异常目录",
			"安全目录",
			"基本重定位表",
			"调试目录",
			"架构特定数据",
			"RVA of GP",
			"TLS目录",
			"加载配置目录",
			"邦定导入目录",
			"导入地址表",
			"延迟导入描述符",
			"COM运行时描述符",
			"保留的"
	};
	data_directory* p_directory=NULL;
	p_directory=GetPE_DirectoryPtr(p,0);
	char strformat[100];
	for(int i=0;i<16;i++)
	{
		sprintf(strformat,"%s[%s]",directory_title_zh[i],directory_title_en[i]);
		printCentered(strformat,50);
		printf("\tVirtualAddress: 0x%08x\n\tSize: 0x%08x\n",
				(&p_directory[i])->VirtualAddress,
				(&p_directory[i])->Size);
	}
	
}


#endif // !defined(AFX_PETOOL2_H__5285B99C_CEF4_4CFF_B6E2_6687C068D54A__INCLUDED_)
