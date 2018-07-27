/****************************************************************************/
/* FreeRTOS functions includes. */
#include "FreeRTOS_util.h"

/* Demo utlities includes. */
#include "gap_common.h"
#include "pinmap.h"
#include "PeripheralPins.h"

/****************************************************************************/

/* Test task to test FreeRTOS port. */
void vTestHyperRam( void *parameters );

/* Utilities to control tasks. */
TaskHandle_t tasks[NBTASKS];
uint8_t taskSuspended;


#if configSUPPORT_STATIC_ALLOCATION == 1
StaticTask_t xTaskBuffer;
StackType_t xStack[ configMINIMAL_STACK_SIZE ];
#endif //configSUPPORT_STATIC_ALLOCATION

/****************************************************************************/

/* Variables used. */
#define BUFFER_SIZE       1024
#define CONF_REG_DEFAULT  0x8F1F
#define RXTX_ADDR         0x800

GAP_L2_DATA  uint8_t txHyperbusSamples[BUFFER_SIZE];
GAP_L2_DATA  uint8_t rxHyperbusSamples[BUFFER_SIZE];

#define   ID0  0x0
#define   ID1  0x1

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

/* Configuration Register 0 */
#define   CR0    0x1000
/* Configuration Register 1 */
#define   CR1    0x1001
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

void hyperbus_init(PinName dq0, PinName dq1, PinName dq2, PinName dq3,
                   PinName dq4, PinName dq5, PinName dq6, PinName dq7,
                   PinName ck, PinName ckn, PinName rwds, PinName csn0, PinName csn1)
{
    /* pin out the hyperbus pins */
    pinmap_pinout(ck,   PinMap_HYPERBUS_CLK);
    pinmap_pinout(ckn,  PinMap_HYPERBUS_CLKN);
    pinmap_pinout(rwds, PinMap_HYPERBUS_RWDS);
    pinmap_pinout(csn0, PinMap_HYPERBUS_CSN0);
    pinmap_pinout(csn1, PinMap_HYPERBUS_CSN1);
    pinmap_pinout(dq0,  PinMap_HYPERBUS_DQ0);
    pinmap_pinout(dq1,  PinMap_HYPERBUS_DQ1);
    pinmap_pinout(dq2,  PinMap_HYPERBUS_DQ2);
    pinmap_pinout(dq3,  PinMap_HYPERBUS_DQ3);
    pinmap_pinout(dq4,  PinMap_HYPERBUS_DQ4);
    pinmap_pinout(dq5,  PinMap_HYPERBUS_DQ5);
    pinmap_pinout(dq6,  PinMap_HYPERBUS_DQ6);
    pinmap_pinout(dq7,  PinMap_HYPERBUS_DQ7);

    hyperbus_master_config_t  masterConfig;

    HYPERBUS_MasterGetDefaultConfig(&masterConfig);
    HYPERBUS_MasterInit((HYPERBUS_Type *) HYPERBUS_BASE_PTRS, &masterConfig, FLL_GetFrequency( uFLL_SOC ) );
}
/*-----------------------------------------------------------*/

static inline void hyperbus_write( hyperbus_transfer_t *masterXfer, void *tx,
                                   uint32_t addr, uint32_t size, uint8_t hyperbus_access )
{
    /*
     * hyperbus_access = 0 -> Mem else Reg
     */
    masterXfer->rxData = 0;
    masterXfer->rxDataSize = 0;
    masterXfer->configFlags = 32;
    masterXfer->device = uHYPERBUS_Ram;
    masterXfer->addr = addr;
    masterXfer->txData = (uint16_t *) tx;
    masterXfer->txDataSize = size;
    masterXfer->reg_access = hyperbus_access;

    HYPERBUS_MasterTransferBlocking((HYPERBUS_Type *) HYPERBUS_BASE_PTRS, masterXfer);
}
/*-----------------------------------------------------------*/

static inline void hyperbus_read( hyperbus_transfer_t *masterXfer, void *rx,
                                  uint32_t addr, uint32_t size, uint8_t hyperbus_access )
{
    /*
     * hyperbus_access = 0 -> Mem else Reg
     */
    masterXfer->txData = 0;
    masterXfer->txDataSize = 0;
    masterXfer->configFlags = 32;
    masterXfer->device = uHYPERBUS_Ram;
    masterXfer->addr = addr;
    masterXfer->rxData = ( uint16_t *) rx;
    masterXfer->rxDataSize = size;
    masterXfer->reg_access = hyperbus_access;
    HYPERBUS_MasterTransferBlocking((HYPERBUS_Type *) HYPERBUS_BASE_PTRS, masterXfer);
}
/*-----------------------------------------------------------*/

uint32_t TestHyperRam( void *arg )
{
    /* Default is 6 Clock Lateny */
    /*
     *   0000 - 5 Clock Latency
     *   0001 - 6 Clock Latency
     *   0010 - Reserved
     *   0000 - Reserved
     *   ...
     *   1110 - 3 Clock Latency
     *   1111 - 4 Clock Latency
     */
    uint32_t latency = 1, errors = 0,read_val = 0;
    hyperbus_transfer_t masterXfer;

    /* HYPERBUS pins init, HYPERBUS udma channel init */
    hyperbus_init(HYPERBUS_DQ0, HYPERBUS_DQ1, HYPERBUS_DQ2, HYPERBUS_DQ3,
                  HYPERBUS_DQ4, HYPERBUS_DQ5, HYPERBUS_DQ6, HYPERBUS_DQ7,
                  HYPERBUS_CLK, HYPERBUS_CLKN, HYPERBUS_RWDS, HYPERBUS_CSN0, HYPERBUS_CSN1);
    /* Config memory maximum transfer data length for TX and RX*/
    HYPERBUS_SetMaxLength((HYPERBUS_Type *) HYPERBUS_BASE_PTRS, 1, 0x1ff, 0, uHYPERBUS_Ram);
    HYPERBUS_SetMaxLength((HYPERBUS_Type *) HYPERBUS_BASE_PTRS, 1, 0x1ff, 1, uHYPERBUS_Ram);
    /* Config memory access timing for TX and RX*/
    HYPERBUS_SetTiming((HYPERBUS_Type *) HYPERBUS_BASE_PTRS, 4, 4, 4, latency, 0, uHYPERBUS_Ram);
    HYPERBUS_SetTiming((HYPERBUS_Type *) HYPERBUS_BASE_PTRS, 4, 4, 4, latency, 1, uHYPERBUS_Ram);

    /* Initialize the samples */
    for ( uint32_t i = 0; i < BUFFER_SIZE; i++ )
    {
        txHyperbusSamples[i] = i;
        //printf("TX_DATA[%d] = 0x%04x\t", i, txHyperbusSamples[i]);
    }

    /* Write to RAM */
    hyperbus_write( &masterXfer, txHyperbusSamples, RXTX_ADDR, BUFFER_SIZE, uHYPERBUS_Mem_Access );

    /* Read from RAM */
    hyperbus_read( &masterXfer, rxHyperbusSamples, RXTX_ADDR, BUFFER_SIZE, uHYPERBUS_Mem_Access );

    for( uint32_t i = 0; i < BUFFER_SIZE; i++ )
    {
        if( ( rxHyperbusSamples[i] % 256 ) != ( uint8_t ) i )
            errors++;
    }

    /* Read RAM register */
    hyperbus_read( &masterXfer, &read_val, CR0, 2, uHYPERBUS_Reg_Access );
    config_reg = ( ( read_val >> 16 ) & 0xffff );
    if(config_reg != (CONF_REG_DEFAULT & (0xFF0F | (latency << 4))))
        errors++;
    printf("Read CR0 6 clock lateny = 0x%02x\n", config_reg);

    /* Write RAM register to change latenny to 5 clock */
    config_reg &= 0xFF0F;
    hyperbus_write( &masterXfer, &config_reg, CR0, 2, uHYPERBUS_Reg_Access );

    /* Read RAM register */
    hyperbus_read( &masterXfer, &read_val, CR0, 2, uHYPERBUS_Reg_Access );
    config_reg = ( ( read_val >> 16 ) & 0xffff );
    printf("Read CR0 5 clock lateny = 0x%02x\n", config_reg);

    /* Write RAM register to change latenny back to 6 clock */
    config_reg |= 0x0010;
    hyperbus_write( &masterXfer, &config_reg, CR0, 2, uHYPERBUS_Reg_Access );

    /* Release hyperbus driver */
    HYPERBUS_MasterDeInit( (HYPERBUS_Type *) HYPERBUS_BASE_PTRS );

    return errors;
}
/*-----------------------------------------------------------*/

void vTestHyperRam( void *parameters )
{
    ( void ) parameters;
    char *taskname = pcTaskGetName( NULL );
    int32_t errors = 0;

    printf("%s executing HyperRam function :\n\n", taskname);

    errors = TestHyperRam( NULL );

    printf("clk  = %d \n" "clkn = %d \n" "rwds = %d \n"
           "dq0  = %d \n" "dq1  = %d \n" "dq2  = %d \n" "dq3  = %d \n"
           "dq4  = %d \n" "dq5  = %d \n" "dq6  = %d \n" "dq7  = %d \n",
           HYPERBUS_CLK, HYPERBUS_CLKN, HYPERBUS_RWDS,
           HYPERBUS_DQ0, HYPERBUS_DQ1, HYPERBUS_DQ2, HYPERBUS_DQ3,
           HYPERBUS_DQ4, HYPERBUS_DQ5, HYPERBUS_DQ6, HYPERBUS_DQ7);

    if( errors )
        printf("Test failed with %d errors !\n", errors);
    else
        printf("Test success with %d errors !\n", errors);

    printf("\n%s on core %d of cluster %d suspending(%d).\n",
           taskname, __core_ID(), __cluster_ID(), taskSuspended++);
    vTaskSuspend( NULL );
}
/*-----------------------------------------------------------*/
