#pragma once
#include <windows.h>
#include <winnt.h>

#define PAGE_SIZE 0x1000;


typedef struct _FindCode
{
	UCHAR code[0x200];
	ULONG len;
	int offset;
	ULONG lastAddressOffset;
}FindCode, *PFindCode;

BOOLEAN IsAddressValid(void* addr);

void initFindCodeStruct(PFindCode findCode, PCHAR code, ULONG_PTR offset, ULONG_PTR lastAddrOffset);

ULONG_PTR findAddressByCode(ULONG_PTR beginAddr, ULONG_PTR endAddr, PFindCode  findCode, ULONG numbers);


// 特征码搜索，参数: 起始地址、结束地址、特征码字符、目标值偏移
// 返回 目标值地址
ULONG_PTR searchOtherCode(ULONG_PTR startAdd, ULONG_PTR endAddr, char * code, int offset);
