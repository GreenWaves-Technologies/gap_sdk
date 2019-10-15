#include <stdio.h>
#include "cmsis.h"
#include "gap_performance.h"
performance_t perf;

int main()
{
    PERFORMANCE_Start(&perf, PCER_CYCLE_Msk | PCER_INSTR_Msk);

    printf("Fabric controller code execution for mbed_os Performance \n");

    PERFORMANCE_Stop(&perf);

    printf("Total CYCLE (exclude core sleep cycles): %d\n" , (int)PERFORMANCE_Get(&perf, PCER_CYCLE_Pos));
    printf("Total INSTR : %d\n" , (int)PERFORMANCE_Get(&perf, PCER_INSTR_Pos));




    PERFORMANCE_Start(&perf, PERFORMANCE_USING_TIMER_MASK | PCER_INSTR_Msk);

    printf("Fabric controller code execution for mbed_os Performance \n");

    PERFORMANCE_Stop(&perf);

    printf("Total CYCLE (include core sleep cycles): %d\n" , (int)PERFORMANCE_Get(&perf, PERFORMANCE_USING_TIMER_SHIFT));
    printf("Total INSTR : %d\n" , (int)PERFORMANCE_Get(&perf, PCER_INSTR_Pos));

    printf("Test success\n");

    return 0;
}
