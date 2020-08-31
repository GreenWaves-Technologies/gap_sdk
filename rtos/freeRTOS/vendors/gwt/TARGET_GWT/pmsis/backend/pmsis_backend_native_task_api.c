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

    uint32_t stack_size = (uint32_t) MAIN_APP_STACK_SIZE;
    stack_size /= sizeof(configSTACK_DEPTH_TYPE);
    xTask = xTaskCreate(arg, "main", stack_size,
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

    extern SemaphoreHandle_t g_printf_mutex;
    g_printf_mutex = xSemaphoreCreateMutex();
    if (g_printf_mutex == NULL)
    {
        printf("Error : printf mutex not created !\n", g_printf_mutex);
        pmsis_exit(-4322);
    }

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

struct pi_task_delayed_s
{
    struct pi_task *fifo_head;
    struct pi_task *fifo_tail;
};

struct pi_task_delayed_s delayed_task = {0};

void pi_task_delayed_fifo_enqueue(struct pi_task *task, uint32_t delay_us)
{
    task->data[8] = (delay_us/1000);
    task->next = NULL;
    if (delayed_task.fifo_head == NULL)
    {
        delayed_task.fifo_head = task;
    }
    else
    {
        delayed_task.fifo_tail->next = task;
    }
    delayed_task.fifo_tail = task;
}

void pi_task_delayed_increment_push(void)
{
    struct pi_task *task = delayed_task.fifo_head;
    struct pi_task *prev_task = delayed_task.fifo_head;
    while (task != NULL)
    {
        task->data[8]--;
        if ((int32_t) task->data[8] <= 0)
        {
            if (task == delayed_task.fifo_head)
            {
                delayed_task.fifo_head = task->next;
            }
            else
            {
                prev_task->next = task->next;
            }
            pi_task_push(task);
        }
        prev_task = task;
        task = task->next;
    }
}
