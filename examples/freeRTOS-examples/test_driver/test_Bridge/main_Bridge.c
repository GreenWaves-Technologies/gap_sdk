/****************************************************************************/
/* FreeRTOS functions includes. */
#include "FreeRTOS_util.h"

/* Demo utlities includes. */
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "gap_common.h"

/****************************************************************************/

/* Test task to test FreeRTOS port. */
void vTestBridge( void *parameters );

/* Utilities to control tasks. */
TaskHandle_t tasks[NBTASKS];
uint8_t taskSuspended;

/****************************************************************************/

/* Variables used. */

int8_t buffer[8192];

/****************************************************************************/

/* Program Entry. */
int main( void )
{
    printf("\n\n\t *** Bridge Test ***\n\n");

    #if configSUPPORT_DYNAMIC_ALLOCATION == 1
    BaseType_t xTask;
    TaskHandle_t xHandler0 = NULL;

    xTask = xTaskCreate( vTestBridge, "TestBridge", configMINIMAL_STACK_SIZE * 2,
                         NULL, tskIDLE_PRIORITY + 1, &xHandler0 );
    if( xTask != pdPASS )
    {
        printf("TestBridge is NULL !\n");
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

void vTestBridge( void *parameters )
{
    ( void ) parameters;
    char *taskname = pcTaskGetName( NULL );

    BRIDGE_Init();

    printf("Connecting to bridge\n");
    BRIDGE_Connect(0, NULL);
    printf("Connection done\n");

    int32_t file = BRIDGE_Open( "../../../main_Bridge.c", 0, 0, NULL );
    int32_t file2 = BRIDGE_Open( "../../../out_Bridge.c", O_RDWR | O_CREAT, S_IRWXU, NULL );
    if( file == -1 || file2 == -1 )
    {
        printf("Error open file !\n");
        vTaskSuspend( NULL );
    }

    while ( 1 )
    {
        int32_t res = BRIDGE_Read( file, buffer, 8192, NULL );
        if( res == 0 )
        {
            break;
        }
        BRIDGE_Write( file2, buffer, res, NULL );

        buffer[res] = 0;
        //puts( buffer );
    }

    BRIDGE_Disconnect( NULL );

    printf("%s suspending.\n", taskname);
    vTaskSuspend( NULL );
}
