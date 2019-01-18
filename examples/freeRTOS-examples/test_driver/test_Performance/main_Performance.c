/****************************************************************************/
/* FreeRTOS functions includes. */
#include "FreeRTOS_util.h"

/* Demo utlities includes. */
#include "gap_common.h"
#include "gap_performance.h"

/****************************************************************************/

/* Test task to test FreeRTOS port. */
void vTestPerformance( void *parameters );

/* Utilities to control tasks. */
TaskHandle_t tasks[NBTASKS];
uint8_t taskSuspended = 0;

/****************************************************************************/

/* Variables used. */
performance_t perf;

/****************************************************************************/

/* Program Entry. */
int main( void )
{
    printf("\n\n\t *** Performance Test ***\n\n");

    #if configSUPPORT_DYNAMIC_ALLOCATION == 1
    BaseType_t xTask;
    TaskHandle_t xHandleDynamic = NULL;

    xTask = xTaskCreate(
        vTestPerformance,
        "TestPerformance",
        configMINIMAL_STACK_SIZE * 1,
        NULL,
        tskIDLE_PRIORITY + 1,
        &xHandleDynamic
        );
    if( xTask != pdPASS )
    {
        printf("TestPerformance is NULL !\n");
        exit(0);
    }
    #endif //configSUPPORT_DYNAMIC_ALLOCATION

    tasks[0] = xHandleDynamic;

    /* Start the kernel.  From here on, only tasks and interrupts will run. */
    printf("\nScheduler starts !\n");
    vTaskStartScheduler();

    /* Exit FreeRTOS */
    return 0;
}
/*-----------------------------------------------------------*/

void vTestPerformance( void *parameters )
{
    char *taskname = pcTaskGetName( NULL );

    PERFORMANCE_Start(&perf, PCER_CYCLE_Msk | PCER_INSTR_Msk);
    printf("%s executing Performance test.\n", taskname);
    PERFORMANCE_Stop(&perf);

    printf("Total CYCLE (exclude core sleep cycles): %d\n" , PERFORMANCE_Get(&perf, PCER_CYCLE_Pos));
    printf("Total INSTR : %d\n" , PERFORMANCE_Get(&perf, PCER_INSTR_Pos));


    PERFORMANCE_Start(&perf, PERFORMANCE_USING_TIMER_MASK | PCER_INSTR_Msk);
    printf("%s executing Performance test.\n", taskname);
    PERFORMANCE_Stop(&perf);

    printf("Total CYCLE (include core sleep cycles): %d\n" , PERFORMANCE_Get(&perf, PERFORMANCE_USING_TIMER_SHIFT));
    printf("Total INSTR : %d\n" , PERFORMANCE_Get(&perf, PCER_INSTR_Pos));

    printf("Test success.\n");

    vTaskSuspend( NULL );
}
/*-----------------------------------------------------------*/
