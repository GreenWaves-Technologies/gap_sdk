/****************************************************************************/
/* FreeRTOS functions includes. */
#include "FreeRTOS_util.h"

/* Demo utlities includes. */
#include "gap_common.h"
#include "gap_cluster.h"
#include "gap_dmamchan.h"

/****************************************************************************/

/* Test task to test FreeRTOS port. */
void vTestClusterDMA( void *parameters );

/* Utilities to control tasks. */
TaskHandle_t tasks[NBTASKS];
uint8_t taskSuspended;

/****************************************************************************/

/* Variables used. */
#define BUFFER_SIZE      2048
#define CORE_NUMBER      8

static char L2_IN[BUFFER_SIZE];
static char L2_OUT[BUFFER_SIZE];

/****************************************************************************/

/* Program Entry. */
int main( void )
{
    printf("\n\n\t *** Cluster DMA Test ***\n\n");

    #if configSUPPORT_DYNAMIC_ALLOCATION == 1

    /* Init memory regions to alloc memory. */
    vPortDefineHeapRegions( xHeapRegions );

    BaseType_t xTask;
    TaskHandle_t xHandleDynamic = NULL;

    xTask = xTaskCreate(
        vTestClusterDMA,
        "TestClusterDMA",
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

void ClusterDMA( void *arg )
{
    char *L1_BUFFER = ( char *) arg;
    uint32_t coreid = __core_ID(), start = 0, end = 0;

    if( !coreid )
    {
        printf("Core %d requesting DMA transfer from L2_IN to L1_BUFFER.\n", coreid);
        dma_req_t req;
        DMAMCHAN_Memcpy_1D( ( uint32_t ) L2_IN, ( uint32_t ) L1_BUFFER, BUFFER_SIZE, GAP_DMA_L22TCDM, &req );
        DMAMCHAN_WaitRequestEnd( &req );
        printf("Core %d : Transfer done.\n", coreid);
    }

    start = coreid * BUFFER_SIZE / NBCORES;
    end = start - 1 + BUFFER_SIZE / NBCORES;
    printf("Core %d From %d to %d.\n", coreid, start, end);
    /* Barrier synchronisation before starting to compute. */
    CLUSTER_SynchBarrier();

    for(uint32_t i = start; i <= end; i++)
        L1_BUFFER[i] = ( L1_BUFFER[i] * 3 );

    /* Barrier synchronisation to wait all cores. */
    CLUSTER_SynchBarrier();

    if( !coreid )
    {
        printf("Core %d requesting DMA transfer from L1_BUFFER to L2_OUT.\n", coreid);
        dma_req_t req;
        DMAMCHAN_Memcpy_1D( ( uint32_t ) L2_OUT, ( uint32_t ) L1_BUFFER, BUFFER_SIZE, GAP_DMA_TCDM2L2, &req );
        DMAMCHAN_WaitRequestEnd( &req );
        printf("Core %d : Transfer done.\n", coreid);
    }
}
/*-----------------------------------------------------------*/

void Master_Entry( void *arg )
{
    CLUSTER_CoresFork(ClusterDMA, arg);
}
/*-----------------------------------------------------------*/

void vTestClusterDMA( void *parameters )
{
    ( void ) parameters;
    char *taskname = pcTaskGetName( NULL );
    uint32_t errors = 0;
    printf("%s executing ClusterDMA function on Computing Cluster :\n\n", taskname);

    /* L2 Array Init. */
    for( uint32_t i = 0; i < BUFFER_SIZE; i++ )
        L2_IN[i] = i;

    /* Power On Computing Cluster. */
    CLUSTER_Start( 0, NBCORES );

    /* Allocating array in Cluster L1. */
    char *L1_BUFFER = L1_Malloc( BUFFER_SIZE );

    /* Sending tasks to Computing Cluster. */
    CLUSTER_SendTask( 0, Master_Entry, L1_BUFFER , 0 );

    /* Wait for the tasks to end. */
    CLUSTER_Wait( 0 );

    /* Free allocated array. */
    L1_MallocFree( L1_BUFFER, BUFFER_SIZE );

    /* Shut down Computing Cluster. */
    CLUSTER_Stop( 0 );

    /* Verification. */
    for( uint32_t i = 0; i < BUFFER_SIZE; i++ )
    {
        /* Need to cast before verification : using char! */
        if( L2_OUT[i] != ( char ) ( i * 3 ) )
            errors++;
    }

    if( !errors )
        printf("\n\nTest success !\n");
    else
        printf("\n\nTest failed with %d errors !\n", errors);

    printf("%s on core %d of cluster %d suspending(%d).\n",
           taskname, __core_ID(), __cluster_ID(), taskSuspended++);
    vTaskSuspend( NULL );
}
/*-----------------------------------------------------------*/
