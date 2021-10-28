#include "VirtualEnv-Detector.h"


void PrintStart();
void Quit();


int main()
{
    PrintStart();
#ifdef _WIN32
    printf("\n==================== Detect Windows Virtual Env ===================\n");
    vmware();
#else
    printf("\n==================== Detect Linux Virtual Env  ====================\n");
    CkVmLinuxPrint();
#endif
    Quit();
}

void PrintStart()
{
    printf("\n"
"__      ___      _               _   ______              _____       _            _                     \n"
"\\ \\    / (_)    | |             | | |  ____|            |  __ \\     | |          | |                 \n"
" \\ \\  / / _ _ __| |_ _   _  __ _| | | |__   _ ____   __ | |  | | ___| |_ ___  ___| |_ ___  _ __       \n"
"  \\ \\/ / | | '__| __| | | |/ _` | | |  __| | '_ \\ \\ / / | |  | |/ _ \\ __/ _ \\/ __| __/ _ \\| '__| \n"
"   \\  /  | | |  | |_| |_| | (_| | | | |____| | | \\ V /  | |__| |  __/ ||  __/ (__| || (_) | |         \n"
"    \\/   |_|_|   \\__|\\__,_|\\__,_|_| |______|_| |_|\\_/   |_____/ \\___|\\__\\___|\\___|\\__\\___/|_|\n\n");

    printf("\nDebug Environment Detector v1.0, by Group 6\n");
}


void Quit()
{
    printf("\nWe done here, have a good day~ (Press any key to quit)\n");
    getchar();
}