/****************************************************************************/
/* FreeRTOS functions includes. */
#include "FreeRTOS_util.h"

/* Demo utlities includes. */

/****************************************************************************/

/* Test task to test FreeRTOS port. */
void vTaskTest0( void *parameters );
void vTaskTest1( void *parameters );

/* Utilities to control tasks. */
TaskHandle_t tasks[NBTASKS+3];
uint8_t taskSuspended;


#if configSUPPORT_STATIC_ALLOCATION == 1
StaticTask_t xTaskBuffer;
StackType_t xStack[ configMINIMAL_STACK_SIZE * 2 ];
#endif //configSUPPORT_STATIC_ALLOCATION

/****************************************************************************/

/* Variables used. */
volatile uint8_t task0 = 0, task1 = 0, task2 = 0, resumeTask0 = 1;

/****************************************************************************/

/* Program Entry. */
int main( void )
{
    printf("\n\n\t *** Tasks Test ***\n\n");

    #if configSUPPORT_STATIC_ALLOCATION == 1
    TaskHandle_t xHandler0 = NULL;
    xHandler0 = xTaskCreateStatic( vTaskTest0, "Thread0", configMINIMAL_STACK_SIZE * 2,
                                   ( void * ) 10, tskIDLE_PRIORITY + 1, xStack, &xTaskBuffer );
    if( xHandler0 == NULL )
    {
        printf("Thread0 is NULL !\n");
        return 0;
    }
    #endif //configSUPPORT_STATIC_ALLOCATION

    #if configSUPPORT_DYNAMIC_ALLOCATION == 1
    BaseType_t xTask;
    TaskHandle_t xHandler1 = NULL, xHandler2 = NULL, xHandler3 = NULL;

    xTask = xTaskCreate( vTaskTest0, "Thread1", configMINIMAL_STACK_SIZE * 2,
                         ( void * ) 20, tskIDLE_PRIORITY + 1, &xHandler1 );
    if( xTask != pdPASS )
    {
        printf("Thread1 is NULL !\n");
        return 0;
    }

    xTask = xTaskCreate( vTaskTest0, "Thread2", configMINIMAL_STACK_SIZE * 2,
                         ( void * ) 30, tskIDLE_PRIORITY + 1, &xHandler2 );
    if( xTask != pdPASS )
    {
        printf("Thread2 is NULL !\n");
        return 0;
    }

    xTask = xTaskCreate( vTaskTest1, "Thread3", configMINIMAL_STACK_SIZE * 2,
                         NULL, tskIDLE_PRIORITY + 1, &xHandler3 );
    if( xTask != pdPASS )
    {
        printf("Thread3 is NULL !\n");
        return 0;
    }
    #endif //configSUPPORT_DYNAMIC_ALLOCATION

    tasks[0] = xHandler0;
    tasks[1] = xHandler1;
    tasks[2] = xHandler2;
    tasks[3] = xHandler3;

    /* Start the kernel.  From here on, only tasks and interrupts will run. */
    printf("\nScheduler starts !\n");
    vTaskStartScheduler();

    /* Exit FreeRTOS */
    return 0;
}
/*-----------------------------------------------------------*/

void vTaskTest0( void *parameters )
{
    char *taskname = pcTaskGetName( NULL );
    uint32_t pid = 0, cid = 0, mepc = 0;
    uint32_t loop = ( uint32_t ) parameters;
    for( uint32_t i = 0; i < loop; i += 2 )
    {

        asm volatile("csrr %0, mepc":"=r"(mepc));
        //printf("MEPC %x\n", mepc);
        printf("%s : %d\t TICK = %d\t ProcID = %d\t ClusterID = %d\n",
        taskname, i, xTaskGetTickCount(), pid, cid);
        #if configUSE_PREEMPTION == 0
        taskYIELD();
        #endif
    }
    #if 0
    for( uint32_t i = 0; i < loop; i += 2 )
    {
        pid = procid();
        cid = clusterid();
        printf("%s : %d\t TICK = %d\t ProcID = %d\t ClusterID = %d\n",
        taskname, i, xTaskGetTickCount(), pid, cid);
        #if configUSE_PREEMPTION == 0
        taskYIELD();
        #endif
    }
    printf("%s suspending.\n", taskname);
    vTaskSuspend( NULL );
    printf("%s resumed.\n", taskname);
    for( uint32_t i = 0; i < loop; i++ )
    {
        pid = procid();
        cid = clusterid();
        printf("%s : %d\t TICK = %d\t ProcID = %d\t ClusterID = %d\n",
        taskname, i, xTaskGetTickCount(), pid, cid);
        #if configUSE_PREEMPTION == 0
        taskYIELD();
        #endif
    }
    #endif
    printf("%s suspending again.\n", taskname);
    vTaskSuspend( NULL );
}
/*-----------------------------------------------------------*/

void vTaskTest1( void *parameters )
{
    ( void ) parameters;
    char *taskname = pcTaskGetName( NULL );
    for(;;)
    {
        printf("%s \t TICK = %d\n", taskname, xTaskGetTickCount());
        #if 1
        if( ( eTaskGetState( tasks[0] ) == eSuspended ) && resumeTask0 )
        {
            printf("%s resuming %s\n", taskname, pcTaskGetName( tasks[0] ));
            resumeTask0 = 0;
            vTaskResume( tasks[0] );
        }

        if( ( ( eTaskGetState( tasks[0] ) == eSuspended ) || ( eTaskGetState( tasks[0] ) == eDeleted ) ) &&
        ( ( eTaskGetState( tasks[1] ) == eSuspended ) || ( eTaskGetState( tasks[1] ) == eDeleted ) ) &&
        ( ( eTaskGetState( tasks[2] ) == eSuspended ) || ( eTaskGetState( tasks[2] ) == eDeleted ) ) )
        {
            printf("%s all tasks suspended, exiting...\n", taskname);
            printf("Test success\n");
            vTaskSuspend( NULL );
        }
        #endif
        #if configUSE_PREEMPTION == 0
        taskYIELD();
        #endif
    }
}
/*-----------------------------------------------------------*/
