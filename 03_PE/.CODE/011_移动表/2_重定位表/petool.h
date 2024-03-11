#if !defined(AFX_PETOOL_H__BC9AE6DA_8D52_4356_9802_921968C7E3DA__INCLUDED_)
#define AFX_PETOOL_H__BC9AE6DA_8D52_4356_9802_921968C7E3DA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//#include "petool2.h"

size_t AutoAlign(size_t x,size_t align_num)
{
	return x%align_num==0?(x!=0?x:align_num):(x/align_num+1)*align_num;
}

size_t GetFileSize(char* fpath) {
    FILE* file = fopen(fpath, "rb");
    if (file != NULL) 
	{
		fseek(file, 0, SEEK_END);
		size_t size = ftell(file);
		fclose(file);

		return size;
	}else{
        printf("Failed to open file.\n\n");
        return -1;
    }
}


void WriteToFile(char* data, char* fpath, size_t fileSize) {
    FILE* file = fopen(fpath, "wb");
    if (file != NULL) {
       		
		// printf("%d\n",fileSize);
		fseek(file,0,SEEK_SET);
		size_t result=fwrite(data, 1, fileSize, file);
		// printf("%d\n",result);

		if(result==fileSize)
		{
			printf("Data Successfully written to %s.\n\n",fpath);
		}else{
			printf("Data Unsuccessfully written to %s.\n\n",fpath);
		}
		fclose(file);
    }else{
		printf("Failed to open %s.\n\n",fpath);
	}
}


char* ReadPE(char* pepath)
{
	char* buffer=NULL;
	int i=0;
	FILE* fp=NULL;

	if(fp=fopen(pepath,"rb"))
	{
		i++;
		fseek(fp,0,SEEK_END);
		int size=ftell(fp);
		rewind(fp);
		buffer=(char*)malloc(sizeof(char)*size);
		if(buffer)
		{
			i++;
			size_t result=fread(buffer,1,size,fp);
			if(result==(unsigned int)size){
				i++;
			}
		}
	}

	switch(i)
	{
		case 0:
			printf("%s file do't exist.\n",pepath);
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
			printf("read pe successful.\n");
			//free(buffer);
			fclose(fp);
			return buffer;
	}

	return NULL;
}


dos_head* GetPE_DosPtr(char* buffer)
{
	struct dos_head* p=(struct dos_head*)buffer;
	if(p)
	{
		return p;
	}else{
		return NULL;
	}
}


nt_head* GetPE_NtPtr(char* buffer)
{
	struct nt_head* p=(struct nt_head*)&buffer[((struct dos_head*)buffer)->e_lfanew];
	if(p)
	{
		return p;
	}else{
		return NULL;
	}
}


stdpe_head* GetPE_StdPtr(char* buffer)
{
	struct stdpe_head* p=(struct stdpe_head*)&((struct nt_head*)&buffer[((struct dos_head*)buffer)->e_lfanew])->FileHeader;
	if(p)
	{
		return p;
	}else{
		return NULL;
	}
}

optpe_head* GetPE_OptPtr(char* buffer)
{
	struct optpe_head* p=(struct optpe_head*)&((struct nt_head*)&buffer[((struct dos_head*)buffer)->e_lfanew])->OptionalHeader;

	if(p)
	{
		return p;
	}else{
		return NULL;
	}
}


optpe64_head* GetPE_Opt64Ptr(char* buffer)
{
	struct optpe64_head* p=(struct optpe64_head*)&((struct nt64_head*)&buffer[((struct dos_head*)buffer)->e_lfanew])->OptionalHeader;
	if(p)
	{
		return p;
	}else{
		return NULL;
	}
}



// SECTION_HEADER
section_head* GetPE_sectionPtr(char* buffer,int index)
{
	int section_num=GetPE_StdPtr(buffer)->NumberOfSections;	
	int section_size=40;
	if(index<section_num)
	{
		return (struct section_head*)&buffer[GetPE_DosPtr(buffer)->e_lfanew+(4+20+GetPE_StdPtr(buffer)->SizeOfOptionalHeader)+index*section_size];
	}else{
		return NULL;
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
	
	stdpe_head* p_stdpe=GetPE_StdPtr(p);
	int section_num=p_stdpe->NumberOfSections;
	section_head* p_section=NULL;
	int virtual_address=0;
	int rawdata_size=0;
	int pointer_rawdata=0;

	char* Img_buffer=NULL;
	if(p)
	{
		log_++;
		// x86,x86-64
		if(p_stdpe->Machine==0x14c)
		{
			img_size=GetPE_OptPtr(p)->SizeOfImage;
			headers_size=GetPE_OptPtr(p)->SizeOfHeaders;
		}else{
			img_size=GetPE_Opt64Ptr(p)->SizeOfImage;
			headers_size=GetPE_Opt64Ptr(p)->SizeOfHeaders;
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

				/*printf("%08x %08x %08x \n",
						virtual_address,
						rawdata_size,
						pointer_rawdata);*/
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
			printf("get a NULL.\n");
			break;
		case 1:
			printf("memory not enough.\n");
			break;
		default:
			return Img_buffer;
	}

	return NULL;

}

char* ImageBufferToFileBuffer(char* p)
{
	int log_=0;
	int i=0;
	int j=0;
	int headers_size=0;
	
	stdpe_head* p_stdpe=GetPE_StdPtr(p);
	int section_num=p_stdpe->NumberOfSections;
	section_head* p_section=NULL;
	int virtual_address=0;
	int rawdata_size=0;
	int pointer_rawdata=0;
	int file_size=0;

	char* file_buffer=NULL;
	if(p)
	{
		log_++;
		// x86,x86-64
		if(p_stdpe->Machine==0x14c)
		{

			headers_size=GetPE_OptPtr(p)->SizeOfHeaders;
		}else{
			headers_size=GetPE_Opt64Ptr(p)->SizeOfHeaders;
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

				/*printf("%08x %08x %08x \n",
						virtual_address,
						rawdata_size,
						pointer_rawdata);*/
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
			printf("get a NULL.\n");
			break;
		case 1:
			printf("memory not enough.\n");
			break;
		default:
			return file_buffer;
	}

	return NULL;
}


// rva=内存中相对偏地址
__int64 RVAtoFOA(__int64 rva,char* p)
{
	if(p==NULL)
	{
		printf("get a NULL.\n");
		return 0;
	}
	__int64 img_base;
	int virtual_address;
	int file_address;
	__int64 roa;

	int section_num=GetPE_StdPtr(p)->NumberOfSections;
	int i=0;

	if(GetPE_StdPtr(p)->Machine==0x14c)
	{
		img_base=(__int64)GetPE_OptPtr(p)->ImageBase;
	}else{
		img_base=GetPE_Opt64Ptr(p)->ImageBase;
	}

	// 确定内存中偏移
	//roa=rva-img_base;
	roa=rva;

	// 确定所在节
	for(i=0;i<section_num;i++)
	{
		// vc++6.0 __int64类型变量作比较存在问题
		if((size_t)roa>=(size_t)GetPE_sectionPtr(p,i)->VirtualAddress && (size_t)roa<(size_t)(GetPE_sectionPtr(p,i)->VirtualAddress+GetPE_sectionPtr(p,i)->Misc.VirtualSize))
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

size_t getFileBufferSize(char* file_buffer)
{
	if(file_buffer==NULL)
	{
		printf("Get a NULL.\n");
		return 0;
	}

	stdpe_head* p_stdpe=GetPE_StdPtr(file_buffer);
	int section_num=p_stdpe->NumberOfSections;
	section_head* p_section=GetPE_sectionPtr(file_buffer,section_num-1);

	return p_section->PointerToRawData+p_section->SizeOfRawData;

}


void AddMessageBoxToSection(char* pbuffer,char* messageBox_title,char* messageBox_content)
{

	if(!pbuffer)
	{
		printf("get a NULL.\n");
		exit(0);
	}
	
	section_head* p_section=NULL;
	int section_num=GetPE_StdPtr(pbuffer)->NumberOfSections;
	int virtual_address=0;
	int size_rawdata=0;
	int pointer_rawdata=0;
	int p_MessageBox=(int)MessageBox;
	int foa=0;
	char* p=NULL;

	int title_rva=0;
	int title_len=strlen(messageBox_title)+1;


	int content_rva=0;
	int content_len=strlen(messageBox_content)+1;
	
	unsigned char shellcode[24]={
			0x6a,0x00,
			0x68,0x00,0x00,0x00,0x00,
			0x68,0x00,0x00,0x00,0x00,
			0x6a,0x00,
			0xe8,0x00,0x00,0x00,0x00,
			0xe9,0x00,0x00,0x00,0x00
	};
	int oep=GetPE_OptPtr(pbuffer)->AddressOfEntryPoint;
	int img_base=GetPE_OptPtr(pbuffer)->ImageBase;
	int e8_rva=0;
	int e8_x=0;
	int e9_rva=0;
	int e9_x=0;

	// 选择存放的节	
	/*
	for(int i=0;i<4;i++)
	{
		p_section = GetPE_sectionPtr(pbuffer,i);
		GetPEinfo_section(p_section);
	}
	*/
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
	GetPE_OptPtr(pbuffer)->AddressOfEntryPoint=foa;
}


// Add new section
int Add_NewSection(char** fbuffer,size_t file_size,section_head* new_section_head,char* new_section_buffer,int new_section_size)
{
	if(!fbuffer[0])
	{
		printf("Get a NULL.\n");
		exit(0);
	}

	char* new_fbuffer=NULL;
	int log_code=0;
	dos_head* p_dos=GetPE_DosPtr(fbuffer[0]);
	int dos_e_lfanew=p_dos->e_lfanew;
	section_head* p_section=NULL;
	int section_num=GetPE_StdPtr(fbuffer[0])->NumberOfSections;
	int img_size=0;
	// size_t file_size=GetPE_sectionPtr(fbuffer[0],section_num-1)->PointerToRawData+GetPE_sectionPtr(fbuffer[0],section_num-1)->SizeOfRawData;
	// 使用最后的节的PointerToRawData+SizeOfRawData可能不准确,file_size
	int headers_size=0;
	int i=0;
	int file_align=0;
	int section_align=0;
	if(GetPE_StdPtr(fbuffer[0])->Machine==0x14c)
	{
		img_size=GetPE_OptPtr(fbuffer[0])->SizeOfImage;
		headers_size=GetPE_OptPtr(fbuffer[0])->SizeOfHeaders;
		file_align=GetPE_OptPtr(fbuffer[0])->FileAlignment;
		section_align=GetPE_OptPtr(fbuffer[0])->SectionAlignment;
	}else{
		img_size=GetPE_Opt64Ptr(fbuffer[0])->SizeOfImage;
		headers_size=GetPE_Opt64Ptr(fbuffer[0])->SizeOfHeaders;
		file_align=GetPE_Opt64Ptr(fbuffer[0])->FileAlignment;
		section_align=GetPE_Opt64Ptr(fbuffer[0])->SectionAlignment;
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
			GetPE_StdPtr(fbuffer[0])->NumberOfSections+=1;
			if(GetPE_StdPtr(fbuffer[0])->Machine==0x14c)
			{
				GetPE_OptPtr(fbuffer[0])->SizeOfImage=img_size+new_section_size;
			}else{
				GetPE_Opt64Ptr(fbuffer[0])->SizeOfImage=img_size+new_section_size;
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
				Add_NewSection(fbuffer,file_size,new_section_head,new_section_buffer,new_section_size);
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
int expandSection(char* img_buffer,char* new_section,size_t new_section_size_,OUT char** new_img_buffer)
{
	if(img_buffer==NULL)
	{
		printf("Get a NULL.\n");
		return 0;
	}

	int file_align=0;
	int section_align=0;
	size_t img_size=0;
	int section_num=0;
	size_t rawdata_size=0;
	size_t virtual_size=0;
	size_t new_section_size=new_section_size_;

	stdpe_head* p_stdpe=GetPE_StdPtr(img_buffer);
	optpe_head* p_optpe=NULL;
	optpe64_head* p_optpe64=NULL;
	section_head* p_section=NULL;

	section_num=p_stdpe->NumberOfSections;

	if(p_stdpe->Machine==0x14c)
	{
		p_optpe=GetPE_OptPtr(img_buffer);
		file_align=p_optpe->FileAlignment;
		section_align=p_optpe->SectionAlignment;
		img_size=p_optpe->SizeOfImage;
	}else{
		p_optpe64=GetPE_Opt64Ptr(img_buffer);
		file_align=p_optpe64->FileAlignment;
		section_align=p_optpe64->SectionAlignment;
		img_size=p_optpe64->SizeOfImage;
	}

	p_section=GetPE_sectionPtr(img_buffer,section_num-1);
	rawdata_size=p_section->SizeOfRawData;
	virtual_size=p_section->Misc.VirtualSize;
	
	// modify section header,sizeofimage,Characteristics
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
int combineSection(char* img_buffer)
{
	if(img_buffer==0)
	{
		printf("Get a NULL;\n");
		return 0;
	}

	size_t file_align=0;
	size_t section_align=0;
	size_t max_size=0;
	size_t headers_size=0;
	size_t virtual_size=0;
	size_t rawdata_size=0;
	int section_num=0;
	stdpe_head* p_stdpe=NULL;
	optpe_head* p_optpe=NULL;
	optpe64_head* p_optpe64=NULL;
	section_head* p_section_1=NULL;
	section_head* p_section_2=NULL;
	
	p_stdpe=GetPE_StdPtr(img_buffer);
	section_num=p_stdpe->NumberOfSections;
	p_section_1=GetPE_sectionPtr(img_buffer,0);
	p_section_2=GetPE_sectionPtr(img_buffer,section_num-1);
	virtual_size=p_section_2->Misc.VirtualSize;
	rawdata_size=p_section_2->SizeOfRawData;

	if(p_stdpe->Machine==0x14c)
	{
		p_optpe=GetPE_OptPtr(img_buffer);
		headers_size=p_optpe->SizeOfHeaders;
		file_align=p_optpe->FileAlignment;
		section_align=p_optpe->SectionAlignment;
		
	}else{
		p_optpe64=GetPE_Opt64Ptr(img_buffer);
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
data_directory* GetPE_DirectoryPtr(char* file_buffer,int index)
{
	if(file_buffer==NULL)
	{
		printf("Get a NULL.\n");
		return NULL;
	}
	
	stdpe_head* p_stdpe=NULL;
	optpe_head* p_optpe=NULL;
	optpe64_head* p_optpe64=NULL;

	p_stdpe=GetPE_StdPtr(file_buffer);
	if(p_stdpe->Machine==0x14c)
	{
		p_optpe=GetPE_OptPtr(file_buffer);
		return &p_optpe->DataDirectory[index];
	}else{
		p_optpe64=GetPE_Opt64Ptr(file_buffer);
		return &p_optpe64->DataDirectory[index];
	}
}



// GetFunctionAddrByName(FileBuffer指针，函数名字指针)
char* getFunAddrByName(char* file_buffer,char* fun_name)
{
	if(file_buffer==NULL) 
	{
		printf("get a NULL.\n");
		return NULL;
	}
	data_directory* p_directory=NULL;
	export_directory* p_export=NULL;

	p_directory=GetPE_DirectoryPtr(file_buffer,0);
	p_export=(export_directory*)(&file_buffer[RVAtoFOA(p_directory->VirtualAddress,file_buffer)]);

	if(!p_export)
	{
		printf("RVAtoFOA UNsuccessfal.\n");
		return NULL;
	}
	
	int fun_num=p_export->NumberOfFunctions;
	int name_num=p_export->NumberOfNames;
	void** ptr_fun_addr=(void**)(&file_buffer[RVAtoFOA(p_export->AddressOfFunctions,file_buffer)]);
	char** ptr_name_addr=(char**)(&file_buffer[RVAtoFOA(p_export->AddressOfNames,file_buffer)]);
	short* ptr_ord_addr=(short*)(&file_buffer[RVAtoFOA(p_export->AddressOfNameOrdinals,file_buffer)]);

	// printf("debug: %s\n",fun_name);
	for(int i=0;i<name_num;i++)
	{
		// printf("debug: %s\n",&file_buffer[RVAtoFOA((__int64)ptr_name_addr[i],file_buffer)]);
		if(strcmp(&file_buffer[RVAtoFOA((__int64)ptr_name_addr[i],file_buffer)],fun_name)==0)
		{
			// printf("debug: 0x%08x\n",RVAtoFOA((__int64)ptr_fun_addr[ptr_ord_addr[i]],file_buffer));
			return (char*)(&file_buffer[RVAtoFOA((__int64)ptr_fun_addr[ptr_ord_addr[i]],file_buffer)]);
		}
		
	}
	return NULL;

}



// GetFunAddrByOrdinals(FileBuffer指针，函数名导出序号)
char* GetFunAddrByOrdinal(char* file_buffer,int fun_ord)
{
	if(file_buffer==NULL) 
	{
		printf("get a NULL.\n");
		return NULL;
	}
	data_directory* p_directory=NULL;
	export_directory* p_export=NULL;

	p_directory=GetPE_DirectoryPtr(file_buffer,0);
	p_export=(export_directory*)(&file_buffer[RVAtoFOA(p_directory->VirtualAddress,file_buffer)]);

	if(!p_export)
	{
		printf("RVAtoFOA UNsuccessfal.\n");
		return NULL;
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
		return (char*)(&file_buffer[RVAtoFOA((__int64)ptr_fun_addr[fun_ord],file_buffer)]);
	}
	return NULL;

}


// 文件大小有可能大于最后一个节的文件偏移+其大小
int getPEExtraSize(char* fpath,char* p)
{
	if(p==NULL)
	{
		printf("isPEExtra: p is a NULL.\n");
		return 0;
	}
	size_t file_size_1=GetFileSize(fpath);
	size_t file_size_2=0;
	int section_num=GetPE_StdPtr(p)->NumberOfSections;
	section_head* p_section=GetPE_sectionPtr(p,section_num-1);
	file_size_2=p_section->PointerToRawData+p_section->SizeOfRawData;
	printf("getPEExtraSize: %d\n",(file_size_1-file_size_2));
	return (file_size_1-file_size_2)>0?(file_size_1-file_size_2):-1;
	
}


// move export table
int moveExportTable(char* file_buffer,char** new_section,size_t* new_section_size)
{
	if(file_buffer==NULL)
	{
		printf("moveExportTable: file_buffer is a NULL.\n");
		return 0;
	}

	int fun_num=0;
	int name_num=0;
	char* fun_addr=NULL;
	char** name_addr=NULL;
	size_t name_total_size=0;
	size_t export_table_size=40;
	short* ord_addr=NULL;
	
	int section_align=0;
	int file_align=0;
	if(GetPE_StdPtr(file_buffer)->Machine==0x14c)
	{
		section_align=GetPE_OptPtr(file_buffer)->SectionAlignment;
		file_align=GetPE_OptPtr(file_buffer)->FileAlignment;
	}else{
		section_align=GetPE_Opt64Ptr(file_buffer)->SectionAlignment;
		file_align=GetPE_Opt64Ptr(file_buffer)->FileAlignment;
	}
	int section_num=GetPE_StdPtr(file_buffer)->NumberOfSections;
	section_head* p_section=GetPE_sectionPtr(file_buffer,section_num-1);
	data_directory* p_directory=GetPE_DirectoryPtr(file_buffer,0); 
	export_directory* p_export=(export_directory*)(&file_buffer[RVAtoFOA((__int64)(p_directory->VirtualAddress),file_buffer)]);

	fun_num=p_export->NumberOfFunctions;
	name_num=p_export->NumberOfNames;
	fun_addr=(&file_buffer[RVAtoFOA((__int64)p_export->AddressOfFunctions,file_buffer)]);
	name_addr=(char**)(&file_buffer[RVAtoFOA((__int64)p_export->AddressOfNames,file_buffer)]);
	ord_addr=(short*)(&file_buffer[RVAtoFOA((__int64)p_export->AddressOfNameOrdinals,file_buffer)]);
	
	//printf("debug: %x %s(0x%08x) %d\n",fun_addr[0],&file_buffer[RVAtoFOA((__int64)name_addr[0],file_buffer)],name_addr[0],ord_addr[0]);
	
	// 统计总函数名字大小
	for(int i=0;i<name_num;i++)
	{
		name_total_size+=(strlen(&file_buffer[RVAtoFOA((__int64)name_addr[i],file_buffer)])+1);  // 加上\0
	}
	// 拼接函数名
	char* name_total=(char*)malloc(name_total_size);
	if(name_total==NULL)
	{
		printf("moveExportTable: name_total is a NULL.\n");
		return 0;
	}
	memset(name_total,0,name_total_size);
	name_total_size=0;
	for(i=0;i<name_num;i++)
	{
		strcat(name_total,&file_buffer[RVAtoFOA((__int64)name_addr[i],file_buffer)]);
		sprintf(name_total,"%s ",name_total);
		name_addr[i]=(char*)(p_section->VirtualAddress+AutoAlign(p_section->Misc.VirtualSize,section_align)+fun_num*4+name_num*4+name_num*2+name_total_size);
		name_total_size=strlen(name_total);  // 加上\0
	}
	i=0;
	while(i<name_total_size)
	{
		if(name_total[i]==' ') name_total[i]='\0';
		i++;
	}
	
	new_section_size[0]=fun_num*4+name_num*4+name_num*2+name_total_size+export_table_size;
	new_section_size[0]=AutoAlign(AutoAlign(new_section_size[0],section_align),file_align);
	new_section[0] = (char*)malloc(new_section_size[0]);
	if(new_section[0]==NULL)
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
	memcpy(&new_section[0][fun_num*4+name_num*2+name_num*4],name_total,name_total_size);
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
int moveRelocationTable(char* file_buffer,char** new_section,size_t* new_section_size)
{
	if(file_buffer==NULL)
	{
		printf("moveExportTable: file_buffer is a NULL.\n");
		return 0;
	}

	int section_align=0;
	int file_align=0;
	if(GetPE_StdPtr(file_buffer)->Machine==0x14c)
	{
		section_align=GetPE_OptPtr(file_buffer)->SectionAlignment;
		file_align=GetPE_OptPtr(file_buffer)->FileAlignment;
	}else{
		section_align=GetPE_Opt64Ptr(file_buffer)->SectionAlignment;
		file_align=GetPE_Opt64Ptr(file_buffer)->FileAlignment;
	}
	int section_num=GetPE_StdPtr(file_buffer)->NumberOfSections;
	section_head* p_section=GetPE_sectionPtr(file_buffer,section_num-1);
	data_directory* p_directory=GetPE_DirectoryPtr(file_buffer,5); 
	relocation_directory* p_relocation=(relocation_directory*)(&file_buffer[RVAtoFOA((__int64)(p_directory->VirtualAddress),file_buffer)]);
	relocation_directory* p_relocation_=p_relocation;
	size_t virtual_addr=0;
	size_t block_size=0; 
	WORD reloc_table_size=0;
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
			p_relocation_=(relocation_directory*)((char*)p_relocation_+block_size);
		}

		
		
	}
	// printf("debug: %x\n",reloc_table_size);

	new_section_size[0]=AutoAlign(reloc_table_size,section_align);
	new_section[0]=(char*)malloc(new_section_size[0]);
	if(new_section[0]==NULL)
	{
		printf("moveRelocationTable: new_section is a NULL.\n");
		return 0;
	}
	memcpy(new_section[0],(char*)p_relocation,reloc_table_size);
	
	p_directory->VirtualAddress=p_section->VirtualAddress+AutoAlign(p_section->Misc.VirtualSize,section_align);


	return 1;
}


#endif // !defined(AFX_PETOOL_H__BC9AE6DA_8D52_4356_9802_921968C7E3DA__INCLUDED_)