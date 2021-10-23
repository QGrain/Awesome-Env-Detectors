#include <Shlwapi.h>
#include <TlHelp32.h>
#include <Windows.h>
#include <stdio.h>
#include <tchar.h>

#pragma comment(lib, "Advapi32.lib")
#pragma comment(lib, "Shlwapi.lib")
#pragma comment(lib, "Mpr.lib")

BOOL vmware_reg_keys();
BOOL vmware_reg_key_value();
BOOL vmware_firmware_SMBIOS();
BOOL vmware_processes();
void vmware();
