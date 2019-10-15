/* PMSIS includes */
#include "pmsis.h"

/* Variables used. */
TaskHandle_t xHandler[4] = {NULL};

void vTaskTest( void *parameters )
{
    char *taskname = pcTaskGetName( NULL );
    uint32_t pid = 0, cid = 0, mepc = 0;
    uint32_t loop = ( uint32_t ) parameters;

    for( uint32_t i = 0; i < loop; i += 2 )
    {
        pid = procid();
        cid = clusterid();
        printf("%s : %d\t TICK = %d\t ProcID = %d\t ClusterID = %d\n",
               taskname, i, xTaskGetTickCount(), pid, cid);
        #if (configUSE_PREEMPTION == 0)
        taskYIELD();
        #endif  /* configUSE_PREEMPTION == 0 */
    }
    printf("%s suspending.\n", taskname);
    vTaskSuspend( NULL );
}

void test_tasks(void)
{
    printf("Entering main controller\n");

    BaseType_t xTask;
    char taskname[8];
    for (uint32_t i = 0; i < 4; i++)
    {
        sprintf(taskname, "Thread%d", i);
        xTask = xTaskCreate( vTaskTest, taskname, configMINIMAL_STACK_SIZE * 2,
                             ( void * ) (i * 100), tskIDLE_PRIORITY + 1, &xHandler[i] );
        if( xTask != pdPASS )
        {
            printf("%s is NULL !\n", taskname);
            pmsis_exit(0 - i);
        }
    }

    while( ( eTaskGetState(xHandler[0]) != eSuspended ) ||
           ( eTaskGetState(xHandler[1]) != eSuspended ) ||
           ( eTaskGetState(xHandler[2]) != eSuspended ) ||
           ( eTaskGetState(xHandler[3]) != eSuspended ) )
    {
        pi_yield();
    }

    printf("Test success !\n");

    pmsis_exit(0);
}

/* Program Entry. */
int main(void)
{
    printf("\n\n\t *** Tasks Test ***\n\n");
    return pmsis_kickoff((void *) test_tasks);
}
