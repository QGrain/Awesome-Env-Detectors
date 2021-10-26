#include "feature-api/antidbgwinapi.h"
#include "feature-sys/systrace.h"
#include "feature-behavior/demo.h"
#include "checkpeb/checkpeb.h"

#include <debugapi.h>


void PrintStart();


int main()
{
    PrintStart();

    printf("\n=============Detect from Windows API=============\n");
    if (IsDebuggerPresent()) {
		printf("[*] %-50s%10s\n","IsDebuggerPresent API","[ BAD  ]");
	}
	else {
		printf("[*] %-50s%10s\n","IsDebuggerPresent API","[ GOOD ]");
	}

	if(ChkRmtDbgPr()) {
		printf("[*] %-50s%10s\n","CheckRemoteDebuggerPresent API","[ BAD  ]");
	}
	else {
		printf("[*] %-50s%10s\n","CheckRemoteDebuggerPresent API","[ GOOD ]");
	}

	if(NTQryInfoProc()) {
		printf("[*] %-50s%10s\n","NtQueryInformationProcess API","[ BAD  ]");
	}
	else {
		printf("[*] %-50s%10s\n","NtQueryInformationProcess API","[ GOOD ]");
	}

	if(ErrValueTest()) {
		printf("[*] %-50s%10s\n","ErrorValueTest","[ BAD  ]");
	}
	else {
		printf("[*] %-50s%10s\n","ErrorValueTest","[ GOOD ]");
	}

    printf("\n=============Detect from System Traces=============\n");
}

void PrintStart()
{
    printf("\nDebug Environment Dtector v1.0\n");
}