#ifndef _ANTIDBGBEHAVIOR_H_
#define _ANTIDBGBEHAVIOR_H_

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <winnt.h>
#include <Tlhelp32.h>


typedef struct tagPROCESS_BASIC_INFORMATION
{
	DWORD ExitStatus;
	DWORD PebBaseAddress;
	DWORD AffinityMask;
	DWORD BasePriority;
	ULONG UniqueProcessId;
	ULONG InheritedFromUniqueProcessId;
} PROCESS_BASIC_INFORMATION;

typedef LONG(WINAPI *PNTQUERYINFORMATIONPROCESS)(HANDLE, UINT, PVOID, ULONG, PULONG);

typedef enum enumSYSTEM_INFORMATION_CLASS
{
	SystemBasicInformation,
	SystemProcessorInformation,
	SystemPerformanceInformation,
	SystemTimeOfDayInformation,
} SYSTEM_INFORMATION_CLASS;


BOOL CheckINT3();
BOOL CheckDebugReg();
BOOL CheckExecTime();
BOOL CheckFatherProc();
BOOL CheckStartInfo();
BOOL CheckSeDbgPrivilege();
int AntiDbgBehaviorPrint();

#endif