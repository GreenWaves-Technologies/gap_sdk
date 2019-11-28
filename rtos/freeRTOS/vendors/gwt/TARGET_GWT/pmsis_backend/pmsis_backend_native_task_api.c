#include "pmsis_backend_native_types.h"
#include "pmsis.h"
#include "gap_io.h"

/**
 * Kickoff the first "main" os task and scheduler
 */
int __os_native_kickoff(void *arg)
{
    BaseType_t xTask;
    TaskHandle_t xHandler0 = NULL;

    xTask = xTaskCreate(arg, "main", 1024,
                        NULL, tskIDLE_PRIORITY + 1, &xHandler0);
    if (xTask != pdPASS)
    {
        printf("main is NULL !\n");
        pmsis_exit(-4321);
    }

    __enable_irq();

    struct pmsis_event_kernel_wrap *wrap;
    pmsis_event_kernel_init(&wrap, pmsis_event_kernel_main);

    pmsis_event_set_default_scheduler(wrap);

    hal_compiler_barrier();

    /*
     * This should be used in case of printf via uart before scheduler has started.
     * Output will be on terminal instead of uart. After scheduler has started, output
     * will be via uart.
     */
    extern uint8_t g_freertos_scheduler_started;
    g_freertos_scheduler_started = 1;

    /* Start the kernel.  From here on, only tasks and interrupts will run. */
    vTaskStartScheduler();

    hal_compiler_barrier();

    /* Exit FreeRTOS */
    return 0;
}

void pi_time_wait_us(int time_us)
{
    /* Wait less than 1 ms. */
    if (time_us < 1000)
    {
        for (volatile int i=0; i<time_us; i++);
    }
    else
    {
        vTaskDelay((time_us/1000)/portTICK_PERIOD_MS);
    }
}
