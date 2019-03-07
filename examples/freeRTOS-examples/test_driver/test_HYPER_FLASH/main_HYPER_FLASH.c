/****************************************************************************/
/* FreeRTOS functions includes. */
#include "FreeRTOS_util.h"

/* Demo utlities includes. */
#include "hyperbus_io.h"

/****************************************************************************/

/* Test task to test FreeRTOS port. */
void vTestHyperFlash( void *parameters );

/* Utilities to control tasks. */
TaskHandle_t tasks[NBTASKS];
uint8_t taskSuspended;

/****************************************************************************/

/* Variables used. */
#define BUF_SIZE   ( 256 )
#define START_ADDR ( 0x2000 )

/* The high 16 bits for txHyperbusSamples is useless */
GAP_L2_DATA uint16_t txSamples[BUF_SIZE];
GAP_L2_DATA uint16_t rxSamples[BUF_SIZE];

/****************************************************************************/

/* Program Entry. */
int main( void )
{
    printf("\n\n\t *** HyperFlash Test ***\n\n");

    #if configSUPPORT_DYNAMIC_ALLOCATION == 1
    BaseType_t xTask;
    TaskHandle_t xHandleDynamic = NULL;

    xTask = xTaskCreate(
        vTestHyperFlash,
        "TestHyperFlash",
        configMINIMAL_STACK_SIZE,
        NULL,
        tskIDLE_PRIORITY + 1,
        &xHandleDynamic
        );
    if( xTask != pdPASS )
    {
        printf("TestHyperFlash is NULL !\n");
        vTaskDelete( xHandleDynamic );
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

void vTestHyperFlash( void *parameters )
{
    ( void ) parameters;
    char *taskname = pcTaskGetName( NULL );
    int32_t errors = 0;

    printf("%s executing HyperFlash function :\n\n", taskname);

    HYPERBUS_IO_Init( uHYPERBUS_Flash );

    printf("clk  = %d \n" "clkn = %d \n" "rwds = %d \n"
           "dq0  = %d \n" "dq1  = %d \n" "dq2  = %d \n" "dq3  = %d \n"
           "dq4  = %d \n" "dq5  = %d \n" "dq6  = %d \n" "dq7  = %d \n",
           HYPERBUS_CLK, HYPERBUS_CLKN, HYPERBUS_RWDS,
           HYPERBUS_DQ0, HYPERBUS_DQ1, HYPERBUS_DQ2, HYPERBUS_DQ3,
           HYPERBUS_DQ4, HYPERBUS_DQ5, HYPERBUS_DQ6, HYPERBUS_DQ7);

    /* Initialize the samples */
    for( uint32_t i = 0; i < BUF_SIZE; i++ )
        txSamples[i] = i;

    /* Erase chip */
    HYPERBUS_IO_Erase( START_ADDR );
    HYPERBUS_IO_Sync();
    printf("Erase finished.\n");

    /* Write buffer no burst */
    HYPERBUS_IO_Write( START_ADDR, BUF_SIZE * sizeof( txSamples[0] ), txSamples, uHYPERBUS_Flash, uHYPERBUS_Mem_Access );
    HYPERBUS_IO_Sync();
    printf("Write finished.\n");

    /* Read buffer burst */
    HYPERBUS_IO_Read( START_ADDR, BUF_SIZE * sizeof( rxSamples[0] ), rxSamples, uHYPERBUS_Flash, uHYPERBUS_Mem_Access );
    printf("Read finished.\n");

    /* Release hyperbus driver */
    HYPERBUS_IO_Deinit();

    for( uint32_t i = 0; i < BUF_SIZE; i++ )
    {
        if( rxSamples[i] != ( uint16_t ) i )
            errors++;
        printf(" %2x ", rxSamples[i]);
    }

    if( errors )
        printf("\nTest failed with %ld errors !\n", errors);
    else
        printf("\nTest success with %ld errors !\n", errors);

    printf("\n%s on core %ld of cluster %ld suspending(%d).\n",
           taskname, __core_ID(), __cluster_ID(), taskSuspended++);
    vTaskSuspend( NULL );
}
/*-----------------------------------------------------------*/
