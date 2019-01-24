/****************************************************************************/
/* FreeRTOS functions includes. */
#include "FreeRTOS_util.h"

/* Demo utlities includes. */
#include "gap_common.h"
#include "gap_dmacpy.h"

/****************************************************************************/

/* Test task to test FreeRTOS port. */
void vTestDmacpy( void *parameters );

/* Utilities to control tasks. */
TaskHandle_t tasks[NBTASKS];
uint8_t taskSuspended = 0;

/****************************************************************************/

/* Variables used. */
#define BUF_SIZE ( 256 )
GAP_L2_DATA uint32_t src_L2[BUF_SIZE], dst_L2[BUF_SIZE];
GAP_FC_DATA uint32_t src[BUF_SIZE], dst[BUF_SIZE];

static DMACPY_Type *dmaCopy = DMACPY_BASE_PTRS;

/****************************************************************************/

/* Program Entry. */
int main( void )
{
    printf("\n\n\t *** DMACPY Test ***\n\n");

    #if configSUPPORT_DYNAMIC_ALLOCATION == 1
    BaseType_t xTask;
    TaskHandle_t xHandleDynamic = NULL;

    xTask = xTaskCreate(
        vTestDmacpy,
        "TestDmacpy",
        configMINIMAL_STACK_SIZE * 1,
        NULL,
        tskIDLE_PRIORITY + 1,
        &xHandleDynamic
        );
    if( xTask != pdPASS )
    {
        printf("TestMEMCPY is NULL !\n");
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

static void printData( uint32_t *buf, uint32_t size )
{
    for( uint32_t i = 0; i <  size; i++ )
        printf(" %2x ", buf[i]);
    printf("\n");
}
/*-----------------------------------------------------------*/

static void checkData( uint32_t *cmpA, uint32_t *cmpB, uint32_t size, uint32_t *errSum )
{
    for( uint32_t i = 0; i <  size; i++ )
    {
        if( cmpA[i] != cmpB[i] )
        {
            (*errSum)++;
            if( (*errSum) == 1 )
                printf("Errors : \n");
            printf(" [%x - %x] ", cmpA[i], cmpB[i]);
        }
    }
}

/*-----------------------------------------------------------*/

void vTestDmacpy( void *parameters )
{
    char *taskname = pcTaskGetName( NULL );
    uint32_t errors = 1;

    for( uint32_t i = 0; i < BUF_SIZE; i++ )
        src[i] = i;

    printf("%s executing UDMA Memcpy test FC( src %x ) -> L2( src_L2 %x ) -> L2( dst_L2 %x) -> FC( dst %x ).\n\n",
           taskname, &src, &src_L2, &dst_L2, &dst);

    DMACPY_Init( dmaCopy );
    DMACPY_BlockingTransfer( dmaCopy, src, src_L2, sizeof( src ), uDMACPY_FC2L2 );

    DMACPY_BlockingTransfer( dmaCopy, src_L2, dst_L2, sizeof( src ), uDMACPY_L22L2 );

    DMACPY_BlockingTransfer( dmaCopy, dst_L2, dst, sizeof( src ), uDMACPY_L22FC );

    if( errors )
    {
        printf("\t FC SRC\n");
        printData( src, BUF_SIZE );
        printf("\t L2 SRC\n");
        printData( src_L2, BUF_SIZE );
        printf("\t L2 DST\n");
        printData( dst_L2, BUF_SIZE );
        printf("\t FC DST\n");
        printData( dst, BUF_SIZE );
        errors = 0;
    }

    checkData( src, src_L2, BUF_SIZE, &errors );
    checkData( src, dst_L2, BUF_SIZE, &errors );
    checkData( src, dst, BUF_SIZE, &errors );

    DMACPY_Deinit( dmaCopy );

    printf("\nTest %s with %d errors.\n", ( errors ? "failed": "success" ), errors);

    vTaskSuspend( NULL );
}
/*-----------------------------------------------------------*/
