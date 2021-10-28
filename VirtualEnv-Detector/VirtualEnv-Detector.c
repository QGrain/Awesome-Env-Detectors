#include "VirtualEnv-Detector.h"


void PrintStart();
void Quit();
#ifdef _WIN32
int notify_warning_win(const char *env, const char *title, int confidence, int max_conf);
#else
int notify_warning_linux(const char *env, const char *title, int confidence, int max_conf);
#endif

int main()
{
    PrintStart();
    int confidence = 0;
    int exit = 0;
#ifdef _WIN32
    printf("\n==================== Detect Windows Virtual Env ===================\n");
    confidence = vmware();
    if(confidence > 0) {
        exit = notify_warning_win("virtual environment", "Warning by VirtualEnv-Detector", confidence, 4);
    }

    if(exit == 1) {
        goto Exit;
    }

#else
    printf("\n==================== Detect Linux Virtual Env  ====================\n");
    confidence = CkVmLinuxPrint();
    if(confidence > 0) {
        exit = notify_warning_linux("virtual environment", "Warning by VirtualEnv-Detector", confidence, 3);
    }

    if(exit == 1) {
        goto Exit;
    }
#endif

    printf("\nYou choose to continue.\n");
    
Exit:
    Quit();
}

#ifdef _WIN32
int notify_warning_win(const char *env, const char *title, int confidence, int max_conf)
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
#else
int notify_warning_linux(const char *env, const char *title, int confidence, int max_conf)
{
    char cmd[512] = {0};
    sprintf(cmd, "zenity --question --width=240 --height=120 --title=\'%s\' --text=\'You are in %s.\nconfidence level = %d (max=3)\nExit\';echo $?;", title, env, confidence);
    char output[255] = {0};
    FILE *fp;

    //return 0-是, 1-否
    if((fp = popen(cmd, "r")) != NULL) {
        char *ret = fgets(output, 255, fp);
        pclose(fp);
    }

    if(output[0] == '1') {
        return 0;
    }
    else {
        return 1;
    }
}
#endif


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