#ifndef _ANTIDBGWINAPI_H_
#define _ANTIDBGWINAPI_H_
#include <stdio.h>
#include <windows.h>
typedef DWORD (WINAPI *NtQueryInformationProcessPtr)(
       HANDLE processHandle,
       DWORD processInformationClass,
       PVOID processInformation,
       ULONG processInformationLength,
       PULONG returnLength);
BOOL IsDbgPr();
BOOL ChkRmtDbgPr();
BOOL NTQryInfoProc();
BOOL ErrValueTest();
void AntiDbgWinApiPrint()
#endif
