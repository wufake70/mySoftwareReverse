#include "Search.h"

BOOLEAN IsAddressValid(void* addr) {
	MEMORY_BASIC_INFORMATION mbi;
	if (VirtualQuery(addr, &mbi, sizeof(mbi)) == 0) {
		// VirtualQuery 失败，地址无效
		return FALSE;
	}
	return TRUE; // 检查内存状态是否为非空闲
}

UCHAR charToHex(UCHAR * ch)
{
	unsigned char temps[2] = { 0 };
	for (int i = 0; i < 2; i++)
	{
		if (ch[i] >= '0' && ch[i] <= '9')
		{
			temps[i] = (ch[i] - '0');
		}
		else if (ch[i] >= 'A' && ch[i] <= 'F')
		{
			temps[i] = (ch[i] - 'A') + 0xA;
		}
		else if (ch[i] >= 'a' && ch[i] <= 'f')
		{
			temps[i] = (ch[i] - 'a') + 0xA;
		}
	}
	return ((temps[0] << 4) & 0xf0) | (temps[1] & 0xf);
}



void initFindCodeStruct(PFindCode findCode, PCHAR code, ULONG_PTR offset, ULONG_PTR lastAddrOffset)
{

	memset(findCode, 0, sizeof(FindCode));

	findCode->lastAddressOffset = lastAddrOffset;
	findCode->offset = offset;

	PCHAR pTemp = code;
	ULONG_PTR i = 0;
	for (i = 0; *pTemp != '\0'; i++)
	{
		if (*pTemp == '*' || *pTemp == '?')
		{
			findCode->code[i] = *pTemp;
			pTemp++;
			continue;
		}

		findCode->code[i] = charToHex((PUCHAR)pTemp);
		pTemp += 2;

	}

	findCode->len = i;
}


ULONG_PTR findAddressByCode(ULONG_PTR beginAddr, ULONG_PTR endAddr, PFindCode  findCode, ULONG numbers)
{
	ULONG64 j = 0;
	LARGE_INTEGER rtna = { 0 };

	for (ULONG_PTR i = beginAddr; i <= endAddr; i++)
	{
		
		if (!IsAddressValid((PVOID)i))
		{
			i = i & (~0xfff) + PAGE_SIZE - 1;
			continue;
		}

		

		for (j = 0; j < numbers; j++)
		{
			FindCode  fc = findCode[j];
			ULONG_PTR tempAddress = i;

			UCHAR * code = (UCHAR *)(tempAddress + fc.offset);
			BOOLEAN isFlags = FALSE;

			for (ULONG_PTR k = 0; k < fc.len; k++)
			{
				if (!IsAddressValid((PVOID)(code + k)))
				{
					isFlags = TRUE;
					break;
				}

				if (fc.code[k] == '*' || fc.code[k] == '?') continue;

				if (code[k] != fc.code[k])
				{
					isFlags = TRUE;
					break;
				}
			}

			if (isFlags) break;

		}

		//找到了
		if (j == numbers)
		{
			rtna.QuadPart = i;
			rtna.LowPart += findCode[0].lastAddressOffset;
			break;
		}

	}

	return rtna.QuadPart;
}

char * CharToUper(char * wstr, BOOLEAN isAllocateMemory)
{
	char * ret = NULL;
	int len = strlen(wstr) + 2;

	if (isAllocateMemory)
	{
		ret = (PCHAR)VirtualAlloc(NULL, len, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
		if (ret == NULL) return ret;
		memset(ret, 0, len);
		memcpy(ret, wstr, len - 2);
	}
	else
	{
		ret = wstr;
	}

	_strupr_s(ret,len);

	return ret;
}



ULONG_PTR searchOtherCode(ULONG_PTR startAdd, ULONG_PTR endAddr,char * code, int offset)
{
	FindCode fs[1] = { 0 };
	initFindCodeStruct(&fs[0], code, 0, offset);


	ULONG_PTR func = findAddressByCode(startAdd, endAddr, fs, 1);

	return func;
}
