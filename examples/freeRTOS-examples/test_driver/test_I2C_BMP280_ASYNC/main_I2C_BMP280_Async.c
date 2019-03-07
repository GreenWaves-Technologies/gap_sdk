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

/* Array of I2C and PORT peripheral base address. */
static PORT_Type *const xPort_addrs[] = PORT_BASE_PTRS;
static I2C_Type *const i2c_address[] = I2C_BASE_PTRS;

/****************************************************************************/


/* Program Entry. */
int main( void )
{
    printf("\n\n\t *** Driver Test : I2C - BMP280 Async ***\n\n");

    #if configSUPPORT_DYNAMIC_ALLOCATION == 1
    BaseType_t xTask;
    TaskHandle_t xHandleDynamic = NULL;

    xTask = xTaskCreate(
        vTestDriverI2C,
        "TestDriverI2C",
        configMINIMAL_STACK_SIZE * 2,
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
void callback1( void *arg )
{
    printf("I2C callback : Write done !\n");
}
/*-----------------------------------------------------------*/

void callback2( void *arg )
{
    printf("I2C callback : Read done !\n");
}
/*-----------------------------------------------------------*/

void vTestDriverI2C( void *parameters )
{
    PinName xI2C_SDA = I2C0_SDA;
    PinName xI2C_SCL = I2C0_SCL;
    uint32_t ulPort_nb1  = GET_GPIO_PORT( xI2C_SDA );
    uint32_t ulPin_nb1   = GET_GPIO_PIN_NUM( xI2C_SDA );
    uint32_t ulPort_nb2  = GET_GPIO_PORT( xI2C_SCL );
    uint32_t ulPin_nb2   = GET_GPIO_PIN_NUM( xI2C_SCL );
    /* Init both PORT as GPIO/Alter0. */
    port_pin_config_t xPort_config = { .pullSelect = uPORT_PullUpEnable,
                                       .driveStrength = uPORT_LowDriveStrength,
                                       .mux = uPORT_MuxAlt0};
    PORT_SetPinConfig( xPort_addrs[ulPort_nb1], ulPin_nb1, &xPort_config);
    PORT_SetPinConfig( xPort_addrs[ulPort_nb2], ulPin_nb2, &xPort_config);

    i2c_config_t config;

    I2C_GetDefaultConfig( &config );

    /* Init I2C device : Default frequence is 100kHz. */
    I2C_Init( i2c_address[0], &config, SystemCoreClock );

    printf("I2C0_SDA  = %d \n", I2C0_SDA);
    printf("I2C0_SCL  = %d \n", I2C0_SCL);

    /* Handler. */
    i2c_handle_t handle;
    I2C_CreateHandler( i2c_address[0], &handle, callback1, NULL );

    /* Read ID value of BMP280. */
    if( I2C_Write_Async( i2c_address[0], BMP280_Device_Address,
                         ( const char * ) &cmd, 1, &handle ) != uStatus_Success )
    {
        printf("Test failed : Write !\n");
        exit( -1 );
    }
    while( (volatile)handle.state != uI2C_Idle ) __WFI();

    handle.callback = callback2;
    if( I2C_Read_Async( i2c_address[0], BMP280_Device_Address,
                        ( char * ) &id_read, 1, &handle ) != uStatus_Success )
    {
        printf("Test failed : Read !\n");
        exit( -1 );
    }
    while( (volatile)handle.state != uI2C_Idle ) __WFI();

    printf("BMP280_ID_VALUE : 0x%x\n", id_read);

    if( id_read == BMP280_ID_VALUE )
        printf("Test success !\n");
    else
        printf("Test failed !\n");

    /* Suicide. */
    vTaskSuspend( NULL );
}

/*-----------------------------------------------------------*/
