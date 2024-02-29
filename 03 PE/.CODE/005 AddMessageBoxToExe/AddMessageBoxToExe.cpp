#include "stdafx.h"											
#include "petool.h"											
#include "petool2.h"											
											
//int dos_size=sizeof _IMAGE_DOS_HEADER;											
											
int main(int argc, char* argv[])											
{											
	// AddBinrayCodeToSection										
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
	file_buffer=readpe(pepath);										
											
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
	system("pause>nul");										
	return 0;										
}											
