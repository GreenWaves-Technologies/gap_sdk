/****************************************************************************/
/* FreeRTOS functions includes. */
#include "FreeRTOS_util.h"

/* Demo utlities includes. */
#include "gap_common.h"
#include "gap_cluster.h"

/****************************************************************************/

/* Test task to test FreeRTOS port. */
void vTestClusterSynchroSW( void *parameters );

/* Utilities to control tasks. */
TaskHandle_t tasks[NBTASKS];
uint8_t taskSuspended;

/****************************************************************************/

/* Variables used. */
volatile uint8_t counter = 0;

SemaphoreHandle_t xSemaphoreDynamic = NULL;

/****************************************************************************/

/* Program Entry. */
int main( void )
{
    printf("\n\n\t *** Cluster Synchronisation SW Test ***\n\n");

    #if configSUPPORT_DYNAMIC_ALLOCATION == 1

    /* Init memory regions to alloc memory. */
    vPortDefineHeapRegions( xHeapRegions );

    BaseType_t xTask;
    TaskHandle_t xHandleDynamic = NULL;

    xTask = xTaskCreate(
        vTestClusterSynchroSW,
        "TestClusterSynchroSW",
        configMINIMAL_STACK_SIZE,
        NULL,
        tskIDLE_PRIORITY + 1,
        &xHandleDynamic
        );
    if( xTask != pdPASS )
    {
        printf("Task Dynamic is NULL !\n");
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

void Synchronisation_SW( void *arg )
{
    uint32_t coreid = __core_ID();
    /* Cores should break in order from the first loop. */
    for(;;)
    {
        if( xSemaphoreTake( xSemaphoreDynamic, 0 ) )
        {
            if( counter == coreid )
            {
                printf("Core %d breaking at TICK = %d.\n",
                       coreid, xTaskGetTickCount());
                counter++;
                xSemaphoreGive( xSemaphoreDynamic );
                break;
            }
            else
                xSemaphoreGive( xSemaphoreDynamic );
        }
    }
}
/*-----------------------------------------------------------*/

void Master_Entry( void *arg )
{
    CLUSTER_CoresFork(Synchronisation_SW, arg);
}
/*-----------------------------------------------------------*/

void vTestClusterSynchroSW( void *parameters )
{
    ( void ) parameters;
    char *taskname = pcTaskGetName( NULL );

    /* Binary Semaphore. */
    xSemaphoreDynamic = xSemaphoreCreateBinary();
    if( xSemaphoreDynamic == NULL )
        vTaskSuspend( NULL );
    xSemaphoreGive( xSemaphoreDynamic );

    printf("%s executing ClusterDMA function on Computing Cluster :\n\n", taskname);

    /* Power On Computing Cluster. */
    CLUSTER_Start( 0, NBCORES );

    /* Sending tasks to Computing Cluster. */
    CLUSTER_SendTask( 0, Master_Entry, 0 , 0 );

    /* Wait for the tasks to end. */
    CLUSTER_Wait( 0 );

    /* Shut down Computing Cluster. */
    CLUSTER_Stop( 0 );

    printf("Test success !\n");
    printf("%s on core %d of cluster %d suspending.\n",
           taskname, __core_ID(), __cluster_ID());

    vSemaphoreDelete( xSemaphoreDynamic );
    vTaskSuspend( NULL );
}
/*-----------------------------------------------------------*/
