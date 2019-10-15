/* PMSIS includes */
#include "pmsis.h"

/* Variables used. */
TaskHandle_t xHandler[3] = {NULL};

SemaphoreHandle_t xSemaphoreDynamic = NULL;

void vTestSemaphore0( void *parameters )
{
    char *taskname = pcTaskGetName( NULL );
    uint32_t delay = 10;
    for(;;)
    {
        if( xSemaphoreDynamic == NULL )
        {
            vTaskSuspend( NULL );
        }

        if( xSemaphoreTake( xSemaphoreDynamic, (TickType_t) 50 ) )
        {
            printf("\n%s takes Semaphore.\n", taskname);
            for(uint32_t i = 0; i < 10 ; i++)
            {
                printf("|-->%s : %2d\n", taskname, i);
            }

            printf("|<--%s releases Semaphore.\n", taskname);
            if( xTaskGetTickCount() > ( uint32_t ) parameters )
            {
                printf("%s deletes Semaphore.\n", taskname);
                vSemaphoreDelete( xSemaphoreDynamic );
                /* NULL so that task do not continue execution. */
                xSemaphoreDynamic = NULL;
                vTaskSuspend( NULL );
            }
            xSemaphoreGive( xSemaphoreDynamic );
            vTaskDelay( delay/portTICK_PERIOD_MS );
        }
    }
}

void vTestSemaphore1( void *parameters )
{
    char *taskname = pcTaskGetName( NULL );
    uint32_t delay = 10;
    for(;;)
    {
        if( xSemaphoreDynamic == NULL )
        {
            vTaskSuspend( NULL );
        }

        if( xSemaphoreTake( xSemaphoreDynamic, (TickType_t) 50 ) )
        {
            printf("\n%s takes Semaphore.\n", taskname);

            for(uint32_t i = 0; i < 100 ; i += 10)
            {
                printf("|-->%s : %2d\n", taskname, i);
            }

            printf("|<--%s releases Semaphore.\n", taskname);
            if( xTaskGetTickCount() > ( uint32_t ) parameters )
            {
                printf("%s deletes Semaphore.\n", taskname);
                vSemaphoreDelete( xSemaphoreDynamic );
                /* NULL so that task do not continue execution. */
                xSemaphoreDynamic = NULL;
                vTaskSuspend( NULL );
            }
            xSemaphoreGive( xSemaphoreDynamic );
            vTaskDelay( delay/portTICK_PERIOD_MS );
        }
    }
}

void vSemaphoreCreator( void *parameters )
{
    ( void ) parameters;
    char *taskname = pcTaskGetName( NULL );
    xSemaphoreDynamic = xSemaphoreCreateBinary();
    if( xSemaphoreDynamic == NULL )
    {
        pmsis_exit(-21);
    }
    printf("%s created a Semaphore.\n", taskname );
    /* Binary semaphores must first be released ! */
    xSemaphoreGive( xSemaphoreDynamic );
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

void test_semaphore(void)
{
    printf("Entering main controller\n");

    BaseType_t xTask;
    xTask = xTaskCreate( vTestSemaphore0, "TestSemaphore0", configMINIMAL_STACK_SIZE * 2,
                         ( void * ) 150, tskIDLE_PRIORITY + 1, &xHandler[0] );
    if( xTask != pdPASS )
    {
        printf("TestSemaphore0 is NULL !\n");
        pmsis_exit(-1);
    }

    xTask = xTaskCreate( vTestSemaphore1, "TestSemaphore1", configMINIMAL_STACK_SIZE * 2,
                         ( void * ) 100, tskIDLE_PRIORITY + 1, &xHandler[1] );
    if( xTask != pdPASS )
    {
        printf("TestSemaphore1 is NULL !\n");
        pmsis_exit(-2);
    }

    xTask = xTaskCreate( vSemaphoreCreator, "SemaphoreCreator", configMINIMAL_STACK_SIZE * 2,
                         NULL, tskIDLE_PRIORITY + 1, &xHandler[2] );
    if( xTask != pdPASS )
    {
        printf("TestSemaphore2 is NULL !\n");
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
    printf("\n\n\t *** Semaphore Test ***\n\n");
    return pmsis_kickoff((void *) test_semaphore);
}
