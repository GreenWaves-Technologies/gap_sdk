/****************************************************************************/
/* FreeRTOS functions includes. */
#include "FreeRTOS_util.h"

/* Demo utlities includes. */
#include "gap_common.h"
#include "gap_cluster.h"

/****************************************************************************/

/* Test task to test FreeRTOS port. */
void vTestMalloc( void *parameters );

/* Utilities to control tasks. */
TaskHandle_t tasks[NBTASKS];
uint8_t taskSuspended = 0;

/****************************************************************************/

/* Variables used. */

/****************************************************************************/

/* Program Entry. */
int main( void )
{
    printf("\n\n\t *** Cluster Hello World Test ***\n\n");

    #if configSUPPORT_DYNAMIC_ALLOCATION == 1
    BaseType_t xTask;
    TaskHandle_t xHandleDynamic = NULL;

    xTask = xTaskCreate(
        vTestMalloc,
        "TestMalloc",
        configMINIMAL_STACK_SIZE * 2,
        NULL,
        tskIDLE_PRIORITY + 1,
        &xHandleDynamic
        );
    if( xTask != pdPASS )
    {
        printf("TestFCMalloc is NULL !\n");
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
    uint8_t *p = NULL;

    /* Allocate in FC. */
    for( uint32_t i = 0; i < 1000; i++ )
    {
        p = ( uint8_t * ) L1_Malloc( 2048 );
        if( p == NULL )
            break;
        printf("Core %d L1_Malloc : %p\n", __core_ID(), p);
    }
    if( p != NULL )
        L1_MallocFree( p, 2048 );

    /* Allocate in L2. */
    for( uint32_t i = 0; i < 1000; i++ )
    {
        p = ( uint8_t * ) pvPortMalloc( 2048*8 );
        if( p == NULL )
            break;
        printf("Core %d L2_Malloc from Cluster : %p\n", __core_ID(), p);
    }
    if( p != NULL )
        vPortFree( p );
}
/*-----------------------------------------------------------*/

void Master_Entry( void *arg )
{
    L1_MallocInit();
    CLUSTER_CoresFork( HelloWorld, arg );
}
/*-----------------------------------------------------------*/

void vTestMalloc( void *parameters )
{
    ( void ) parameters;
    char *taskname = pcTaskGetName( NULL );
    uint32_t coreid = __core_ID(), clusterid = __cluster_ID();
    uint8_t *p = NULL;
    printf("%s : Hello World from Core %d on cluster %d.\n", taskname, coreid, clusterid);

    FC_MallocInit();

    /* Allocate in FC. */
    for( uint32_t i = 0; i < 1000; i++ )
    {
        p = ( uint8_t * ) FC_Malloc( 2048 );
        if( p == NULL )
            break;
        printf("FC_Malloc : %p\n", p);
    }
    if( p != NULL )
        FC_MallocFree( p, 2048 );

    /* Power On Computing Cluster. */
    CLUSTER_Start( 0, 1, 0 );

    /* Sending tasks to Computing Cluster. */
    CLUSTER_SendTask( 0, Master_Entry, 0, 0 );

    /* Wait for tasks to end and shut down Computing Cluster. */
    CLUSTER_Wait( 0 );
    CLUSTER_Stop( 0 );

    printf("Test success !\n");
    vTaskSuspend( NULL );
}
/*-----------------------------------------------------------*/
