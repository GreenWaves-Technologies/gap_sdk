/* PMSIS includes */
#include "pmsis.h"

/* Variables used. */
TaskHandle_t xHandler[3] = {NULL};

/*
 * Structure sent and received.
 * In FreeRTOS, data sent are sent by copy not reference.
 */
struct _message
{
    uint8_t ucMessageID;
    char ucData[25];
};

typedef struct _message message;
#define QUEUE_LENGTH    ( 5 )
#define ITEM_SIZE       ( sizeof( message ) )
#define TABSIZE         ( 2 * QUEUE_LENGTH )

QueueHandle_t xQueueHandler = NULL;

void vTestQueue0( void *parameters )
{
    ( void ) parameters;
    char *taskname = pcTaskGetName( NULL );
    uint32_t received = 0, ulValue = 0;
    message msgTab[ TABSIZE ];

    for(;;)
    {
        if( xQueueHandler == NULL )
        {
            vTaskSuspend( NULL );
        }

        if( uxQueueMessagesWaiting( xQueueHandler ) )
        {
            if( !xQueueReceive( xQueueHandler, ( void * ) &msgTab[received], ( TickType_t ) 0 ) )
            {
                printf("Error : %s receiving messages !\n", taskname);
                vTaskSuspend( NULL );
            }
            received++;
        }
        else
        {
            xTaskNotify( xHandler[1], 0, eNoAction );
            printf("%s waiting for Queue to be full.\n", taskname);
            xTaskNotifyWait( 0x00, portMAX_DELAY, &ulValue, portMAX_DELAY );
        }
        if( received == TABSIZE )
        {
            printf("%s received %d times \n", taskname, TABSIZE);
            for( uint32_t i = 0; i < TABSIZE; i++ )
            {
                printf("%s received Message[%d] %s\n",
                       taskname, msgTab[i].ucMessageID, msgTab[i].ucData);
            }
            xTaskNotify( xHandler[2], 0x0F, eSetBits );
            vTaskSuspend( NULL );
        }
    }
}

void vTestQueue1( void *parameters )
{
    ( void ) parameters;
    char *taskname = pcTaskGetName( NULL );
    uint32_t sent = 0, ulValue = 0;
    message msgTab[ TABSIZE ];

    for(;;)
    {
        if( xQueueHandler == NULL )
        {
            vTaskSuspend( NULL );
        }

        if( uxQueueSpacesAvailable( xQueueHandler ) )
        {
            msgTab[sent].ucMessageID = sent;
            sprintf(msgTab[sent].ucData, "Hello from %s %d", taskname, sent);
            printf("%s sending Message[%d] %s\n",
                   taskname, msgTab[sent].ucMessageID, msgTab[sent].ucData);
            if( !xQueueSendToBack( xQueueHandler,
                                   ( void * ) &msgTab[sent],
                                   ( TickType_t ) 0 ) )
            {
                printf("Error : %s sending messages !\n", taskname);
                vTaskSuspend( NULL );
            }
            printf("%s Number of messages in Queue : %d\n",
                   taskname, uxQueueMessagesWaiting( xQueueHandler ));
            sent++;
        }
        else
        {
            xTaskNotify( xHandler[0], 0, eNoAction );
            printf("%s waiting for Queue to be empty.\n", taskname);
            xTaskNotifyWait( 0x00, portMAX_DELAY, &ulValue, portMAX_DELAY );
        }
        if( sent == TABSIZE )
        {
            xTaskNotify( xHandler[0], 0, eNoAction );
            printf("%s sent %d times \n", taskname, TABSIZE);
            xTaskNotify( xHandler[2], 0xF0, eSetBits );
            vTaskSuspend( NULL );
        }
    }
}

void vQueueCreator( void *parameters )
{
    ( void ) parameters;
    char *taskname = pcTaskGetName( NULL );
    uint32_t ulValue = 0, wait_val = 0;

    xQueueHandler = xQueueCreate( QUEUE_LENGTH, ITEM_SIZE );

    if( xQueueHandler == NULL )
    {
        vTaskSuspend( NULL );
    }
    printf("%s created a MessageQueue of %d rooms.\n", taskname, QUEUE_LENGTH);
    /* Wake tasks if suspended. */
    if( eTaskGetState( xHandler[0] ) == eSuspended )
    {
        vTaskResume( xHandler[0] );
    }
    if( eTaskGetState( xHandler[1] ) == eSuspended )
    {
        vTaskResume( xHandler[1] );
    }
    while ( wait_val != 0xFF )
    {
        xTaskNotifyWait( 0xFFFFFFFF, 0x00, &ulValue, portMAX_DELAY );
        wait_val |= ulValue;
    }
    printf("%s deleting Queue and suspending.\n", taskname);
    vQueueDelete( xQueueHandler );

    vTaskSuspend( NULL );
}

void test_simple_queue(void)
{
    printf("Entering main controller\n");

    BaseType_t xTask;
    xTask = xTaskCreate( vTestQueue0, "QueueReceiver", configMINIMAL_STACK_SIZE * 2,
                         NULL, tskIDLE_PRIORITY + 1, &xHandler[0] );
    if( xTask != pdPASS )
    {
        printf("QueueReceiver is NULL !\n");
        pmsis_exit(-1);
    }

    xTask = xTaskCreate( vTestQueue1, "QueueSender", configMINIMAL_STACK_SIZE * 2,
                         NULL, tskIDLE_PRIORITY + 1, &xHandler[1] );
    if( xTask != pdPASS )
    {
        printf("QueueSender is NULL !\n");
        pmsis_exit(-2);
    }

    xTask = xTaskCreate( vQueueCreator, "QueueCreator", configMINIMAL_STACK_SIZE * 2,
                         NULL, tskIDLE_PRIORITY + 1, &xHandler[2] );
    if( xTask != pdPASS )
    {
        printf("QueueSender is NULL !\n");
        pmsis_exit(-3);
    }

    while( ( eTaskGetState(xHandler[0]) != eSuspended ) ||
           ( eTaskGetState(xHandler[1]) != eSuspended ) ||
           ( eTaskGetState(xHandler[2]) != eSuspended ) )
    {
        pi_yield();
    }

    printf("Test success !\n");

    pmsis_exit(0);
}

/* Program Entry. */
int main(void)
{
    printf("\n\n\t *** Queue Message Test ***\n\n");
    return pmsis_kickoff((void *) test_simple_queue);
}
