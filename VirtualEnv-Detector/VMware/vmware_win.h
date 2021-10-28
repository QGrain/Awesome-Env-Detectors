#include <Shlwapi.h>
#include <TlHelp32.h>
#include <Windows.h>
#include <ShlObj_core.h>
#include <stdio.h>
#include <tchar.h>
#include <iphlpapi.h>
#include <iptypes.h>

#pragma comment(lib, "Advapi32.lib")
#pragma comment(lib, "Shlwapi.lib")
#pragma comment(lib, "Mpr.lib")
#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "Shell32.lib")


#define	MALLOC(x)	HeapAlloc(GetProcessHeap(), 0, x)
#define FREE(x)		HeapFree(GetProcessHeap(), 0, x)



BOOL vmware_reg_keys();
BOOL vmware_reg_key_value();
BOOL vmware_firmware_SMBIOS();
BOOL vmware_processes();
BOOL vmware_dir();
BOOL vmware_files();
BOOL vmware_mac();
void vmware();
