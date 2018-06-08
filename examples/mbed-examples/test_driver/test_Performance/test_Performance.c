#include <stdio.h>
#include <string.h>
#include "cmsis.h"
#include "gap_performance.h"

performance_t perf;

int main()
{
    PERFORMANCE_Start(&perf, PCER_CYCLE_Msk | PCER_INSTR_Msk);

    printf("Fabric controller code execution for mbed_os Performance \n");

    PERFORMANCE_Stop(&perf);

    printf("Total CYCLE : %d\n" , (int)PERFORMANCE_Get(&perf, PCER_CYCLE_Pos));
    printf("Total INSTR : %d\n" , (int)PERFORMANCE_Get(&perf, PCER_INSTR_Pos));
    return 0;
}
