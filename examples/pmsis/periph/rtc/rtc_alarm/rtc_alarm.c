/* PMSIS includes. */
#include "pmsis.h"

/* Variables used. */
#define ALARM_REPEAT_LOOP     ( 3 )

static struct pi_device rtc;
static volatile uint32_t g_flag = 0;

void alarm_irq_handler(void *arg)
{
    printf("RTC Alarm ..... %x\n", *((uint32_t *) arg));
    g_flag++;
}

void test_rtc_alarm(void)
{
    printf("Entering main controller\n");

    int32_t errors = 0;

    /* Init & open RTC. */
    struct pi_rtc_conf rtc_conf = {0};
    pi_rtc_conf_init(&rtc_conf);
    rtc_conf.mode = PI_RTC_MODE_CALENDAR | PI_RTC_MODE_ALARM;
    /* Date & time. */
    rtc_conf.time.tm_year = 2001;
    rtc_conf.time.tm_mon  = 2;
    rtc_conf.time.tm_mday = 28;
    rtc_conf.time.tm_hour = 23;
    rtc_conf.time.tm_min  = 59;
    rtc_conf.time.tm_sec  = 58;
    /* Alarm date & time. */
    rtc_conf.alarm.tm_year = 2001;
    rtc_conf.alarm.tm_mon  = 3;
    rtc_conf.alarm.tm_mday = 1;
    rtc_conf.alarm.tm_hour = 0;
    rtc_conf.alarm.tm_min  = 0;
    rtc_conf.alarm.tm_sec  = 10;
    pi_open_from_conf(&rtc, &rtc_conf);
    errors = pi_rtc_open(&rtc);
    if (errors)
    {
        printf("Error rtc open %d\n", errors);
        pmsis_exit(-1);
    }

    struct tm time = {0}, time_now = {0};

    /* Initial date & time. */
    pi_rtc_datetime_get(&rtc, &time_now);
    printf ("Initial date & time %d/%d/%d %d:%d:%d\n",
            time_now.tm_mday, time_now.tm_mon, time_now.tm_year,
            time_now.tm_hour, time_now.tm_min, time_now.tm_sec);

    /* Alarm date & time. */
    pi_rtc_alarm_get(&rtc, &time_now);
    printf ("Alarm set %d/%d/%d %d:%d:%d\n",
            time_now.tm_mday, time_now.tm_mon, time_now.tm_year,
            time_now.tm_hour, time_now.tm_min, time_now.tm_sec);

    /* Start calendar. */
    pi_rtc_ioctl(&rtc, PI_RTC_CALENDAR_START, NULL);

    for (uint32_t i=0; i<5; i++)
    {
        /* Wait 1 second. */
        pi_time_wait_us(1000 * 1000);

        pi_rtc_datetime_get(&rtc, &time_now);
        printf ("Calendar now %d/%d/%d %d:%d:%d\n",
                time_now.tm_mday, time_now.tm_mon, time_now.tm_year,
                time_now.tm_hour, time_now.tm_min, time_now.tm_sec);
    }

    /* Binding RTC IRQ. */
    uint32_t arg = 0xabbadead;
    struct pi_task task = {0};
    pi_task_callback(&task, alarm_irq_handler, &arg);
    pi_rtc_ioctl(&rtc, PI_RTC_ATTACH_CB_ALARM, &task);

    pi_rtc_alarm_repeat_e repeat = PI_RTC_ALARM_RPT_HOUR;
    memcpy((void *) &time, (void *) &(rtc_conf.alarm), sizeof(struct tm));

    /* Start alarm. */
    pi_rtc_ioctl(&rtc, PI_RTC_ALARM_START, (void *) repeat);

    uint8_t alarm = 0;
    do
    {
        /* Wait 1 second. */
        pi_time_wait_us(1000 * 1000);

        /* Get current date & time. */
        pi_rtc_datetime_get(&rtc, &time_now);
        printf ("Calendar now %d/%d/%d %d:%d:%d\n",
                time_now.tm_mday, time_now.tm_mon, time_now.tm_year,
                time_now.tm_hour, time_now.tm_min, time_now.tm_sec);
        if (g_flag)
        {
            g_flag = 0;

            if (repeat == PI_RTC_ALARM_RPT_NONE)
            {
                alarm = (uint8_t) ALARM_REPEAT_LOOP;
            }
            else
            {
                alarm++;
                /* ReSet current date & time. */
                time.tm_year = (repeat == PI_RTC_ALARM_RPT_YEAR) ? time.tm_year + 1 : time.tm_year;
                time.tm_mon  = (repeat == PI_RTC_ALARM_RPT_MON ) ? time.tm_mon  + 1 : time.tm_mon;
                time.tm_mday = (repeat == PI_RTC_ALARM_RPT_DAY ) ? time.tm_mday + 1 : time.tm_mday;
                time.tm_hour = (repeat == PI_RTC_ALARM_RPT_HOUR) ? time.tm_hour + 1 : time.tm_hour;
                time.tm_min  = (repeat == PI_RTC_ALARM_RPT_MIN ) ? time.tm_min  + 1 : time.tm_min;
                time.tm_sec  = 5;
                errors += pi_rtc_datetime_set(&rtc, &time);
                if (errors)
                {
                    printf("Error setting time %d\n", errors);
                    pmsis_exit(-4);
                }
            }
        }
    } while (alarm < (uint8_t) ALARM_REPEAT_LOOP);

    pi_rtc_close(&rtc);

    printf("\nTest %s with %ld error(s) !\n", (errors) ? "failed" : "success", errors);

    pmsis_exit(errors);
}

/* Program Entry. */
int main(void)
{
    printf("\n\n\t *** Test RTC Alarm ***\n\n");
    return pmsis_kickoff((void *) test_rtc_alarm);
}
