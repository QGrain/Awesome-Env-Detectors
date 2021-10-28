#include "systrace.h"


void UpToLow(char *a, int max)
{
	int i = 0;
	while(a[i] != 0) {
		if(a[i] >= 65 && a[i] <= 90) {
			a[i] = a[i] + ('a'-'A');
		}
		++i;
	}
}


BOOL CALLBACK EnumWndProc(HWND hwnd, LPARAM lParam)    
{
	char cur_window[1024];
	memset(cur_window, 0, 1024);
	GetWindowTextA(hwnd, cur_window, 1023);
	UpToLow(cur_window, 1024);
	if (strstr(cur_window, "windbg")!=NULL || strstr(cur_window, "x32dbg")!=NULL || strstr(cur_window, "ollyice")!=NULL || strstr(cur_window, "ollydbg")!=NULL || strstr(cur_window, "immunity")!=NULL) {  
		*((BOOL*)lParam) = TRUE;
	}  
	return TRUE;
}


BOOL WindowChck()
{
	BOOL ret = FALSE;
	EnumWindows(EnumWndProc, (LPARAM)&ret);
	return ret;
}


BOOL RegAedbg()
{

	HKEY hkey = NULL;
	char key[] = "Debugger";
	char reg_dir_32bit[] = "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\AeDebug";
	DWORD ret = 0;
	DWORD type;
	char tmp[256];
	DWORD len = 256;
	ret = RegCreateKeyA(HKEY_LOCAL_MACHINE, reg_dir_32bit, &hkey);
	if (ret != ERROR_SUCCESS) {
		return FALSE;
	}

	ret = RegQueryValueExA(hkey, key, NULL, &type, (LPBYTE)tmp, &len);
	UpToLow(tmp,256);
	if (strstr(tmp, "ollyice") != NULL || strstr(tmp, "ollydbg") != NULL || strstr(tmp, "windbg") != NULL || strstr(tmp, "x32dbg") != NULL || strstr(tmp, "immunity") != NULL) {
		return TRUE;
	}
	else {
		return FALSE;
	}
}


BOOL DbgProcCheck()
{
	DWORD ID;
	DWORD ret = 0;
	PROCESSENTRY32 pe32;
	
	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	BOOL bMore = Process32First(hProcessSnap, &pe32);
	pe32.dwSize = sizeof(pe32);
	if (hProcessSnap == INVALID_HANDLE_VALUE) {
		return FALSE;
	}
	
	while (bMore) {
		if (stricmp(pe32.szExeFile, "OllyDBG.EXE") == 0 || stricmp(pe32.szExeFile, "OllyICE.exe") == 0 || stricmp(pe32.szExeFile, "x64dbg.exe") == 0 || stricmp(pe32.szExeFile, "x32dbg.exe") == 0 || stricmp(pe32.szExeFile, "windbg.exe") == 0 || stricmp(pe32.szExeFile, "ImmunityDebugger.exe") == 0) {
			return TRUE;
		}
		bMore = Process32Next(hProcessSnap, &pe32);
	}

	CloseHandle(hProcessSnap);
	return FALSE;
}


int SysTracePrint()
{
	int confidence = 0;

	if (RegAedbg()) {
		printf("[*] %-50s%10s\n","RegisterAedbgEntry","[ BAD  ]");
		confidence += 1;
	}
	else {
		printf("[*] %-50s%10s\n","RegisterAedbgEntry","[ GOOD ]");
	}

	if(WindowChck()) {
		printf("[*] %-50s%10s\n","FrontWindowsCheck","[ BAD  ]");
		confidence += 1;
	}
	else {
		printf("[*] %-50s%10s\n","FrontWindowsCheck","[ GOOD ]");
	}

	if(DbgProcCheck()) {
		printf("[*] %-50s%10s\n","DebugProcessCheck","[ BAD  ]");
		confidence += 1;
	}
	else {
		printf("[*] %-50s%10s\n","DebugProcessCheck","[ GOOD ]");
	}
	
	return confidence;
}
