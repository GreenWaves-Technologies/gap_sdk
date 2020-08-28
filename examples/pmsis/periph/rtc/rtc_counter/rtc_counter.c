/* PMSIS includes. */
#include "pmsis.h"

/* Variables used. */
#define TIMER_SECONDS         ( 3 )

static struct pi_device rtc;
static volatile uint32_t g_flag = 0;

void timer_irq_handler(void *arg)
{
    printf("Count down arrive ..... %x\n", *((uint32_t *)arg));
    g_flag++;
}

void test_rtc_counter(void)
{
    printf("Entering main controller\n");

    int32_t errors = 0;
    uint32_t repeat = 3;
    uint8_t repeat_en = (repeat > 0) ? 1 : 0;

    /* Init & open RTC. */
    struct pi_rtc_conf rtc_conf = {0};
    pi_rtc_conf_init(&rtc_conf);
    rtc_conf.mode = PI_RTC_MODE_TIMER;
    rtc_conf.counter = (uint32_t) TIMER_SECONDS;
    pi_open_from_conf(&rtc, &rtc_conf);
    errors = pi_rtc_open(&rtc);
    if (errors)
    {
        printf("Error rtc open %d\n", errors);
        pmsis_exit(-1);
    }

    /* Timer initial value. */
    uint32_t counter = 0;
    counter = pi_rtc_timer_get(&rtc);
    printf("Start Timer value now = %d, repeat %d time(s).\n", counter, repeat);

    /* Binding RTC IRQ. */
    uint32_t arg = 0xabbadead;
    struct pi_task task = {0};
    pi_task_callback(&task, timer_irq_handler, &arg);
    pi_rtc_ioctl(&rtc, PI_RTC_ATTACH_CB_TIMER, &task);

    /* Start timer */
    pi_rtc_ioctl(&rtc, PI_RTC_TIMER_START, (void *) ((uint32_t) repeat_en));

    uint32_t timer_now = 0, timer = 0;
    do
    {
        timer_now = pi_rtc_timer_get(&rtc);
        printf("Timer value now = %d\n", timer_now);

        /* Wait 1 second. */
        pi_time_wait_us(1000 * 1000);
    } while (g_flag != repeat);

    pi_rtc_close(&rtc);

    printf("\nTest %s with %ld error(s) !\n", (errors) ? "failed" : "success", errors);

    pmsis_exit(errors);
}

/* Program Entry. */
int main(void)
{
    printf("\n\n\t *** Test RTC counter ***\n\n");
    return pmsis_kickoff((void *) test_rtc_counter);
}
