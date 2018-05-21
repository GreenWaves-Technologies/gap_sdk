#include<string.h>
#include "gap_common.h"
#include "mbed_wait_api.h"

#define COUNT 10

int main()
{
    printf("Fabric controller code execution for mbed_os RTC driver test\n");

    int repeat_en = 0;
    rtc_config_t config;
    RTC_GetDefaultConfig(&config);

    RTC_Init(RTC_APB, &config);

    RTC_SetCountDown(RTC_APB, COUNT);

    printf("Start CountDown value now = %d\n", COUNT);

    RTC_StartCountDown(RTC_APB, repeat_en);

    wait(2);

    printf("After 2 seconds, CountDown value now = %d\n", RTC_GetCountDown(RTC_APB));

    return 0;
}
