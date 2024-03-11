#include "stdafx.h"
#include "petool.h"
#include "petool2.h"

//int dos_size=sizeof _IMAGE_DOS_HEADER;

typedef int(*Plus)(int,int);
Plus plus=NULL;

int main(int argc, char* argv[])
{
	/*int operate_code=4;
	char pepath[100]="notepad.exe";
	char* file_buffer=NULL;
	dos_head* p_dos=NULL;
	nt_head* p_nt=NULL;
	stdpe_head* p_stdpe=NULL;
	optpe_head* p_optpe=NULL;
	optpe64_head* p_optpe64=NULL;

	printf("Put a pe path:\n\t");
	scanf("%s",pepath);

	file_buffer=ReadPE(pepath);
	
	

	while(1){
		printf("[0]DOS_HEADER [1]NT_HEADER [2]STDPE_HEADER [3]OPTPE_HEADER [4]OtherPE [5]EXIT:\n\t");
		scanf("%d",&operate_code);
		switch(operate_code)
		{
			case 0:
				p_dos=GetPE_DosPtr(buffer);
				GetPEinfo_Dos(p_dos);
				break;
			case 1:
				p_nt=GetPE_NtPtr(buffer);
				GetPEinfo_NT(p_nt);
				break;
			case 2:
				p_stdpe=GetPE_StdPtr(buffer);
				GetPEinfo_stdPE(p_stdpe);
				break;
			case 3:

				// 可选pe头有x32，x86
				p_stdpe=GetPE_StdPtr(buffer);
				if(p_stdpe->Machine==0x14c)
				{
					p_optpe=GetPE_OptPtr(buffer);
					GetPEinfo_optPE(p_optpe);				
				}else{
					p_optpe64=GetPE_Opt64Ptr(buffer);
					GetPEinfo_optPE64(p_optpe64);
				}
				break;
			case 4:
				free(buffer); // 避免内存泄露
				printf("Put a pe path:\n\t");
				scanf("%s",pepath);
				buffer=ReadPE(pepath);
				break;

			case 5:
				exit(0);
		}
		printf("\n\n");
	}

	free(buffer);
	getchar();
	getchar();
	*/
	
	/* 节表
	char pepath[100]="notepad.exe";
	char* file_buffer=NULL;
	section_head* p_section=NULL;
	file_buffer=ReadPE(pepath);
	
	for(int i=0;i<6;i++)
	{
		p_section=GetPE_sectionPtr(file_buffer,i);
		GetPEinfo_section(p_section);
	}

	*/
	
	/*	
	char pepath[100]="notepad.exe";
	char* file_buffer=NULL;
	int file_size=0;
	char* img_buffer=NULL;
	section_head* p_section=NULL;
	file_buffer=ReadPE(pepath);

	FILE* p_pe=fopen(pepath,"rb");
	if(p_pe)
	{
		fseek(p_pe,0,SEEK_END);
		file_size=ftell(p_pe);
		fclose(p_pe);
	}

	img_buffer=FileBufferToImageBuffer(file_buffer);
	free(file_buffer);
	file_buffer=ImageBufferToFileBuffer(img_buffer); ??????????????????

	p_pe=fopen("a.exe","wb");
	if(p_pe)
	{
		size_t result = fwrite(file_buffer,1,file_size,p_pe);
		fclose(p_pe);

		if(result==(size_t)file_size)
		{
			printf("write successful!\n");
		}else{
			printf("write fail!\n");
		}
	}

	free(file_buffer);
	*/

	/* //RVAtoFOA 
	char pepath[100];
	char* file_buffer=NULL;
	memset(pepath,0,100);
	__int64 rva=0;

	printf("Put a pe path: ");
	scanf("%s",pepath);

	printf("Put a RVA: ");
	scanf("%x",&rva);
	file_buffer=ReadPE(pepath);
	
	printf("FOA: 0x%I64X\n",RVAtoFOA(rva,file_buffer));

	getchar();
	*/
	
	
	// AddBinrayCodeToSection
	/*
	BYTE log_code=0;
	char pepath[100];
	char new_pepath[100];
	char messageBox_title[100];
	char messageBox_content[100];
	char* file_buffer=NULL;
	char* img_buffer=NULL;
	section_head* p_section=NULL;
	int file_size=0;
	int section_num=0;
	FILE* fp=NULL;
	
	memset(pepath,0,100);
	printf("Put a exe path: ");
	scanf("%s",pepath);
	file_buffer=ReadPE(pepath);

	if(GetPE_StdPtr(file_buffer)->Machine==0x14c)
	{
		log_code++;
		// User Input
		memset(new_pepath,0,100);
		memset(messageBox_title,0,100);
		memset(messageBox_content,0,100);
		sprintf(new_pepath,"%s-new.exe",pepath);
		printf("Put a messageBox title: ");
		scanf("%s",messageBox_title);
		printf("Put a messageBox content: ");
		scanf("%s",messageBox_content);	
	
		section_num=GetPE_StdPtr(file_buffer)->NumberOfSections;
		file_size=GetPE_sectionPtr(file_buffer,section_num-1)->PointerToRawData+GetPE_sectionPtr(file_buffer,section_num-1)->SizeOfRawData;
		AddMessageBoxToSection(file_buffer,messageBox_title,messageBox_content);
		
		fp=fopen(new_pepath,"wb");
		if(fp)
		{
			log_code++;
			fseek(fp,0,SEEK_SET);
			size_t result=fwrite(file_buffer,1,file_size,fp);
			if(result==(size_t)file_size)
			{
				log_code++;
			}
		}
	}
	

	switch(log_code)
	{
		case 0:
			printf("Not a x86 program.");
			break;
		case 1:
			printf("%s open fail.",new_pepath);
			break;
		case 2:
			fclose(fp);
			printf("Add fail.\n");
			break;
		default:
			fclose(fp);
			printf("Add successful!\n");

	}
	*/


	// Add new section
	/*
	BYTE log_code=0;
	char pepath[100];
	char new_pepath[100];
	char* file_buffer=NULL;
	section_head* p_section=NULL;
	size_t file_size=0;
	int section_num=0;
	FILE* fp=NULL;
	section_head* new_section_head=NULL;

	size_t new_section_size=0x1000;
	char* new_section_buffer=NULL;
	
	memset(pepath,0,100);
	printf("Put a exe path: ");
	scanf("%s",pepath);
	file_size=GetFileSize(pepath);
	file_buffer=ReadPE(pepath);

	new_section_head=(section_head*)malloc(40);
	if(new_section_head)
	{
		log_code++;
		new_section_buffer=(char*)malloc(new_section_size);
		if(new_section_buffer)
		{	
			log_code++;
			memset(new_section_buffer,0xe8,new_section_size);
			if(Add_NewSection(&file_buffer,file_size,new_section_head,new_section_buffer,new_section_size))
			{
				log_code++;
				sprintf(new_pepath,"%s-new.exe",pepath);
				WriteToFile(file_buffer,new_pepath,file_size+new_section_size);
			}
		}
	}

	switch(log_code)
	{
		case 0:
			printf("new_section_head memory not enough.\n");
			break;
		case 1:
			free(new_section_head);
			printf("new_section_buffer don't get enough memory.\n");
			break;
		case 2:
			free(new_section_head);
			free(new_section_buffer);
			printf("Add_NewSection() Unsuccessful.\n");
			break;
		default:
			free(new_section_head);
			free(new_section_buffer);
			printf("Add_NewSection() Successful.\n");
		
	}
	*/
	

	// expand section
	/*
	BYTE log_code=0;
	char exe_path[100];
	char new_exe_path[100];
	char* file_buffer=NULL;
	char* img_buffer=NULL;
	char* new_img_buffer=NULL;
	char* new_section=NULL;
	section_head* p_section=NULL;
	size_t new_section_size=0x1000;
	size_t file_size=0;
	int section_num=0;
	
	printf("Put a exe path: ");
	scanf("%s",exe_path);
	
	file_buffer=ReadPE(exe_path);
	if(file_buffer)
	{
		log_code++;
		img_buffer=FileBufferToImageBuffer(file_buffer);
		if(img_buffer)
		{
			log_code++;
			new_section=(char*)malloc(new_section_size);
			if(new_section)
			{
				log_code++;
				memset(new_section,0xcc,new_section_size);

				if(expandSection(img_buffer,new_section,new_section_size,&new_img_buffer))
				{
					log_code++;
					free(file_buffer);
					file_buffer=ImageBufferToFileBuffer(new_img_buffer);
					free(new_img_buffer);
					sprintf(new_exe_path,"%s-new.exe",exe_path);

					section_num=GetPE_StdPtr(file_buffer)->NumberOfSections;
					p_section=GetPE_sectionPtr(file_buffer,section_num-1);
					file_size=p_section->PointerToRawData+p_section->SizeOfRawData;

					WriteToFile(file_buffer,new_exe_path,file_size);
				}
				
			}
			
		}
	}

	switch(log_code)
	{
		case 0:
			printf("file_buffer is a NULL.\n");
			break;
		case 1:
			free(file_buffer);
			printf("img_buffer is a NULL.\n");
			break;
		case 2:
			free(file_buffer);
			free(img_buffer);
			printf("new_section Don't get enough memory.\n");
			break;
		case 3:
			free(file_buffer);
			free(img_buffer);
			free(new_section);
			printf("expandSection() Fail.\n");
			break;
		default:
			free(file_buffer);
			free(img_buffer);
			free(new_section);
			printf("Expand last section successful.\n");

	}
	*/


	// combine section header
	/*
	int log_code=0;
	char exe_path[100];
	char new_exe_path[100];
	char* file_buffer=NULL;
	char* img_buffer=NULL;
	section_head* p_section=NULL;
	size_t file_size=0;
	int section_num=0;
	
	printf("Put a exe path: ");
	scanf("%s",exe_path);
	
	file_buffer=ReadPE(exe_path);
	if(file_buffer)
	{
		log_code++;
		img_buffer=FileBufferToImageBuffer(file_buffer);
		if(img_buffer)
		{
			log_code++;
			if(combineSection(img_buffer))
			{
				log_code++;
				free(file_buffer);
				file_buffer=ImageBufferToFileBuffer(img_buffer);
				free(img_buffer);
				
				if(GetPE_StdPtr(file_buffer)->Machine==0x14c)
				{
					file_size=GetPE_OptPtr(file_buffer)->SizeOfImage;
				}else{
					file_size=GetPE_Opt64Ptr(file_buffer)->SizeOfImage;
				}
				file_size=getFileBufferSize(file_buffer);

				sprintf(new_exe_path,"%s-new.exe",exe_path);
				WriteToFile(file_buffer,new_exe_path,file_size);
				free(file_buffer);
			}
		}
	
	}
	
	switch(log_code)
	{
		case 0:
			printf("file_buffer Get a NULL.\n");
			break;
			
		case 1:
			printf("img_buffer Get a NULL.\n");
			break;
		case 2:
			printf("combineSection() Fail.\n");
			break;
		case 3:
			printf("Combine Sections Successful.\n");
	}
	*/


	// directory entry
	/*
	int log_code=0;
	char exe_path[100];
	char* file_buffer=NULL;
	data_directory* directory_entry=NULL;

	printf("Put a exe path: ");
	scanf("%s",exe_path);
	
	file_buffer=ReadPE(exe_path);
	if(file_buffer)
	{
		log_code++;
		checkDirectoryEntry(file_buffer);
	}

	switch(log_code)
	{
		case 0:
			printf("file_buffer Get a NULL.\n");
			break;
		default:
			printf("checkDirectoryEntry() Successful.\n");

	}
	*/



	// Check Export Directory & getFunAddrByName & GetFunAddrByOrdinal
	/*
	int log_code=0;
	char exe_path[100];
	char* file_buffer=NULL;
	int fun_ord=0;
	char fun_name[100]="Plus";
	data_directory* directory_entry=NULL;

	printf("Put a exe path: ");
	scanf("%s",exe_path);
	// printf("Put a name to search for: ");
	// scanf("%s",fun_name);
	printf("Put a Function Ordinal to search for: ");
	scanf("%d",&fun_ord);
	
	file_buffer=ReadPE(exe_path);
	if(file_buffer)
	{
		log_code++;
		checkExportDirectory(file_buffer);
		
		// printf("\nSearch Function By name.\n");
		// plus=(Plus)getFunAddrByName(file_buffer,fun_name);

		printf("\nSearch Function By name.\n");
		plus=(Plus)GetFunAddrByOrdinal(file_buffer,fun_ord);

		if(plus)
		{
			log_code++;
		
		}

	}

	switch(log_code)
	{
		case 0:
			printf("file_buffer Get a NULL.\n");
			break;
		case 1:
			// printf("%s() Search UNsuccesssful by name.\n",fun_name);
			printf("Function Ordinal @%d Search UNsuccesssful.\n",fun_ord);
			break;
		default:
			// printf("[0x%08X]%s() Search successsful by name.\n",plus,fun_name);
			printf("Function Ordinal @%d Search successsful.\n",fun_ord);
			//printf("12+14=%d\n",plus(12,14));

	}
	*/


	// Check Relocation table 
	/*
	int log_code=0;
	char exe_path[100];
	char* file_buffer=NULL;
	data_directory* directory_entry=NULL;

	printf("Put a exe path: ");
	scanf("%s",exe_path);

	
	file_buffer=ReadPE(exe_path);
	if(file_buffer)
	{
		log_code++;
		
	}

	switch(log_code)
	{
		case 0:
			printf("file_buffer Get a NULL.\n");
			break;
		default:
			CheckRelocationInfo(file_buffer);

	}
	*/


	// move export table
	BYTE log_code=0;
	char pepath[100];
	char new_pepath[100];
	char* file_buffer=NULL;
	section_head* p_section=NULL;
	size_t file_size=0;
	int section_num=0;
	FILE* fp=NULL;
	section_head* new_section_head=NULL;

	size_t new_section_size=0;
	char* new_section_buffer=NULL;
	
	memset(pepath,0,100);
	printf("Put a exe path: ");
	scanf("%s",pepath);
	file_size=GetFileSize(pepath);
	file_buffer=ReadPE(pepath);

	new_section_head=(section_head*)malloc(40);
	if(new_section_head)
	{
		log_code++;
		if(moveRelocationTable(file_buffer,&new_section_buffer,&new_section_size))
		{
			log_code++;	
			// pe文件大小可能大于最后一个节的文件偏移+其大小
			int extra_size=getPEExtraSize(pepath,file_buffer);
			if(extra_size>0)
			{
				file_size-=extra_size;
			}
			if(Add_NewSection(&file_buffer,file_size,new_section_head,new_section_buffer,new_section_size))
			{
				log_code++;
				sprintf(new_pepath,"%s-new.dll",pepath);
				WriteToFile(file_buffer,new_pepath,file_size+new_section_size);
			}	
			
				
		}
	}

	switch(log_code)
	{
		case 0:
			printf("new_section_head memory not enough.\n");
			break;
		case 1:
			free(new_section_head);
			printf("moveExportTable() UNsuccessful.\n");
			break;
		case 2:
			free(new_section_head);
			free(new_section_buffer);
			printf("Add_NewSection() Unsuccessful.\n");
			break;
		default:
			free(new_section_head);
			free(new_section_buffer);
			printf("Add_NewSection() Successful.\n");
		
	}
	
	system("pause>nul");
	return 0;
}
