/****************************************************************************/
/* FreeRTOS functions includes. */
#include "FreeRTOS_util.h"

/* Demo utlities includes. */
#include "gap_common.h"
#include "pinmap.h"
#include "gap_gpio.h"
#include "PeripheralPins.h"

/****************************************************************************/

/* Test task to test FreeRTOS port. */
void vTestHyperFlash( void *parameters );

/* Utilities to control tasks. */
TaskHandle_t tasks[NBTASKS];
uint8_t taskSuspended;


#if configSUPPORT_STATIC_ALLOCATION == 1
StaticTask_t xTaskBuffer;
StackType_t xStack[ configMINIMAL_STACK_SIZE ];
#endif //configSUPPORT_STATIC_ALLOCATION

/****************************************************************************/

/* Variables used. */
#define BUFFER_SIZE             256
#define DEVICE_READY_OFFSET     7
#define ERASE_STATUS_OFFSET     5
#define PROGRAM_STATUS_OFFSET   4

/* Write and read address */
#define SA                      0x4000

typedef struct {
    uint16_t data;
    uint16_t addr;
} cmdSeq;

/* Chip erase */
/* GAP_L2_DATA  cmdSeq eraseSeq[6] = {{0xAA, 0x555}, {0x55, 0x2AA}, {0x80, 0x555}, {0xAA, 0x555}, {0x55, 0x2AA}, {0x10, 0x555}}; */

/* Sector erase */
GAP_L2_DATA  cmdSeq Erase_Seq[6] = {{0xAA, 0x555}, {0x55, 0x2AA}, {0x80, 0x555},
                                    {0xAA, 0x555}, {0x55, 0x2AA}, {0x30, SA}};
GAP_L2_DATA  cmdSeq VCR_Seq[4]   = {{0xAA, 0x555}, {0x55, 0x2AA}, {0x38, 0x555}, {0x8e0b, 0x0}};
GAP_L2_DATA  cmdSeq Reg_Seq     = {0x70, 0x555};

/* Write 512/4 = 128 word to Sector addr 0x4xxx */
GAP_L2_DATA  cmdSeq WB_Seq[4]   = {{0xAA, 0x555}, {0x55, 0x2AA}, {0x25, SA}, {BUFFER_SIZE - 1, SA}};
GAP_L2_DATA  cmdSeq WP_Seq[3]   = {{0xAA, 0x555}, {0x55, 0x2AA}, {0xA0, 0x555}};
GAP_L2_DATA  cmdSeq BPC_Seq     = {0x29, SA};

/* The high 16 bits for txHyperbusSamples is useless */
GAP_L2_DATA  uint16_t txHyperbusSamples[BUFFER_SIZE];
GAP_L2_DATA  uint16_t rxHyperbusSamples[BUFFER_SIZE];

GAP_L2_DATA uint32_t read_val = 0;
GAP_L2_DATA uint32_t write_val = 0;
/****************************************************************************/

/* Program Entry. */
int main( void )
{
    printf("\n\n\t *** HyperFlash Test ***\n\n");

    #if configSUPPORT_DYNAMIC_ALLOCATION == 1

    /* Init memory regions to alloc memory. */
    vPortDefineHeapRegions( xHeapRegions );

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

void HYPERBUS_init(PinName dq0, PinName dq1, PinName dq2, PinName dq3,
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

static inline void conf_flash( hyperbus_transfer_t *masterXfer )
{
    /* Set VCR to 5 delay cycles */
    for ( uint32_t i = 0; i < 4; i++ )
    {
        masterXfer->txData = &VCR_Seq[i].data;
        masterXfer->txDataSize = 2;
        masterXfer->rxData = 0;
        masterXfer->rxDataSize = 0;
        masterXfer->configFlags = 32;
        masterXfer->addr = ( VCR_Seq[i].addr << 1 );
        masterXfer->device = uHYPERBUS_Flash;
        masterXfer->reg_access = uHYPERBUS_Mem_Access;

        HYPERBUS_MasterTransferBlocking((HYPERBUS_Type *) HYPERBUS_BASE_PTRS, masterXfer);
    }
}
/*-----------------------------------------------------------*/

static inline void erase_page_in_flash( hyperbus_transfer_t *masterXfer, uint32_t flash_addr )
{
    /* Erase sector 0x0E00 */
    for ( uint32_t i = 0; i < 5; i++ )
    {
        masterXfer->txData = &Erase_Seq[i].data;
        masterXfer->txDataSize = 2;
        masterXfer->rxData = 0;
        masterXfer->rxDataSize = 0;
        masterXfer->configFlags = 32;
        masterXfer->addr = Erase_Seq[i].addr << 1;
        masterXfer->device = uHYPERBUS_Flash;
        masterXfer->reg_access = uHYPERBUS_Mem_Access;

        HYPERBUS_MasterTransferBlocking((HYPERBUS_Type *) HYPERBUS_BASE_PTRS, masterXfer);
    }
    masterXfer->txData = &Erase_Seq[5].data;
    masterXfer->txDataSize = 2;
    masterXfer->rxData = 0;
    masterXfer->rxDataSize = 0;
    masterXfer->configFlags = 32;
    masterXfer->addr = flash_addr;
    masterXfer->device = uHYPERBUS_Flash;
    masterXfer->reg_access = uHYPERBUS_Mem_Access;

    HYPERBUS_MasterTransferBlocking((HYPERBUS_Type *) HYPERBUS_BASE_PTRS, masterXfer);
}
/*-----------------------------------------------------------*/
static inline void write_page_in_flash( hyperbus_transfer_t *masterXfer, uint32_t flash_addr )
{
    /* Write to Buffer command sequence */
    for ( uint32_t i = 0; i < 3; i++ )
    {
        masterXfer->txData = &WP_Seq[i].data;
        masterXfer->txDataSize = 2;
        masterXfer->rxData = 0;
        masterXfer->rxDataSize = 0;
        masterXfer->configFlags = 32;
        masterXfer->addr = WP_Seq[i].addr << 1;
        masterXfer->device = uHYPERBUS_Flash;
        masterXfer->reg_access = uHYPERBUS_Mem_Access;

        HYPERBUS_MasterTransferBlocking( (HYPERBUS_Type *) HYPERBUS_BASE_PTRS, masterXfer );
    }

    /* Word Program */
    masterXfer->txData = txHyperbusSamples;
    masterXfer->txDataSize = BUFFER_SIZE * sizeof(uint16_t);
    masterXfer->rxData = 0;
    masterXfer->rxDataSize = 0;
    masterXfer->configFlags = 32;
    masterXfer->addr = flash_addr;
    masterXfer->device = uHYPERBUS_Flash;
    masterXfer->reg_access = uHYPERBUS_Mem_Access;

    HYPERBUS_MasterTransferBlocking( (HYPERBUS_Type *) HYPERBUS_BASE_PTRS, masterXfer );
}
/*-----------------------------------------------------------*/

static inline void read_page_from_flash( hyperbus_transfer_t *masterXfer, uint32_t flash_addr )
{

    masterXfer->txData = 0;
    masterXfer->txDataSize = 0;
    masterXfer->rxData = rxHyperbusSamples;
    masterXfer->rxDataSize = BUFFER_SIZE * sizeof(uint16_t);
    masterXfer->configFlags = 32;
    masterXfer->addr = flash_addr;
    masterXfer->device = uHYPERBUS_Flash;
    masterXfer->reg_access = uHYPERBUS_Mem_Access;

    HYPERBUS_MasterTransferBlocking( (HYPERBUS_Type *) HYPERBUS_BASE_PTRS, masterXfer );

}
/*-----------------------------------------------------------*/

int wait_process_end( uint32_t err_bit )
{
    uint16_t reg;
    hyperbus_transfer_t masterXferWrite, masterXferRead;
    /* Wait the end of process
     * Status Register (SR)
     * bit 4 -> program status bit, 0-success ; 1-failure
     * bit 5 -> erase status bit,   0-success ; 1-failure
     * bit 7 -> device ready bit,   0-busy    ; 1-ready
     */
    write_val = Reg_Seq.data;
    masterXferWrite.txData = ( uint16_t * ) &write_val;
    masterXferWrite.txDataSize = 2;
    masterXferWrite.rxData = 0;
    masterXferWrite.rxDataSize = 0;
    masterXferWrite.configFlags = 32;
    masterXferWrite.addr = Reg_Seq.addr << 1;
    masterXferWrite.device = uHYPERBUS_Flash;
    masterXferWrite.reg_access = uHYPERBUS_Mem_Access;

    masterXferRead.txData = 0;
    masterXferRead.txDataSize = 0;
    masterXferRead.rxData = ( uint16_t * ) &read_val;
    masterXferRead.rxDataSize = 2;
    masterXferRead.configFlags = 32;
    masterXferRead.addr = 0;
    masterXferRead.device = uHYPERBUS_Flash;
    masterXferRead.reg_access = uHYPERBUS_Mem_Access;

    do {
        HYPERBUS_MasterTransferBlocking( (HYPERBUS_Type *) HYPERBUS_BASE_PTRS, &masterXferWrite );
        HYPERBUS_MasterTransferBlocking( (HYPERBUS_Type *) HYPERBUS_BASE_PTRS, &masterXferRead );
        reg = ( ( read_val >> 16 ) & 0xffff );

        if ( reg & (1 << err_bit))
        {
            printf("ERR bit = 1, an error occurs in  operation\n");
            return 1;
        }
    } while( !( reg & ( 1 << DEVICE_READY_OFFSET ) ) );

    return 0;
}
/*-----------------------------------------------------------*/

uint32_t TestHyperFlash( void *arg )
{
    /* Hyperbus Flash Default latency is 5 clock, can not change */
    uint32_t latency = 0;

    /* HYPERBUS pins init, HYPERBUS udma channel init */
    HYPERBUS_init( HYPERBUS_DQ0, HYPERBUS_DQ1, HYPERBUS_DQ2, HYPERBUS_DQ3,
                   HYPERBUS_DQ4, HYPERBUS_DQ5, HYPERBUS_DQ6, HYPERBUS_DQ7,
                   HYPERBUS_CLK, HYPERBUS_CLKN, HYPERBUS_RWDS, HYPERBUS_CSN0, HYPERBUS_CSN1 );
    /* Config memory access timing for TX and RX*/
    HYPERBUS_SetTiming( ( HYPERBUS_Type * ) HYPERBUS_BASE_PTRS, 4, 4, 4, latency, 0, uHYPERBUS_Flash );
    HYPERBUS_SetTiming( ( HYPERBUS_Type * ) HYPERBUS_BASE_PTRS, 4, 4, 4, latency, 1, uHYPERBUS_Flash );

    /* Initialize the samples */
    for ( uint32_t i = 0; i < BUFFER_SIZE; i++ )
        txHyperbusSamples[i] = i;

    /* Configuration */
    hyperbus_transfer_t masterXfer;
    conf_flash( &masterXfer );

    /* Erase chip */
    erase_page_in_flash( &masterXfer, SA );
    wait_process_end( ERASE_STATUS_OFFSET );
    printf("Erase finished.\n");

    /* Write buffer no burst */
    write_page_in_flash( &masterXfer, SA );
    wait_process_end( PROGRAM_STATUS_OFFSET );
    printf("Write finished.\n");

    /* Read buffer burst */
    read_page_from_flash( &masterXfer, SA );
    printf("Read finished.\n");

    /* Release hyperbus driver */
    HYPERBUS_MasterDeInit( (HYPERBUS_Type *) HYPERBUS_BASE_PTRS );

    latency = 0;
    for ( uint32_t i = 0; i < BUFFER_SIZE; i++ )
    {
        if( rxHyperbusSamples[i] != (uint16_t) i )
            latency++;
        //printf("RX_DATA[%d] = 0x%04x\t", i, rxHyperbusSamples[i]);
    }

    return latency;
}
/*-----------------------------------------------------------*/

void vTestHyperFlash( void *parameters )
{
    ( void ) parameters;
    char *taskname = pcTaskGetName( NULL );
    int32_t errors = 0;

    printf("%s executing HyperFlash function :\n\n", taskname);

    errors = TestHyperFlash( NULL );

    printf("clk  = %d \n" "clkn = %d \n" "rwds = %d \n"
           "dq0  = %d \n" "dq1  = %d \n" "dq2  = %d \n" "dq3  = %d \n"
           "dq4  = %d \n" "dq5  = %d \n" "dq6  = %d \n" "dq7  = %d \n",
           HYPERBUS_CLK, HYPERBUS_CLKN, HYPERBUS_RWDS,
           HYPERBUS_DQ0, HYPERBUS_DQ1, HYPERBUS_DQ2, HYPERBUS_DQ3,
           HYPERBUS_DQ4, HYPERBUS_DQ5, HYPERBUS_DQ6, HYPERBUS_DQ7);

    if( errors )
        printf("\nTest failed with %ld errors !\n", errors);
    else
        printf("\nTest success with %ld errors !\n", errors);

    printf("\n%s on core %ld of cluster %ld suspending(%d).\n",
           taskname, __core_ID(), __cluster_ID(), taskSuspended++);
    vTaskSuspend( NULL );
}
/*-----------------------------------------------------------*/
