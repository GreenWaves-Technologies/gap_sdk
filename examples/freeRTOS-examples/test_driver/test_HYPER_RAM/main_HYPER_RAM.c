/****************************************************************************/
/* FreeRTOS functions includes. */
#include "FreeRTOS_util.h"

/* Demo utlities includes. */
#include "hyperbus_io.h"

/****************************************************************************/

/* Test task to test FreeRTOS port. */
void vTestHyperRam( void *parameters );

/* Utilities to control tasks. */
TaskHandle_t tasks[NBTASKS];
uint8_t taskSuspended;

/****************************************************************************/

/* Variables used. */
#define BUF_SIZE         ( 256 )
#define CONF_REG_DEFAULT ( 0x8F1F )
#define RXTX_ADDR        ( 0x800 )
/* Configuration Register 0 */
#define   CR0            ( 0x1000 )
/* Configuration Register 1 */
#define   CR1            ( 0x1001 )

#define   ID0            ( 0x0 )
#define   ID1            ( 0x1 )

GAP_L2_DATA uint8_t txSamples[BUF_SIZE];
GAP_L2_DATA uint8_t rxSamples[BUF_SIZE];


/*
 * CA bits     47    46    45    44-40  |  39-32  |  31-24  |  23-16  |  15-8  |  7-0
 *
 * ID0 (RO)       C0h or E0h                00h       00h       00h       00h     00h
 * ID1 (RO)       C0h or E0h                00h       00h       00h       00h     01h
 * CR0_R          C0h or E0h                00h       01h       00h       00h     00h
 * CR0_W             60h                    00h       01h       00h       00h     00h
 * CR1_R          C0h or E0h                00h       01h       00h       00h     01h
 * CR1_W             60h                    00h       01h       00h       00h     01h
 *
 * CA[44-16]  => ADDR[31-3]
 * CA[2-0]    => ADDR[2-0]
 */

/* Need to be here, global var are in L2. */
uint16_t config_reg = 0;

/****************************************************************************/

/* Program Entry. */
int main( void )
{
    printf("\n\n\t *** HyperRam Test ***\n\n");

    #if configSUPPORT_DYNAMIC_ALLOCATION == 1

    /* Init memory regions to alloc memory. */
    vPortDefineHeapRegions( xHeapRegions );

    BaseType_t xTask;
    TaskHandle_t xHandleDynamic = NULL;

    xTask = xTaskCreate(
        vTestHyperRam,
        "TestHyperRam",
        configMINIMAL_STACK_SIZE,
        NULL,
        tskIDLE_PRIORITY + 1,
        &xHandleDynamic
        );
    if( xTask != pdPASS )
    {
        printf("TestHyperRam is NULL !\n");
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

void vTestHyperRam( void *parameters )
{
    ( void ) parameters;
    char *taskname = pcTaskGetName( NULL );
    uint32_t errors = 0, read_val = 0, latency = 1;

    printf("%s executing HyperRam function :\n\n", taskname);

    HYPERBUS_IO_Init( uHYPERBUS_Ram );

    printf("clk  = %d \n" "clkn = %d \n" "rwds = %d \n"
           "dq0  = %d \n" "dq1  = %d \n" "dq2  = %d \n" "dq3  = %d \n"
           "dq4  = %d \n" "dq5  = %d \n" "dq6  = %d \n" "dq7  = %d \n",
           HYPERBUS_CLK, HYPERBUS_CLKN, HYPERBUS_RWDS,
           HYPERBUS_DQ0, HYPERBUS_DQ1, HYPERBUS_DQ2, HYPERBUS_DQ3,
           HYPERBUS_DQ4, HYPERBUS_DQ5, HYPERBUS_DQ6, HYPERBUS_DQ7);

    /* Initialize the samples */
    for( uint32_t i = 0; i < BUF_SIZE; i++ )
    {
        txSamples[i] = i;
    }

    /* Write to RAM */
    HYPERBUS_IO_Write( RXTX_ADDR, BUF_SIZE * sizeof( txSamples[0] ), txSamples, uHYPERBUS_Ram, uHYPERBUS_Mem_Access );

    /* Read from RAM */
    HYPERBUS_IO_Read( RXTX_ADDR, BUF_SIZE * sizeof( rxSamples[0] ), rxSamples, uHYPERBUS_Ram, uHYPERBUS_Mem_Access );

    for( uint32_t i = 0; i < BUF_SIZE; i++ )
    {
        if( ( rxSamples[i] % 256 ) != ( uint8_t ) i )
            errors++;
        //printf(" %2x ", rxSamples[i]);
    }

    /* Read RAM register */
    HYPERBUS_IO_Read( CR0, 2, &read_val, uHYPERBUS_Ram, uHYPERBUS_Reg_Access );
    config_reg = ( ( read_val >> 16 ) & 0xffff );
    if( config_reg != ( CONF_REG_DEFAULT & ( 0xFF0F | ( latency << 4 ) ) ) )
        errors++;
    printf("Read CR0 6 clock lateny = 0x%02x\n", config_reg);

    /* Write RAM register to change latenny to 5 clock */
    config_reg &= 0xFF0F;
    HYPERBUS_IO_Write( CR0, 2, &config_reg, uHYPERBUS_Ram, uHYPERBUS_Reg_Access );

    /* Read RAM register */
    HYPERBUS_IO_Read( CR0, 2, &read_val, uHYPERBUS_Ram, uHYPERBUS_Reg_Access );
    config_reg = ( ( read_val >> 16 ) & 0xffff );
    printf("Read CR0 5 clock lateny = 0x%02x\n", config_reg);

    /* Write RAM register to change latenny back to 6 clock */
    config_reg |= 0x0010;
    HYPERBUS_IO_Write( CR0, 2, &config_reg, uHYPERBUS_Ram, uHYPERBUS_Reg_Access );

    /* Release hyperbus driver */
    HYPERBUS_IO_Deinit();

    if( errors )
        printf("Test failed with %d errors !\n", errors);
    else
        printf("Test success with %d errors !\n", errors);

    printf("\n%s on core %d of cluster %d suspending(%d).\n",
           taskname, __core_ID(), __cluster_ID(), taskSuspended++);
    vTaskSuspend( NULL );
}
/*-----------------------------------------------------------*/
