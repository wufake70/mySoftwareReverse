#pragma once
#include <ntifs.h>

PVOID MyExpLookupHandleTableEntry(HANDLE pid);

BOOLEAN ProtectProcess(HANDLE pid);