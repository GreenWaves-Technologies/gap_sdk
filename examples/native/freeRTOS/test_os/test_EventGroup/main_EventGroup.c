/* PMSIS includes */
#include "pmsis.h"

/* Variables used. */
TaskHandle_t xHandler[5] = {NULL};

#define TASK0                ( 0x1111 )
#define TASK1                ( 0x2222 )
#define TASK2                ( 0x4444 )
#define TASK3                ( 0x8888 )
#define SYNC_TASKS           ( 0xffff )

/* EventGroup Handlers. */
EventGroupHandle_t xEventGroup0 = NULL, xEventGroup1 = NULL;

/* EventGroupHandle_t'data structure. */
StaticEventGroup_t xEventGroupStorage;

void vTestEventGroup0( void *parameters )
{
    char *taskname = pcTaskGetName( NULL );
    uint32_t delay = 100;
    EventBits_t uxBits = 0, uxBitsToSet = ( uint32_t ) parameters;

    if( ( xEventGroup0 == NULL ) || ( xEventGroup1 == NULL ) )
    {
        vTaskSuspend( NULL );
    }

    printf("%s delaying for %d ms.\n", taskname, delay);
    vTaskDelay( pdMS_TO_TICKS( delay ) );
    printf("%s coming back.\n", taskname);

    uxBits = xEventGroupSetBits( xEventGroup1, ( uxBitsToSet << 1) );
    if( (uxBits & ( uxBitsToSet << 1 ) ) == ( uxBitsToSet  << 1 ) )
    {
        printf("%s all bits have been set : %x.\n", taskname, uxBits);
        printf("%s waiting for SYNC. Setting %x\t Waiting for %x\n",
               taskname, uxBitsToSet, SYNC_TASKS);
        uxBits = xEventGroupSync( xEventGroup0, /* EventGroup. */
                                  uxBitsToSet,  /* Bits to set. */
                                  SYNC_TASKS,   /* Bits to wait for. */
                                  portMAX_DELAY /* Timeout. */
            );
        if( ( uxBits & SYNC_TASKS ) == SYNC_TASKS )
        {
            printf("%s all synchronisation bits have been set : %x. Suspending !\n", taskname, uxBits);
            vTaskSuspend( NULL );
        }
    }
}

void vTestEventGroup1( void *parameters )
{
    char *taskname = pcTaskGetName( NULL );
    EventBits_t uxBits = 0, uxBitsToWait = ( uint32_t ) parameters;

    if( ( xEventGroup0 == NULL ) || ( xEventGroup1 == NULL ) )
    {
        vTaskSuspend( NULL );
    }

    printf("%s waiting for bits to be set : %x.\n", taskname, uxBitsToWait);
    uxBits = xEventGroupWaitBits( xEventGroup1,     /* EventGroup. */
                                  uxBitsToWait,     /* Bits to set. */
                                  pdFALSE,          /* When returning, bits above not cleared. */
                                  pdTRUE,           /* All bits above should be set(AND). */
                                  portMAX_DELAY     /* Timeout. */
        );
    if( ( uxBits & uxBitsToWait ) == uxBitsToWait )
    {
        printf("%s all bits waited for have been set : %x.\n\n", taskname, uxBits);
        printf("%s waiting for SYNC. Setting %x\t Waiting for %x\n",
               taskname, uxBitsToWait, SYNC_TASKS);
        uxBits = xEventGroupSync( xEventGroup0, /* EventGroup. */
                                  uxBitsToWait, /* Bits to set. */
                                  SYNC_TASKS,   /* Bits to wait for. */
                                  portMAX_DELAY /* Timeout. */
            );
        if( ( uxBits & SYNC_TASKS ) == SYNC_TASKS )
        {
            printf("%s all synchronisation bits have been set : %x. Suspending !\n", taskname, uxBits);
            vTaskSuspend( NULL );
        }
    }
}

void vEventGroupCreator( void *parameters )
{
    char *taskname = pcTaskGetName( NULL );
    EventBits_t uxBits = 0, uxBitsToSet = ( uint32_t ) parameters;

    xEventGroup0 = xEventGroupCreate();
    if( xEventGroup0 == NULL )
    {
        vTaskSuspend( NULL );
    }
    xEventGroup1 = xEventGroupCreate();
    if( xEventGroup1 == NULL )
    {
        vTaskSuspend( NULL );
    }
    printf("%s created xEventGroup0 and xEventGroup1.\n", taskname);

    for( uint32_t i = 0; i < 4; i++)
    {
        if( eTaskGetState( xHandler[i] ) == eSuspended )
        {
            vTaskResume( xHandler[i] );
        }
    }
    printf("%s waiting for SYNC. Setting %x\t Waiting for %x\n",
           taskname, uxBitsToSet, SYNC_TASKS);
    uxBits = xEventGroupSync( xEventGroup0,     /* EventGroup. */
                              uxBitsToSet,      /* Bits to set. */
                              SYNC_TASKS,       /* Bits to wait for. */
                              portMAX_DELAY     /* Timeout. */
        );
    if( ( uxBits & SYNC_TASKS ) == SYNC_TASKS )
    {
        printf("%s all synchronisation bits have been set : %x. Suspending !\n", taskname, uxBits);
        vEventGroupDelete( xEventGroup0 );
        vEventGroupDelete( xEventGroup1 );
        vTaskSuspend( NULL );
    }
}

void test_eventgroups(void)
{
    printf("Entering main controller\n");

    BaseType_t xTask;
    xTask = xTaskCreate( vTestEventGroup0, "EventGroup0", configMINIMAL_STACK_SIZE * 2,
                         ( void * ) TASK0, tskIDLE_PRIORITY + 1, &xHandler[0] );
    if( xTask != pdPASS )
    {
        printf("EventGroup0 is NULL !\n");
        pmsis_exit(-1);
    }

    xTask = xTaskCreate( vTestEventGroup1, "EventGroup1", configMINIMAL_STACK_SIZE * 2,
                         ( void * ) TASK1, tskIDLE_PRIORITY + 1, &xHandler[1] );
    if( xTask != pdPASS )
    {
        printf("EventGroup1 is NULL !\n");
        pmsis_exit(-2);
    }

    xTask = xTaskCreate( vTestEventGroup0, "EventGroup2", configMINIMAL_STACK_SIZE * 2,
                         ( void * ) TASK2, tskIDLE_PRIORITY + 1, &xHandler[2] );
    if( xTask != pdPASS )
    {
        printf("EventGroup2 is NULL !\n");
        pmsis_exit(-3);
    }

    xTask = xTaskCreate( vTestEventGroup1, "EventGroup3", configMINIMAL_STACK_SIZE * 2,
                         ( void * ) TASK3, tskIDLE_PRIORITY + 1, &xHandler[3] );
    if( xTask != pdPASS )
    {
        printf("EventGroup3 is NULL !\n");
        pmsis_exit(-4);
    }

    xTask = xTaskCreate( vEventGroupCreator, "EventGroupCreator", configMINIMAL_STACK_SIZE * 2,
                         ( void * ) !(SYNC_TASKS), tskIDLE_PRIORITY + 1, &xHandler[4] );
    if( xTask != pdPASS )
    {
        printf("EventGroupCreator is NULL !\n");
        pmsis_exit(-5);
    }

    while( ( eTaskGetState(xHandler[0]) != eSuspended ) ||
           ( eTaskGetState(xHandler[1]) != eSuspended ) ||
           ( eTaskGetState(xHandler[2]) != eSuspended ) ||
           ( eTaskGetState(xHandler[3]) != eSuspended ) ||
           ( eTaskGetState(xHandler[4]) != eSuspended ) )
    {
        pi_yield();
    }

    printf("Test success !\n");

    pmsis_exit(0);
}

/* Program Entry. */
int main(void)
{
    printf("\n\n\t *** EventGroup Test ***\n\n");
    return pmsis_kickoff((void *) test_eventgroups);
}
