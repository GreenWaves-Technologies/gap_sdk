/* PMSIS includes */
#include "pmsis.h"
#include "bsp/bsp.h"

static struct pi_device rtc;

int32_t rtc_setup(void)
{
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
    rtc_conf.alarm.tm_sec  = 5;
    pi_open_from_conf(&rtc, &rtc_conf);
    errors = pi_rtc_open(&rtc);
    if (errors)
    {
        printf("Error rtc open %d\n", errors);
        return -11;
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

    /* Start alarm. */
    pi_rtc_ioctl(&rtc, PI_RTC_ALARM_START, (void *) PI_RTC_ALARM_RPT_NONE);
    return 0;
}

void helloworld(void)
{
    int32_t errors = 0;
    uint32_t core_id = pi_core_id(), cluster_id = pi_cluster_id();
    printf("[%d %d] Hello World!\n", cluster_id, core_id);

    uint32_t bootstate = pi_pmu_boot_state_get(PI_PMU_DOMAIN_FC);
    printf("Bootstate = %d\n", bootstate);
    if (bootstate == PI_PMU_BOOT_COLD)
    {
        printf("Cold boot\n");

        struct pi_pmu_sleep_conf_s sleep_conf = {0};
        sleep_conf.pmu_cluster_state = PI_PMU_DOMAIN_STATE_OFF;
        sleep_conf.power_state = PI_PMU_WAKEUP_STATE_POWER_HIGH;
        sleep_conf.wakeup = PI_PMU_WAKEUP_RTC;
        sleep_conf.sleep_mode = PI_PMU_MODE_RET_DEEP_SLEEP;
        pi_pmu_sleep_mode_set(PI_PMU_DOMAIN_FC, &sleep_conf);

        printf("Setup RTC alarm\n");
        errors = rtc_setup();
        if (errors)
        {
            printf("Error setting RTC alarm !\n");
            pmsis_exit(errors);
        }
        pi_pmu_sleep_mode_enable(PI_PMU_DOMAIN_FC);
    }
    else if (bootstate == PI_PMU_BOOT_DEEP_SLEEP)
    {
        printf("Deep sleep boot\n");
        pi_gpio_pin_configure(NULL, GPIO_USER_LED, PI_GPIO_OUTPUT);
        while (1)
        {
            pi_gpio_pin_write(NULL, GPIO_USER_LED, 0);
            pi_time_wait_us(100000);
            pi_gpio_pin_write(NULL, GPIO_USER_LED, 1);
            pi_time_wait_us(100000);
        }
    }
    else
    {
        printf("Retentive deep sleep boot\n");
        pi_gpio_pin_configure(NULL, GPIO_USER_LED, PI_GPIO_OUTPUT);
        while (1)
        {
            pi_gpio_pin_write(NULL, GPIO_USER_LED, 0);
            pi_time_wait_us(100000);
            pi_gpio_pin_write(NULL, GPIO_USER_LED, 1);
            pi_time_wait_us(100000);
        }
    }

    printf("Test success !\n");

    pmsis_exit(errors);
}

/* Program Entry. */
int main(void)
{
    return pmsis_kickoff((void *) helloworld);
}
