#ifndef _SYSTRACE_H_
#define _SYSTRACE_H_
#include <stdio.h>
#include <windows.h>
#include <tlhelp32.h>
#include <string.h>
void UpToLow(char *, int);
BOOL CALLBACK EnumWndProc(HWND, LPARAM);
BOOL WindowChck();
BOOL RegAedbg();
BOOL DbgProcCheck();
void SysTracePrint();
#endif