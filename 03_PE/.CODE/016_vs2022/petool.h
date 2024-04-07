
#if !defined(AFX_PETOOL_H__BC9AE6DA_8D52_4356_9802_921968C7E3DA__INCLUDED_)
#define AFX_PETOOL_H__BC9AE6DA_8D52_4356_9802_921968C7E3DA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//#include "petool2.h"

DWORD AutoAlign(DWORD x,DWORD alignNum)
{
	return x%alignNum==0?(x!=0?x:alignNum):(x/alignNum+1)*alignNum;
}

DWORD GetFileSize(char* fPath) {
	FILE* file = nullptr;
	fopen_s(&file,fPath, "rb");
    if (file != nullptr) 
	{
		fseek(file, 0, SEEK_END);
		DWORD size = ftell(file);
		fclose(file);

		return size;
	}else{
        printf("Failed to open file.\n\n");
        return -1;
    }
}


void WriteToFile(char* data, char* fPath, DWORD fileSize) {
	FILE* file = nullptr;
	fopen_s(&file,fPath, "wb");
    if (file != nullptr) {
       		
		fseek(file,0,SEEK_SET);
		DWORD result=fwrite(data, 1, fileSize, file);

		if(result==fileSize)
		{
			printf("Data Successfully written to %s.\n\n",fPath);
		}else{
			printf("Data Unsuccessfully written to %s.\n\n",fPath);
		}
		fclose(file);
    }else{
		printf("Failed to open %s.\n\n",fPath);
	}
}


char* ReadPE(char* pePath)
{
	char* buffer=nullptr;
	int i=0;
	FILE* fp=nullptr;
	fopen_s(&fp, pePath, "rb");
	if(fp!=nullptr)
	{
		i++;
		fseek(fp,0,SEEK_END);
		int size=ftell(fp);
		rewind(fp);
		buffer=(char*)malloc(sizeof(char)*size);
		if(buffer)
		{
			i++;
			DWORD result=fread(buffer,1,size,fp);
			if(result==(unsigned int)size){
				i++;
			}
		}
	}

	switch(i)
	{
		case 0:
			printf("%s file do't exist.\n",pePath);
			break;
		case 1:
			printf("memory not enough.\n");
			fclose(fp);
			break;
		case 2:
			printf("read pe fail.\n");
			free(buffer);
			fclose(fp);
			break;

		default:
			fclose(fp);
			return buffer;
	}

	return nullptr;
}


DOS_HEADER* GetDosHeadersPtr(char* buffer)
{
	struct DOS_HEADER* p=(struct DOS_HEADER*)buffer;
	if(p)
	{
		return p;
	}else{
		return nullptr;
	}
}


NT_HEADERS* GetNtHeadersPtr(char* buffer)
{
	struct NT_HEADERS* p=(struct NT_HEADERS*)&buffer[((struct DOS_HEADER*)buffer)->e_lfanew];
	if(p)
	{
		return p;
	}else{
		return nullptr;
	}
}


FILE_HEADER* GetFileHeadersPtr(char* buffer)
{
	struct FILE_HEADER* p=(struct FILE_HEADER*)&((struct NT_HEADERS*)&buffer[((struct DOS_HEADER*)buffer)->e_lfanew])->FileHeader;
	if(p)
	{
		return p;
	}else{
		return nullptr;
	}
}

OPTIONAL_HEADER* GetPEOptionalHeadersPtr(char* buffer)
{
	struct OPTIONAL_HEADER* p=(struct OPTIONAL_HEADER*)&((struct NT_HEADERS*)&buffer[((struct DOS_HEADER*)buffer)->e_lfanew])->OptionalHeader;

	if(p)
	{
		return p;
	}else{
		return nullptr;
	}
}


OPTIONAL_HEADER64* GetOptionalHeadersPtr(char* buffer)
{
	struct OPTIONAL_HEADER64* p=(struct OPTIONAL_HEADER64*)&((struct NT_HEADERS64*)&buffer[((struct DOS_HEADER*)buffer)->e_lfanew])->OptionalHeader;
	if(p)
	{
		return p;
	}else{
		return nullptr;
	}
}



// SECTION_HEADER
SECTION_HEADER* GetPE_sectionPtr(char* buffer,int index)
{
	int section_num=GetFileHeadersPtr(buffer)->NumberOfSections;	
	int section_size=40;
	if(index<section_num)
	{
		return (struct SECTION_HEADER*)&buffer[GetDosHeadersPtr(buffer)->e_lfanew+(4+20+GetFileHeadersPtr(buffer)->SizeOfOptionalHeader)+index*section_size];
	}else{
		return nullptr;
	}
}

// IMAGE_BUFFER
char* FileBufferToImageBuffer(char* p)
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

	char* Img_buffer=nullptr;
	if(p)
	{
		log_++;
		// x86,x86-64
		if(p_stdpe->Machine==0x14c)
		{
			img_size=GetPEOptionalHeadersPtr(p)->SizeOfImage;
			headers_size=GetPEOptionalHeadersPtr(p)->SizeOfHeaders;
		}else{
			img_size=GetOptionalHeadersPtr(p)->SizeOfImage;
			headers_size=GetOptionalHeadersPtr(p)->SizeOfHeaders;
		}
			Img_buffer=(char*)malloc(sizeof(char)*img_size);	

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
				p_section=GetPE_sectionPtr(p,i);
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
			printf("get a nullptr.\n");
			break;
		case 1:
			printf("memory not enough.\n");
			break;
		default:
			return Img_buffer;
	}

	return nullptr;

}

char* ImageBufferToFileBuffer(char* p)
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

	char* file_buffer=nullptr;
	if(p)
	{
		log_++;
		// x86,x86-64
		if(p_stdpe->Machine==0x14c)
		{

			headers_size=GetPEOptionalHeadersPtr(p)->SizeOfHeaders;
		}else{
			headers_size=GetOptionalHeadersPtr(p)->SizeOfHeaders;
		}
		
		p_section=GetPE_sectionPtr(p,section_num-1);
		file_size=p_section->PointerToRawData+p_section->SizeOfRawData;
		file_buffer=(char*)malloc(sizeof(char)*file_size);		

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
				p_section=GetPE_sectionPtr(p,i);
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
			printf("get a nullptr.\n");
			break;
		case 1:
			printf("memory not enough.\n");
			break;
		default:
			return file_buffer;
	}

	return nullptr;
}


// rva=内存中相对偏地址
ULONG64 RVAtoFOA(ULONG64 rva,char* p)
{
	if(p==nullptr)
	{
		printf("get a nullptr.\n");
		return 0;
	}
	ULONG64 img_base;
	int virtual_address;
	int file_address;
	ULONG64 roa;

	int section_num=GetFileHeadersPtr(p)->NumberOfSections;
	int i=0;

	// rva< VirtualAddress of first section 
	if(rva<GetPE_sectionPtr(p,i)->VirtualAddress)
	{
		return rva;
	}

	if(GetFileHeadersPtr(p)->Machine==0x14c)
	{
		img_base=(ULONG64)GetPEOptionalHeadersPtr(p)->ImageBase;
	}else{
		img_base=GetOptionalHeadersPtr(p)->ImageBase;
	}

	// 确定内存中偏移
	//roa=rva-img_base;
	roa=rva;

	// 确定所在节
	for(i=0;i<section_num;i++)
	{
		// vc++6.0 ULONG64类型变量作比较存在问题
		if((DWORD)roa>=(DWORD)GetPE_sectionPtr(p,i)->VirtualAddress && (DWORD)roa<(DWORD)(GetPE_sectionPtr(p,i)->VirtualAddress+GetPE_sectionPtr(p,i)->Misc.VirtualSize))
		{
			virtual_address=GetPE_sectionPtr(p,i)->VirtualAddress;
			file_address=GetPE_sectionPtr(p,i)->PointerToRawData;
			
			//  确定相对于节的偏移
			roa=roa-virtual_address;
			return roa+file_address;		
		}
	}
	return 0;
}

ULONG64 FOAtoRVA(ULONG64 rva,char* p)
{
	if(p==nullptr)
	{
		printf("get a nullptr.\n");
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
		img_base=(ULONG64)GetPEOptionalHeadersPtr(p)->ImageBase;
	}else{
		img_base=GetOptionalHeadersPtr(p)->ImageBase;
	}

	// 确定内存中偏移
	//roa=rva-img_base;
	roa=rva;

	// 确定所在节
	for(i=0;i<section_num;i++)
	{
		// vc++6.0 ULONG64类型变量作比较存在问题
		if((DWORD)roa>=(DWORD)GetPE_sectionPtr(p,i)->PointerToRawData && (DWORD)roa<(DWORD)(GetPE_sectionPtr(p,i)->PointerToRawData+GetPE_sectionPtr(p,i)->SizeOfRawData))
		{
			virtual_address=GetPE_sectionPtr(p,i)->VirtualAddress;
			file_address=GetPE_sectionPtr(p,i)->PointerToRawData;
			
			//  确定相对于节的偏移
			roa=roa-file_address;
			return roa+virtual_address;		
		}
	}
	return 0;
}



DWORD GetFileBufferSize(char* file_buffer)
{
	if(file_buffer==nullptr)
	{
		printf("Get a nullptr.\n");
		return 0;
	}

	FILE_HEADER* p_stdpe=GetFileHeadersPtr(file_buffer);
	int section_num=p_stdpe->NumberOfSections;
	SECTION_HEADER* p_section=GetPE_sectionPtr(file_buffer,section_num-1);

	return p_section->PointerToRawData+p_section->SizeOfRawData;	// 可能不准确

}


void AddMessageBoxToSection(char* pbuffer,char* messageBox_title,char* messageBox_content)
{

	if(!pbuffer)
	{
		printf("get a nullptr.\n");
		exit(0);
	}
	
	SECTION_HEADER* p_section=nullptr;
	int section_num=GetFileHeadersPtr(pbuffer)->NumberOfSections;
	int virtual_address=0;
	int size_rawdata=0;
	int pointer_rawdata=0;
	int p_MessageBox=(int)MessageBox;
	int foa=0;
	char* p=nullptr;

	int title_rva=0;
	int title_len=strlen(messageBox_title)+1;


	int content_rva=0;
	int content_len=strlen(messageBox_content)+1;
	
	unsigned char shellcode[24]={
			0x6a,0x00,
			0x68,0x00,0x00,0x00,0x00,
			0x68,0x00,0x00,0x00,0x00,
			0x6a,0x00,
			0xe8,0x00,0x00,0x00,0x00,		// 绝对地址
			0xe9,0x00,0x00,0x00,0x00
	};
	int oep=GetPEOptionalHeadersPtr(pbuffer)->AddressOfEntryPoint;
	int img_base=GetPEOptionalHeadersPtr(pbuffer)->ImageBase;
	int e8_rva=0;
	int e8_x=0;
	int e9_rva=0;
	int e9_x=0;

	p_section = GetPE_sectionPtr(pbuffer,0);
	virtual_address=p_section->VirtualAddress;
	size_rawdata=p_section->SizeOfRawData;
	pointer_rawdata=p_section->PointerToRawData;
	foa=size_rawdata+pointer_rawdata;
	
	p=&pbuffer[foa-1-title_len-content_len-24];  //  两个\0

	// copy title and content
	int i=0;
	while(p[i]=messageBox_title[i++]);
	int j=0;
	while(p[i++]=messageBox_content[j++]);

	// deal shellcode
	title_rva=foa-1-title_len-content_len-24-pointer_rawdata; // 得到相对于节的偏移
	title_rva=title_rva+virtual_address; // 得到在内存中的偏移
	title_rva+=img_base;				// 得到虚拟内存地址
	content_rva=foa-1-content_len-24-pointer_rawdata;
	content_rva=content_rva+virtual_address;
	content_rva+=img_base;
	// printf("%x,%x\n",title_rva,content_rva);

	((int*)(&shellcode[3]))[0]=title_rva;
	((int*)(&shellcode[8]))[0]=content_rva;
	// printf("%x,%x\n",shellcode[3],shellcode[4]);

	// deal e8
	e8_rva=foa-1-10-pointer_rawdata;
	e8_rva=e8_rva+virtual_address;
	e8_rva+=img_base;
	e8_x=p_MessageBox-e8_rva-5;
	//printf("%x\n",e8_x);
	((int*)(&shellcode[15]))[0]=e8_x;
	e9_rva=foa-1-5+-pointer_rawdata;
	e9_rva=e9_rva+virtual_address;
	e9_rva+=img_base;
	e9_x=img_base+oep-e9_rva-5;
	//printf("%x,%x\n",e8_x,e9_x);
	((int*)(&shellcode[20]))[0]=e9_x;

	p=&pbuffer[foa-1-24];
	for(i=0;i<24;i++)
	{
		p[i]=shellcode[i];
	}
	
	// deal oep
	foa=foa-1-24-pointer_rawdata;
	foa=foa+virtual_address;
	GetPEOptionalHeadersPtr(pbuffer)->AddressOfEntryPoint=foa;
}


// Add new section
int AddNewSection(char** fbuffer,DWORD file_size,SECTION_HEADER* new_section_head,char* new_section_buffer,int new_section_size)
{
	if(!fbuffer[0])
	{
		printf("Get a nullptr.\n");
		exit(0);
	}

	char* new_fbuffer=nullptr;
	int log_code=0;
	DOS_HEADER* p_dos=GetDosHeadersPtr(fbuffer[0]);
	int dos_e_lfanew=p_dos->e_lfanew;
	SECTION_HEADER* p_section=nullptr;
	int section_num=GetFileHeadersPtr(fbuffer[0])->NumberOfSections;
	int img_size=0;
	// DWORD file_size=GetPE_sectionPtr(fbuffer[0],section_num-1)->PointerToRawData+GetPE_sectionPtr(fbuffer[0],section_num-1)->SizeOfRawData;
	// 使用最后的节的PointerToRawData+SizeOfRawData可能不准确,file_size
	int headers_size=0;
	int i=0;
	int file_align=0;
	int section_align=0;
	if(GetFileHeadersPtr(fbuffer[0])->Machine==0x14c)
	{
		img_size=GetPEOptionalHeadersPtr(fbuffer[0])->SizeOfImage;
		headers_size=GetPEOptionalHeadersPtr(fbuffer[0])->SizeOfHeaders;
		file_align=GetPEOptionalHeadersPtr(fbuffer[0])->FileAlignment;
		section_align=GetPEOptionalHeadersPtr(fbuffer[0])->SectionAlignment;
	}else{
		img_size=GetOptionalHeadersPtr(fbuffer[0])->SizeOfImage;
		headers_size=GetOptionalHeadersPtr(fbuffer[0])->SizeOfHeaders;
		file_align=GetOptionalHeadersPtr(fbuffer[0])->FileAlignment;
		section_align=GetOptionalHeadersPtr(fbuffer[0])->SectionAlignment;
	}
	p_section=GetPE_sectionPtr(fbuffer[0],section_num-1)+1;
	//printf("%d\n",(int)p_section-(int)fbuffer[0]);
	
	// 节表后有多余空间
	if(headers_size-(int)p_section+(int)fbuffer[0]>80)
	{
		log_code++;
		//printf("%d\n",headers_size-(int)p_section+(int)fbuffer[0]);

		// 判断是否为80个空数据
		for(i=0;i<80;i++)
		{
			if(((char*)p_section)[i]!=0){
				break;
			}
		}
		if(i==80)
		{
			log_code++;
			p_section-=1;

			strcpy((char*)new_section_head->Name,".new");
			new_section_head->Misc.VirtualSize=new_section_size;
			new_section_head->VirtualAddress=img_size;
			// 确保按文件对齐
			new_section_head->SizeOfRawData=AutoAlign(new_section_size,file_align);
			new_section_head->PointerToRawData=p_section->PointerToRawData+p_section->SizeOfRawData;
			new_section_head->Characteristics=0x60000020;

			p_section+=1;

			// 添加新的节表
			memcpy((char*)p_section,(char*)new_section_head,40);

			// 修改NumberOfSections,SizeOfImage
			GetFileHeadersPtr(fbuffer[0])->NumberOfSections+=1;
			if(GetFileHeadersPtr(fbuffer[0])->Machine==0x14c)
			{
				GetPEOptionalHeadersPtr(fbuffer[0])->SizeOfImage=img_size+AutoAlign(new_section_size,section_align);
			}else{
				GetOptionalHeadersPtr(fbuffer[0])->SizeOfImage=img_size+AutoAlign(new_section_size,section_align);
			}

			// 添加新的节
			new_fbuffer=(char*)malloc(file_size+new_section_size);
			if(new_fbuffer)
			{
				log_code++;
				// printf("%d\n",file_size+new_section_size);
				memcpy(new_fbuffer,fbuffer[0],file_size);
				memcpy(&new_fbuffer[file_size],new_section_buffer,new_section_size);
				free(fbuffer[0]);			
				fbuffer[0]=new_fbuffer;
			
			}	
		}
	}

	switch(log_code)
	{
		case 0:
			printf("PE headers not 80 Byte.\n");
			break;
		case 1:
			printf("Not all 0 in 80 Byte.\n");

			// 判断dos头后的垃圾数据大于80字节，则将nt头到节表的数据向前填充
			if(dos_e_lfanew-64>80)
			{
				for(i=0;i<(int)p_section-(int)fbuffer[0]-dos_e_lfanew;i++)
				{
					fbuffer[0][i+64]=fbuffer[0][i+dos_e_lfanew];
					fbuffer[0][i+dos_e_lfanew]=0;
				}

				p_dos->e_lfanew=64;
				printf("\nRecursive call Add_NewSection().\n");
				AddNewSection(fbuffer,file_size,new_section_head,new_section_buffer,new_section_size);
				printf("\n");
				printf("Dos_stub is greater than 80 Byte.\n");
				return 1;
			}
			printf("\tFail add section header\n");
			break;
		case 2:
			printf("new_fbuffer Don't get enough memory.\n");
			break;

		default:
			printf("Written new section into FileBuffer.\n");
			return 1;
	}
	return 0;
}


// Expand Section
int ExpandSection(char* img_buffer,char* new_section,DWORD new_section_size_,OUT char** new_img_buffer)
{
	if(img_buffer==nullptr)
	{
		printf("Get a nullptr.\n");
		return 0;
	}

	int file_align=0;
	int section_align=0;
	DWORD img_size=0;
	int section_num=0;
	DWORD rawdata_size=0;
	DWORD virtual_size=0;
	DWORD new_section_size=new_section_size_;

	FILE_HEADER* p_stdpe=GetFileHeadersPtr(img_buffer);
	OPTIONAL_HEADER* p_optpe=nullptr;
	OPTIONAL_HEADER64* p_optpe64=nullptr;
	SECTION_HEADER* p_section=nullptr;

	section_num=p_stdpe->NumberOfSections;

	if(p_stdpe->Machine==0x14c)
	{
		p_optpe=GetPEOptionalHeadersPtr(img_buffer);
		file_align=p_optpe->FileAlignment;
		section_align=p_optpe->SectionAlignment;
		img_size=p_optpe->SizeOfImage;
	}else{
		p_optpe64=GetOptionalHeadersPtr(img_buffer);
		file_align=p_optpe64->FileAlignment;
		section_align=p_optpe64->SectionAlignment;
		img_size=p_optpe64->SizeOfImage;
	}

	p_section=GetPE_sectionPtr(img_buffer,section_num-1);
	rawdata_size=p_section->SizeOfRawData;
	virtual_size=p_section->Misc.VirtualSize;
	
	new_section_size+=virtual_size>rawdata_size?AutoAlign(virtual_size,section_align):rawdata_size;
	new_section_size=AutoAlign(AutoAlign(new_section_size,section_align),file_align);
	p_section->SizeOfRawData=new_section_size;
	p_section->Misc.VirtualSize=new_section_size;
	p_section->Characteristics=0x60000020;

	if(p_stdpe->Machine==0x14c)
	{
		p_optpe->SizeOfImage=p_section->VirtualAddress+new_section_size;
		
	}else{
		p_optpe64->SizeOfImage=p_section->VirtualAddress+new_section_size;
	}

	// new image buffer
	new_img_buffer[0]=(char*)malloc(p_section->VirtualAddress+new_section_size);
	if(new_img_buffer)
	{
		memcpy(new_img_buffer[0],img_buffer,img_size);
		memcpy(&new_img_buffer[0][img_size],new_section,new_section_size_);
		return 1;
	}

	return 0;

}



// Conbine Section Header
int CombineSection(char* img_buffer)
{
	if(img_buffer==0)
	{
		printf("Get a nullptr;\n");
		return 0;
	}

	DWORD file_align=0;
	DWORD section_align=0;
	DWORD max_size=0;
	DWORD headers_size=0;
	DWORD virtual_size=0;
	DWORD rawdata_size=0;
	int section_num=0;
	FILE_HEADER* p_stdpe=nullptr;
	OPTIONAL_HEADER* p_optpe=nullptr;
	OPTIONAL_HEADER64* p_optpe64=nullptr;
	SECTION_HEADER* p_section_1=nullptr;
	SECTION_HEADER* p_section_2=nullptr;
	
	p_stdpe=GetFileHeadersPtr(img_buffer);
	section_num=p_stdpe->NumberOfSections;
	p_section_1=GetPE_sectionPtr(img_buffer,0);
	p_section_2=GetPE_sectionPtr(img_buffer,section_num-1);
	virtual_size=p_section_2->Misc.VirtualSize;
	rawdata_size=p_section_2->SizeOfRawData;

	if(p_stdpe->Machine==0x14c)
	{
		p_optpe=GetPEOptionalHeadersPtr(img_buffer);
		headers_size=p_optpe->SizeOfHeaders;
		file_align=p_optpe->FileAlignment;
		section_align=p_optpe->SectionAlignment;
		
	}else{
		p_optpe64=GetOptionalHeadersPtr(img_buffer);
		headers_size=p_optpe64->SizeOfHeaders;
		file_align=p_optpe64->FileAlignment;
		section_align=p_optpe64->SectionAlignment;
	}
	max_size=AutoAlign(virtual_size,section_align);

	p_section_1->Misc.VirtualSize=p_section_2->VirtualAddress+max_size-AutoAlign(headers_size,section_align);
	p_section_1->Misc.VirtualSize=AutoAlign(p_section_1->Misc.VirtualSize,section_align);
	p_section_1->SizeOfRawData=p_section_1->Misc.VirtualSize;
	p_section_1->Characteristics=0xffffffff;
	p_stdpe->NumberOfSections=1;
	
	return 1;
}


// Directory Entry
DATA_DIRECTORY* GetPE_DirectoryPtr(char* file_buffer,int index)
{
	if(file_buffer==nullptr)
	{
		printf("Get a nullptr.\n");
		return nullptr;
	}
	
	FILE_HEADER* p_stdpe=nullptr;
	OPTIONAL_HEADER* p_optpe=nullptr;
	OPTIONAL_HEADER64* p_optpe64=nullptr;

	p_stdpe=GetFileHeadersPtr(file_buffer);
	if(p_stdpe->Machine==0x14c)
	{
		p_optpe=GetPEOptionalHeadersPtr(file_buffer);
		return &p_optpe->DataDirectory[index];
	}else{
		p_optpe64=GetOptionalHeadersPtr(file_buffer);
		return &p_optpe64->DataDirectory[index];
	}
}



// GetFunctionAddrByName(FileBuffer指针，函数名字指针)
char* getFunAddrByName(char* file_buffer,char* fun_name)
{
	if(file_buffer==nullptr) 
	{
		printf("get a nullptr.\n");
		return nullptr;
	}
	DATA_DIRECTORY* p_directory=nullptr;
	EXPORT_DIRECTORY* p_export=nullptr;

	p_directory=GetPE_DirectoryPtr(file_buffer,0);
	p_export=(EXPORT_DIRECTORY*)(&file_buffer[RVAtoFOA(p_directory->VirtualAddress,file_buffer)]);

	if(!p_export)
	{
		printf("RVAtoFOA UNsuccessfal.\n");
		return nullptr;
	}
	
	int fun_num=p_export->NumberOfFunctions;
	int name_num=p_export->NumberOfNames;
	void** ptr_fun_addr=(void**)(&file_buffer[RVAtoFOA(p_export->AddressOfFunctions,file_buffer)]);
	char** ptr_name_addr=(char**)(&file_buffer[RVAtoFOA(p_export->AddressOfNames,file_buffer)]);
	short* ptr_ord_addr=(short*)(&file_buffer[RVAtoFOA(p_export->AddressOfNameOrdinals,file_buffer)]);

	// printf("debug: %s\n",fun_name);
	for(int i=0;i<name_num;i++)
	{
		// printf("debug: %s\n",&file_buffer[RVAtoFOA((ULONG64)ptr_name_addr[i],file_buffer)]);
		if(strcmp(&file_buffer[RVAtoFOA((ULONG64)ptr_name_addr[i],file_buffer)],fun_name)==0)
		{
			// printf("debug: 0x%08x\n",RVAtoFOA((ULONG64)ptr_fun_addr[ptr_ord_addr[i]],file_buffer));
			return (char*)(&file_buffer[RVAtoFOA((ULONG64)ptr_fun_addr[ptr_ord_addr[i]],file_buffer)]);
		}
		
	}
	return nullptr;

}



// GetFunAddrByOrdinals(FileBuffer指针，函数名导出序号)
char* GetFunAddrByOrdinal(char* file_buffer,int fun_ord)
{
	if(file_buffer==nullptr) 
	{
		printf("get a nullptr.\n");
		return nullptr;
	}
	DATA_DIRECTORY* p_directory=nullptr;
	EXPORT_DIRECTORY* p_export=nullptr;

	p_directory=GetPE_DirectoryPtr(file_buffer,0);
	p_export=(EXPORT_DIRECTORY*)(&file_buffer[RVAtoFOA(p_directory->VirtualAddress,file_buffer)]);

	if(!p_export)
	{
		printf("RVAtoFOA UNsuccessfal.\n");
		return nullptr;
	}
	
	int ord_base=p_export->Base;
	int fun_num=p_export->NumberOfFunctions;
	int name_num=p_export->NumberOfNames;
	void** ptr_fun_addr=(void**)(&file_buffer[RVAtoFOA(p_export->AddressOfFunctions,file_buffer)]);
	char** ptr_name_addr=(char**)(&file_buffer[RVAtoFOA(p_export->AddressOfNames,file_buffer)]);
	short* ptr_ord_addr=(short*)(&file_buffer[RVAtoFOA(p_export->AddressOfNameOrdinals,file_buffer)]);

	// printf("debug: %s\n",fun_ord);
	fun_ord-=ord_base;
	if(fun_ord<fun_num)
	{
		return (char*)(&file_buffer[RVAtoFOA((ULONG64)ptr_fun_addr[fun_ord],file_buffer)]);
	}
	return nullptr;

}


// 文件大小有可能大于最后一个节的文件偏移+其大小
int GetExtraSize(char* fpath,char* p)
{
	if(p==nullptr)
	{
		printf("isPEExtra: p is a nullptr.\n");
		return 0;
	}
	DWORD file_size_1=GetFileSize(fpath);
	DWORD file_size_2=0;
	int section_num=GetFileHeadersPtr(p)->NumberOfSections;
	SECTION_HEADER* p_section=GetPE_sectionPtr(p,section_num-1);
	file_size_2=p_section->PointerToRawData+p_section->SizeOfRawData;
	printf("getPEExtraSize: %d\n",(file_size_1-file_size_2));
	return (file_size_1-file_size_2)>0?(file_size_1-file_size_2):-1;
	
}


// move export table
int MoveExportTable(char* file_buffer,char** new_section,DWORD* new_section_size)
{
	if(file_buffer==nullptr)
	{
		printf("moveExportTable: file_buffer is a nullptr.\n");
		return 0;
	}

	int fun_num=0;
	int name_num=0;
	char* fun_addr=nullptr;
	char** name_addr=nullptr;
	DWORD name_total_size=0;
	DWORD export_table_size=40;
	short* ord_addr=nullptr;
	
	int section_align=0;
	int file_align=0;
	if(GetFileHeadersPtr(file_buffer)->Machine==0x14c)
	{
		section_align=GetPEOptionalHeadersPtr(file_buffer)->SectionAlignment;
		file_align=GetPEOptionalHeadersPtr(file_buffer)->FileAlignment;
	}else{
		section_align=GetOptionalHeadersPtr(file_buffer)->SectionAlignment;
		file_align=GetOptionalHeadersPtr(file_buffer)->FileAlignment;
	}
	int section_num=GetFileHeadersPtr(file_buffer)->NumberOfSections;
	SECTION_HEADER* p_section=GetPE_sectionPtr(file_buffer,section_num-1);
	DATA_DIRECTORY* p_directory=GetPE_DirectoryPtr(file_buffer,0); 
	EXPORT_DIRECTORY* p_export=(EXPORT_DIRECTORY*)(&file_buffer[RVAtoFOA((ULONG64)(p_directory->VirtualAddress),file_buffer)]);

	fun_num=p_export->NumberOfFunctions;
	name_num=p_export->NumberOfNames;
	fun_addr=(&file_buffer[RVAtoFOA((ULONG64)p_export->AddressOfFunctions,file_buffer)]);
	name_addr=(char**)(&file_buffer[RVAtoFOA((ULONG64)p_export->AddressOfNames,file_buffer)]);
	ord_addr=(short*)(&file_buffer[RVAtoFOA((ULONG64)p_export->AddressOfNameOrdinals,file_buffer)]);
	
	//printf("debug: %x %s(0x%08x) %d\n",fun_addr[0],&file_buffer[RVAtoFOA((ULONG64)name_addr[0],file_buffer)],name_addr[0],ord_addr[0]);
	
	// 统计总函数名字大小
	for(int i=0;i<name_num;i++)
	{
		name_total_size+=(strlen(&file_buffer[RVAtoFOA((ULONG64)name_addr[i],file_buffer)])+1);  // 加上\0
	}
	// 拼接函数名
	char* name_total=(char*)malloc(name_total_size);
	if(name_total==nullptr)
	{
		printf("moveExportTable: name_total is a nullptr.\n");
		return 0;
	}
	memset(name_total,0,name_total_size);
	name_total_size=0;
	for(int i=0;i<name_num;i++)
	{
		strcat(name_total,&file_buffer[RVAtoFOA((ULONG64)name_addr[i],file_buffer)]);
		sprintf(name_total,"%s ",name_total);
		name_addr[i]=(char*)(p_section->VirtualAddress+AutoAlign(p_section->Misc.VirtualSize,section_align)+fun_num*4+name_num*4+name_num*2+name_total_size);
		name_total_size=strlen(name_total);  // 加上\0
	}
	int i=0;
	while((DWORD)i<name_total_size)
	{
		if(name_total[i]==' ') name_total[i]='\0';
		i++;
	}
	
	new_section_size[0]=fun_num*4+name_num*4+name_num*2+name_total_size+export_table_size;
	new_section_size[0]=AutoAlign(AutoAlign(new_section_size[0],section_align),file_align);
	new_section[0] = (char*)malloc(new_section_size[0]);
	if(new_section[0]==nullptr)
	{
		printf("moveExportTable: new_section malloc fail.\n");
		return 0;
	}

	// copy Functions table & modify AddressOfFunctions
	memcpy(new_section[0],fun_addr,fun_num*4);
	p_export->AddressOfFunctions=p_section->VirtualAddress+AutoAlign(p_section->Misc.VirtualSize,section_align);
	// copy NameOrdinals talbe & modify AddressOfNameOrdinals
	memcpy(&new_section[0][fun_num*4],ord_addr,name_num*2);
	p_export->AddressOfNameOrdinals=p_section->VirtualAddress+AutoAlign(p_section->Misc.VirtualSize,section_align)+fun_num*4;
	// copy AddressOfNames table & modify name address
	memcpy(&new_section[0][fun_num*4+name_num*2],name_addr,name_num*4);

	memcpy((char*) & new_section[0][fun_num * 4 + name_num * 2 + name_num * 4], (char*)name_total, name_total_size);
	p_export->AddressOfNames=p_section->VirtualAddress+AutoAlign(p_section->Misc.VirtualSize,section_align)+fun_num*4+name_num*2;
	// copy export table
	memcpy(&new_section[0][fun_num*4+name_num*2+name_num*4+name_total_size],(char*)p_export,40);
	p_directory->VirtualAddress=p_section->VirtualAddress+AutoAlign(p_section->Misc.VirtualSize,section_align)+fun_num*4+name_num*2+name_num*4+name_total_size;

	//printf(":%x\n",p_directory->VirtualAddress);
	/*
	name_total_size=0;
	for(i=0;i<fun_num;i++)
	{
		// printf("%08x\n",((int*)new_section[0])[i]);
		// printf("%d\n",(WORD)new_section[0][fun_num*4+i*2]);
		
		printf("%08x\n",name_addr[i]);
		// printf("%08X\n",(DWORD)new_section[0][fun_num*4+name_num*2+i*4]); 无法输出4字节数据? 以char*取数据得到1字节数据
		// printf("%08X\n",((DWORD*)&new_section[0][fun_num*4+name_num*2+i*4])[0]); // 应该以int*获取数据
		printf("%s\n",&new_section[0][fun_num*4+name_num*2+name_num*4+name_total_size]);
		name_total_size+=strlen(&new_section[0][fun_num*4+name_num*2+name_num*4+name_total_size])+1;
		
	}
	*/
	return 1;
}


// move relocation table
int MoveRelocationTable(char* file_buffer,char** new_section,DWORD* new_section_size)
{
	if(file_buffer==nullptr)
	{
		printf("moveExportTable: file_buffer is a nullptr.\n");
		return 0;
	}

	int section_align=0;
	int file_align=0;
	if(GetFileHeadersPtr(file_buffer)->Machine==0x14c)
	{
		section_align=GetPEOptionalHeadersPtr(file_buffer)->SectionAlignment;
		file_align=GetPEOptionalHeadersPtr(file_buffer)->FileAlignment;
	}else{
		section_align=GetOptionalHeadersPtr(file_buffer)->SectionAlignment;
		file_align=GetOptionalHeadersPtr(file_buffer)->FileAlignment;
	}
	int section_num=GetFileHeadersPtr(file_buffer)->NumberOfSections;
	SECTION_HEADER* p_section=GetPE_sectionPtr(file_buffer,section_num-1);
	DATA_DIRECTORY* p_directory=GetPE_DirectoryPtr(file_buffer,5); 
	BASE_RELOCATION* p_relocation=(BASE_RELOCATION*)(&file_buffer[RVAtoFOA((ULONG64)(p_directory->VirtualAddress),file_buffer)]);
	BASE_RELOCATION* p_relocation_=p_relocation;
	DWORD virtual_addr=0;
	DWORD block_size=0; 
	DWORD reloc_table_size=0;
	// int block_num=0;
	while(1)
	{
		virtual_addr=p_relocation_->VirtualAddress;
		block_size=p_relocation_->SizeOfBlock;
	
		if(virtual_addr==0&&block_size==0)
		{
			reloc_table_size+=8;
			break;
		}else{
			reloc_table_size+=block_size;
			p_relocation_=(BASE_RELOCATION*)((char*)p_relocation_+block_size);
		}

		
		
	}
	// printf("debug: %x\n",reloc_table_size);

	new_section_size[0]=AutoAlign(reloc_table_size,section_align);
	new_section[0]=(char*)malloc(new_section_size[0]);
	if(new_section[0]==nullptr)
	{
		printf("moveRelocationTable: new_section is a nullptr.\n");
		return 0;
	}
	memcpy((char*)new_section[0],(char*)p_relocation,reloc_table_size);
	
	p_directory->VirtualAddress=p_section->VirtualAddress+AutoAlign(p_section->Misc.VirtualSize,section_align);


	return 1;
}



// repair By Relocation Table
int repairByRelocaTable(char* file_buffer,DWORD old_img_base)
{
	if(file_buffer==nullptr)
	{
		printf("moveExportTable: file_buffer is a nullptr.\n");
		return 0;
	}

	DWORD current_img_base=0;
	int img_delta=0;
	FILE_HEADER* p_stdpe=GetFileHeadersPtr(file_buffer);
	OPTIONAL_HEADER* p_optpe=nullptr;
	// optpe64_head* p_optpe64=nullptr;

	if(p_stdpe->Machine=0x14c)
	{
		p_optpe=GetPEOptionalHeadersPtr(file_buffer);
		current_img_base=p_optpe->ImageBase;
		img_delta=current_img_base-old_img_base;
		
		printf("img_delta: %x.\n",img_delta);
	}else{
		printf("repairByRelocaTable: Not a x86 PE.\n");
		return 0;
	}
	
	DATA_DIRECTORY* p_directory=nullptr;
	BASE_RELOCATION* p_relocation=nullptr;
	BASE_RELOCATION* p_relocation_=nullptr;
	DWORD virtual_addr=0;
	DWORD block_size=0; 
	p_directory=GetPE_DirectoryPtr(file_buffer,5);  // 重定位表位于第6目录项
	p_relocation=(BASE_RELOCATION*)(&file_buffer[RVAtoFOA((ULONG64)p_directory->VirtualAddress,file_buffer)]);
	p_relocation_=p_relocation;

	int block_item=0;
	int block_item_num=0;
	int block_item_type=0;

	int i=0,
		j=0;
	while(1)//virtual_addr!=0&&block_size!=0
	{

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
				block_item_type=((WORD*)(&p_relocation_[1]))[j]>>12;  //  获取高4位的值，
				// 类型为 3 就修改绝对内存地址
				if(block_item_type==3)
				{
					((DWORD*)(&file_buffer[(int)RVAtoFOA((ULONG64)(block_item+virtual_addr),file_buffer)]))[0]+=img_delta;
				}
			}
		}else{
			break;
		}

		i++;
	}

	return 1;
}



#endif // !defined(AFX_PETOOL_H__BC9AE6DA_8D52_4356_9802_921968C7E3DA__INCLUDED_)