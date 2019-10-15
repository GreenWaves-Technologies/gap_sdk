/* PMSIS includes */
#include "pmsis.h"

/* Variables used. */
TaskHandle_t xHandler[3] = {NULL};

SemaphoreHandle_t xMutexHandler = NULL;

void vTestMutex0( void *parameters )
{
    char *taskname = pcTaskGetName( NULL );
    uint32_t delay = 5;
    for(;;)
    {
        if( xMutexHandler == NULL )
        {
            vTaskSuspend( NULL );
        }

        if( xSemaphoreTake( xMutexHandler, (TickType_t) 50 ) )
        {
            printf("\n%s takes Mutex.\n", taskname);

            for( uint32_t i = 0; i < 10 ; i++ )
            {
                printf("|-->%s : %2d, Priority : %d\n",
                       taskname, i, uxTaskPriorityGet( xTaskGetCurrentTaskHandle() ));
            }

            printf("|<--%s releases Mutex.\n", taskname);
            if( xTaskGetTickCount() > ( uint32_t ) parameters )
            {
                printf("%s deletes Mutex.\n", taskname);
                vSemaphoreDelete( xMutexHandler );
                /* NULL so that task do not continue execution. */
                xMutexHandler = NULL;
                vTaskSuspend( NULL );
            }
            xSemaphoreGive( xMutexHandler );
            vTaskDelay( delay/portTICK_PERIOD_MS );
        }
    }
}

void vTestMutex1( void *parameters )
{
    char *taskname = pcTaskGetName( NULL );
    uint32_t delay = 10;
    for(;;)
    {
        if( xMutexHandler == NULL )
        {
            vTaskSuspend( NULL );
        }

        if( xSemaphoreTake( xMutexHandler, (TickType_t) 50 ) )
        {
            printf("\n%s takes Mutex.\n", taskname);

            for( uint32_t i = 0; i < 100 ; i += 10 )
            {
                printf("|-->%s : %2d, Priority : %d\n",
                       taskname, i, uxTaskPriorityGet( xTaskGetCurrentTaskHandle() ));
            }

            printf("|<--%s releases Mutex.\n", taskname);
            if( xTaskGetTickCount() > ( uint32_t ) parameters )
            {
                printf("%s deletes Mutex.\n", taskname);
                vSemaphoreDelete( xMutexHandler );
                /* NULL so that task do not continue execution. */
                xMutexHandler = NULL;
                vTaskSuspend( NULL );
            }
            xSemaphoreGive( xMutexHandler );
            vTaskDelay( delay/portTICK_PERIOD_MS );
        }
    }
}

void vMutexCreator( void *parameters )
{
    ( void ) parameters;
    char *taskname = pcTaskGetName( NULL );

    xMutexHandler = xSemaphoreCreateMutex();

    if( xMutexHandler == NULL )
    {
        vTaskSuspend( NULL );
    }
    printf("%s created a Mutex.\n", taskname);
    /* Wake tasks if suspended. */
    if( eTaskGetState( xHandler[0] ) == eSuspended )
    {
        vTaskResume( xHandler[0] );
    }
    if( eTaskGetState( xHandler[1] ) == eSuspended )
    {
        vTaskResume( xHandler[1] );
    }
    printf("%s suspending.\n", taskname);
    vTaskSuspend( NULL );
}

void test_mutex( void )
{
    printf("Entering main controller\n");

    BaseType_t xTask;
    xTask = xTaskCreate( vTestMutex0, "TestMutex0", configMINIMAL_STACK_SIZE * 2,
                         ( void * ) 200, tskIDLE_PRIORITY + 1, &xHandler[0] );
    if( xTask != pdPASS )
    {
        printf("TestMutex0 is NULL !\n");
        pmsis_exit(-1);
    }

    xTask = xTaskCreate( vTestMutex1, "TestMutex1", configMINIMAL_STACK_SIZE * 2,
                         ( void * ) 100, tskIDLE_PRIORITY + 2, &xHandler[1] );
    if( xTask != pdPASS )
    {
        printf("vTestMutex1 is NULL !\n");
        pmsis_exit(-2);
    }

    xTask = xTaskCreate( vMutexCreator, "MutexCreator", configMINIMAL_STACK_SIZE * 2,
                         NULL, tskIDLE_PRIORITY + 1, &xHandler[2] );
    if( xTask != pdPASS )
    {
        printf("TestMutex2 is NULL !\n");
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
    printf("\n\n\t *** Mutex Test ***\n\n");
    return pmsis_kickoff((void *) test_mutex);
}
