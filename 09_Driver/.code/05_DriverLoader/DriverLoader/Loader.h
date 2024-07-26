#pragma once
#include <ntifs.h>

BOOLEAN UpdataIAT(char * imageBuffer);

VOID UpdateCookie(char * imageBuffer);

BOOLEAN LoadDriver(PUCHAR fileBuffer);