/****************************************************************************/
/* FreeRTOS functions includes. */
#include "FreeRTOS_util.h"

/* Demo utlities includes. */

/****************************************************************************/

/* Test task to test FreeRTOS port. */
void vTestSemaphore0( void *parameters );
void vTestSemaphore1( void *parameters );
void vTestSemaphore2( void *parameters );

/* Utilities to control tasks. */
TaskHandle_t tasks[NBTASKS+2];
uint8_t taskSuspended;


#if configSUPPORT_STATIC_ALLOCATION == 1
StaticTask_t xTaskBuffer;
StackType_t xStack[ configMINIMAL_STACK_SIZE * 2 ];
#endif //configSUPPORT_STATIC_ALLOCATION

/****************************************************************************/

/* Variables used. */
SemaphoreHandle_t xSemaphoreDynamic = NULL;

/****************************************************************************/

/* Program Entry. */
int main( void )
{
    printf("\n\n\t *** Semaphore Test ***\n\n");

    #if configSUPPORT_STATIC_ALLOCATION == 1
    TaskHandle_t xHandleStatic = NULL;

    xHandleStatic = xTaskCreateStatic(
        vTestSemaphore0,
        "TestSemaphore0",
        configMINIMAL_STACK_SIZE * 2,
        ( void * ) 1500,
        tskIDLE_PRIORITY + 1,
        xStack,
        &xTaskBuffer
        );
    if( xHandleStatic == NULL )
    {
        printf("TestStatic is NULL !\n");
        exit(0);
    }
    #endif //configSUPPORT_STATIC_ALLOCATION


    #if configSUPPORT_DYNAMIC_ALLOCATION == 1
    BaseType_t xTask, xTask2;
    TaskHandle_t xHandleDynamic = NULL,        xHandleDynamic2 = NULL;

    xTask = xTaskCreate(
        vTestSemaphore1,
        "TestSemaphore1",
        configMINIMAL_STACK_SIZE * 2,
        ( void * ) 1000,
        tskIDLE_PRIORITY + 1,
        &xHandleDynamic
        );
    if( xTask != pdPASS )
    {
        printf("TestSemaphore1 is NULL !\n");
        exit(0);
    }

    xTask2 = xTaskCreate(
        vTestSemaphore2,
        "TestSemaphore2",
        configMINIMAL_STACK_SIZE * 2,
        NULL,
        tskIDLE_PRIORITY + 1,
        &xHandleDynamic2
        );
    if( xTask2 != pdPASS )
    {
        printf("TestSemaphore2 is NULL !\n");
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

void vTestSemaphore0( void *parameters )
{
    char *taskname = pcTaskGetName( NULL );
    uint32_t delay = 10;
    for(;;)
    {
        if( xSemaphoreDynamic == NULL )
            vTaskSuspend( NULL );

        if( xSemaphoreTake( xSemaphoreDynamic, (TickType_t) 50 ) )
        {
            printf("\n%s takes Semaphore.\n", taskname);
            for(int i = 0; i < 10 ; i++)
                printf("|-->%s : %2d\n", taskname, i);

            printf("|<--%s releases Semaphore.\n", taskname);
            if( xTaskGetTickCount() > ( uint32_t ) parameters )
            {
                printf("%s deletes Semaphore.\n", taskname);
                vSemaphoreDelete( xSemaphoreDynamic );
                /* NULL so that task do not continue execution. */
                xSemaphoreDynamic = NULL;
                printf("Test success\n");
                vTaskSuspend( NULL );
            }
            xSemaphoreGive( xSemaphoreDynamic );
            vTaskDelay( delay/portTICK_PERIOD_MS );
        }
    }
}
/*-----------------------------------------------------------*/

void vTestSemaphore1( void *parameters )
{
    char *taskname = pcTaskGetName( NULL );
    uint32_t delay = 10;
    for(;;)
    {
        if( xSemaphoreDynamic == NULL )
            vTaskSuspend( NULL );

        if( xSemaphoreTake( xSemaphoreDynamic, (TickType_t) 50 ) )
        {
            printf("\n%s takes Semaphore.\n", taskname);

            for(int i = 0; i < 100 ; i += 10)
                printf("|-->%s : %2d\n", taskname, i);

            printf("|<--%s releases Semaphore.\n", taskname);
            if( xTaskGetTickCount() > ( uint32_t ) parameters )
            {
                printf("%s deletes Semaphore.\n", taskname);
                vSemaphoreDelete( xSemaphoreDynamic );
                /* NULL so that task do not continue execution. */
                xSemaphoreDynamic = NULL;
                printf("Test success\n");
                vTaskSuspend( NULL );
            }
            xSemaphoreGive( xSemaphoreDynamic );
            vTaskDelay( delay/portTICK_PERIOD_MS );
        }
    }
}
/*-----------------------------------------------------------*/

void vTestSemaphore2( void *parameters )
{
    ( void ) parameters;
    char *taskname = pcTaskGetName( NULL );
    xSemaphoreDynamic = xSemaphoreCreateBinary();
    if( xSemaphoreDynamic == NULL )
        exit(0);
    printf("%s created a Semaphore.\n", taskname );
    /* Binary semaphores must first be released ! */
    xSemaphoreGive( xSemaphoreDynamic );
    /* Wake tasks if suspended. */
    if( eTaskGetState( tasks[0] ) == eSuspended )
        vTaskResume( tasks[0] );
    if( eTaskGetState( tasks[1] ) == eSuspended )
        vTaskResume( tasks[1] );
    printf("%s is suiciding.\n", taskname);
    vTaskSuspend( NULL );
}
/*-----------------------------------------------------------*/
