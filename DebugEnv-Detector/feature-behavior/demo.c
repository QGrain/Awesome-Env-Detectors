#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

BOOL SearchCC(DWORD dwAddr, DWORD dwCodeSize)
{
    UINT32 i = 0, cnt = 0;
    UINT8 *p = (UINT8 *)dwAddr;
    printf("p=%x\n", p);
    for(; i < dwCodeSize; ++i) {
        if((UINT8)(*(p+i)) == 0xCC) {
            cnt++;
            printf("%d: virtual_mem[%d]=0x%x\n", cnt, i, *(p+i));
        }
    }
    if(cnt >= 1) {
        printf("There are %d 0xCC in program .text section in memory\n", cnt);
        return 1;
    }
    return 0;
}


BOOL CheckDebug()  
{  
    PIMAGE_DOS_HEADER pDosHeader;  
    PIMAGE_NT_HEADERS32 pNtHeaders;  
    PIMAGE_SECTION_HEADER pSectionHeader;  
    DWORD dwBaseImage = (DWORD)GetModuleHandle(NULL);   
    pDosHeader = (PIMAGE_DOS_HEADER)dwBaseImage;  
    pNtHeaders = (PIMAGE_NT_HEADERS32)((DWORD)pDosHeader + pDosHeader->e_lfanew);  
    pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pNtHeaders + sizeof(pNtHeaders->Signature) + sizeof(IMAGE_FILE_HEADER) +   
                     (WORD)pNtHeaders->FileHeader.SizeOfOptionalHeader);  
    DWORD dwAddr = pSectionHeader->VirtualAddress + dwBaseImage;   
    DWORD dwCodeSize = pSectionHeader->SizeOfRawData;      
    BOOL Found = FALSE;  
    printf("dwAddr=%x, dwCodeSize=%d\n", dwAddr, dwCodeSize);

    PIMAGE_OPTIONAL_HEADER pOpHeader = (PIMAGE_OPTIONAL_HEADER)((DWORD)pNtHeaders + sizeof(pNtHeaders->Signature) + sizeof(IMAGE_FILE_HEADER));
    DWORD dwBaseOfCode = pOpHeader->BaseOfCode;
    DWORD dwImageBase = pOpHeader->ImageBase;
    printf("dwBaseOfCode=%x, dwImageBase=%x\n", dwBaseOfCode, dwImageBase);

    Found = SearchCC(dwAddr, dwCodeSize);
    // __asm__ ( "cld\n\t"               
    //     "mov     %edi,dwAddr\n\t"
    //     "mov     %ecx,dwCodeSize\n\t"
    //     "mov     %al,CCh\n\t"
    //     "repne   scasb\n\t"
    //     "jnz     NotFound\n\t"
    //     "mov Found,1\n"
    // "NotFound:\n" );
    return Found;  
}


void TestAsm()
{
    int foo = 10, bar = 15;
    __asm__ __volatile__ ( "addl %%ebx, %%eax"
                                       : "=a"(foo)
                                       : "a"(foo), "b"(bar)
                                     );
 
    printf("foo+bar=%d\n", foo);
    return;
}

int main(void)
{
    BOOL stats = FALSE;
    char c;
    stats = CheckDebug();
    if(stats == TRUE)
        printf("You are being debuged, stats = %d\n", stats);
    else
        printf("Nothing detected\n");

    printf("wait for user input: ");
    scanf("%c", &c);
    return 0;
}