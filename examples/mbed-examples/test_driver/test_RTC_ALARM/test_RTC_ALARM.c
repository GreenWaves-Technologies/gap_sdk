#include<string.h>
#include "gap_common.h"
#include "mbed_wait_api.h"
#include <time.h>

int main()
{
    printf("Fabric controller code execution for mbed_os RTC driver test\n");

    rtc_config_t config;
    RTC_GetDefaultConfig(&config);

    RTC_Init(RTC_APB, &config);

    rtc_datetime_t setTime, now;

    setTime.year   = 2017;
    setTime.month  = 4;
    setTime.day    = 26;
    setTime.hour   = 17;
    setTime.minute = 45;
    setTime.second = 0;

    printf ("Calendar Time now %d/%d/%d %d:%d:%d\n",
            setTime.year,
            setTime.month,
            setTime.day,
            setTime.hour,
            setTime.minute,
            setTime.second);
    /* Start calendar */
    RTC_SetCalendar(RTC_APB, &setTime);
    RTC_StartCalendar(RTC_APB);

    printf ("Wait 10s...\n");
    wait(10);
    RTC_GetCalendar(RTC_APB, &now);

    printf ("Calendar Time now %d/%d/%d %d:%d:%d\n",
            now.year,
            now.month,
            now.day,
            now.hour,
            now.minute,
            now.second);

    /* Start Alarm */
    int repeat_mode = 0;
    setTime.hour   = 17;
    setTime.minute = 45;
    setTime.second = 20;

    printf ("Alarm Time set %d/%d/%d %d:%d:%d\n",
            setTime.year,
            setTime.month,
            setTime.day,
            setTime.hour,
            setTime.minute,
            setTime.second);

    if (RTC_SetAlarm(RTC_APB, &setTime) != uStatus_Success) {
        printf("Set alarm failed!\n");
        return -1;
    }

    RTC_StartAlarm(RTC_APB, repeat_mode);
    RTC_GetAlarm(RTC_APB, &now);

    printf ("Alarm Time now %d/%d/%d %d:%d:%d\n",
            now.year,
            now.month,
            now.day,
            now.hour,
            now.minute,
            now.second);

    while(1);
    return 0;
}
