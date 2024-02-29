#if !defined(AFX_PETOOL_H__BC9AE6DA_8D52_4356_9802_921968C7E3DA__INCLUDED_)								
#define AFX_PETOOL_H__BC9AE6DA_8D52_4356_9802_921968C7E3DA__INCLUDED_								
								
#if _MSC_VER > 1000								
#pragma once								
#endif // _MSC_VER > 1000								
#include "petool2.h"								
								
char* readpe(char* pepath)								
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
								
				printf("%08x %08x %08x \n",				
						virtual_address,		
						rawdata_size,		
						pointer_rawdata);		
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
								
				printf("%08x %08x %08x \n",				
						virtual_address,		
						rawdata_size,		
						pointer_rawdata);		
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
	roa=rva-img_base;							
								
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
								
								
								
void AddMessageBoxToSection(char* pbuffer,char* messageBox_title,char* messageBox_content)								
{								
								
	if(!pbuffer)							
	{							
		printf("get a NULL.");						
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
								
#endif // !defined(AFX_PETOOL_H__BC9AE6DA_8D52_4356_9802_921968C7E3DA__INCLUDED_)								
