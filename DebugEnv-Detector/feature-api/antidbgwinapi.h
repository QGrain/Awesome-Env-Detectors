#ifndef _ANTIDBGWINAPI_H_
#define _ANTIDBGWINAPI_H_
#include <stdio.h>
#include <windows.h>
typedef DWORD (WINAPI *NtQueryInformationProcessPtr)(
       HANDLE,
       DWORD,
       PVOID,
       ULONG,
       PULONG);
BOOL IsDbgPr();
BOOL ChkRmtDbgPr();
BOOL NTQryInfoProc();
BOOL ErrValueTest();
void AntiDbgWinApiPrint()
#endif
