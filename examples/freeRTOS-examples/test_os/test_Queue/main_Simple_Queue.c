/****************************************************************************/
/* FreeRTOS functions includes. */
#include "FreeRTOS_util.h"

/* Demo utlities includes. */
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

/****************************************************************************/

/* Test task to test FreeRTOS port. */
void vTestQueue0( void *parameters );
void vTestQueue1( void *parameters );
void vTestQueue2( void *parameters );

/* Utilities to control tasks. */
TaskHandle_t tasks[NBTASKS+2];
uint8_t taskSuspended;


#if configSUPPORT_STATIC_ALLOCATION == 1
StaticTask_t xTaskBuffer;
StackType_t xStack[ configMINIMAL_STACK_SIZE ];
#endif //configSUPPORT_STATIC_ALLOCATION

/****************************************************************************/

/* Variables used. */
/*
 * Structure sent and received.
 * In FreeRTOS, data sent are sent by copt not reference.
 */
struct _message
{
    uint8_t ucMessageID;
    char ucData[25];
};

typedef struct _message message;
#define QUEUE_LENGTH ( 5 )
#define ITEM_SIZE ( sizeof( message ) )
#define TABSIZE ( 3 * QUEUE_LENGTH )

#define STATIC ( 1 )

QueueHandle_t xQueueHandler = NULL;
#if STATIC == 1
/* xQueueHandler's data storage. */
uint8_t xQueueBuffer[ QUEUE_LENGTH * ITEM_SIZE ];
/* xQueueHandler's data structure. */
StaticQueue_t xQueueStaticStorage;
#endif

/****************************************************************************/

/* Program Entry. */
int main( void )
{
    printf("\n\n\t *** QueueMessage Test ***\n\n");

    #if configSUPPORT_STATIC_ALLOCATION == 1
    TaskHandle_t xHandleStatic = NULL;

    xHandleStatic = xTaskCreateStatic(
        vTestQueue0,
        "TaskReceiver",
        configMINIMAL_STACK_SIZE,
        NULL,
        tskIDLE_PRIORITY + 1,
        xStack,
        &xTaskBuffer
        );
    if( xHandleStatic == NULL )
    {
        printf("TestQueue0 is NULL !\n");
        exit(0);
    }
    #endif //configSUPPORT_STATIC_ALLOCATION


    #if configSUPPORT_DYNAMIC_ALLOCATION == 1

    /* Init memory regions to alloc memory. */
    vPortDefineHeapRegions( xHeapRegions );

    BaseType_t xTask, xTask2;
    TaskHandle_t xHandleDynamic = NULL,        xHandleDynamic2 = NULL;

    xTask = xTaskCreate(
        vTestQueue1,
        "TaskSender",
        configMINIMAL_STACK_SIZE,
        NULL,
        tskIDLE_PRIORITY + 1,
        &xHandleDynamic
        );
    if( xTask != pdPASS )
    {
        printf("TestQueue1 is NULL !\n");
        exit(0);
    }

    xTask2 = xTaskCreate(
        vTestQueue2,
        "TaskCreator",
        configMINIMAL_STACK_SIZE,
        NULL,
        tskIDLE_PRIORITY + 1,
        &xHandleDynamic2
        );
    if( xTask2 != pdPASS )
    {
        printf("TestQueue2 is NULL !\n");
        exit(0);
    }
    #endif //configSUPPORT_DYNAMIC_ALLOCATION

    tasks[0] = xHandleStatic;
    tasks[1] = xHandleDynamic;
    tasks[2] = xHandleDynamic2;

    /* Start the kernel.  From here on, only tasks and interrupts will run. */
    printf("\nScheduler starts !\n");
    vTaskStartScheduler();

    /* Exit FreeRTOS */
    return 0;
}
/*-----------------------------------------------------------*/

void vTestQueue0( void *parameters )
{
    ( void ) parameters;
    char *taskname = pcTaskGetName( NULL );
    uint32_t received = 0, ulValue = 0;
    message msgTab[ TABSIZE ];

    for(;;)
    {
        if( xQueueHandler == NULL )
            vTaskSuspend( NULL );

        if( uxQueueMessagesWaiting( xQueueHandler ) )
        {
            if( !xQueueReceive( xQueueHandler, ( void * ) &msgTab[received], ( TickType_t ) 0 ) )
            {
                printf("Error : %s receiving messages !\n", taskname);
                exit(0);
            }
            printf("\n%s received Message[%d] : %s\n",
                   taskname, msgTab[received].ucMessageID, msgTab[received].ucData);
            received++;
        }
        else
        {
            xTaskNotify( tasks[1], 0, eNoAction );
            printf("\n|<-%s waiting for Queue to be full.\n", taskname);
            xTaskNotifyWait( 0x00, portMAX_DELAY, &ulValue, portMAX_DELAY );
        }
        if( received == TABSIZE )
        {
            printf("\n%s received %d times \n", taskname, TABSIZE);
            for( uint32_t i = 0; i < TABSIZE; i++ )
            {
                printf("End %s received Message[%d] %s\n",
                       taskname, msgTab[i].ucMessageID, msgTab[i].ucData);
            }
            xTaskNotify( tasks[2], 0xff, eSetValueWithOverwrite );
            vTaskSuspend( NULL );
        }
    }
}
/*-----------------------------------------------------------*/

void vTestQueue1( void *parameters )
{
    ( void ) parameters;
    char *taskname = pcTaskGetName( NULL );
    uint32_t sent = 0, ulValue = 0;
    message msgTab[ TABSIZE ];

    for(;;)
    {
        if( xQueueHandler == NULL )
            vTaskSuspend( NULL );

        if( uxQueueSpacesAvailable( xQueueHandler ) )
        {
            msgTab[sent].ucMessageID = sent;
            sprintf(msgTab[sent].ucData, "Hello from %s %d", taskname, sent);
            printf("\n%s sending Message[%d] %s\n",
                   taskname, msgTab[sent].ucMessageID, msgTab[sent].ucData);
            if( !xQueueSendToBack( xQueueHandler,
                                   ( void * ) &msgTab[sent],
                                   ( TickType_t ) 0 ) )
            {
                printf("Error : %s sending messages !\n", taskname);
                exit(0);
            }
            printf("Number of messages in Queue : %d\n", uxQueueMessagesWaiting( xQueueHandler ));
            sent++;
        }
        else
        {
            xTaskNotify( tasks[0], 0, eNoAction );
            printf("\n|<-%s waiting for Queue to be empty.\n", taskname);
            xTaskNotifyWait( 0x00, portMAX_DELAY, &ulValue, portMAX_DELAY );
        }
        if( sent == TABSIZE )
        {
            xTaskNotify( tasks[0], 0, eNoAction );
            printf("\n%s sent %d times \n", taskname, TABSIZE);
            for( uint32_t i = 0; i < TABSIZE; i++ )
            {
                printf("End %s sent Message[%d] %s\n",
                       taskname, msgTab[i].ucMessageID, msgTab[i].ucData);
            }
            xTaskNotify( tasks[2], 0xff, eSetValueWithOverwrite );
            vTaskSuspend( NULL );
        }
    }
}
/*-----------------------------------------------------------*/

void vTestQueue2( void *parameters )
{
    ( void ) parameters;
    char *taskname = pcTaskGetName( NULL );
    uint32_t ulValue = 0;

    #if STATIC == 0
    xQueueHandler = xQueueCreate( QUEUE_LENGTH, ITEM_SIZE );
    #else
    xQueueHandler = xQueueCreateStatic( QUEUE_LENGTH,
                                        ITEM_SIZE,
                                        xQueueBuffer,
                                        &xQueueStaticStorage);
    #endif

    if( xQueueHandler == NULL )
        vTaskSuspend( NULL );
    printf("%s created a MessageQueue of %d rooms.\n", taskname, QUEUE_LENGTH);
    /* Wake tasks if suspended. */
    if( eTaskGetState( tasks[0] ) == eSuspended )
        vTaskResume( tasks[0] );
    if( eTaskGetState( tasks[1] ) == eSuspended )
        vTaskResume( tasks[1] );
    xTaskNotifyWait( 0x00, 0x00, &ulValue, portMAX_DELAY );
    printf("%s is suiciding after deleting Queue.\n", taskname);
    vQueueDelete( xQueueHandler );
    printf("Test success\n");
    vTaskSuspend( NULL );
}
/*-----------------------------------------------------------*/
