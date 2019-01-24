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
#define EEPROM_Device_Address    0xA0

#define EEPROM_Memory_Address    0x0070
#define BUF_SIZE                 16

GAP_L2_DATA uint8_t write_buff[BUF_SIZE+2] = {0}, read_buff[BUF_SIZE] = {0},
    addr_buff[2] = {0};

/* Array of I2C and PORT peripheral base address. */
static PORT_Type *const xPort_addrs[] = PORT_BASE_PTRS;
static I2C_Type *const i2c_address[] = I2C_BASE_PTRS;

/****************************************************************************/


/* Program Entry. */
int main( void )
{
    printf("\n\n\t *** Driver Test : I2C - EEPROM ***\n\n");

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

    addr_buff[0] = ( ( uint32_t ) EEPROM_Memory_Address ) >> 8;
    addr_buff[1] = ( ( uint32_t ) EEPROM_Memory_Address ) & 0xff;
    write_buff[0] = ( ( uint32_t ) EEPROM_Memory_Address ) >> 8;
    write_buff[1] = ( ( uint32_t ) EEPROM_Memory_Address ) & 0xff;

    for( uint32_t i = 0; i < BUF_SIZE; i++ )
    {
        write_buff[i+2] = i + 0x45;
    }

    /* Write into EEPROM. */
    I2C_Write( i2c_address[0], EEPROM_Device_Address,
               ( const char * ) write_buff, BUF_SIZE+2, 1 );
    vTaskDelay( 10 );
    /* Read from EEPROM. */
    I2C_Write( i2c_address[0], EEPROM_Device_Address,
               ( const char * ) addr_buff, 2, 0 );
    I2C_Read( i2c_address[0], EEPROM_Device_Address,
              ( char * ) read_buff, BUF_SIZE, 1 );

    uint32_t errors = 0;
    for( uint32_t i = 0; i < BUF_SIZE; i++ )
    {
        if( write_buff[i+2] != read_buff[i] )
        {
            errors++;
        }
        printf("Write : %x Read : %x\n", write_buff[i+2], read_buff[i]);
    }

    printf("Test %s with %d errors !\n", (errors ? "failed" : "success"), errors);

    /* Suicide. */
    vTaskSuspend( NULL );
}

/*-----------------------------------------------------------*/
