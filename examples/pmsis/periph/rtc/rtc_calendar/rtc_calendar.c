/* PMSIS includes. */
#include "pmsis.h"

/* Variables used. */
#define TIME_LOOP_SECONDS     ( 10 )

static struct pi_device rtc;

void test_rtc_calendar(void)
{
    printf("Entering main controller\n");

    int32_t errors = 0;

    /* Init & open RTC. */
    struct pi_rtc_conf rtc_conf = {0};
    pi_rtc_conf_init(&rtc_conf);
    rtc_conf.mode = PI_RTC_MODE_CALENDAR;
    /* Date & time. */
    rtc_conf.time.tm_year = 2001;
    rtc_conf.time.tm_mon  = 2;
    rtc_conf.time.tm_mday = 28;
    rtc_conf.time.tm_hour = 23;
    rtc_conf.time.tm_min  = 59;
    rtc_conf.time.tm_sec  = 58;
    pi_open_from_conf(&rtc, &rtc_conf);
    errors = pi_rtc_open(&rtc);
    if (errors)
    {
        printf("Error rtc open %d\n", errors);
        pmsis_exit(-1);
    }

    struct tm time = {0}, time_now = {0};
    memcpy((void *) &time, (void *) &(rtc_conf.time), sizeof(struct tm));

    uint8_t days_month[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    /* Leap year check. */
    if ((((time.tm_year & 0x3) == 0) && ((time.tm_year % 100) != 0)) ||
        ((time.tm_year % 400) == 0))
    {
        days_month[2] = 29;
    }


    int32_t d_sec = (time.tm_sec + (int32_t) TIME_LOOP_SECONDS) % 60;
    int32_t d_min = (time.tm_min +
                     (time.tm_sec + (int32_t) TIME_LOOP_SECONDS) / 60) % 60;
    int32_t d_hou = (time.tm_hour +
                     (time.tm_min +
                      (time.tm_sec + (int32_t) TIME_LOOP_SECONDS) / 60) / 60) % 24;
    int32_t d_day = (time.tm_mday +
                     (time.tm_hour +
                      (time.tm_min +
                       (time.tm_sec + (int32_t) TIME_LOOP_SECONDS) / 60) / 60) / 24) % days_month[time.tm_mon];
    int32_t d_mon = (time.tm_mon +
                     (time.tm_mday +
                     (time.tm_hour +
                      (time.tm_min +
                       (time.tm_sec + (int32_t) TIME_LOOP_SECONDS) / 60) / 60) / 24) / days_month[time.tm_mon]) % 12;
    int32_t d_year = (time.tm_year +
                      (time.tm_mon +
                       (time.tm_mday +
                        (time.tm_hour +
                         (time.tm_min +
                          (time.tm_sec + (int32_t) TIME_LOOP_SECONDS) / 60) / 60) / 24) / days_month[time.tm_mon]) / 12);
    //printf("Expected date & time: %d/%d/%d %d:%d:%d\n", d_day, d_mon, d_year, d_hou, d_min, d_sec);

    /* Initial date & time. */
    pi_rtc_datetime_get(&rtc, &time_now);
    printf ("Initial date & time %d/%d/%d %d:%d:%d\n",
            time_now.tm_mday, time_now.tm_mon, time_now.tm_year,
            time_now.tm_hour, time_now.tm_min, time_now.tm_sec);

    /* Start calendar. */
    pi_rtc_ioctl(&rtc, PI_RTC_CALENDAR_START, NULL);

    for (uint32_t i=0; i<10; i++)
    {
        /* Wait 1 second. */
        pi_time_wait_us(1000 * 1000);

        pi_rtc_datetime_get(&rtc, &time_now);
        printf ("Calendar now %d/%d/%d %d:%d:%d\n",
                time_now.tm_mday, time_now.tm_mon, time_now.tm_year,
                time_now.tm_hour, time_now.tm_min, time_now.tm_sec);
    }

    pi_rtc_close(&rtc);

    errors += (time_now.tm_sec != d_sec) ? 1 : 0;
    errors += (time_now.tm_min != d_min) ? 1 : 0;
    errors += (time_now.tm_hour != d_hou) ? 1 : 0;
    errors += (time_now.tm_mday != d_day) ? 1 : 0;
    errors += (time_now.tm_mon != d_mon) ? 1 : 0;
    errors += (time_now.tm_year != d_year) ? 1 : 0;
    printf("\nTest %s with %ld error(s) !\n", (errors) ? "failed" : "success", errors);

    pmsis_exit(errors);
}

/* Program Entry. */
int main(void)
{
    printf("\n\n\t *** Test RTC Calendar ***\n\n");
    return pmsis_kickoff((void *) test_rtc_calendar);
}
