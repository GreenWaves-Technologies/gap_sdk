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

    /* Enable IRQ for context switch. */
    NVIC_EnableIRQ(PENDSV_IRQN);

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
    //time_us--;
    if (time_us > 0)
    {
        /* Wait less than 1 ms. */
        if (time_us < 1000)
        {
            uint32_t irq = pi_irq_disable();
            uint32_t freq_fc = pi_freq_get(PI_FREQ_DOMAIN_FC);
            //uint64_t counter = (uint64_t) (((uint64_t) time_us) * freq_fc) / 1000000;
            //uint64_t counter = (uint64_t) (((uint64_t) time_us) * freq_fc);
            uint64_t freq = (uint64_t) ((uint64_t) time_us * (uint64_t) freq_fc);
            uint32_t counter = (uint32_t) ((freq) >> 20);
            //counter >>= 20; /* Div 10^6 */
            //uint32_t counter = (uint32_t) time_us;
            //counter = (counter * freq_fc) / 1000000;
            //printf("counter=%ld, freq=%ld, time_us=%d\n", counter, freq_fc, time_us);
            pi_irq_restore(irq);
            for (volatile uint32_t i=0; i<counter; i++);
        }
        else
        {
            pi_task_t task_block;
            pi_task_block(&task_block);
            pi_task_delayed_fifo_enqueue(&task_block, time_us);
            pi_task_wait_on(&task_block);
        }
    }
}

unsigned long long int pi_time_get_us()
{
    uint64_t time = 0;
    uint32_t irq = pi_irq_disable();
    uint32_t cur_tick = xTaskGetTickCount();
    uint64_t cur_timer_val = pi_timer_value_read(SYS_TIMER);
    uint32_t freq_timer = system_core_clock_get();
    cur_timer_val = (cur_timer_val * 1000000) / freq_timer;
    time = (cur_tick * 1000);
    time += cur_timer_val;
    //time += 95; /* Around 95us between main() and kernel start. */
    pi_irq_restore(irq);
    return time;
}

PI_FC_L1 struct pi_task_delayed_s delayed_task = {0};

void pi_task_delayed_fifo_enqueue(struct pi_task *task, uint32_t delay_us)
{
    task->data[8] = (delay_us/1000)/portTICK_PERIOD_MS;
    /* Add 1ms, in case the next SysTick IRQ is too close. This will ensure at
       least 1ms has passed. */
    task->data[8]++;
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

// return value allows to skip some OS logic when a switch has already been triggered
int pi_task_delayed_increment_push(void)
{
    struct pi_task *task = delayed_task.fifo_head;
    struct pi_task *prev_task = delayed_task.fifo_head;
    int ret = 1;
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
            __pi_irq_handle_end_of_task(task);
            ret = 0;
        }
        prev_task = task;
        task = task->next;
    }
    return ret;
}
