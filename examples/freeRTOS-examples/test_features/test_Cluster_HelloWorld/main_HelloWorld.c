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

    #ifdef NO_SCHEDULER
    vTestHelloWorld( NULL );
    #else
    #if configSUPPORT_DYNAMIC_ALLOCATION == 1
    BaseType_t xTask;
    TaskHandle_t xHandler0 = NULL;

    xTask = xTaskCreate( vTestHelloWorld, "TestHelloWorld", configMINIMAL_STACK_SIZE * 2,
                         NULL, tskIDLE_PRIORITY + 1, &xHandler0 );
    if( xTask != pdPASS )
    {
        printf("TestHelloWorld is NULL !\n");
        exit(0);
    }
    #endif //configSUPPORT_DYNAMIC_ALLOCATION

    tasks[0] = xHandler0;

    /* Start the kernel.  From here on, only tasks and interrupts will run. */
    printf("\nScheduler starts !\n");
    vTaskStartScheduler();
    #endif
    /* Exit FreeRTOS */
    return 0;
}
/*-----------------------------------------------------------*/

void HelloWorld( void *arg )
{
    uint32_t coreid = 0, args = (uint32_t) arg;
    for(;;)
    {
        coreid = __core_ID();
        printf("Hello World from Core %d\t%d.\n", coreid,  args++);
        if( coreid % 2 )
        {
            printf("On core %d of cluster %d suspending.\n", __core_ID(), __cluster_ID());
            break;
        }
        if( args == 20 )
        {
            printf("On core %d of cluster %d suspending.\n", __core_ID(), __cluster_ID());
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
    #ifdef NO_SCHEDULER
    char *taskname = "Thread0";
    #else
    char *taskname = pcTaskGetName( NULL );
    #endif

    uint32_t coreid = __core_ID(), clusterid = __cluster_ID();

    printf("%s executing HelloWorld function on Computing Cluster :\n", taskname);
    printf("Hello World from Core %d on cluster %d.\n", coreid, clusterid);

    /* Power On Computing Cluster. */
    CLUSTER_Start( 0, NBCORES, 0 );

    /* Sending tasks to Computing Cluster. */
    CLUSTER_SendTask( 0, Master_Entry, 0, 0 );

    /* Wait for tasks to end and shut down Computing Cluster. */
    CLUSTER_Wait( 0 );
    CLUSTER_Stop( 0 );

    printf("Test success !\n");
    printf("%s on core %d of cluster %d suspending.\n",
           taskname, coreid, clusterid);

    #ifndef NO_SCHEDULER
    vTaskSuspend( NULL );
    #endif
}
/*-----------------------------------------------------------*/
