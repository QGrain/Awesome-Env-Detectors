#include "checkpeb.h"
BOOL CheckPEBBeDbged()
{
	int result = 0;
	__asm
	{
		mov eax, fs:[30h]
		mov eax, [eax + 68h]
		and eax, 0x70
		mov result, eax
	}
	return result != 0;
}
BOOL CheckProcHeap()
{
	int result = 0;
	DWORD dwVersion = GetVersion();
	DWORD dwWindowsMajorVersion = (DWORD)(LOBYTE(LOWORD(dwVersion)));
	if (dwWindowsMajorVersion == 5)
	{
		__asm
		{
			mov eax, fs:[30h]
			mov eax, [eax + 18h]
			mov eax, [eax + 0ch]
			mov result, eax
		}
	}
	else
	{
		__asm
		{
			mov eax, fs:[30h]
			mov eax, [eax + 18h]
			mov eax, [eax + 40h]
			mov result, eax
		}
	}
	return result != 2;
}
BOOL CheckNTGlobalFlag()  
{  
    int result = 0;  
    __asm  
    {  
        mov eax, fs:[30h]  
        mov eax, [eax + 68h]  
        and eax, 0x70  
        mov result, eax  
    }  
    return result != 0;  
}  
void CheckPEBPrint()
{
	if(CheckPEBBeDbged())
	{
		printf("[*] %-50s%10s\n","PEB BeingDebugged Bit","[ BAD  ]");
	}
	else
	{
		printf("[*] %-50s%10s\n","PEB BeingDebugged Bit","[ GOOD ]");
	}
	if(CheckProcHeap())
	{
		printf("[*] %-50s%10s\n","ProcessHeap ForceFlag","[ BAD  ]");
	}
	else
	{
		printf("[*] %-50s%10s\n","ProcessHeap ForceFlag","[ GOOD ]");
	}
	if(CheckNTGlobalFlag())
	{
		printf("[*] %-50s%10s\n","NTGlobalFlag","[ BAD  ]");
	}
	else
	{
		printf("[*] %-50s%10s\n","NTGlobalFlag","[ GOOD ]");
	}
}