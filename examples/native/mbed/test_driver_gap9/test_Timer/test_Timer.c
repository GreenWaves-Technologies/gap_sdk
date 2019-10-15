#include<stdio.h>
#include "cmsis.h"
#include "gap_common.h"

int main()
{
    printf("Fabric controller code execution for mbed_os Timer \n");

    Timer_Initialize(TIMER1, 0);

    Timer_Enable(TIMER1);

    printf("Timer CYCLE: %d\n" , (int)Timer_ReadCycle(TIMER1));

    Timer_Disable(TIMER1);

    printf("Test success\n");

    return 0;
}
