/****************************************************************************/
/**
 * This test changes frequency values of the FC core.
 * While voltage decreases from 1200mv to 1000mv, frequency decreases by
 * FREQUENCY_DELTA.
 */
/* FreeRTOS functions includes. */
#include "FreeRTOS_util.h"

/* Demo utlities includes. */
#include "gap_common.h"
#include "gap_cluster.h"

/****************************************************************************/

/* Test task to test FreeRTOS port. */
void vTestFreq( void *parameters );

/* Utilities to control tasks. */
TaskHandle_t tasks[NBTASKS];
uint8_t taskSuspended;

/****************************************************************************/

/* Variables used. */
#define FREQUENCY_DELTA  1000000

/****************************************************************************/

/* Program Entry. */
int main( void )
{
    printf("\n\n\t *** Cluster Frequency Test ***\n\n");

    #if configSUPPORT_DYNAMIC_ALLOCATION == 1
    BaseType_t xTask;
    TaskHandle_t xHandleDynamic = NULL;

    xTask = xTaskCreate(
        vTestFreq,
        "TestFreq",
        configMINIMAL_STACK_SIZE * 2,
        NULL,
        tskIDLE_PRIORITY + 1,
        &xHandleDynamic
        );
    if( xTask != pdPASS )
    {
        printf("TestFreq is NULL !\n");
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

void vTestFreq( void *parameters )
{
    ( void ) parameters;
    char *taskname = pcTaskGetName( NULL );
    uint32_t frequency = 1000000, voltage = 1000, get_frequency;

    printf("%s executing Frequency test on Fabric Controller :\n\n", taskname);

    printf("Setting frequency %d and voltage %d.\n", frequency, voltage);

    if( PMU_SetVoltage( voltage, 1 ) )
    {
        printf("Error changing voltage, check Frequency value!\n");
        printf("Test failed\n");
        exit( -1 );
    }

    if( FLL_SetFrequency( uFLL_SOC, frequency, 1 ) == -1 )
    {
        printf("Error changing freqency, check Voltage value!\n");
        printf("Test failed\n");
        exit( -1 );
    }
    printf("Frequency = %d , Voltage = %d mv\n", FLL_GetFrequency( uFLL_SOC ), voltage);

    for( volatile uint32_t i = 0; i < 10000; i++ );

    printf("Frequency = %d , Voltage = %d mv\n", FLL_GetFrequency( uFLL_SOC ), voltage);

    for( uint32_t voltage = DCDC_DEFAULT_NV; voltage >= DCDC_DEFAULT_LV; voltage -= 100 )
    {
        /* Set voltage */
        if( PMU_SetVoltage( voltage, 1 ) )
        {
            printf("Error changing voltage, check Frequency value!\n");
            printf("Test failed\n");
            exit( -1 );
        }

        for( uint32_t frequency = FLL_SoCMaxFreqAtV( voltage ); frequency >= FREQUENCY_DELTA; frequency -= FREQUENCY_DELTA )
        {
            if( FLL_SetFrequency( uFLL_SOC, frequency, 1 ) == -1 )
            {
                printf("Error changing freqency, check Voltage value!\n");
                printf("Test failed\n");
                exit( -1 );
            }
            get_frequency = FLL_GetFrequency( uFLL_SOC );
            printf("Set Freq = %d , Get freq = %d , Voltage = %d mv  ", frequency, get_frequency, voltage);
            if((int)frequency-(int)get_frequency > 0){
                int error = ((frequency-get_frequency)*1000)/frequency;
                printf("Error (per-mille): %d\n", error);
            }
            else{
                int error = ((get_frequency-frequency)*1000)/frequency;
                printf("Error (per-mille): %d\n", error);
            }

            for( volatile uint32_t i = 0; i < 10000; i++ );
        }
    }

    printf("Test success\n");
    printf("%s on core %d of cluster %d suspending(%d).\n",
           taskname, __core_ID(), __cluster_ID(), taskSuspended++);
    vTaskSuspend( NULL );
}
