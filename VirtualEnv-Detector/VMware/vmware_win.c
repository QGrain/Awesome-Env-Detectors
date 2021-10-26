#include "vmware_win.h"

#define API_COUNT (sizeof(ApiData) / sizeof(*ApiData))
typedef UINT(WINAPI *pGetSystemFirmwareTable)(DWORD, DWORD, PVOID, DWORD);

BOOL Is_RegKeyExists(HKEY hKey, const TCHAR *lpSubKey) {

  HKEY hkResult = NULL;

  if (RegOpenKeyEx(hKey, lpSubKey, NULL, KEY_READ, &hkResult) ==
      ERROR_SUCCESS) {

    RegCloseKey(hkResult);

    return 1;
  }

  return 0;
}

BOOL Is_RegKeyValueExists(HKEY hKey, const TCHAR *lpSubKey,
                          const TCHAR *lpValueName, const TCHAR *search_str) {

  HKEY hkResult = NULL;

  TCHAR lpData[1024] = {0};

  DWORD cbData = MAX_PATH;

  if (RegOpenKeyEx(hKey, lpSubKey, NULL, KEY_READ, &hkResult) ==
      ERROR_SUCCESS) {

    if (RegQueryValueEx(hkResult, lpValueName, NULL, NULL, (LPBYTE)lpData,
                        &cbData) == ERROR_SUCCESS) {

      if (StrStrI((LPCTSTR)lpData, search_str) != NULL) {

        RegCloseKey(hkResult);

        return 1;
      }
    }

    RegCloseKey(hkResult);
  }

  return 0;
}

DWORD GetProcessIdFromName(LPCTSTR szProcessName) {

  PROCESSENTRY32 pe32;
  HANDLE hSnapshot = NULL;
  SecureZeroMemory(&pe32, sizeof(PROCESSENTRY32));
  hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
  if (hSnapshot == INVALID_HANDLE_VALUE) {

    printf("CreateToolhelp32Snapshot\n");

    return 0;
  }
  pe32.dwSize = sizeof(PROCESSENTRY32);
  if (Process32First(hSnapshot, &pe32) == 0) {
    printf("Process32First\n");
    CloseHandle(hSnapshot);
    return 0;
  }

  if (StrCmpI(pe32.szExeFile, szProcessName) == 0) {
    CloseHandle(hSnapshot);
    return pe32.th32ProcessID;
  }
  while (Process32Next(hSnapshot, &pe32)) {

    if (StrCmpI(pe32.szExeFile, szProcessName) == 0) {
      CloseHandle(hSnapshot);
      return pe32.th32ProcessID;
    }
  }
  CloseHandle(hSnapshot);
  return 0;
}

PBYTE get_system_firmware(_In_ DWORD signature, _In_ DWORD table,
                          _Out_ PDWORD pBufferSize) {
  HMODULE hLib = LoadLibraryA("kernel32.dll");
  void *pointer = GetProcAddress(hLib, "GetSystemFirmwareTable");

  DWORD bufferSize = 4096;
  PBYTE firmwareTable = (PBYTE)(malloc(bufferSize));

  if (firmwareTable == NULL)
    return NULL;

  SecureZeroMemory(firmwareTable, bufferSize);

  pGetSystemFirmwareTable GetSystemFirmwareTable = (pGetSystemFirmwareTable)(pointer);

  DWORD resultBufferSize =
      GetSystemFirmwareTable(signature, table, firmwareTable, bufferSize);
  if (resultBufferSize == 0) {
    printf("First call failed :(\n");
    free(firmwareTable);
    return NULL;
  }

  if (resultBufferSize > bufferSize) {
    PBYTE tmp;

    tmp = (BYTE *)(realloc(firmwareTable, resultBufferSize));
    if (tmp) {
      firmwareTable = tmp;
      SecureZeroMemory(firmwareTable, resultBufferSize);
      if (GetSystemFirmwareTable(signature, table, firmwareTable,
                                 resultBufferSize) == 0) {
        printf("Second call failed :(\n");
        free(firmwareTable);
        return NULL;
      }
    }
  }

  *pBufferSize = resultBufferSize;
  return firmwareTable;
}

BOOL find_str_in_data(PBYTE needle, size_t needleLen, PBYTE haystack,
                      size_t haystackLen) {
  for (size_t i = 0; i < haystackLen - needleLen; i++) {
    if (memcmp(&haystack[i], needle, needleLen) == 0) {
      return 1;
    }
  }
  return 0;
}

BOOL vmware_reg_keys() {
  const TCHAR *szKeys[] = {
      _T("SOFTWARE\\VMware, Inc.\\VMware Tools"),
  };
  WORD dwlength = sizeof(szKeys) / sizeof(szKeys[0]);
  for (int i = 0; i < dwlength; i++) {
    if (Is_RegKeyExists(HKEY_LOCAL_MACHINE, szKeys[i])) {
      return 1;
    }
  }
  return 0;
}

BOOL vmware_reg_key_value() {
  const TCHAR *szEntries[][3] = {

      {_T("HARDWARE\\DEVICEMAP\\Scsi\\Scsi Port 0\\Scsi Bus 0\\Target Id ")

       _T("0\\Logical Unit Id 0"),

       _T("Identifier"), _T("VMWARE")},

      {_T("HARDWARE\\DEVICEMAP\\Scsi\\Scsi Port 1\\Scsi Bus 0\\Target Id ")
       _T("0\\Logical Unit Id 0"),
       _T("Identifier"), _T("VMWARE")},

      {_T("HARDWARE\\DEVICEMAP\\Scsi\\Scsi Port 2\\Scsi Bus 0\\Target Id ")
       _T("0\\Logical Unit Id 0"),
       _T("Identifier"), _T("VMWARE")},

      {_T("SYSTEM\\ControlSet001\\Control\\SystemInformation"),
       _T("SystemManufacturer"), _T("VMWARE")},

      {_T("SYSTEM\\ControlSet001\\Control\\SystemInformation"),
       _T("SystemProductName"), _T("VMWARE")},

  };

  WORD dwLength = sizeof(szEntries) / sizeof(szEntries[0]);
  for (int i = 0; i < dwLength; i++) {
    if (Is_RegKeyValueExists(HKEY_LOCAL_MACHINE, szEntries[i][0],
                             szEntries[i][1], szEntries[i][2])) {
      return 1;
    }
  }

  return 0;
}

BOOL vmware_firmware_SMBIOS() {
  BOOL result = 0;
  const DWORD Signature = (DWORD)('RSMB');

  DWORD smbiosSize = 0;
  PBYTE smbios =
      get_system_firmware((DWORD)('RSMB'), 0x0000, &smbiosSize);
  if (smbios != NULL) {
    PBYTE vmwareString = (PBYTE) "VMware";
    size_t vmwwareStringLen = 6;

    if (find_str_in_data(vmwareString, vmwwareStringLen, smbios, smbiosSize)) {
      result = 1;
    }

    free(smbios);
  }

  return result;
}

BOOL vmware_processes() {
  const TCHAR *szProcesses[] = {
      _T("vmtoolsd.exe"),      _T("vmwaretray.exe"), _T("vmwareuser.exe"),
      _T("VGAuthService.exe"), _T("vmacthlp.exe"),
  };

  WORD iLength = sizeof(szProcesses) / sizeof(szProcesses[0]);
  for (int i = 0; i < iLength; i++) {
    if (GetProcessIdFromName(szProcesses[i])) {
      return 1;
    }
  }
  return 0;
}

void vmware() {
  if (vmware_reg_keys()) {
    printf("[*] %-50s%10s\n", "VMWareRegKeys", "[ BAD  ]");
  } else {
    printf("[*] %-50s%10s\n", "VMWareRegKeys", "[ GOOD ]");
  }
  if (vmware_reg_key_value()) {
    printf("[*] %-50s%10s\n", "VMWareRegKeyValue", "[ BAD  ]");
  } else {
    printf("[*] %-50s%10s\n", "VMWareRegKeyValue", "[ GOOD ]");
  }
  if (vmware_firmware_SMBIOS()) {
    printf("[*] %-50s%10s\n", "VMWareFirmware", "[ BAD  ]");
  } else {
    printf("[*] %-50s%10s\n", "VMWareFirmware", "[ GOOD ]");
  }
  if (vmware_processes()) {
    printf("[*] %-50s%10s\n", "VMWareProcess", "[ BAD  ]");
  } else {
    printf("[*] %-50s%10s\n", "VMWareProcess", "[ GOOD ]");
  }
}
