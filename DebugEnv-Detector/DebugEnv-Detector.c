#include "DebugEnv-Detector.h"


void PrintStart();
void Quit();


int main()
{
    PrintStart();

    printf("\n==================== Detect from Windows API ===================\n");
    AntiDbgWinApiPrint();

    printf("\n=================== Detect from System Traces ==================\n");
    SysTracePrint();
    CheckPEBPrint();

    printf("\n================== Detect from Debugger Behavior ===============\n");
    AntiDbgBehaviorPrint();

    Quit();
}


void PrintStart()
{
    printf("\n"
" _____       _                   ______              _____       _            _                    \n"
"|  __ \\     | |                 |  ____|            |  __ \\     | |          | |                 \n"
"| |  | | ___| |__  _   _  __ _  | |__   _ ____   __ | |  | | ___| |_ ___  ___| |_ ___  _ __        \n"
"| |  | |/ _ \\ '_ \\| | | |/ _` | |  __| | '_ \\ \\ / / | |  | |/ _ \\ __/ _ \\/ __| __/ _ \\| '__|\n"
"| |__| |  __/ |_) | |_| | (_| | | |____| | | \\ V /  | |__| |  __/ ||  __/ (__| || (_) | |         \n"
"|_____/ \\___|_.__/ \\__,_|\\__, | |______|_| |_|\\_/   |_____/ \\___|\\__\\___|\\___|\\__\\___/|_|\n"
"                          __/ |                                                                    \n"
"                         |___/                                                              \n");

    printf("\nDebug Environment Detector v1.0, by Group 6\n");
}


void Quit()
{
    printf("\nWe done here, have a good day~ (Press any key to quit)\n");
    getchar();
}