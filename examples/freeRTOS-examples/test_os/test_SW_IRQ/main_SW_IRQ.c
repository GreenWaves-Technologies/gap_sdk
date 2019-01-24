/****************************************************************************/
/* FreeRTOS functions includes. */
#include "FreeRTOS_util.h"

/* Demo utlities includes. */
#include "gap_common.h"

/****************************************************************************/

/* Test task to test FreeRTOS port. */
void vTestSwIrq0( void *parameters );
void vTestSwIrq1( void *parameters );

/* Utilities to control tasks. */
TaskHandle_t tasks[NBTASKS+1];
uint8_t taskSuspended;


#if configSUPPORT_STATIC_ALLOCATION == 1
StaticTask_t xTaskBuffer;
StackType_t xStack[ configMINIMAL_STACK_SIZE * 2 ];
#endif //configSUPPORT_STATIC_ALLOCATION

/****************************************************************************/

/* Variables used. */
#define IRQ_NUM  6

volatile uint32_t count = 0;

void sw_IRQ_handler()
{
    printf("%s handling SW_IRQ.\n", pcTaskGetName( NULL ));
    count++;
}

/* Handler wrapper  */
Handler_Wrapper_Light(sw_IRQ_handler);
/****************************************************************************/

/* Program Entry. */
int main( void )
{
    printf("\n\n\t *** Software IRQ Test ***\n\n");

    #if configSUPPORT_STATIC_ALLOCATION == 1
    TaskHandle_t xHandleStatic = NULL;

    xHandleStatic = xTaskCreateStatic(
        vTestSwIrq0,
        "TestSwIrq0",
        configMINIMAL_STACK_SIZE * 2,
        NULL,
        tskIDLE_PRIORITY + 1,
        xStack,
        &xTaskBuffer
        );
    if( xHandleStatic == NULL )
    {
        printf("TestSwIrq0 is NULL !\n");
        exit(0);
    }
    #endif //configSUPPORT_STATIC_ALLOCATION


    #if configSUPPORT_DYNAMIC_ALLOCATION == 1
    BaseType_t xTask;
    TaskHandle_t xHandleDynamic = NULL;

    xTask = xTaskCreate(
        vTestSwIrq1,
        "TestSwIrq1",
        configMINIMAL_STACK_SIZE * 2,
        NULL,
        tskIDLE_PRIORITY + 1,
        &xHandleDynamic
        );
    if( xTask != pdPASS )
    {
        printf("TestSwIrq1 is NULL !\n");
        exit(0);
    }
    #endif //configSUPPORT_DYNAMIC_ALLOCATION

    tasks[0] = xHandleStatic;
    tasks[1] = xHandleDynamic;

    /* Start the kernel.  From here on, only tasks and interrupts will run. */
    printf("\nScheduler starts !\n");
    vTaskStartScheduler();

    /* Exit FreeRTOS */
    return 0;
}
/*-----------------------------------------------------------*/

void vTestSwIrq0( void *parameters )
{
    ( void ) parameters;
    char *taskname = pcTaskGetName( NULL );
    uint32_t loop = 0;
    NVIC_SetVector(IRQ_NUM, (uint32_t)__handler_wrapper_light_sw_IRQ_handler);
    NVIC_EnableIRQ(IRQ_NUM);
    for(;;)
    {
        loop += 1;
        printf("%s : %d\n", taskname, loop);
        if( loop == 15 )
        {
            EU_FC_EVT_DemuxTrigSet(IRQ_NUM, 1);
        }
    }
}
/*-----------------------------------------------------------*/

void vTestSwIrq1( void *parameters )
{
    ( void ) parameters;
    char *taskname = pcTaskGetName( NULL );
    uint32_t loop = 0;
    for(;;)
    {
        loop += 5;
        printf("%s : %d\n", taskname, loop);
        if( count )
        {
            printf("Suspending %s\n", pcTaskGetName( tasks[0] ));
            vTaskSuspend( tasks[0] );
            printf("Suicide %s\n", taskname);
            printf("Test success\n");
            vTaskSuspend( NULL );
        }
        #if ( configUSE_PREEMPTION == 0 ) || ( ( configUSE_PREEMPTION == 1 ) && ( configUSE_TIME_SLICING == 0 ) )
        taskYIELD();
        #endif
    }
}
/*-----------------------------------------------------------*/
