/****************************************************************************/
/* FreeRTOS functions includes. */
#include "FreeRTOS_util.h"

/* Demo utlities includes. */
#include "gap_fs.h"

/****************************************************************************/

/* Test task to test FreeRTOS port. */
void vTest_FSRO( void *parameters );

/* Utilities to control tasks. */
TaskHandle_t tasks[NBTASKS];
uint8_t taskSuspended;

/****************************************************************************/

/* Variables used. */
#define BUF_SIZE 1024
#define COUNT 5

static fs_file_t *file;
static char buff[ BUF_SIZE + 1 ];
static uint32_t count_done = 0;

/****************************************************************************/

/* Program Entry. */
int main( void )
{
    printf("\n\n\t *** Read Only Filesystem Test ***\n\n");

    #if configSUPPORT_DYNAMIC_ALLOCATION == 1
    BaseType_t xTask;
    TaskHandle_t xHandleDynamic = NULL;

    xTask = xTaskCreate(
        vTest_FSRO,
        "Test_FSRO",
        configMINIMAL_STACK_SIZE * 2,
        NULL,
        tskIDLE_PRIORITY + 1,
        &xHandleDynamic
        );
    if( xTask != pdPASS )
    {
        printf("Test_FSRO is NULL !\n");
        vTaskDelete( xHandleDynamic );
    }
    #endif //configSUPPORT_DYNAMIC_ALLOCATION

    tasks[0] = xHandleDynamic;

    /* Start the kernel.  From here on, only tasks and interrupts will run. */
    printf("\nScheduler starts !\n");
    vTaskStartScheduler();

  return 0;
}
/*-----------------------------------------------------------*/

static void check_file( void )
{
    int32_t size = 0;
    for( count_done = 0; count_done < COUNT; count_done++ )
    {
        size = fs_read( file, BUF_SIZE, buff );
        printf("%ld Read %ld bytes : \n%s\n", count_done, size, buff);
        if( size < BUF_SIZE )
            fs_seek( file, 0 );
    }
}
/*-----------------------------------------------------------*/

void vTest_FSRO( void *parameters )
{
    ( void ) parameters;
    char *taskname = pcTaskGetName( NULL );
    printf("%s executing Filesystem test :\n\n", taskname);

    char name[] = "hello.txt";
    fs_config_t conf;
    fs_config_default( &conf );

    fs_handle_t *fs = ( fs_handle_t * ) pvPortMalloc( sizeof( fs_handle_t ) );
    uint32_t err = fs_mount( fs, fs_HYPER, &conf );
    if( err )
    {
        printf("Error FS mounting !\n");
        vPortFree( fs );
        exit(-1);
    }
    printf("FS mounted.\n");

    file = fs_open( fs, name, 0 );
    if( file == NULL )
    {
        printf("File open failed !\n");
        exit(-1);
    }
    printf("File %s open success.\n", file->name);

    check_file();

    fs_close( file );
    printf("File %s closed.\n", file->name);

    fs_unmount( fs );
    vPortFree( fs );
    printf("FS unmounted.\n");

    exit(0);
    vTaskSuspend( NULL );
}
/*-----------------------------------------------------------*/
