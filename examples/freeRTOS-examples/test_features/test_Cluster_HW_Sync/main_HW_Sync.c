/****************************************************************************/
/* FreeRTOS functions includes. */
#include "FreeRTOS_util.h"

/* Demo utlities includes. */
#include "gap_common.h"
#include "gap_cluster.h"

/****************************************************************************/

/* Test task to test FreeRTOS port. */
void vTestClusterHWSync( void *parameters );

/* Utilities to control tasks. */
TaskHandle_t tasks[NBTASKS];
uint8_t taskSuspended;

/****************************************************************************/

/* Variables used. */
typedef struct
{
    void *func;
    void *arg;
} arg_t;

/****************************************************************************/

/* Program Entry. */
int main( void )
{
    printf("\n\n\t *** Cluster HW Synchronisation Test ***\n\n");

    #if configSUPPORT_DYNAMIC_ALLOCATION == 1
    BaseType_t xTask;
    TaskHandle_t xHandleDynamic = NULL;

    xTask = xTaskCreate(
        vTestClusterHWSync,
        "TestClusterHWSync",
        configMINIMAL_STACK_SIZE * 2,
        NULL,
        tskIDLE_PRIORITY + 1,
        &xHandleDynamic
        );
    if( xTask != pdPASS )
    {
        printf("TestClusterHWSync is NULL !\n");
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

void HW_Synchronisation( void *arg )
{
    uint32_t *val = ( uint32_t * ) arg;
    uint32_t coreid = __core_ID();

    val[coreid] = 0;
    for( uint32_t i = 0; i < coreid; i++ )
        val[coreid]++;

    CLUSTER_SynchBarrier();
    val[coreid] = val[coreid] + val[(coreid == 7) ? 0 : coreid+1];
    printf("Core %d : %d\n", coreid, val[coreid]);
}
/*-----------------------------------------------------------*/

void No_HW_Synchronisation( void *arg )
{
    uint32_t *val = ( uint32_t * ) arg;
    uint32_t coreid = __core_ID();

    val[coreid] = 0;
    for( uint32_t i = 0; i < coreid; i++ )
        val[coreid]++;

    val[coreid] = val[coreid] + val[(coreid == 7) ? 0 : coreid+1];
    printf("Core %d : %d\n", coreid, val[coreid]);
}
/*-----------------------------------------------------------*/

void Master_Entry( void *arg )
{
    CLUSTER_CoresFork( ( ( arg_t * ) arg )->func, ( ( arg_t * ) arg )->arg );
}
/*-----------------------------------------------------------*/

uint32_t vErrorCheck( uint32_t *ref, uint32_t *cmp, uint32_t size )
{
    uint32_t errors = 0;
    for( uint32_t i = 0; i < size; i++ )
    {
        if( ref[i] != cmp[i] )
            errors++;
    }
    return errors;
}
/*-----------------------------------------------------------*/
void vTestClusterHWSync( void *parameters )
{
    ( void ) parameters;
    char *taskname = pcTaskGetName( NULL );
    uint32_t errors = 0;

    printf("%s executing Hardware Barrier Sync function on Computing Cluster :\n\n", taskname);

    uint32_t *ref = pvPortMalloc( NBCORES * sizeof( uint32_t ) );
    uint32_t *cmp = pvPortMalloc( NBCORES * sizeof( uint32_t ) );

    printf("Values should be :\n");
    for( uint32_t i = 0; i < 8; i++ )
    {
        if( i != 7 )
            ref[i] = i + i+1;
        else
            ref[i] = i + 0;
        printf("Core %d : %d\n", i, ref[i]);
    }
    /* Power On Computing Cluster. */
    CLUSTER_Start( 0, NBCORES, 0 );

    printf("\nWithout HW Barrier.\n");
    arg_t arg = { .func = ( void * ) No_HW_Synchronisation, .arg = ( void * ) cmp };
    /* Sending tasks to Computing Cluster. */
    CLUSTER_SendTask( 0, Master_Entry, ( void * ) &arg, 0 );

    /* Wait for the tasks to end. */
    CLUSTER_Wait( 0 );
    errors = vErrorCheck( ref, cmp, NBCORES );
    printf("Without HW Barrier : %d error(s).\n", errors);

    printf("\nWith HW Barrier.\n");
    arg.func = ( void * ) HW_Synchronisation;
    /* Sending tasks to Computing Cluster. */
    CLUSTER_SendTask( 0, Master_Entry, ( void * ) &arg, 0 );

    /* Wait for the tasks to end. */
    CLUSTER_Wait( 0 );

    /* Shut down Computing Cluster. */
    CLUSTER_Stop( 0 );
    errors = vErrorCheck( ref, cmp, NBCORES );
    printf("With HW Barrier : %d error(s).\n", errors);

    if( errors )
        printf("Test failed !\n");
    else
        printf("Test success !\n");
    printf("%s on core %d of cluster %d suspending.\n",
           taskname, __core_ID(), __cluster_ID());

    vTaskSuspend( NULL );
}
/*-----------------------------------------------------------*/
