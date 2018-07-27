/****************************************************************************/
/* FreeRTOS functions includes. */
#include "FreeRTOS_util.h"

/* Demo utlities includes. */
//#include "stdlib.h"
#include "gap_common.h"
#include "gap_cluster.h"

/****************************************************************************/

/* Test task to test FreeRTOS port. */
void vTestClusterFreq( void *parameters );

/* Utilities to control tasks. */
TaskHandle_t tasks[NBTASKS];
uint8_t taskSuspended;

/****************************************************************************/

/* Variables used. */
#define FREQUENCY_DELTA  10000000

/****************************************************************************/

/* Program Entry. */
int main( void )
{
    printf("\n\n\t *** Cluster Frequency Test ***\n\n");

    #if configSUPPORT_DYNAMIC_ALLOCATION == 1

    /* Init memory regions to alloc memory. */
    vPortDefineHeapRegions( xHeapRegions );

    BaseType_t xTask;
    TaskHandle_t xHandleDynamic = NULL;

    xTask = xTaskCreate(
        vTestClusterFreq,
        "TestClusterFreq",
        configMINIMAL_STACK_SIZE,
        NULL,
        tskIDLE_PRIORITY + 1,
        &xHandleDynamic
        );
    if( xTask != pdPASS )
    {
        printf("TestClusterFreq is NULL !\n");
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


void Hello( void *arg )
{
    for( volatile uint32_t i = 0; i < 10000; i++ );
}

void Master_Entry( void *arg )
{
    CLUSTER_CoresFork( Hello, arg );
}

void vTestClusterFreq( void *parameters )
{
    ( void ) parameters;
    char *taskname = pcTaskGetName( NULL );

    printf("%s executing Cluster Frequency test on Computing Cluster :\n\n", taskname);

    /* Cluster Start - Power on */
    CLUSTER_Start( 0, NBCORES );

    for( uint32_t voltage = DCDC_DEFAULT_NV; voltage >= DCDC_DEFAULT_LV; voltage -= 100 )
    {
        /* Set voltage */
        if( PMU_SetVoltage( voltage, 1 ) )
        {
            printf("Error of changing voltage, check Frequency value!\n");
            printf("Test failed\n");
            exit( -1 );
        }

        for( uint32_t frequency = FLL_SoCMaxFreqAtV( voltage ); frequency >= FREQUENCY_DELTA; frequency -= FREQUENCY_DELTA )
        {
            if( FLL_SetFrequency( uFLL_SOC, frequency, 1 ) == -1 )
            {
                printf("Error of changing freqency, check Voltage value!\n");
                printf("Test failed\n");
                exit( -1 );
            }
            printf("Frequency = %d , Voltage = %d mv\t", FLL_GetFrequency( uFLL_SOC ), voltage);

            for( volatile uint32_t i = 0; i < 10000; i++ );

            for( uint32_t cl_freq = FLL_ClusterMaxFreqAtV( voltage ); cl_freq >= FREQUENCY_DELTA; cl_freq -= FREQUENCY_DELTA )
            {
                if( FLL_SetFrequency( uFLL_CLUSTER, cl_freq, 1 ) == -1 )
                {
                    printf("Error of changing freqency, check Voltage value!\n");
                    printf("Test failed\n");
                    exit( -1 );
                }

                for( uint32_t i =0; i < 5; i++ )
                {
                    /* FC send a task to Cluster */
                    CLUSTER_SendTask( 0, Master_Entry, NULL, 0 );
                }
                CLUSTER_Wait( 0 );
                printf("Cluster Frequency = %d Hz\n", FLL_GetFrequency( uFLL_CLUSTER ));
            }

        }
    }


    printf("Test success\n");

    printf("%s on core %d of cluster %d suspending(%d).\n",
           taskname, __core_ID(), __cluster_ID(), taskSuspended++);
    vTaskSuspend( NULL );
}
