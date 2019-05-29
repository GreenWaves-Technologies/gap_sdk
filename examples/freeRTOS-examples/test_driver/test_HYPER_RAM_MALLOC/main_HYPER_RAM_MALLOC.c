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
    BaseType_t xTask;
    TaskHandle_t xHandler0 = NULL;

    xTask = xTaskCreate( vTestHyperRam, "TestHyperRam", configMINIMAL_STACK_SIZE,
                         NULL, tskIDLE_PRIORITY + 1, &xHandler0 );
    if( xTask != pdPASS )
    {
        printf("TestHyperAlloc is NULL !\n");
        exit(0);
    }
    #endif //configSUPPORT_DYNAMIC_ALLOCATION

    tasks[0] = xHandler0;

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
    uint32_t errors = 0;

    printf("%s executing HyperRam function :\n\n", taskname);

    HYPERBUS_IO_Init( uHYPERBUS_Ram );

    int32_t hyper_malloc_init = HYPERBUS_IO_Malloc_Init(0x0, BUF_SIZE+0x20);
    if( hyper_malloc_init )
    {
        printf("Hyper malloc init failed !\n");
        vTaskSuspend( NULL );
    }

    uint32_t *ptr_hyper = (uint32_t *) HYPERBUS_IO_Malloc(BUF_SIZE);
    if( ptr_hyper == NULL )
    {
        printf("Hyper malloc failed !\n");
        vTaskSuspend( NULL );
    }
    printf("Allocated in HyperRam at : %x !\n", ptr_hyper);

    uint32_t *ptr_hyper2 = (uint32_t *) HYPERBUS_IO_Malloc(0x8);
    if( ptr_hyper2 == NULL )
    {
        printf("Hyper malloc failed !\n");
        vTaskSuspend( NULL );
    }
    printf("Allocated in HyperRam at : %x !\n", ptr_hyper2);

    HYPERBUS_IO_MallocFree(ptr_hyper2, 0x8);
    ptr_hyper2 = (uint32_t *) HYPERBUS_IO_Malloc(0x8);
    if( ptr_hyper2 == NULL )
    {
        printf("Hyper malloc failed !\n");
        vTaskSuspend( NULL );
    }
    printf("Allocated in HyperRam at : %x !\n", ptr_hyper2);

    ptr_hyper2 = (uint32_t *) HYPERBUS_IO_Malloc(0x8);

    if( ptr_hyper2 == NULL )
    {
        printf("Hyper malloc failed !\n");
        vTaskSuspend( NULL );
    }
    printf("Allocated in HyperRam at : %x !\n", ptr_hyper2);

    HYPERBUS_IO_MallocFree(ptr_hyper2, 0x8);

    ptr_hyper2 = (uint32_t *) HYPERBUS_IO_Malloc(0x8);
    if( ptr_hyper2 == NULL )
    {
        printf("Hyper malloc failed !\n");
        vTaskSuspend( NULL );
    }
    printf("Allocated in HyperRam at : %x !\n", ptr_hyper2);

    /* Initialize the samples */
    for( uint32_t i = 0; i < BUF_SIZE; i++ )
    {
        txSamples[i] = i;
    }
        /* Write to RAM */
    HYPERBUS_IO_Write( (uint32_t) ptr_hyper, BUF_SIZE * sizeof( txSamples[0] ), txSamples, uHYPERBUS_Ram, uHYPERBUS_Mem_Access );

    /* Read from RAM */
    HYPERBUS_IO_Read( (uint32_t) ptr_hyper, BUF_SIZE * sizeof( rxSamples[0] ), rxSamples, uHYPERBUS_Ram, uHYPERBUS_Mem_Access );

    for( uint32_t i = 0; i < BUF_SIZE; i++ )
    {
        if( ( rxSamples[i] % 256 ) != ( uint8_t ) i )
        {
            errors++;
            printf("Write %2x - Read %2x\n", txSamples[i], rxSamples[i]);
        }
    }

    HYPERBUS_IO_MallocFree(ptr_hyper, sizeof(uint32_t) * BUF_SIZE);

    HYPERBUS_IO_Malloc_Deinit();

    printf("Test %s with %d error(s) !\n", (errors) ? "failed" : "success", errors);

    vTaskSuspend( NULL );
}
/*-----------------------------------------------------------*/
