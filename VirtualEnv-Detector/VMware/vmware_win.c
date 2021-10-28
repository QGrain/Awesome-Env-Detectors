#include "vmware_win.h"

#define API_COUNT (sizeof(ApiData) / sizeof(*ApiData))
typedef UINT(WINAPI *pGetSystemFirmwareTable)(DWORD, DWORD, PVOID, DWORD);
typedef BOOL(WINAPI *pIsWow64Process)(HANDLE, PBOOL);

BOOL Is_Wow64() {
  BOOL bIsWow64 = FALSE;
  HMODULE hLib = LoadLibraryA("kernel32.dll");
  pIsWow64Process IsWow64Process =
      (pIsWow64Process)GetProcAddress(hLib, "IsWow64Process");
  IsWow64Process(GetCurrentProcess(), &bIsWow64);
  return bIsWow64;
}

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

PBYTE get_system_firmware(DWORD signature, DWORD table, PDWORD pBufferSize) {
  HMODULE hLib = LoadLibraryA("kernel32.dll");
  void *pointer = GetProcAddress(hLib, "GetSystemFirmwareTable");

  DWORD bufferSize = 4096;
  PBYTE firmwareTable = (PBYTE)(malloc(bufferSize));

  if (firmwareTable == NULL)
    return NULL;

  SecureZeroMemory(firmwareTable, bufferSize);

  pGetSystemFirmwareTable GetSystemFirmwareTable =
      (pGetSystemFirmwareTable)(pointer);

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

BOOL find_strdata(PBYTE needle, size_t needleLen, PBYTE haystack,
                  size_t haystackLen) {
  for (size_t i = 0; i < haystackLen - needleLen; i++) {
    if (memcmp(&haystack[i], needle, needleLen) == 0) {
      return 1;
    }
  }
  return 0;
}

BOOL is_FileExists(TCHAR *szPath) {
  DWORD dwAttrib = GetFileAttributes(szPath);
  return (dwAttrib != INVALID_FILE_ATTRIBUTES) &&
         !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY);
}

BOOL is_DirectoryExists(TCHAR *szPath) {
  DWORD dwAttrib = GetFileAttributes(szPath);
  return (dwAttrib != INVALID_FILE_ATTRIBUTES) &&
         (dwAttrib & FILE_ATTRIBUTE_DIRECTORY);
}

BOOL check_mac_addr(const TCHAR *szMac) {
  BOOL bResult = FALSE;
  PIP_ADAPTER_INFO pAdapterInfo, pAdapterInfoPtr;
  ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);

  pAdapterInfo = (PIP_ADAPTER_INFO)MALLOC(sizeof(IP_ADAPTER_INFO));
  if (pAdapterInfo == NULL) {
    return -1;
  }
  DWORD dwResult = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen);
  if (dwResult == ERROR_BUFFER_OVERFLOW) {
    FREE(pAdapterInfo);
    pAdapterInfo = (PIP_ADAPTER_INFO)MALLOC(ulOutBufLen);
    if (pAdapterInfo == NULL) {
      printf("Error allocating memory needed to call GetAdaptersinfo\n");
      return 1;
    }
    dwResult = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen);
  }
  if (dwResult == ERROR_SUCCESS) {
    CHAR szMacMultiBytes[4];
    for (int i = 0; i < 4; i++) {
      szMacMultiBytes[i] = (CHAR)szMac[i];
    }
    pAdapterInfoPtr = pAdapterInfo;
    while (pAdapterInfoPtr) {
      if (pAdapterInfoPtr->AddressLength == 6 &&
          !memcmp(szMacMultiBytes, pAdapterInfoPtr->Address, 3)) {
        bResult = TRUE;
        break;
      }
      pAdapterInfoPtr = pAdapterInfoPtr->Next;
    }
  }

  FREE(pAdapterInfo);

  return bResult;
}

BOOL vmware_reg_keys() {
  const TCHAR *szKeys[] = {
      _T("SOFTWARE\\VMware, Inc.\\VMware Tools"),
      _T("SOFTWARE\\VMware, Inc.\\VMware Drivers"),
      _T("SOFTWARE\\VMware, Inc.\\VMware VGAuth"),
      _T("SOFTWARE\\VMware, Inc.\\VMwareHostOpen"),
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
  PBYTE smbios = get_system_firmware((DWORD)('RSMB'), 0x0000, &smbiosSize);
  if (smbios != NULL) {
    PBYTE vmwareString = (PBYTE) "VMware";
    size_t vmwwareStringLen = 6;

    if (find_strdata(vmwareString, vmwwareStringLen, smbios, smbiosSize)) {
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

BOOL vmware_files() {
  BOOL flag = 0;
  const TCHAR *szPaths[] = {
      _T("System32\\drivers\\vmmouse.sys"),
      _T("System32\\drivers\\vmusb.sys"),
      _T("System32\\drivers\\vm3dmp.sys"),
      _T("System32\\drivers\\vmhgfs.sys"),
      _T("System32\\drivers\\vmmemctl.sys"),
      _T("System32\\drivers\\vmrawdsk.sys"),
      _T("System32\\drivers\\vmusbmouse.sys"),
  };

  WORD dwlength = sizeof(szPaths) / sizeof(szPaths[0]);
  TCHAR szWinDir[MAX_PATH] = _T("");
  TCHAR szPath[MAX_PATH] = _T("");
  PVOID OldValue = NULL;
  int wow64 = Is_Wow64();
  GetWindowsDirectory(szWinDir, MAX_PATH);
  if (wow64) {
    Wow64DisableWow64FsRedirection(&OldValue);
  }
  for (int i = 0; i < dwlength; i++) {
    PathCombine(szPath, szWinDir, szPaths[i]);
    if (is_FileExists(szPath)) {
      flag = 1;
      break;
    }
  }
  if (wow64)
    Wow64RevertWow64FsRedirection(&OldValue);
  return flag;
}

BOOL vmware_dir() {
  TCHAR szProgramFile[MAX_PATH];
  TCHAR szPath[MAX_PATH] = _T("");
  TCHAR szTarget[MAX_PATH] = _T("VMWare\\");

  int wow64 = Is_Wow64();
  if (wow64)
    ExpandEnvironmentStrings(_T("%ProgramW6432%"), szProgramFile,
                             ARRAYSIZE(szProgramFile));
  else
    SHGetSpecialFolderPath(NULL, szProgramFile, CSIDL_PROGRAM_FILES, FALSE);
  PathCombine(szPath, szProgramFile, szTarget);
  return is_DirectoryExists(szPath);
}

BOOL vmware_mac() {
  const TCHAR *szMac[][2] = {
      {_T("\x00\x05\x69"), _T("00:05:69")},
      {_T("\x00\x0C\x29"), _T("00:0c:29")},
      {_T("\x00\x1C\x14"), _T("00:1C:14")},
      {_T("\x00\x50\x56"), _T("00:50:56")},
  };

  WORD dwLength = sizeof(szMac) / sizeof(szMac[0]);

  for (int i = 0; i < dwLength; i++) {
    if (check_mac_addr(szMac[i][0]))
      return 1;
  }
  return 0;
}

BOOL vmware_service() {

  char *known_services[] = {"vmhgfs",       "VMMemCtl",     "vmmouse",
                            "vmrawdsk",     "vmusbmouse",   "vm3dmp",
                            "vm3dmp-debug", "vm3dmp-stats", "vm3dmp_loader"};
  ENUM_SERVICE_STATUS services[5000];
  int bytesNeeded, servicesNum;
  SC_HANDLE sc_handle = OpenSCManager(NULL, SERVICES_ACTIVE_DATABASE,
                                      SC_MANAGER_ENUMERATE_SERVICE);
  EnumServicesStatus(sc_handle, SERVICE_DRIVER, SERVICE_STATE_ALL, services,
                     sizeof(services), &bytesNeeded, &servicesNum, NULL);

  for (int i = 0; i < servicesNum; i++) {

    for (int j = 0; j < 9; j++)
      if (strcmp(services[i].lpServiceName, known_services[j]) == 0)
        return 1;
  }
  return 0;
}
void vmware() {
  char *s[2] = {"[ GOOD ]", "[ BAD  ]"};
  printf("[*] %-50s%10s\n", "VMWareRegKeys", s[vmware_reg_keys()]);
  printf("[*] %-50s%10s\n", "VMWareRegKeyValue", s[vmware_reg_key_value()]);
  printf("[*] %-50s%10s\n", "VMWareFirmware", s[vmware_firmware_SMBIOS()]);
  printf("[*] %-50s%10s\n", "VMWareProcess", s[vmware_processes()]);
  printf("[*] %-50s%10s\n", "VMWareMacAddress", s[vmware_mac()]);
  printf("[*] %-50s%10s\n", "VMWareFile", s[vmware_files()]);
  printf("[*] %-50s%10s\n", "VMWareDir", s[vmware_dir()]);
  printf("[*] %-50s%10s\n", "VMWareService", s[vmware_service()]);
}

