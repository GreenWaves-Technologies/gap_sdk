#include <stdlib.h>
#include <string.h>
#include "gap_common.h"
#include "mbed_wait_api.h"

#define COUNT_SECONDS 10

int flag = 0;

void timer_irq_handler() {
    printf("Count down arrive .....\n");
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
    RTC_SetCountDown(RTC_APB, COUNT_SECONDS);

    /* Binding RTC IRQ */
    RTC_IRQHandlerBind((uint32_t)timer_irq_handler);

    printf("Start CountDown value now = %d\n", COUNT_SECONDS);

    /* Start Timer */
    RTC_StartCountDown(RTC_APB, repeat_en);

    /* Wait several seconds */
    wait(2);

    /* Read Timer */
    int countdown_now = RTC_GetCountDown(RTC_APB);
    printf("After 2 seconds, CountDown value now = %d\n", countdown_now);

    wait(10);

    int error = 0;
    if (!flag) {
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
