/****************************************************************************/
/* FreeRTOS functions includes. */
#include "FreeRTOS_util.h"

/* Demo utlities includes. */
#include "PinNames.h"
#include "PeripheralPins.h"
#include "gap_common.h"
#include "pinmap.h"

/****************************************************************************/

/* Test task to test FreeRTOS port. */
void vTestDriverI2C( void *parameters );

/* Utilities to control tasks. */
TaskHandle_t tasks[NBTASKS];
uint8_t taskSuspended;

/****************************************************************************/

/* Variables used. */
#define BMP280_Device_Address    0xEC

#define BMP280_ID_REG            0xD0
#define BMP280_ID_VALUE          0x58

#define BMP280_RESET_REG         0xE0

GAP_L2_DATA uint8_t id_read = 0;
GAP_L2_DATA uint8_t cmd[] = {BMP280_ID_REG};

/****************************************************************************/


/* Program Entry. */
int main( void )
{
    printf("\n\n\t *** Driver Test : I2C - BMP280 ***\n\n");

    #if configSUPPORT_DYNAMIC_ALLOCATION == 1

    /* Init memory regions to alloc memory. */
    vPortDefineHeapRegions( xHeapRegions );

    BaseType_t xTask;
    TaskHandle_t xHandleDynamic = NULL;

    xTask = xTaskCreate(
        vTestDriverI2C,
        "TestDriverI2C",
        configMINIMAL_STACK_SIZE,
        NULL,
        tskIDLE_PRIORITY + 1,
        &xHandleDynamic
        );
    if( xTask != pdPASS )
    {
        printf("Task Dynamic is NULL !\n");
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

void vTestDriverI2C( void *parameters )
{
    printf("Fabric controller code execution for mbed_os I2C driver BMP280 test\n");

    I2C_Type *const i2c_address[] = I2C_BASE_PTRS;

    pinmap_pinout( I2C0_SDA, PinMap_I2C_SDA );
    pinmap_pinout( I2C0_SCL, PinMap_I2C_SCL );

    i2c_config_t config;

    I2C_GetDefaultConfig( &config );

    /* Init I2C device : Default fequence is 100kHz */
    I2C_Init( i2c_address[0], &config, FLL_GetFrequency( uFLL_SOC ) );

    printf("I2C0_SDA  = %d \n", I2C0_SDA);
    printf("I2C0_SCL  = %d \n", I2C0_SCL);
    /* Read ID value iof BMP280 */
    I2C_Write( i2c_address[0], BMP280_Device_Address, ( const char * ) &cmd, 1, 0 );
    I2C_Read( i2c_address[0], BMP280_Device_Address, ( char * ) &id_read, 1, 1 );

    printf("BMP280_ID_VALUE : 0x%x\n", id_read);

    if( id_read == BMP280_ID_VALUE )
        printf("Test success !\n");
    else
        printf("Test failed !\n");

    /* Suicide. */
    vTaskSuspend( NULL );
}

/*-----------------------------------------------------------*/
