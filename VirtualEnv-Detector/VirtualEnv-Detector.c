#include "VirtualEnv-Detector.h"


void PrintStart();
void Quit();
int notify_warning(int confidence);


int main()
{
    PrintStart();
    int confidence = 0;
    int quit = 0;
#ifdef _WIN32
    printf("\n==================== Detect Windows Virtual Env ===================\n");
    confidence = vmware();
    
#else
    printf("\n==================== Detect Linux Virtual Env  ====================\n");
    confidence = CkVmLinuxPrint();
    if(confidence > 0) {
        quit = notify_warning(confidence);
    }

    if(quit == 1) {
        goto Quit;
    }
#endif

    printf("\nYou choose to continue.\n");
    
Quit:
    Quit();
}


int notify_warning(int confidence)
{
    char cmd[512] = {0};
    sprintf(cmd, "zenity --question --width=240 --height=120 --title=\'Warning by VirtualEnv-Detector\' --text=\'You are in Virtual Environment.\nconfidence level = %d (max=3)\nExit\';echo $?;", confidence);
    char output[255] = {0};
    FILE *fp;

    //0-是 1-否
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