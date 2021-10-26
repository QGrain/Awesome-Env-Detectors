#include "checkpeb.h"
BOOL CheckPEBBeDbged()
{
	int result = 0;
	asm(
	"   mov     %%fs: 0x30, %%eax\n"
	"   mov     0x2(%%eax), %%al\n"
	"   mov     %%al, %0\n"
	:"=m"(result)
	:
	:"memory", "esi", "edi", "eax", "ebx", "ecx", "edx"
	);

	return result != 0;
}
BOOL CheckProcHeap()
{
	int result = 0;
	DWORD dwVersion = GetVersion();
	DWORD dwWindowsMajorVersion = (DWORD)(LOBYTE(LOWORD(dwVersion)));
	if (dwWindowsMajorVersion == 5)
	{
		asm(
		"   movl     %%fs: 0x30, %%eax\n"
		"   mov     0x18(%%eax), %%eax\n"
		"   mov     0x0c(%%eax), %%eax\n"
		"   mov     %%eax, %0\n"
		:"=m"(result)
		:
		:"memory","esi","edi","eax","ebx","ecx","edx"
		);
	}
	else
	{

		asm(
		"   movl     %%fs: 0x30, %%eax\n"
		"   mov     0x18(%%eax), %%eax\n"
		"   mov     0x40(%%eax), %%eax\n"
		"   mov     %%eax, %0\n"
		:"=m"(result)
		:
		:"memory","esi","edi","eax","ebx","ecx","edx"
		);
		
	}
	return result != 2;
}
BOOL CheckNTGlobalFlag()  
{
	int result = 0;  
	asm(
	"   movl     %%fs: 0x30, %%eax\n"
	"   mov     0x68(%%eax), %%eax\n"
	"   and     $0x70, %%eax\n"
	"   mov     %%eax, %0\n"
	:"=m"(result)
	:
	:"memory","esi","edi","eax","ebx","ecx","edx"
	);
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
