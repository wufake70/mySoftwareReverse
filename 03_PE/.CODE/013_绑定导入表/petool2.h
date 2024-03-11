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



// Check Export Directory
void checkExportDirectory(char* p)
{
	if(p==NULL) printf("get a NULL.\n");
	data_directory* p_directory=NULL;
	export_directory* p_export=NULL;

	char* strformat="_IMAGE_EXPORT_DIRECTORY{"
					"\n\t..."
					"\n\tName: 0x%08x(%s)"
					"\n\tBase: 0x%08X"
					"\n\tNumberOfFunctions: 0x%08X"
					"\n\tNumberOfNames: 0x%08X"
					"\n\tAddressOfFunctions: 0x%08X"
					"\n\tAddressOfNames: 0x%08X"
					"\n\tAddressOfNameOrdinals: 0x%08X\n}\n";

	p_directory=GetPE_DirectoryPtr(p,0);
	p_export=(export_directory*)(&p[RVAtoFOA(p_directory->VirtualAddress,p)]);

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
		printf("%d     | 0x%08X\t\t ",i,ptr_fun_addr[i]);
		if(i<name_num)
		{
			printf("| %d\t\t | %s\n",ptr_ord_addr[i],&p[RVAtoFOA((__int64)ptr_name_addr[i],p)]);
		}else{
			printf("| %s\t\t | %s\n","NULL","NULL");
		}
	}

	for(i;i<name_num;i++)
	{
		printf("%d     | 0x%08X\t\t | %d\t\t\t | ",i,NULL,NULL);
		printf("%s\n",&p[RVAtoFOA((__int64)ptr_name_addr[i],p)]);
	}
	
}



// CheckRelocationInfo
void CheckRelocationInfo(char* p)
{
	if(p==NULL)
	{
		printf("Get a NULL.\n");
		return ;
	}

	int i=0;
	int j=0;
	data_directory* p_directory=NULL;
	relocation_directory* p_relocation=NULL;
	relocation_directory* p_relocation_=NULL;
	size_t virtual_addr=0;
	size_t block_size=0; 
	char str[100];
	int block_item=0;
	int block_item_num=0;
	int block_item_type=0;
	
	p_directory=GetPE_DirectoryPtr(p,5);  // 重定位表位于第6目录项
	p_relocation=(relocation_directory*)(&p[RVAtoFOA((__int64)p_directory->VirtualAddress,p)]);
	p_relocation_=p_relocation;
	while(1)//virtual_addr!=0&&block_size!=0
	{
		sprintf(str,"[%dth Block]",i+1);
		printCentered(str,50);

		// 重定向表结构体并不连续，
		p_relocation_=(relocation_directory*)((char*)p_relocation_+block_size);
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
				printf("%d  %08X(rva)  %08X(foa)  %d(type)\n",j,block_item+virtual_addr,(int)RVAtoFOA((__int64)(block_item+virtual_addr),p),block_item_type);
			}
			system("pause>nul");
		}else{
			break;
		}

		i++;
	}
}



// Check Import table
void CheckIportTable(char* file_buffer)
{
	if(file_buffer==NULL)
	{
		printf("CheckIportTable: file_buffer is a NULL.\n");
		return ;
	}
	
	data_directory* p_directory=GetPE_DirectoryPtr(file_buffer,1);
	import_descriptor* p_import=(import_descriptor*)(&file_buffer[RVAtoFOA((__int64)p_directory->VirtualAddress,file_buffer)]);
	import_descriptor* p_import_=p_import;

	int import_num=0;
	char strformat[100];
	char* strformat_="_IMAGE_IMPORT_DESCRIPTOR{"
						"\n\tOriginalFirstThunk: 0x%08x"
						"\n\tTimeDateStamp: 0x%08x"
						"\n\tForwarderChain: 0x%08x"
						"\n\tName: %s(0x%08x)"
						"\n\tFirstThunk: 0x%08x\n}\n";
	int origin_thunk=0;
	int origin_num=0;
	int first_thunk=0;
	int first_num=0;
	thunk_data* p_thunk=NULL;
	thunk_data* p_thunk_=NULL;
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
			p_thunk=(thunk_data*)(&file_buffer[RVAtoFOA((__int64)p_import_->OriginalFirstThunk,file_buffer)]);
			while(p_thunk->u1.Ordinal)
			{
				//printf("\t%s:%d\n",(p_thunk->u1.Ordinal & 0x80000000)==1?"Ordinal",p_thunk->u1.Ordinal-0x80000000:"Name");
				(p_thunk->u1.Ordinal & 0x80000000)?
					printf("\tOrdinal: %d\n",
					p_thunk->u1.Ordinal - 0x80000000)
				:printf("\tName: %d-%s\n",
					((import_name*)(&file_buffer[RVAtoFOA((__int64)p_thunk->u1.AddressOfData,file_buffer)]))->Hint,
					((import_name*)(&file_buffer[RVAtoFOA((__int64)p_thunk->u1.AddressOfData,file_buffer)]))->Name);
				p_thunk++;
				
			}
			// FirstThunk	IAT
			printf("\t******** [ FirstThunk ] ********\n");
			p_thunk=(thunk_data*)(&file_buffer[RVAtoFOA((__int64)p_import_->FirstThunk,file_buffer)]);
			while(p_thunk->u1.Ordinal)
			{
				//printf("\t%s:%d\n",(p_thunk->u1.Ordinal & 0x80000000)==1?"Ordinal",p_thunk->u1.Ordinal-0x80000000:"Name");
				(p_thunk->u1.Ordinal & 0x80000000)?
					printf("\tOrdinal: %d\n",
					p_thunk->u1.Ordinal - 0x80000000)
				:((int)p_import_->TimeDateStamp==-1)?	// if timestamp==-1,pinrtf address ...
					printf("\tFun_Addr: 0x%x\n",p_thunk->u1.Function,p_thunk++)
					:printf("\tName: %d-%s\n",
						((import_name*)(&file_buffer[RVAtoFOA((__int64)p_thunk->u1.AddressOfData,file_buffer)]))->Hint,
						((import_name*)(&file_buffer[RVAtoFOA((__int64)p_thunk->u1.AddressOfData,file_buffer)]))->Name);
				p_thunk++;
				//system("pause>nul");
			}
			
			p_import_=(import_descriptor*)(((char*)p_import_)+20);
			// printf("debug: %x\n",p_import_);

			system("pause>nul");
		}else{
			printf("Over...\n");
			break;
		}
	}
}



// Check Bound Import
void CheckBoundImport(char* file_buffer)
{
	if(file_buffer==NULL)
	{
		printf("CheckBoundImport: file_buffer is a NULL.\n");
		return ;
	}

	data_directory* p_directory=GetPE_DirectoryPtr(file_buffer,11);
	if(p_directory->VirtualAddress==NULL)  // maybe 0x00000000
	{
		printf("CheckBoundImport: no Bound Import Table.\n");
		return ;
	}
	// bound import behind the section head
	bound_import* p_bound=(bound_import*)(&file_buffer[RVAtoFOA((__int64)p_directory->VirtualAddress,file_buffer)]);
	bound_import* p_bound_=p_bound;
	bound_ref* p_ref=NULL;
	int bound_ref_num=0;
	DWORD first_offset_name=p_directory->VirtualAddress;

	char* strformat="_IMAGE_BOUND_IMPORT_DESCRIPTOR{"
				"\n\tTimeDateStamp: %d(%s)"
				"\n\tOffsetModuleName: 0x%x(%s)"
				"\n\tNumberOfModuleForwarderRefs: %d\n}\n";

	char* strformat_="_IMAGE_BOUND_FORWARDER_REF{"
				"\n\tTimeDateStamp: %d(%s)"
				"\n\tOffsetModuleName: 0x%x(%s)"
				"\n\tReserved: %d\n}\n";
	// timestamp
	struct tm* time_info=NULL;
	char time_buffer[80];
	
	while(p_bound_->TimeDateStamp+p_bound_->OffsetModuleName+p_bound_->NumberOfModuleForwarderRefs)
	{
		time_info=localtime((const long*)&p_bound_->TimeDateStamp);
		strftime(time_buffer,sizeof(time_buffer),"%Y-%m-%d %H:%M:%S",time_info);

		printf(strformat,
				p_bound_->TimeDateStamp,time_buffer,
				p_bound_->OffsetModuleName,&file_buffer[p_bound_->OffsetModuleName+first_offset_name],
				p_bound_->NumberOfModuleForwarderRefs);

		bound_ref_num=p_bound_->NumberOfModuleForwarderRefs;
		for(int i=0;i<bound_ref_num;i++)
		{
			p_ref=(bound_ref*)(&p_bound_[i+1]);
			time_info=localtime((const long*)&p_ref->TimeDateStamp);
			strftime(time_buffer,sizeof(time_buffer),"%Y-%m-%d %H:%M:%S",time_info);
			
			printf(strformat_,
				p_ref->TimeDateStamp,time_buffer,
				p_ref->OffsetModuleName,&file_buffer[p_ref->OffsetModuleName+first_offset_name],
				p_ref->Reserved);
		}
		p_bound_=&p_bound_[bound_ref_num+1];
	}
}

#endif // !defined(AFX_PETOOL2_H__5285B99C_CEF4_4CFF_B6E2_6687C068D54A__INCLUDED_)
