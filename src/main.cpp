#include <cstdlib>     //exit()
#include <ctime>
#include <cstdio>
#include <string>

#include <signal.h>     //signal()

#include "ADS1256.h"

void  Handler(int signo)
{
    //System Exit
    DEV_ModuleExit();

    exit(0);
}

int main(void)
{
    UDOUBLE ADC[8],i;
    DEV_ModuleInit();

    // Exception handling:ctrl + c
    signal(SIGINT, Handler);

    if(ADS1256_init() == 1){
        DEV_ModuleExit();
        exit(0);
    }

    while(true) {
        
        ADS1256_GetAll(ADC);

        printf("%f,", ADC[0]*5.0/0x7fffff);
        printf("%f,", ADC[1]*5.0/0x7fffff);
        printf("%f,", ADC[2]*5.0/0x7fffff);
        printf("%f,", ADC[3]*5.0/0x7fffff);
        printf("%f,", ADC[4]*5.0/0x7fffff);
        printf("%f", ADC[5]*5.0/0x7fffff);
        printf("\n");
    }

    return 0;
}
