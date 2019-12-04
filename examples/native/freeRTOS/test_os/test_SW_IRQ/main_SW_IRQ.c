/* PMSIS includes */
#include "pmsis.h"

/* App includes. */
#include "gap_handler_wrapper.h"

/* Variables used. */
TaskHandle_t xHandler[2] = {NULL};

#define IRQ_NUM  6
volatile uint32_t count = 0;

void sw_IRQ_handler();

/* Handler wrapper  */
Handler_Wrapper_Light(sw_IRQ_handler);
void sw_IRQ_handler()
{
    printf("%s handling SW_IRQ.\n", pcTaskGetName( NULL ));
    count++;
}

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
            hal_eu_fc_evt_demux_trig_set(IRQ_NUM, 1);
        }
    }
}

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
            printf("%s suspending %s\n", taskname, pcTaskGetName( xHandler[0] ));
            vTaskSuspend( xHandler[0] );
            printf("%s suspending\n", taskname);
            vTaskSuspend( NULL );
        }
        #if ( configUSE_PREEMPTION == 0 ) || ( ( configUSE_PREEMPTION == 1 ) && ( configUSE_TIME_SLICING == 0 ) )
        taskYIELD();
        #endif
    }
}

void test_sw_irq(void)
{
    printf("Entering main controller\n");

    BaseType_t xTask;
    xTask = xTaskCreate( vTestSwIrq0, "TestSwIrq0", configMINIMAL_STACK_SIZE * 2,
                         NULL, tskIDLE_PRIORITY + 1, &xHandler[0] );
    if( xTask != pdPASS )
    {
        printf("TestSwIrq0 is NULL !\n");
        pmsis_exit(-1);
    }

    xTask = xTaskCreate( vTestSwIrq1, "TestSwIrq1", configMINIMAL_STACK_SIZE * 2,
                         NULL, tskIDLE_PRIORITY + 1, &xHandler[1] );
    if( xTask != pdPASS )
    {
        printf("TestSwIrq1 is NULL !\n");
        pmsis_exit(-2);
    }

    while( ( eTaskGetState(xHandler[0]) != eSuspended ) ||
           ( eTaskGetState(xHandler[1]) != eSuspended ) )
    {
        pi_yield();
    }

    printf("Test success !\n");

    pmsis_exit(0);
}

/* Program Entry. */
int main(void)
{
    printf("\n\n\t *** SW IRQ Test ***\n\n");
    return pmsis_kickoff((void *) test_sw_irq);
}
