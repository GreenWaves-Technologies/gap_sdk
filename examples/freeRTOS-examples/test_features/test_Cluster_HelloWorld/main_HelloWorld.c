/****************************************************************************/
/* FreeRTOS functions includes. */
#include "FreeRTOS_util.h"

/* Demo utlities includes. */
#include "gap_common.h"
#include "gap_cluster.h"

/****************************************************************************/

/* Test task to test FreeRTOS port. */
void vTestHelloWorld( void *parameters );

/* Utilities to control tasks. */
TaskHandle_t tasks[NBTASKS];
uint8_t taskSuspended;

/****************************************************************************/

/* Variables used. */

/****************************************************************************/

/* Program Entry. */
int main( void )
{
    printf("\n\n\t *** Cluster Hello World Test ***\n\n");

    #if configSUPPORT_DYNAMIC_ALLOCATION == 1

    /* Init memory regions to alloc memory. */
    vPortDefineHeapRegions( xHeapRegions );

    BaseType_t xTask;
    TaskHandle_t xHandleDynamic = NULL;

    xTask = xTaskCreate(
        vTestHelloWorld,
        "TestHelloWorld",
        configMINIMAL_STACK_SIZE * 2,
        NULL,
        tskIDLE_PRIORITY + 1,
        &xHandleDynamic
        );
    if( xTask != pdPASS )
    {
        printf("TestHelloWorld is NULL !\n");
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

void HelloWorld( void *arg )
{
    char *taskname = pcTaskGetName( NULL );
    uint32_t coreid = 0, args = (uint32_t) arg;
    for(;;)
    {
        coreid = __core_ID();
        printf("Hello World from Core %d\t%d.\n", coreid,  args++);
        if( coreid % 2 )
        {
            printf("%s on core %d of cluster %d suspending.\n",
                   taskname, __core_ID(), __cluster_ID());
            break;
        }
        if( args == 20 )
        {
            printf("%s on core %d of cluster %d suspending.\n",
                   taskname, __core_ID(), __cluster_ID());
            break;
        }
    }
}
/*-----------------------------------------------------------*/

void Master_Entry( void *arg )
{
    CLUSTER_CoresFork( HelloWorld, arg );
}
/*-----------------------------------------------------------*/

void vTestHelloWorld( void *parameters )
{
    ( void ) parameters;
    char *taskname = pcTaskGetName( NULL );
    uint32_t coreid = __core_ID(), clusterid = __cluster_ID();

    printf("Hello World from Core %d on cluster %d.\n", coreid, clusterid);
    printf("%s executing HelloWorld function on Computing Cluster :\n", taskname);

    /* Power On Computing Cluster. */
    CLUSTER_Start( 0, NBCORES );

    /* Sending tasks to Computing Cluster. */
    CLUSTER_SendTask( 0, Master_Entry, 0, 0 );

    /* Wait for tasks to end and shut down Computing Cluster. */
    CLUSTER_Wait( 0 );
    CLUSTER_Stop( 0 );

    printf("Test success !\n");
    printf("%s on core %d of cluster %d suspending.\n",
           taskname, coreid, clusterid);
    vTaskSuspend( NULL );
}
/*-----------------------------------------------------------*/
