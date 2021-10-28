#include "antidbgbehavior.h"

int Search0xCC(DWORD dwAddr, DWORD dwCodeSize, BOOL verbose)
{
    UINT32 i = 0, cnt = 0;
    UINT8 *p = (UINT8 *)dwAddr;
    if(verbose == TRUE)
        printf("p=%x\n", p);
    for(; i < dwCodeSize; ++i) {
        if((UINT8)(*(p+i)) == 0xCC) {
            cnt++;
            if(verbose == TRUE)
                printf("%d: virtual_mem[%d]=0x%x\n", cnt, i, *(p+i));
        }
    }
    if(verbose == TRUE) {
        printf("There are %d 0xCC in program .text section in memory\n", cnt);
    }
    return cnt;
}


BOOL CheckINT3()  
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

    // PIMAGE_OPTIONAL_HEADER pOpHeader = (PIMAGE_OPTIONAL_HEADER)((DWORD)pNtHeaders + sizeof(pNtHeaders->Signature) + sizeof(IMAGE_FILE_HEADER));
    // DWORD dwBaseOfCode = pOpHeader->BaseOfCode;
    // DWORD dwCodeSize = pOpHeader->SizeOfCode;
    // DWORD dwImageBase = pOpHeader->ImageBase;

    // printf("dwAddr=%x, dwCodeSize=%d\n", dwAddr, dwCodeSize);
    // printf("dwBaseOfCode=%x, dwImageBase=%x\n", dwBaseOfCode, dwImageBase);

    int cnt = Search0xCC(dwAddr, dwCodeSize, FALSE);
    Found = (cnt > 33);
    // __asm__ __volatile__ ( "cld\n\t"               
    //         "movl     %1, %%edi\n\t"
    //         "movl     %2, %%ecx\n\t"
    //         "movb     $0xCC, %%al\n\t"
    //         "repne    scasb\n\t"
    //         "jnz      NotFound\n\t"
    //         "mov      $0x1, %0\n"
    //         "NotFound:\n"
    //         : "=g"(Found)
    //         : "g"(dwAddr), "g"(dwCodeSize)
    //         : "memory", "%edi", "%ecx", "%al"
    // );

    return Found;  
}


BOOL CheckDebugReg()
{
    CONTEXT context;    
    HANDLE hThread = GetCurrentThread();    
    context.ContextFlags = CONTEXT_DEBUG_REGISTERS;    
    GetThreadContext(hThread, &context);    
    if (context.Dr0 != 0 || context.Dr1 != 0 || context.Dr2 != 0 || context.Dr3!=0) {    
        return TRUE;    
    }    
    return FALSE;
}


BOOL CheckExecTime()  
{
    int cnt = 0xff;
    DWORD time1, time2;
    BOOL CK_RESULT = FALSE;


    // first judge, by rdtsc
    __asm__ __volatile__ ( "rdtsc\n\t"
        "movl %%eax, %0\n\t"
        :"=g"(time1)
        :
        :"memory", "%eax"
    );

    while(--cnt);

    __asm__ __volatile__ ( "rdtsc\n\t"
        "movl %%eax, %0\n\t"
        :"=g"(time2)
        :
        :"memory", "%eax"
    );

    // printf("time1 = %ld, time2 = %ld\n", time1, time2);
    // printf("time diff1 = %d\n", time2-time1);
    CK_RESULT = (BOOL)((time2-time1) > 0x04ff);

    // second judge, by GetTickCount()
    cnt = 0x7fffff;
    time1 = GetTickCount();
    // __asm__ __volatile__ ( 
    //     "movl     $0xa, %%ecx\n\t"
    //     "movl     $0x6, %%edx\n\t"
    //     "movl     $0xa, %%ecx\n\t"
    // ::
    // :"%eax", "%ecx", "%edx"
    // );
    
    while(--cnt);
    
    time2 = GetTickCount();
    // printf("time1 = %ld, time2 = %ld\n", time1, time2);
    // printf("time diff2 = %d\n", time2-time1);
    CK_RESULT = CK_RESULT || ((time2-time1) > 0x0f);

    return CK_RESULT;
}


BOOL CheckFatherProc()  
{  
    LONG                      status;    
    DWORD                     dwParentPID = 0;    
    HANDLE                    hProcess;    
    PROCESS_BASIC_INFORMATION pbi;    
    int pid = getpid();  
    hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pid);    
    if(!hProcess)    
        return -1;    
    PNTQUERYINFORMATIONPROCESS  NtQueryInformationProcess = (PNTQUERYINFORMATIONPROCESS)GetProcAddress(GetModuleHandleA("ntdll"),"NtQueryInformationProcess");  
    status = NtQueryInformationProcess(hProcess,SystemBasicInformation,(PVOID)&pbi,sizeof(PROCESS_BASIC_INFORMATION),NULL);  
    PROCESSENTRY32 pe32;  
    pe32.dwSize = sizeof(pe32);   
    HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);   
    if(hProcessSnap == INVALID_HANDLE_VALUE) {   
        return FALSE;   
    }  
    BOOL bMore = Process32First(hProcessSnap, &pe32);   
    while(bMore) {  
        if (pbi.InheritedFromUniqueProcessId == pe32.th32ProcessID) {  
            if (stricmp(pe32.szExeFile, "explorer.exe") == 0 || stricmp(pe32.szExeFile, "cmd.exe") || stricmp(pe32.szExeFile, "powershell.exe")) {  
                CloseHandle(hProcessSnap);  
                return FALSE;  
            }  
            else {  
                CloseHandle(hProcessSnap);  
                return TRUE;  
            }  
        }  
        bMore = Process32Next(hProcessSnap, &pe32);   
    }  
    CloseHandle(hProcessSnap);  
}


BOOL CheckStartInfo()  
{  
    STARTUPINFO si;  
    GetStartupInfo(&si);  
    if (si.dwX!=0 || si.dwY!=0 || si.dwFillAttribute!=0 || si.dwXSize!=0 || si.dwYSize!=0 || si.dwXCountChars!=0 || si.dwYCountChars!=0) {  
        return TRUE;  
    }  
    else {  
        return FALSE;  
    }  
}


BOOL CheckSeDbgPrivilege()
{  
    DWORD ID;
    DWORD ret = 0;
    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(pe32); 
    HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if(hProcessSnap == INVALID_HANDLE_VALUE) {
        return FALSE;
    }
    BOOL bMore = Process32First(hProcessSnap, &pe32);
    while(bMore) {
        if (strcmp(pe32.szExeFile, "csrss.exe")==0) {
            ID = pe32.th32ProcessID;
            break;
        }
        bMore = Process32Next(hProcessSnap, &pe32);
    }
    CloseHandle(hProcessSnap);
    if (OpenProcess(PROCESS_QUERY_INFORMATION, NULL, ID) != NULL) {
        return TRUE;
    }
    else {
        return FALSE;
    }
}


int AntiDbgBehaviorPrint()
{
    int confidence = 0;

    if(CheckINT3() == TRUE) {
        printf("[*] %-50s%10s\n","INT3 check","[ BAD  ]");
        confidence += 1;
    }
    else {
        printf("[*] %-50s%10s\n","INT3 check","[ GOOD ]");
    }

    if(CheckDebugReg() == TRUE) {
        printf("[*] %-50s%10s\n","Debug Registers check","[ BAD  ]");
        confidence += 1;
    }
    else {
        printf("[*] %-50s%10s\n","Debug Registers check","[ GOOD ]");
    }

    if(CheckExecTime() == TRUE) {
        printf("[*] %-50s%10s\n","Execution time check","[ BAD  ]");
        confidence += 1;
    }
    else {
        printf("[*] %-50s%10s\n","Execution time check","[ GOOD ]");
    }
        
    if(CheckFatherProc() == TRUE) {
        printf("[*] %-50s%10s\n","Father process check","[ BAD  ]");
        confidence += 1;
    }
    else {
        printf("[*] %-50s%10s\n","Father process check","[ GOOD ]");
    }

    if(CheckStartInfo() == TRUE) {
        printf("[*] %-50s%10s\n","Start info check","[ BAD  ]");
        confidence += 1;
    }
    else {
        printf("[*] %-50s%10s\n","Start info check","[ GOOD ]");
    }

    if(CheckSeDbgPrivilege() == TRUE) {
        printf("[*] %-50s%10s\n","SeDebugPrevilege check","[ BAD  ]");
        confidence += 1;
    }
    else {
        printf("[*] %-50s%10s\n","SeDebugPrivilege check","[ GOOD ]");
    }

    return confidence;
}


// int main(void)
// {
//     AntiDbgBehaviorPrint();
//     printf("Paused, waiting for user termination: ");
//     getchar();
//     return 0;
// }