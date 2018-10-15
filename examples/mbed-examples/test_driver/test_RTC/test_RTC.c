#include <stdlib.h>
#include <string.h>
#include "gap_common.h"
#include "mbed_wait_api.h"

#define TIMER_SECONDS 10

int flag = 0;

void timer_irq_handler() {
    printf("Timer arrive .....\n");
    flag = 1;
}

int main()
{
    printf("Fabric controller code execution for mbed_os RTC driver test\n");

    int repeat_en = 0;
    rtc_config_t config;
    RTC_GetDefaultConfig(&config);

    RTC_Init(RTC_APB, &config);

    /* Set Timer */
    RTC_SetTimer(RTC_APB, TIMER_SECONDS);

    /* Binding RTC IRQ */
    RTC_CreateHandler((rtc_callback_t)timer_irq_handler, NULL);

    printf("Start Timer value now = %d\n", TIMER_SECONDS);

    /* Start Timer */
    RTC_StartTimer(RTC_APB, repeat_en);

    /* Wait several seconds */
    wait(2);

    /* Read Timer */
    int timer_now = RTC_GetTimer(RTC_APB);
    printf("After 2 seconds, Timer value now = %d\n", timer_now);

    wait(10);

    int error = 0;
    if (!flag || (timer_now != (TIMER_SECONDS - 2))) {
        printf("Test failed\n");
        error = 1;
    } else {
        printf("Test success\n");
    }

    #ifdef JENKINS_TEST_FLAG
    exit(error);
    #else
    return error;
    #endif
}
