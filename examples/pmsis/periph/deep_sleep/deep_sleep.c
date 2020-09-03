/* PMSIS includes */
#include "pmsis.h"
#include "bsp/bsp.h"

#define WAKEUP_RTC 1

static struct pi_device wakeup_dev;

#if (WAKEUP_RTC == 1)
int32_t setup_rtc(void)
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
    rtc_conf.alarm.tm_sec  = 15;
    pi_open_from_conf(&wakeup_dev, &rtc_conf);
    errors = pi_rtc_open(&wakeup_dev);
    if (errors)
    {
        printf("Error rtc open %d\n", errors);
        return -11;
    }

    struct tm time = {0}, time_now = {0};

    /* Initial date & time. */
    pi_rtc_datetime_get(&wakeup_dev, &time_now);
    printf ("Initial date & time %d/%d/%d %d:%d:%d\n",
            time_now.tm_mday, time_now.tm_mon, time_now.tm_year,
            time_now.tm_hour, time_now.tm_min, time_now.tm_sec);

    /* Alarm date & time. */
    pi_rtc_alarm_get(&wakeup_dev, &time_now);
    printf ("Alarm set %d/%d/%d %d:%d:%d\n",
            time_now.tm_mday, time_now.tm_mon, time_now.tm_year,
            time_now.tm_hour, time_now.tm_min, time_now.tm_sec);

    /* Start calendar. */
    pi_rtc_ioctl(&wakeup_dev, PI_RTC_CALENDAR_START, NULL);

    /* Start alarm. */
    pi_rtc_ioctl(&wakeup_dev, PI_RTC_ALARM_START, (void *) PI_RTC_ALARM_RPT_NONE);

    return errors;
}
#else
int32_t setup_gpio(struct pi_pmu_sleep_conf_s *sleep_conf)
{
    int32_t errors = 0;
    struct pi_gpio_conf gpio_conf = {0};
    pi_gpio_conf_init(&gpio_conf);
    pi_open_from_conf(&wakeup_dev, &gpio_conf);
    errors = pi_gpio_open(&wakeup_dev);
    if (errors)
    {
        printf("Error opening GPIO %d\n", errors);
        pmsis_exit(errors);
    }

    pi_gpio_e gpio_in = PI_GPIO_A0_PAD_12_A3;
    pi_gpio_flags_e cfg_flags = PI_GPIO_INPUT|PI_GPIO_PULL_DISABLE|PI_GPIO_DRIVE_STRENGTH_LOW;

    sleep_conf->gpio_pin = (gpio_in & PI_GPIO_NUM_MASK);
    sleep_conf->gpio_notif = PI_PMU_GPIO_RISE;
    /* Configure gpio input. */
    printf("Configuring GPIO_%d\n", sleep_conf->gpio_pin);
    pi_gpio_pin_configure(&wakeup_dev, gpio_in, cfg_flags);

    return errors;
}
#endif  /* (WAKEUP_RTC == 1) */

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
        sleep_conf.sleep_mode = PI_PMU_MODE_RET_DEEP_SLEEP;

        #if (WAKEUP_RTC == 1)
        printf("Setup RTC alarm\n");
        sleep_conf.wakeup = PI_PMU_WAKEUP_RTC;

        errors = setup_rtc();
        if (errors)
        {
            printf("Error setting RTC alarm !\n");
            pmsis_exit(errors);
        }
        #else
        printf("Setup GPIO\n");
        sleep_conf.wakeup = PI_PMU_WAKEUP_GPIO;

        errors = setup_gpio(&sleep_conf);
        if (errors)
        {
            printf("Error setting GPIO !\n");
            pmsis_exit(errors);
        }
        #endif  /* (WAKEUP_RTC == 1) */

        pi_pmu_sleep_mode_set(PI_PMU_DOMAIN_FC, &sleep_conf);
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
