#include <string.h>
#include <stdlib.h>
#include "gap_common.h"
#include "mbed_wait_api.h"
#include <time.h>

int flag = 0;

void alarm_irq_handler() {
    printf("Alarm Ding .....\n");
    flag = 1;
}

int main()
{
    printf("Fabric controller code execution for mbed_os RTC driver test\n");

    rtc_config_t config;
    RTC_GetDefaultConfig(&config);

    RTC_Init(RTC_APB, &config);

    rtc_datetime_t setTime, now;

    /* Year between 2001 - 2099 */
    setTime.year   = 2001;
    setTime.month  = 2;
    setTime.day    = 28;
    setTime.hour   = 23;
    setTime.minute = 59;
    setTime.second = 59;

    printf ("Calendar Time now %d/%d/%d %d:%d:%d\n",
            setTime.year,
            setTime.month,
            setTime.day,
            setTime.hour,
            setTime.minute,
            setTime.second);

    /* Set calendar */
    if (RTC_SetCalendar(RTC_APB, &setTime) != uStatus_Success) {
        printf("Set calendar failed!\n");
        return -1;
    }

    /* Start calendar */
    RTC_StartCalendar(RTC_APB);

    for(int i = 0; i < 10; i++) {
        /* Wait 1 second */
        wait(1);

        /* Read calendar */
        RTC_GetCalendar(RTC_APB, &now);

        printf ("Calendar Time now %d/%d/%d %d:%d:%d\n",
                now.year,
                now.month,
                now.day,
                now.hour,
                now.minute,
                now.second);
    }

    int repeat_mode = 0;
    setTime.year   = 2001;
    setTime.month  = 3;
    setTime.day    = 1;
    setTime.hour   = 0;
    setTime.minute = 0;
    setTime.second = 15;

    /* Set Alarm 2001/03/01/00:00:15 */
    if (RTC_SetAlarm(RTC_APB, &setTime, repeat_mode) != uStatus_Success) {
        printf("Set alarm failed!\n");
        return -1;
    }

    /* Start Alarm */
    RTC_StartAlarm(RTC_APB);

    /* Binding RTC IRQ */
    RTC_CreateHandler((rtc_callback_t)alarm_irq_handler, NULL);

    /* Read RTC Alarm */
    RTC_GetAlarm(RTC_APB, &now);
    printf ("Alarm    Time set %d/%d/%d %d:%d:%d\n",
            now.year,
            now.month,
            now.day,
            now.hour,
            now.minute,
            now.second);

    for(int i = 0; i < 10; i++) {
        /* Wait 1 second */
        wait(1);

        /* Read calendar */
        RTC_GetCalendar(RTC_APB, &now);

        printf ("Calendar Time now %d/%d/%d %d:%d:%d\n",
                now.year,
                now.month,
                now.day,
                now.hour,
                now.minute,
                now.second);
    }

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
