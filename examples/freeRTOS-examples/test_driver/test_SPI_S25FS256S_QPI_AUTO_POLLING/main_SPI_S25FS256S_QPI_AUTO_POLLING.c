/****************************************************************************/
/* FreeRTOS functions includes. */
#include "FreeRTOS_util.h"

/* Demo utlities includes. */
#include "spi_flash_io.h"

/****************************************************************************/

/* Test task to test FreeRTOS port. */
void vTestSPI_QPI( void *parameters );

/* Utilities to control tasks. */
TaskHandle_t tasks[NBTASKS];
uint8_t taskSuspended;

/****************************************************************************/

/* Variables used. */
#define NB_PAGE  ( 1 )
#define BUF_SIZE ( 512 * NB_PAGE )

GAP_L2_DATA uint8_t rxBuffer[BUF_SIZE];
GAP_L2_DATA uint8_t txBuffer[BUF_SIZE];

/****************************************************************************/

/* Program Entry. */
int main( void )
{
    printf("\n\n\t *** Driver Test : SPI QPI Auto_polling ***\n\n");

    #if configSUPPORT_DYNAMIC_ALLOCATION == 1
    BaseType_t xTask;
    TaskHandle_t xHandleDynamic = NULL;

    xTask = xTaskCreate(
        vTestSPI_QPI,
        "TestSPI_QPI",
        configMINIMAL_STACK_SIZE,
        NULL,
        tskIDLE_PRIORITY + 1,
        &xHandleDynamic
        );
    if( xTask != pdPASS )
    {
        printf("TestSPI_QPI is NULL !\n");
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

void vTestSPI_QPI( void *parameters )
{
    ( void ) parameters;
    char *taskname = pcTaskGetName( NULL );
    int32_t errors = 0;

    printf("%s executing SPI_QPI Polling function :\n\n", taskname);

    SPI_FLASH_Init( SPI0_SDIO0, SPI0_SDIO1, SPI0_SCLK, SPI0_CSN0, 32, 0, 10000000, 1, 1 );

    printf("SPI0_MOSI  = %d \n" "SPI0_MISO  = %d \n" "SPI0_SDIO2 = %d \n"
           "SPI0_SDIO3 = %d \n" "SPI0_CSN0  = %d \n" "SPI0_SCLK  = %d \n",
           SPI0_SDIO0, SPI0_SDIO1, SPI0_SDIO2,
           SPI0_SDIO3, SPI0_CSN0, SPI0_SCLK);

    /* Initialize the buffer. */
    for( uint32_t i = 0; i< BUF_SIZE; i++ )
        txBuffer[i] = i;

    /* Erase page */
    SPI_FLASH_Erase( 0x400, BUF_SIZE );
    printf("Flash erased ... \n");

    /* Write buffer */
    SPI_FLASH_Write( 0x400, BUF_SIZE, txBuffer );
    printf("Flash written ... \n");

    /* Read back buffer */
    SPI_FLASH_Read( 0x400, BUF_SIZE, rxBuffer );
    printf("Flash read ... \n");

    /* Release SPI driver */
    SPI_FLASH_Deinit();

    for( uint32_t i = 0; i < BUF_SIZE; i++ )
    {
        if( rxBuffer[i] != ( char ) ( i ) )
            errors++;
        printf(" %2x ", rxBuffer[i]);
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
