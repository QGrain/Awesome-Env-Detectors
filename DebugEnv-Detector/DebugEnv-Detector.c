#include "DebugEnv-Detector.h"


void PrintStart();
void Quit();
int notify_warning(const char *env, const char *title, int confidence, int max_conf);


int main()
{
    int confidence = 0;
    int exit = 0;

    PrintStart();

    printf("\n==================== Detect from Windows API ===================\n");
    confidence += AntiDbgWinApiPrint();

    printf("\n=================== Detect from System Traces ==================\n");
    confidence += SysTracePrint();
    confidence += CheckPEBPrint();

    printf("\n================== Detect from Debugger Behavior ===============\n");
    confidence += AntiDbgBehaviorPrint();
    
    if(confidence > 2) {
        exit = notify_warning("debug environment", "Warning by DebugEnv-Detector", confidence, 16);
    }

    if(exit == 1) {
        goto Exit;
    }

    printf("\nYou choose to continue.\n");

Exit:
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


int notify_warning(const char *env, const char *title, int confidence, int max_conf)
{
	char msg[128] = {0};
	sprintf(msg, "You are in %s\nConfidence level = %d (max=%d)\nExit?", env, confidence, max_conf);
	int ret = MessageBox(0, msg, title, MB_YESNO | MB_ICONQUESTION);

	if(ret == IDNO) {
		return 0;
	}
	else if(ret == IDYES) {
		return 1;
	}
	else {
		return 0;
	}
}