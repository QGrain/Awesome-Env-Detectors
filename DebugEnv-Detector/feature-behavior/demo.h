#ifndef _DEMO_H_
#define _DEMO_H_

#include <stdio.h>
#include <stdlib.h>

#ifdef WINVER
#include <windows.h>
#include <winnt.h>
#else
//include some linux headers for cross-platform support
#endif

BOOL CheckINT3();
BOOL CheckDebugReg();
BOOL CheckExecTime();
// BOOL CheckFatherProc();

#endif