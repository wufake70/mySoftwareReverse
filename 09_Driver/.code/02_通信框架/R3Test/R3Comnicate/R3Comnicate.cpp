
#include "R3Comnicate.h"

HANDLE ghDevice;

BOOLEAN DriverInit()
{
	ghDevice = CreateFileW(SYM_NAME, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (ghDevice == NULL || ghDevice == INVALID_HANDLE_VALUE)
	{
		ghDevice = NULL;
		return FALSE;
	}
	return TRUE;
}


BOOLEAN DriverComm(ULONG code,PVOID inData,ULONG inSize, PVOID outData, ULONG outSize)
{
	if (!ghDevice)
	{
		DriverInit();
	}

	if (ghDevice)
	{
		CommPackage packag;
		packag.code = code;
		packag.inData = (ULONG64)inData;
		packag.inLen = (ULONG64)inSize;
		packag.outData = (ULONG64)outData;
		packag.outLen = (ULONG64)outSize;
		SIZE_T pro = NULL;
		packag.id = _COMM_ID;

		return WriteFile(ghDevice, &packag, sizeof(CommPackage), &pro, NULL);
	}

	return FALSE;
}