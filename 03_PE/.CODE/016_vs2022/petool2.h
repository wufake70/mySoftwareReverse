// petool2.h: interface for the petool2 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PETOOL2_H__5285B99C_CEF4_4CFF_B6E2_6687C068D54A__INCLUDED_)
#define AFX_PETOOL2_H__5285B99C_CEF4_4CFF_B6E2_6687C068D54A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "petool.h"

void GetDosHeadersInfo(DOS_HEADER* p_dos)
{
	if(p_dos)
	{
		char* strformat=(char* )"_IMAGE_DOS_HEADER{\n\t"
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




void GetNTHeadersInfo(NT_HEADERS* p_nt)
{
	if(p_nt)
	{
		char* strformat=(char* )"_IMAGE_NT_HEADERS{\n\tSignature: 0x%08x"
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



void GetFileHeadersInfo(FILE_HEADER* p_stdpe)
{
	if(p_stdpe)
	{
		char* strformat=(char* )"_IMAGE_FILE_HEADER{"
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



void GetOptionalHeadersInfo(OPTIONAL_HEADER* p_optpe)
{
	if(p_optpe)
	{
		char* strformat=(char* )"_IMAGE_OPTIONAL_HEADER32{"
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
				p_optpe->DataDirectory[15]);
	}else{
		printf("get a NULL.\n");
	}
}



void GetOptionalHeaders64Info(_IMAGE_OPTIONAL_HEADER64* p_optpe64)
{
	
	if(p_optpe64)
	{
		char* strformat=(char* )"_IMAGE_OPTIONAL_HEADER64{"
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
				p_optpe64->DataDirectory[15]);
	}else{
		printf("get a NULL.\n");
	}
}

void GetPEinfo_section(SECTION_HEADER* p)
{
	if (p == NULL) {
		printf("get a NULL.\n");
		return;
	}
	char* strformat=(char* )"_IMAGE_SECTION_HEADER{"
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
void GetDirectoryEntry(char* p)
{
	if(p==NULL) printf("get a NULL.\n");
	char* directory_title_en[16]={
			(char* )"Export Directory",
			(char* )"Import Directory",
			(char* )"Resource Directory",
			(char* )"Exception Directory",
			(char* )"Security Directory",
			(char* )"Base Relocation",
			(char* )"Debug Directory",
			(char* )"Architecture Specific Data",
			(char* )"RVA of GP",
			(char* )"TLS Directory",
			(char* )"Load Config Directory",
			(char* )"Bound Import Directory",
			(char* )"Import Address",
			(char* )"Delay Import",
			(char* )"COM Runtime",
			(char* )"Reserved"		
	};
	char* directory_title_zh[16]={
			(char* )"导出目录",
			(char* )"导入目录",
			(char* )"资源目录",
			(char* )"异常目录",
			(char* )"安全目录",
			(char* )"基本重定位表",
			(char* )"调试目录",
			(char* )"架构特定数据",
			(char* )"RVA of GP",
			(char* )"TLS目录",
			(char* )"加载配置目录",
			(char* )"邦定导入目录",
			(char* )"导入地址表",
			(char* )"延迟导入描述符",
			(char* )"COM运行时描述符",
			(char* )"保留的"
	};
	DATA_DIRECTORY* p_directory=NULL;
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



// Check Export Directory
void CheckExportDirectory(char* p)
{
	if (p == NULL) {
		printf("get a NULL.\n");
		return;
	}
	DATA_DIRECTORY* p_directory=NULL;
	EXPORT_DIRECTORY* p_export=NULL;

	char* strformat=(char* )"_IMAGE_EXPORT_DIRECTORY{"
					"\n\t..."
					"\n\tName: 0x%08x(%s)"
					"\n\tBase: 0x%08X"
					"\n\tNumberOfFunctions: 0x%08X"
					"\n\tNumberOfNames: 0x%08X"
					"\n\tAddressOfFunctions: 0x%08X"
					"\n\tAddressOfNames: 0x%08X"
					"\n\tAddressOfNameOrdinals: 0x%08X\n}\n";

	p_directory=GetPE_DirectoryPtr(p,0);
	p_export=(EXPORT_DIRECTORY*)(&p[RVAtoFOA(p_directory->VirtualAddress,p)]);

	if(!p_export)
	{
		printf("RVAtoFOA UNsuccessfal.\n");
		return ;
	}

	printf(strformat,
			p_export->Name,&p[RVAtoFOA(p_export->Name,p)],
			p_export->Base,
			p_export->NumberOfFunctions,
			p_export->NumberOfNames,
			p_export->AddressOfFunctions,
			p_export->AddressOfNames,
			p_export->AddressOfNameOrdinals);
	
	int fun_num=p_export->NumberOfFunctions;
	int name_num=p_export->NumberOfNames;
	void** ptr_fun_addr=(void**)(&p[RVAtoFOA(p_export->AddressOfFunctions,p)]);
	char** ptr_name_addr=(char**)(&p[RVAtoFOA(p_export->AddressOfNames,p)]);
	short* ptr_ord_addr=(short*)(&p[RVAtoFOA(p_export->AddressOfNameOrdinals,p)]);

	printf("Index | Funcaton Address\t | Ordinals \t | Name \t\n");
	for(int i=0;i<fun_num;i++)
	{
		printf("%d     | 0x%08X\t\t ",i,(DWORD)ptr_fun_addr[i]);
		if(i<name_num)
		{
			printf("| %d\t\t | %s\n",ptr_ord_addr[i],&p[RVAtoFOA((__int64)ptr_name_addr[i],p)]);
		}else{
			printf("| %s\t\t | %s\n","NULL","NULL");
		}
	}

	// modify
	for(int i=0;i<name_num;i++)
	{
		printf("%d     | 0x%08X\t\t | %d\t\t\t | ",i,NULL,NULL);
		printf("%s\n",&p[RVAtoFOA((__int64)ptr_name_addr[i],p)]);
	}
	
}



// CheckRelocationInfo
void GetRelocationInfo(char* p)
{
	if(p==NULL)
	{
		printf("Get a NULL.\n");
		return ;
	}

	int i=0;
	int j=0;
	DATA_DIRECTORY* p_directory=NULL;
	BASE_RELOCATION* p_relocation=NULL;
	BASE_RELOCATION* p_relocation_=NULL;
	size_t virtual_addr=0;
	size_t block_size=0; 
	char str[100];
	int block_item=0;
	int block_item_num=0;
	int block_item_type=0;
	
	p_directory=GetPE_DirectoryPtr(p,5);  // 重定位表位于第6目录项
	p_relocation=(BASE_RELOCATION*)(&p[RVAtoFOA((__int64)p_directory->VirtualAddress,p)]);
	p_relocation_=p_relocation;
	while(1)//virtual_addr!=0&&block_size!=0
	{
		sprintf(str,"[%dth Block]",i+1);
		printCentered(str,50);

		// 重定向表结构体并不连续，
		p_relocation_=(BASE_RELOCATION*)((char*)p_relocation_+block_size);
		virtual_addr=p_relocation_->VirtualAddress;
		block_size=p_relocation_->SizeOfBlock;

		if(virtual_addr!=0&&block_size!=0)
		{
			block_item_num=(block_size-8)/2;
			
			for(j=0;j<block_item_num;j++)
			{
				block_item=((WORD*)(&p_relocation_[1]))[j]&4095; // 获取低12位
				block_item_type=((WORD*)(&p_relocation_[1]))[j]>>12;  //  获取高4位
				// __int64输出问题
				printf("%d  %08X(rva)  %08X(foa)  %d(type)\n",j,block_item+virtual_addr,(DWORD)RVAtoFOA((__int64)(block_item+virtual_addr),p),block_item_type);
			}
			system("pause>nul");
		}else{
			break;
		}

		i++;
	}
}



// Check Import table
void GetImportTableInfo(char* file_buffer)
{
	if(file_buffer==NULL)
	{
		printf("CheckIportTable: file_buffer is a NULL.\n");
		return ;
	}
	
	DATA_DIRECTORY* p_directory=GetPE_DirectoryPtr(file_buffer,1);
	IMPORT_DESCRIPTOR* p_import=(IMPORT_DESCRIPTOR*)(&file_buffer[RVAtoFOA((__int64)p_directory->VirtualAddress,file_buffer)]);
	IMPORT_DESCRIPTOR* p_import_=p_import;

	int import_num=0;
	char strformat[100];
	char* strformat_=(char* )"_IMAGE_IMPORT_DESCRIPTOR{"
						"\n\tOriginalFirstThunk: 0x%08x"
						"\n\tTimeDateStamp: 0x%08x"
						"\n\tForwarderChain: 0x%08x"
						"\n\tName: %s(0x%08x)"
						"\n\tFirstThunk: 0x%08x\n}\n";
	int origin_thunk=0;
	int origin_num=0;
	int first_thunk=0;
	int first_num=0;
	THUNK_DATA32* p_thunk=NULL;
	THUNK_DATA32* p_thunk_=NULL;
	while(1)
	{
		if(p_import_->OriginalFirstThunk+p_import_->TimeDateStamp+p_import_->ForwarderChain+p_import_->Name+p_import_->FirstThunk)
		{
			sprintf(strformat," [ %dth Import Table ] ",++import_num);
			printCentered(strformat,50);
			printf(strformat_,
					p_import_->OriginalFirstThunk,
					p_import_->TimeDateStamp,
					p_import_->ForwarderChain,
					&file_buffer[RVAtoFOA((__int64)p_import_->Name,file_buffer)],p_import_->Name,
					p_import_->FirstThunk);
		
			// OriginalFirstThunk INT
			printf("\t******** [ OriginalFirstThunk ] ********\n");
			p_thunk=(THUNK_DATA32*)(&file_buffer[RVAtoFOA((__int64)p_import_->OriginalFirstThunk,file_buffer)]);
			while(p_thunk->u1.Ordinal)
			{
				//printf("\t%s:%d\n",(p_thunk->u1.Ordinal & 0x80000000)==1?"Ordinal",p_thunk->u1.Ordinal-0x80000000:"Name");
				(p_thunk->u1.Ordinal & 0x80000000)?
					printf("\tOrdinal: %d\n",
					p_thunk->u1.Ordinal - 0x80000000)
				:printf("\tName: %d-%s\n",
					((IMPORT_BY_NAME*)(&file_buffer[RVAtoFOA((__int64)p_thunk->u1.AddressOfData,file_buffer)]))->Hint,
					((IMPORT_BY_NAME*)(&file_buffer[RVAtoFOA((__int64)p_thunk->u1.AddressOfData,file_buffer)]))->Name);
				p_thunk++;
				
			}
			// FirstThunk	IAT
			printf("\t******** [ FirstThunk ] ********\n");
			p_thunk=(THUNK_DATA32*)(&file_buffer[RVAtoFOA((__int64)p_import_->FirstThunk,file_buffer)]);
			while(p_thunk->u1.Ordinal)
			{
				//printf("\t%s:%d\n",(p_thunk->u1.Ordinal & 0x80000000)==1?"Ordinal",p_thunk->u1.Ordinal-0x80000000:"Name");
				(p_thunk->u1.Ordinal & 0x80000000)?
					printf("\tOrdinal: %d\n",
					p_thunk->u1.Ordinal - 0x80000000)
				:((int)p_import_->TimeDateStamp==-1)?	// if timestamp==-1,pinrtf address ...
					printf("\tFun_Addr: 0x%x\n",(++p_thunk)->u1.Function)
					:printf("\tName: %d-%s\n",
						((IMPORT_BY_NAME*)(&file_buffer[RVAtoFOA((__int64)p_thunk->u1.AddressOfData,file_buffer)]))->Hint,
						((IMPORT_BY_NAME*)(&file_buffer[RVAtoFOA((__int64)p_thunk->u1.AddressOfData,file_buffer)]))->Name);
				p_thunk++;
				//system("pause>nul");
			}
			
			p_import_=(IMPORT_DESCRIPTOR*)(((char*)p_import_)+20);
			// printf("debug: %x\n",p_import_);

			system("pause>nul");
		}else{
			printf("Over...\n");
			break;
		}
	}
}



// Check Bound Import
void GetBoundImportInfo(char* file_buffer)
{
	if(file_buffer==NULL)
	{
		printf("CheckBoundImport: file_buffer is a NULL.\n");
		return ;
	}

	DATA_DIRECTORY* p_directory=GetPE_DirectoryPtr(file_buffer,11);
	if(p_directory->VirtualAddress==NULL)  // maybe 0x00000000
	{
		printf("CheckBoundImport: no Bound Import Table.\n");
		return ;
	}
	// bound import behind the section head
	BOUND_IMPORT_DESCRIPTOR* p_bound=(BOUND_IMPORT_DESCRIPTOR*)(&file_buffer[RVAtoFOA((__int64)p_directory->VirtualAddress,file_buffer)]);
	BOUND_IMPORT_DESCRIPTOR* p_bound_=p_bound;
	BOUND_FORWARDER_REF* p_ref=NULL;
	int bound_ref_num=0;
	DWORD first_offset_name=p_directory->VirtualAddress;

	char* strformat=(char* )"_IMAGE_BOUND_IMPORT_DESCRIPTOR{"
				"\n\tTimeDateStamp: %d(%s)"
				"\n\tOffsetModuleName: 0x%x(%s)"
				"\n\tNumberOfModuleForwarderRefs: %d\n}\n";

	char* strformat_=(char* )"_IMAGE_BOUND_FORWARDER_REF{"
				"\n\tTimeDateStamp: %d(%s)"
				"\n\tOffsetModuleName: 0x%x(%s)"
				"\n\tReserved: %d\n}\n";
	// timestamp
	struct tm* time_info=NULL;
	char time_buffer[80];
	
	while(p_bound_->TimeDateStamp+p_bound_->OffsetModuleName+p_bound_->NumberOfModuleForwarderRefs)
	{
		time_info=localtime((const time_t*)&p_bound_->TimeDateStamp);
		strftime(time_buffer,sizeof(time_buffer),"%Y-%m-%d %H:%M:%S",time_info);

		printf(strformat,
				p_bound_->TimeDateStamp,time_buffer,
				p_bound_->OffsetModuleName,&file_buffer[p_bound_->OffsetModuleName+first_offset_name],
				p_bound_->NumberOfModuleForwarderRefs);

		bound_ref_num=p_bound_->NumberOfModuleForwarderRefs;
		for(int i=0;i<bound_ref_num;i++)
		{
			p_ref=(BOUND_FORWARDER_REF*)(&p_bound_[i+1]);
			time_info=localtime((const time_t*)&p_ref->TimeDateStamp);
			strftime(time_buffer,sizeof(time_buffer),"%Y-%m-%d %H:%M:%S",time_info);
			
			printf(strformat_,
				p_ref->TimeDateStamp,time_buffer,
				p_ref->OffsetModuleName,&file_buffer[p_ref->OffsetModuleName+first_offset_name],
				p_ref->Reserved);
		}
		p_bound_=&p_bound_[bound_ref_num+1];
	}
}

void GetResourceTableInfo(char* file_buffer)
{
	if(file_buffer==NULL)
	{
		printf("CheckBoundImport: file_buffer is a NULL.\n");
		return ;
	}

	DATA_DIRECTORY* p_directory=GetPE_DirectoryPtr(file_buffer,2);
	if(p_directory->VirtualAddress==NULL)  // maybe 0x00000000
	{
		printf("CheckResourceTable: no Recource Table.\n");
		return ;
	}

	RESOURCE_DIRECTORY* p_resource_type = (RESOURCE_DIRECTORY*)(&file_buffer[RVAtoFOA((__int64)p_directory->VirtualAddress,file_buffer)]);
	RESOURCE_DIRECTORY_ENTRY* p_resource_entry = NULL;
	size_t type_number = p_resource_type->NumberOfNamedEntries+p_resource_type->NumberOfIdEntries;
	
	RESOURCE_DIR_STRING_U* pstr=NULL;
	wchar_t name[0x30]={0};

	printCentered("RESOURCE TABLE",50);
	printf("INDEX\tID\tNAME\tPS\n");
	for(size_t i=0;i < type_number;i++)
	{
		p_resource_entry=&((RESOURCE_DIRECTORY_ENTRY*)(&p_resource_type[1]))[i];

		// unicode or id
		if(p_resource_entry->NameIsString)
		{
			pstr=(RESOURCE_DIR_STRING_U*)((char*)p_resource_type+p_resource_entry->NameOffset);
			memset(name,0,0x30);
			memcpy(name,pstr->NameString,pstr->Length*(sizeof (wchar_t)));
			wprintf(L"%d\tNULL\t%s\tNULL\n",i,name);
		}else{
			switch(p_resource_entry->NameOffset)
			{
				
			case (DWORD)RT_CURSOR:
				printf("%d\t%d\tNULL\tRT_CURSOR\n",i,p_resource_entry->NameOffset);
				break;
			case (DWORD)RT_BITMAP:
				printf("%d\t%d\tNULL\tRT_BITMAP\n",i,p_resource_entry->NameOffset);
				 break;
			case (DWORD)RT_ICON:
				printf("%d\t%d\tNULL\tRT_ICON\n",i,p_resource_entry->NameOffset);
				 break;
			case (DWORD)RT_MENU:
				printf("%d\t%d\tNULL\tRT_MENU\n",i,p_resource_entry->NameOffset);
				 break;
			case (DWORD)RT_DIALOG:
				printf("%d\t%d\tNULL\tRT_DIALOG\n",i,p_resource_entry->NameOffset);
				 break;
			case (DWORD)RT_STRING:
				printf("%d\t%d\tNULL\tRT_STRING\n",i,p_resource_entry->NameOffset);
				 break;
			case (DWORD)RT_FONTDIR:
				printf("%d\t%d\tNULL\tRT_FONTDIR\n",i,p_resource_entry->NameOffset);
				 break;
			case (DWORD)RT_FONT:
				printf("%d\t%d\tNULL\tRT_FONT\n",i,p_resource_entry->NameOffset);
				 break;
			case (DWORD)RT_ACCELERATOR:
				printf("%d\t%d\tNULL\tRT_ACCELERATOR\n",i,p_resource_entry->NameOffset);
				 break;
			case (DWORD)RT_RCDATA:
				printf("%d\t%d\tNULL\tRT_RCDATA\n",i,p_resource_entry->NameOffset);
				 break;
			case (DWORD)RT_MESSAGETABLE:
				printf("%d\t%d\tNULL\tRT_MESSAGETABLE\n",i,p_resource_entry->NameOffset);
				 break;
			case (DWORD)RT_GROUP_CURSOR:
				printf("%d\t%d\tNULL\tRT_GROUP_CURSOR\n",i,p_resource_entry->NameOffset);
				 break;
			case (DWORD)RT_GROUP_ICON:
				printf("%d\t%d\tNULL\tRT_GROUP_ICON\n",i,p_resource_entry->NameOffset);
				 break;
			case (DWORD)RT_VERSION:
				printf("%d\t%d\tNULL\tRT_VERSION\n",i,p_resource_entry->NameOffset);
				 break;
			case (DWORD)RT_DLGINCLUDE:
				printf("%d\t%d\tNULL\tRT_DLGINCLUDE\n",i,p_resource_entry->NameOffset);
				 break;
			case (DWORD)RT_PLUGPLAY:
				printf("%d\t%d\tNULL\tRT_PLUGPLAY\n",i,p_resource_entry->NameOffset);
				 break;
			case (DWORD)RT_VXD:
				printf("%d\t%d\tNULL\tRT_VXD\n",i,p_resource_entry->NameOffset);
				 break;
			case (DWORD)RT_ANICURSOR:
				printf("%d\t%d\tNULL\tRT_ANICURSOR\n",i,p_resource_entry->NameOffset);
				 break;
			case (DWORD)RT_ANIICON:
				printf("%d\t%d\tNULL\tRT_ANIICON\n",i,p_resource_entry->NameOffset);
				 break;
			case (DWORD)RT_HTML:
				printf("%d\t%d\tNULL\tRT_HTML\n",i,p_resource_entry->NameOffset);
				 break;
			default:
				printf("%d\t%d\tNULL\tNULL\n",i,p_resource_entry->NameOffset);
			}
		}
	}
}


void GetResourceInfo(char* file_buffer,size_t index)
{
	if(file_buffer==NULL)
	{
		printf("CheckBoundImport: Get a NULL.\n");
		return ;
	}

	DATA_DIRECTORY* p_directory=GetPE_DirectoryPtr(file_buffer,2);
	if(p_directory->VirtualAddress==NULL)  // maybe 0x00000000
	{
		printf("CheckResourceTable: no Recource Table.\n");
		return ;
	}

	RESOURCE_DIRECTORY* p_resource_type = (RESOURCE_DIRECTORY*)(&file_buffer[RVAtoFOA((__int64)p_directory->VirtualAddress,file_buffer)]);
	RESOURCE_DIRECTORY* p_resource_serial = NULL;
	RESOURCE_DIRECTORY* p_resource_charset = NULL;
	RESOURCE_DIRECTORY_ENTRY* p_resource_entry = &((RESOURCE_DIRECTORY_ENTRY*)(&p_resource_type[1]))[index];
	RESOURCE_DIRECTORY_ENTRY* p_resource_serial_entry=NULL;
	RESOURCE_DIRECTORY_ENTRY* p_resource_charset_entry=NULL;
	RESOURCE_DATA_ENTRY* p_resource_charset_entry_=NULL;
	 
	DATA_DIRECTORY* p_data = NULL;
	size_t serial_num = 0;

	RESOURCE_DIR_STRING_U* pstr = NULL;
	wchar_t name[0x30]={0};
	if(p_resource_entry->DataIsDirectory)
	{
		p_resource_serial = (RESOURCE_DIRECTORY*)((char*)p_resource_type+p_resource_entry->OffsetToDirectory);
		serial_num = p_resource_serial->NumberOfNamedEntries+p_resource_serial->NumberOfIdEntries;
		// printf("Debug: %d\n",serial_num);
		printCentered("RESOURCE ENTRY",40);

		for(size_t i=0;i<serial_num;i++)
		{
			p_resource_serial_entry = &((RESOURCE_DIRECTORY_ENTRY*)(&p_resource_serial[1]))[i];
			// unicode or id
			if(p_resource_serial_entry->NameIsString)
			{
				pstr = (RESOURCE_DIR_STRING_U*)((char*)p_resource_type+p_resource_serial_entry->NameOffset);
				memset(name,0,0x30);
				memcpy(name,pstr->NameString,pstr->Length*(sizeof(wchar_t)));

				// 代码页(字符集)
				p_resource_charset = (RESOURCE_DIRECTORY*)((char*)p_resource_type+p_resource_serial_entry->OffsetToDirectory);
				p_resource_charset_entry = (RESOURCE_DIRECTORY_ENTRY*)&p_resource_charset[1];
				p_resource_charset_entry_ = (RESOURCE_DATA_ENTRY*)((char*)p_resource_type+p_resource_charset_entry->OffsetToDirectory);

				wprintf(L"\tindex: %d\n\tid: null\n\tname: %s\n\tcode page: %d\n\tRVA: 0x%x\n\tFOA: 0x%x\n\tSize: 0x%x\n\n",i,name,
														p_resource_charset_entry_->CodePage,
														p_resource_charset_entry_->OffsetToData,
														(int)RVAtoFOA((__int64)p_resource_charset_entry_->OffsetToData,file_buffer),
														p_resource_charset_entry_->Size);
			}else{
				// 代码页(字符集)
				p_resource_charset = (RESOURCE_DIRECTORY*)((char*)p_resource_type+p_resource_serial_entry->OffsetToDirectory);
				p_resource_charset_entry = (RESOURCE_DIRECTORY_ENTRY*)&p_resource_charset[1];
				p_resource_charset_entry_ = (RESOURCE_DATA_ENTRY*)((char*)p_resource_type+p_resource_charset_entry->OffsetToDirectory);

				printf("\tindex: %d\n\tid: %d\n\tname: null\n\tcode page: %d\n\tRVA: 0x%x\n\tFOA: 0x%x\n\tSize: 0x%x\n\n",i,p_resource_serial_entry->NameOffset,
														p_resource_charset_entry_->CodePage,
														p_resource_charset_entry_->OffsetToData,
														(int)RVAtoFOA((__int64)p_resource_charset_entry_->OffsetToData,file_buffer),
														p_resource_charset_entry_->Size);
			}
		}
	}	
}



#endif // !defined(AFX_PETOOL2_H__5285B99C_CEF4_4CFF_B6E2_6687C068D54A__INCLUDED_)
