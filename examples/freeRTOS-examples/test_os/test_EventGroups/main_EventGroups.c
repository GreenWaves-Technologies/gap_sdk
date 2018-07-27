/****************************************************************************/

/* FreeRTOS functions includes. */
#include "FreeRTOS_util.h"

/* Demo utlities includes. */

/****************************************************************************/

/* Test task to test FreeRTOS port. */
void vTestEventGroup0( void *parameters );
void vTestEventGroup1( void *parameters );
void vTestEventGroup2( void *parameters );

/* Utilities to control tasks. */
TaskHandle_t tasks[NBTASKS+4];
uint8_t taskSuspended;


#if configSUPPORT_STATIC_ALLOCATION == 1
StaticTask_t xTaskBuffer, xTaskBuffer2;
StackType_t xStack[ configMINIMAL_STACK_SIZE ], xStack2[ configMINIMAL_STACK_SIZE ];
#endif //configSUPPORT_STATIC_ALLOCATION

/****************************************************************************/

/* Variables used. */
#define TASK0                ( 0x1111 )
#define TASK1                ( 0x2222 )
#define TASK2                ( 0x4444 )
#define TASK3                ( 0x8888 )
#define SYNC_TASKS           ( 0xffff )

/* EventGroup Handlers. */
EventGroupHandle_t xEventGroupD = NULL;
EventGroupHandle_t xEventGroupS = NULL;

/* EventGroupHandle_t'data structure. */
StaticEventGroup_t xEventGroupStorage;

/****************************************************************************/

/* Program Entry. */
int main( void )
{
    printf("\n\n\t *** EventGroups Test ***\n\n");

    #if configSUPPORT_STATIC_ALLOCATION == 1
    TaskHandle_t xHandleStatic = NULL, xHandleStatic2 = NULL;
    xHandleStatic = xTaskCreateStatic(
        vTestEventGroup0,
        "TestEventGroup0",
        configMINIMAL_STACK_SIZE,
        ( void * ) TASK0,
        tskIDLE_PRIORITY + 1,
        xStack,
        &xTaskBuffer
        );
    if( xHandleStatic == NULL )
    {
        printf("TestEventGroup0 is NULL !\n");
        exit(0);
    }

    xHandleStatic2 = xTaskCreateStatic(
        vTestEventGroup1,
        "TestEventGroup1",
        configMINIMAL_STACK_SIZE,
        ( void * ) TASK1,
        tskIDLE_PRIORITY + 1,
        xStack2,
        &xTaskBuffer2
        );
    if( xHandleStatic2 == NULL )
    {
        printf("TestEventGroup1 is NULL !\n");
        exit(0);
    }
    #endif //configSUPPORT_STATIC_ALLOCATION


    #if configSUPPORT_DYNAMIC_ALLOCATION == 1

    /* Init memory regions to alloc memory. */
    vPortDefineHeapRegions( xHeapRegions );

    BaseType_t xTask, xTask2, xTask3;
    TaskHandle_t xHandleDynamic = NULL, xHandleDynamic2 = NULL, xHandleDynamic3 = NULL;

    xTask = xTaskCreate(
        vTestEventGroup0,
        "TestEventGroup2",
        configMINIMAL_STACK_SIZE,
        ( void * ) TASK2,
        tskIDLE_PRIORITY + 1,
        &xHandleDynamic
        );
    if( xTask != pdPASS )
    {
        printf("TestEventGroup2 is NULL !\n");
        exit(0);
    }

    xTask2 = xTaskCreate(
        vTestEventGroup1,
        "TestEventGroup3",
        configMINIMAL_STACK_SIZE,
        ( void * ) TASK3,
        tskIDLE_PRIORITY + 1,
        &xHandleDynamic2
        );
    if( xTask2 != pdPASS )
    {
        printf("TestEventGroup3 is NULL !\n");
        exit(0);
    }

    xTask3 = xTaskCreate(
        vTestEventGroup2,
        "TestEventGroup4",
        configMINIMAL_STACK_SIZE,
        ( void * ) !(SYNC_TASKS),
        tskIDLE_PRIORITY + 1,
        &xHandleDynamic3
        );
    if( xTask3 != pdPASS )
    {
        printf("TestEventGroup4 is NULL !\n");
        exit(0);
    }
    #endif //configSUPPORT_DYNAMIC_ALLOCATION

    tasks[0] = xHandleStatic;
    tasks[1] = xHandleStatic2;
    tasks[2] = xHandleDynamic;
    tasks[3] = xHandleDynamic2;
    tasks[4] = xHandleDynamic3;

    /* Start the kernel.  From here on, only tasks and interrupts will run. */
    printf("\nScheduler starts !\n");
    vTaskStartScheduler();

    /* Exit FreeRTOS */
    return 0;
}
/*-----------------------------------------------------------*/

void vTestEventGroup0( void *parameters )
{
    char *taskname = pcTaskGetName( NULL );
    uint32_t delay = 100;
    EventBits_t uxBits = 0, uxBitsToSet = ( uint32_t ) parameters;

    if( ( xEventGroupD == NULL ) || ( xEventGroupS == NULL ) )
        vTaskSuspend( NULL );

    printf("|-->%s delaying for %d ms.\n", taskname, delay);
    vTaskDelay( pdMS_TO_TICKS( delay ) );
    printf("|<--%s coming back.\n", taskname);

    uxBits = xEventGroupSetBits( xEventGroupS, ( uxBitsToSet << 1) );
    if( (uxBits & ( uxBitsToSet << 1 ) ) == ( uxBitsToSet  << 1 ) )
    {
        printf("|->%s all bits have been set : %x.\n", taskname, uxBits);
        printf("|->%s waiting for SYNC. Setting %x\t Waiting for %x\n",
               taskname, uxBitsToSet, SYNC_TASKS);
        uxBits = xEventGroupSync(
            xEventGroupD, /* EventGroup. */
            uxBitsToSet,  /* Bits to set. */
            SYNC_TASKS,   /* Bits to wait for. */
            portMAX_DELAY /* Timeout. */
            );
        if( ( uxBits & SYNC_TASKS ) == SYNC_TASKS )
        {
            printf("|<-%s all synchronisation bits have been set : %x. Suspending !\n", taskname, uxBits);
            vTaskSuspend( NULL );
        }
    }
}
/*-----------------------------------------------------------*/

void vTestEventGroup1( void *parameters )
{
    char *taskname = pcTaskGetName( NULL );
    EventBits_t uxBits = 0, uxBitsToWait = ( uint32_t ) parameters;

    if( ( xEventGroupD == NULL ) || ( xEventGroupS == NULL ) )
        vTaskSuspend( NULL );

    printf("|-->%s waiting for bits to be set : %x.\n", taskname, uxBitsToWait);
    uxBits = xEventGroupWaitBits(
        xEventGroupS,     /* EventGroup. */
        uxBitsToWait,     /* Bits to set. */
        pdFALSE,          /* When returning, bits above should not be cleared. */
        pdTRUE,           /* All bits above should be set(AND). */
        portMAX_DELAY     /* Timeout. */
        );
    if( ( uxBits & uxBitsToWait ) == uxBitsToWait )
    {
        printf("|<--%s all bits waited for have been set : %x.\n\n", taskname, uxBits);
        printf("|->%s waiting for SYNC. Setting %x\t Waiting for %x\n",
               taskname, uxBitsToWait, SYNC_TASKS);
        uxBits = xEventGroupSync(
            xEventGroupD, /* EventGroup. */
            uxBitsToWait, /* Bits to set. */
            SYNC_TASKS,   /* Bits to wait for. */
            portMAX_DELAY /* Timeout. */
            );
        if( ( uxBits & SYNC_TASKS ) == SYNC_TASKS )
        {
            printf("|<-%s all synchronisation bits have been set : %x. Suspending !\n", taskname, uxBits);
            vTaskSuspend( NULL );
        }
    }
}
/*-----------------------------------------------------------*/

void vTestEventGroup2( void *parameters )
{
    char *taskname = pcTaskGetName( NULL );
    EventBits_t uxBits = 0, uxBitsToSet = ( uint32_t ) parameters;

    xEventGroupD = xEventGroupCreate();
    if( xEventGroupD == NULL )
        vTaskSuspend( NULL );
    xEventGroupS = xEventGroupCreateStatic( &xEventGroupStorage );
    if( xEventGroupS == NULL )
        vTaskSuspend( NULL );
    printf("|->%s created xEventGroupD and xEventGroupS.\n", taskname);

    for( uint32_t i = 0; i < NBTASKS+4; i++)
    {
        if( eTaskGetState( tasks[i] ) == eSuspended )
            vTaskResume( tasks[i] );
    }
    printf("|->%s waiting for SYNC. Setting %x\t Waiting for %x\n",
           taskname, uxBitsToSet, SYNC_TASKS);
    uxBits = xEventGroupSync(
        xEventGroupD,     /* EventGroup. */
        uxBitsToSet,      /* Bits to set. */
        SYNC_TASKS,       /* Bits to wait for. */
        portMAX_DELAY     /* Timeout. */
        );
    if( ( uxBits & SYNC_TASKS ) == SYNC_TASKS )
    {
        printf("|<-%s all synchronisation bits have been set : %x. Suspending !\n", taskname, uxBits);
        vEventGroupDelete( xEventGroupD );
        vEventGroupDelete( xEventGroupS );
        printf("Test success\n");
        vTaskSuspend( NULL );
    }
}
/*-----------------------------------------------------------*/
