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
void vTestSPI_QPI( void *parameters );

/* Utilities to control tasks. */
TaskHandle_t tasks[NBTASKS];
uint8_t taskSuspended;


#if configSUPPORT_STATIC_ALLOCATION == 1
StaticTask_t xTaskBuffer;
StackType_t xStack[ configMINIMAL_STACK_SIZE ];
#endif //configSUPPORT_STATIC_ALLOCATION

/****************************************************************************/

/* Variables used. */
#define BUFFER_SIZE 512
#define DUMMY 15

#define S25FS256_SR_WIP      ( ( uint8_t ) 0x01 )    /*!< Write in progress */
#define S25FS256_SR_WEL      ( ( uint8_t ) 0x02 )    /*!< Write enable latch */

GAP_L2_DATA uint8_t SPI_RX_BUFFER[BUFFER_SIZE];
GAP_L2_DATA uint8_t SPI_TX_BUFFER[BUFFER_SIZE];

spi_command_sequence_t s_command;

/* Array of SPIM peripheral base address. */
static SPIM_Type *const spi_address[] = SPIM_BASE_PTRS;
/* SPI transfer command sequence array */
static uint32_t s_command_sequence[6];

/****************************************************************************/

/* Program Entry. */
int main( void )
{
    printf("\n\n\t *** Driver Test : SPI QPI Auto_polling ***\n\n");

    #if configSUPPORT_DYNAMIC_ALLOCATION == 1

    /* Init memory regions to alloc memory. */
    vPortDefineHeapRegions( xHeapRegions );

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

void SPI_init( spi_master_config_t *master_config,
               PinName mosi, PinName miso, PinName sclk, PinName ssel )
{
    /* pin out the spi pins */
    pinmap_pinout(mosi, PinMap_SPI_MOSI);
    pinmap_pinout(miso, PinMap_SPI_MISO);
    pinmap_pinout(sclk, PinMap_SPI_SCLK);
    pinmap_pinout(ssel, PinMap_SPI_SSEL);
    /* Get default Master config */
    SPI_MasterGetDefaultConfig(master_config);
    /* determine the SPI to use */
    if(ssel == SPI0_CSN1 || ssel == SPI1_CSN1_B2 || ssel == SPI1_CSN1_B15)
        master_config->whichCsn = uSPI_csn1;
    else
        master_config->whichCsn = uSPI_csn0;
}
/*-----------------------------------------------------------*/

void SPI_format( spi_master_config_t *master_config,
                 uint32_t bits, uint32_t mode, uint32_t slave )
{
    /* Master config */
    master_config->bitsPerFrame = ( uint32_t ) bits;
    master_config->cpol = (mode & 0x2) ? uSPI_ClockPolarityActiveLow : uSPI_ClockPolarityActiveHigh;
    master_config->cpha = (mode & 0x1) ? uSPI_ClockPhaseSecondEdge : uSPI_ClockPhaseFirstEdge;

    SPI_MasterInit( spi_address[master_config->whichCsn], master_config, FLL_GetFrequency( uFLL_SOC ) );
}
/*-----------------------------------------------------------*/

void SPI_frequency( spi_master_config_t *master_config, uint32_t hz )
{
    /* Configuration only clock frequence */
    master_config->clkDiv = ( FLL_GetFrequency( uFLL_SOC ) >> 1 ) / hz;
    SPI_MasterFrequencyConfig( spi_address[master_config->whichCsn],  master_config);
}
/*-----------------------------------------------------------*/

void conf_flash( spi_master_config_t *master_config )
{
    {
        /* Master Command Sequence */
        s_command_sequence[0] = SPIM_CMD_SOT( master_config->whichCsn );
        SPI_MasterTransferBlocking( spi_address[master_config->whichCsn],
                                    ( uint32_t * ) s_command_sequence,
                                    sizeof( uint32_t ), NULL, 0, 32 );
        s_command_sequence[0] = SPIM_CMD_SEND_CMD( 0x06, 8, master_config->qpi );
        SPI_MasterTransferBlocking( spi_address[master_config->whichCsn],
                                    ( uint32_t * ) s_command_sequence,
                                    sizeof( uint32_t ), NULL, 0, 32 );
        s_command_sequence[0] = SPIM_CMD_EOT( 0 );
        SPI_MasterTransferBlocking( spi_address[master_config->whichCsn],
                                    ( uint32_t * ) s_command_sequence,
                                    sizeof( uint32_t ), NULL, 0, 32 );
    }
    /* Dummy cycles */
    {
        //memset( &s_command, 0, sizeof(spi_command_sequence_t) );
        s_command.cmd       = 0x71;
        s_command.cmd_bits  = 8;
        s_command.addr_bits = 32;
        s_command.addr      = ( 0x80000380 | DUMMY );
        s_command.cmd_mode  = uSPI_Single;
        s_command.addr_mode = uSPI_Single;
        s_command.csn = master_config->whichCsn;
        SPI_MasterTransferCommandSequence( spi_address[master_config->whichCsn], &s_command );
    }
    {
        /* Master Command Sequence. */
        s_command_sequence[0] = SPIM_CMD_SOT( master_config->whichCsn );
        SPI_MasterTransferBlocking( spi_address[master_config->whichCsn],
                                    ( uint32_t * ) s_command_sequence,
                                    sizeof( uint32_t ), NULL, 0, 32 );
        s_command_sequence[0] = SPIM_CMD_SEND_CMD( 0x06, 8, master_config->qpi );
        SPI_MasterTransferBlocking( spi_address[master_config->whichCsn],
                                    ( uint32_t * ) s_command_sequence,
                                    sizeof( uint32_t ), NULL, 0, 32 );
        s_command_sequence[0] = SPIM_CMD_EOT( 0 );
        SPI_MasterTransferBlocking( spi_address[master_config->whichCsn],
                                    ( uint32_t * ) s_command_sequence,
                                    sizeof( uint32_t ), NULL, 0, 32 );
    }
    /* Set page size to 512 */
    {
        memset(&s_command, 0, sizeof(spi_command_sequence_t));
        s_command.cmd       = 0x71;
        s_command.cmd_bits  = 8;
        s_command.addr_bits = 32;
        s_command.addr      = 0x00800004;
        s_command.cmd_mode  = uSPI_Single;
        s_command.addr_mode = uSPI_Single;
        s_command.tx_bits   = 8;
        s_command.tx_data   = 0x10;
        s_command.data_mode = uSPI_Single;
        s_command.csn = master_config->whichCsn;
        SPI_MasterTransferCommandSequence( spi_address[master_config->whichCsn], &s_command );
    }
    {
        /* Master Command Sequence. */
        s_command_sequence[0] = SPIM_CMD_SOT( master_config->whichCsn );
        SPI_MasterTransferBlocking( spi_address[master_config->whichCsn],
                                    ( uint32_t * ) s_command_sequence,
                                    sizeof( uint32_t ), NULL, 0, 32 );
        s_command_sequence[0] = SPIM_CMD_SEND_CMD( 0x06, 8, master_config->qpi );
        SPI_MasterTransferBlocking( spi_address[master_config->whichCsn],
                                    ( uint32_t * ) s_command_sequence,
                                    sizeof( uint32_t ), NULL, 0, 32 );
        s_command_sequence[0] = SPIM_CMD_EOT( 0 );
        SPI_MasterTransferBlocking( spi_address[master_config->whichCsn],
                                    ( uint32_t * ) s_command_sequence,
                                    sizeof( uint32_t ), NULL, 0, 32 );
    }
    /* QPI Enable : CR2V[6] = 1 */
    {
        memset( &s_command, 0, sizeof( spi_command_sequence_t ) );
        s_command.cmd       = 0x71;
        s_command.cmd_bits  = 8;
        s_command.addr_bits = 32;
        s_command.addr      = 0x00800003;
        s_command.cmd_mode  =  uSPI_Single;
        s_command.addr_mode =  uSPI_Single;
        s_command.tx_bits   = 8;
        s_command.tx_data   = (0xC0 | DUMMY);
        s_command.data_mode = uSPI_Single;
        s_command.csn = master_config->whichCsn;
        SPI_MasterTransferCommandSequence( spi_address[master_config->whichCsn], &s_command );
    }
}
/*-----------------------------------------------------------*/

void wait_process_end( spi_master_config_t *master_config )
{
    /* Wait the end of erasing process
     * Status Register 1(SR1V)
     * bit 0 -> 1-busy 0-idle
     * bit 1 -> 1-we
     * bit 6 -> 1-error 0-OK
     */
    spi_polling_config_t config;
    config.csn         = 0;
    config.cmd         = 0x05;
    config.cmd_mode    = uSPI_Quad;
    config.match_type  = uSPI_Match_Bits_Zero;
    config.match_bits  = 8;
    config.match_value = 0xffff & ~(S25FS256_SR_WIP);
    config.data_mode   = uSPI_Quad;
    config.time_out    = 0xf000;/* Maybe timeout ?*/

    SPI_Master_AutoPolling( spi_address[master_config->whichCsn], &config );
}
/*-----------------------------------------------------------*/

void erase_page_in_flash( spi_master_config_t *master_config )
{
    {
        /* Master Command Sequence. */
        s_command_sequence[0] = SPIM_CMD_SOT( master_config->whichCsn );
        SPI_MasterTransferBlocking( spi_address[master_config->whichCsn],
                                    ( uint32_t * ) s_command_sequence,
                                    sizeof( uint32_t ), NULL, 0, 32 );
        s_command_sequence[0] = SPIM_CMD_SEND_CMD( 0x06, 8, master_config->qpi );
        SPI_MasterTransferBlocking( spi_address[master_config->whichCsn],
                                    ( uint32_t * ) s_command_sequence,
                                    sizeof( uint32_t ), NULL, 0, 32 );
        s_command_sequence[0] = SPIM_CMD_EOT( 0 );
        SPI_MasterTransferBlocking( spi_address[master_config->whichCsn],
                                    ( uint32_t * ) s_command_sequence,
                                    sizeof( uint32_t ), NULL, 0, 32 );
    }
    memset(&s_command, 0, sizeof(spi_command_sequence_t));
    s_command.cmd       = 0x20;
    s_command.cmd_bits  = 8;
    s_command.addr_bits = 32;
    s_command.addr      = 0;
    s_command.cmd_mode  = uSPI_Quad;
    s_command.addr_mode = uSPI_Quad;
    s_command.csn = master_config->whichCsn;
    SPI_MasterTransferCommandSequence( spi_address[master_config->whichCsn], &s_command );
}
/*-----------------------------------------------------------*/

void write_page_in_flash( spi_master_config_t *master_config )
{
    {
        /* Master Command Sequence. */
        s_command_sequence[0] = SPIM_CMD_SOT( master_config->whichCsn );
        SPI_MasterTransferBlocking( spi_address[master_config->whichCsn],
                                    ( uint32_t * ) s_command_sequence,
                                    sizeof( uint32_t ), NULL, 0, 32 );
        s_command_sequence[0] = SPIM_CMD_SEND_CMD( 0x06, 8, master_config->qpi );
        SPI_MasterTransferBlocking( spi_address[master_config->whichCsn],
                                    ( uint32_t * ) s_command_sequence,
                                    sizeof( uint32_t ), NULL, 0, 32 );
        s_command_sequence[0] = SPIM_CMD_EOT( 0 );
        SPI_MasterTransferBlocking( spi_address[master_config->whichCsn],
                                    ( uint32_t * ) s_command_sequence,
                                    sizeof( uint32_t ), NULL, 0, 32 );
    }
    memset(&s_command, 0, sizeof(spi_command_sequence_t));
    s_command.cmd       = 0x02;
    s_command.cmd_bits  = 8;
    s_command.addr_bits = 32;
    s_command.addr      = 0;
    s_command.cmd_mode  = uSPI_Quad;
    s_command.addr_mode = uSPI_Quad;
    s_command.tx_bits   = BUFFER_SIZE*8;
    s_command.tx_data   = 0;
    s_command.tx_buffer = (uint8_t *)SPI_TX_BUFFER;
    s_command.data_mode = uSPI_Quad;
    s_command.csn = master_config->whichCsn;
    SPI_MasterTransferCommandSequence( spi_address[master_config->whichCsn], &s_command );
}
/*-----------------------------------------------------------*/

void read_page_from_flash( spi_master_config_t *master_config, uint32_t flash_addr )
{
    memset( &s_command, 0, sizeof( spi_command_sequence_t ) );
    s_command.cmd       = 0xEC;
    s_command.cmd_bits  = 8;
    s_command.addr_bits = 32;
    s_command.addr      = flash_addr;
    s_command.cmd_mode  = uSPI_Quad;
    s_command.addr_mode = uSPI_Quad;
    s_command.rx_bits   = BUFFER_SIZE*8;
    s_command.alter_data_bits = 8;
    s_command.alter_data      = 0x0A;
    s_command.alter_data_mode = uSPI_Quad;
    s_command.dummy     = DUMMY;
    s_command.rx_buffer = (uint8_t *)SPI_RX_BUFFER;
    s_command.data_mode = uSPI_Quad;
    s_command.csn = master_config->whichCsn;
    SPI_MasterTransferCommandSequence( spi_address[master_config->whichCsn], &s_command );
}
/*-----------------------------------------------------------*/

void vTestSPI_QPI( void *parameters )
{
    ( void ) parameters;
    char *taskname = pcTaskGetName( NULL );
    int32_t errors = 0, id = 0, spi_reg_value = 0;
    spi_master_config_t master_config;

    printf("%s executing SPI_QPI function :\n\n", taskname);

    for (uint32_t i = 0; i< BUFFER_SIZE; i++)
        SPI_TX_BUFFER[i] = i;

    /* SPI pins init, SPI udma channel init */
    SPI_init( &master_config, SPI0_SDIO0, SPI0_SDIO1, SPI0_SCLK, SPI0_CSN0 );

    printf("SPI0_MOSI  = %d \n" "SPI0_MISO  = %d \n" "SPI0_SDIO2 = %d \n"
           "SPI0_SDIO3 = %d \n" "SPI0_CSN0  = %d \n" "SPI0_SCLK  = %d \n",
           SPI0_SDIO0, SPI0_SDIO1, SPI0_SDIO2,
           SPI0_SDIO3, SPI0_CSN0, SPI0_SCLK);

    /* SPI bits, cpha, cpol configuration */
    SPI_format( &master_config, 32, 0, 0 );

    /* Set fequence to 10MHz */
    SPI_frequency( &master_config, 10000000 );

    /* Flash Configuration */
    conf_flash( &master_config );

    /* Set QPI mode - Quad */
    {
        master_config.qpi = uSPI_Quad;
        /* Exit if not SPI0. */
        if( master_config.whichCsn )
            vTaskSuspend( NULL );
        /* SPI pin binding */
        pinmap_pinout(SPI0_SDIO2, PinMap_SPIQ_SDIO2);
        pinmap_pinout(SPI0_SDIO3, PinMap_SPIQ_SDIO3);
    }

    /* Read ID in QPI mode */
    {
        /* Master Command Sequence. */
        s_command_sequence[0] = SPIM_CMD_SOT( master_config.whichCsn );
        SPI_MasterTransferBlocking( spi_address[master_config.whichCsn],
                                    ( uint32_t * ) s_command_sequence,
                                    sizeof( uint32_t ), NULL, 0, 32 );
        s_command_sequence[0] = SPIM_CMD_SEND_CMD( 0x9f, 8, master_config.qpi );
        SPI_MasterTransferBlocking( spi_address[master_config.whichCsn],
                                    ( uint32_t * ) s_command_sequence,
                                    sizeof( uint32_t ), NULL, 0, 32 );

        s_command_sequence[0] = SPIM_CMD_RX_DATA(32, master_config.qpi, 0);
        SPI_MasterTransferBlocking( spi_address[master_config.whichCsn],
                                    ( uint32_t * ) s_command_sequence,
                                    sizeof( uint32_t ), ( uint8_t * ) &spi_reg_value,
                                    4, 32);
        s_command_sequence[0] = SPIM_CMD_EOT( 0 );
        SPI_MasterTransferBlocking( spi_address[master_config.whichCsn],
                                    ( uint32_t * ) s_command_sequence,
                                    sizeof( uint32_t ), NULL, 0, 32 );
        if( master_config.bitsPerFrame == 8 )
            id = spi_reg_value & 0xff;
        else if( master_config.bitsPerFrame == 16 )
            id = spi_reg_value & 0xffff;
        else
            id = spi_reg_value;
        printf("ID = %lx\n", id);
    }

    /* Erase page */
    erase_page_in_flash( &master_config );
    printf("Erasing Flash ... \n");
    wait_process_end( &master_config );

    /* Write buffer */
    write_page_in_flash( &master_config );
    printf("Writing Flash ... \n");
    wait_process_end( &master_config );

    /* Read back buffer */
    printf("Reading Flash ... \n");
    read_page_from_flash( &master_config, 0 );

    /* Release SPI driver */
    SPI_MasterDeInit( spi_address[master_config.whichCsn] );

    for ( uint32_t i = 0; i < BUFFER_SIZE; i++ )
    {
        if( SPI_RX_BUFFER[i] != ( char ) ( i ) )
        {
            errors++;
            printf(" *** Error : %x *** ", SPI_RX_BUFFER[i]);
            continue;
        }
        printf(" %x ", SPI_RX_BUFFER[i]);
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
