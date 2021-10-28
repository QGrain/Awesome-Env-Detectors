#include "antidbgwinapi.h"


BOOL IsDbgPr()
{
	BOOL (_stdcall *IsDbgPrPtr)();
	HMODULE k32Module = LoadLibrary("Kernel32.dll");
	IsDbgPrPtr = (BOOL (_stdcall *)())GetProcAddress(k32Module, "IsDebuggerPresent"); 
	return IsDbgPrPtr();
}


BOOL ChkRmtDbgPr()
{
	BOOL flag;
	BOOL (_stdcall *ChkRmtDbgPrPtr)(HANDLE, PBOOL);
	HMODULE k32Module = LoadLibrary("Kernel32.dll");
	ChkRmtDbgPrPtr = (BOOL (_stdcall *)(HANDLE, PBOOL))GetProcAddress(k32Module, "CheckRemoteDebuggerPresent"); 
	ChkRmtDbgPrPtr(GetCurrentProcess(), &flag);
	return flag;
}


BOOL NTQryInfoProc()
{
	int debugPort = 0;
	HMODULE ntModule = LoadLibrary("Ntdll.dll");
	NtQueryInformationProcessPtr NtQueryInformationProcess = (NtQueryInformationProcessPtr)GetProcAddress(ntModule, "NtQueryInformationProcess");
	NtQueryInformationProcess(GetCurrentProcess(), 0x7, &debugPort, sizeof(debugPort), NULL);  
	return debugPort != 0;  
}


BOOL ErrValueTest()
{
	DWORD errorValue = 10000;
	SetLastError(errorValue);
	OutputDebugString("Test for debugger!");
	if (GetLastError() == errorValue)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


int AntiDbgWinApiPrint()
{
	int confidence = 0;
	
	if (IsDbgPr()) {
		printf("[*] %-50s%10s\n","IsDebuggerPresent API","[ BAD  ]");
		confidence += 1;
	}
	else {
		printf("[*] %-50s%10s\n","IsDebuggerPresent API","[ GOOD ]");
	}

	if(ChkRmtDbgPr()) {
		printf("[*] %-50s%10s\n","CheckRemoteDebuggerPresent API","[ BAD  ]");
		confidence += 1;
	}
	else {
		printf("[*] %-50s%10s\n","CheckRemoteDebuggerPresent API","[ GOOD ]");
	}

	if(NTQryInfoProc()) {
		printf("[*] %-50s%10s\n","NtQueryInformationProcess API","[ BAD  ]");
		confidence += 1;
	}
	else {
		printf("[*] %-50s%10s\n","NtQueryInformationProcess API","[ GOOD ]");
	}

	if(ErrValueTest()) {
		printf("[*] %-50s%10s\n","ErrorValueTest","[ BAD  ]");
		confidence += 1;
	}
	else {
		printf("[*] %-50s%10s\n","ErrorValueTest","[ GOOD ]");
	}

	return confidence;
}
