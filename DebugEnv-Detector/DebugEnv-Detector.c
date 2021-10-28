#include "feature-api/antidbgwinapi.h"
#include "feature-sys/systrace.h"
#include "feature-behavior/antidbgbehavior.h"
#include "checkpeb/checkpeb.h"


void PrintStart();


int main()
{
    PrintStart();

    printf("\n==================== Detect from Windows API ===================\n");
    AntiDbgWinApiPrint();

    printf("\n=================== Detect from System Traces ==================\n");
    SysTracePrint();

    printf("\n================== Detect from Debugger Behavior ===============\n");
    AntiDbgBehaviorPrint();
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