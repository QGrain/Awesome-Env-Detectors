#include "feature-api/antidbgwinapi.h"
#include "feature-sys/systrace.h"
#include "feature-behavior/demo.h"
#include "checkpeb/checkpeb.h"


void PrintStart();


int main()
{
    PrintStart();

    printf("\n=============Detect from Windows API=============\n");
    AntiDbgWinApiPrint();

    printf("\n=============Detect from System Traces=============\n");
}

void PrintStart()
{
    printf("\nDebug Environment Dtector v1.0\n");
}