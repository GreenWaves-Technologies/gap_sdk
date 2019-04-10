#include <stdio.h>
#include "rt/rt_api.h"

#define SLEEP_TIME   5
static RT_L2_RET_DATA int my_var = 0;
unsigned int __rt_iodev=RT_IODEV_UART;

int main()
{
  if (rt_pm_wakeup_state() == RT_PM_WAKEUP_COLD)
  {
    printf("Entering test from cold boot my_var=%d\n", my_var);
    rt_rtc_conf_t conf;

    my_var = 1;

    conf.clkDivider = 0x8000;

    rt_rtc_t *rtc = rt_rtc_open(&conf, NULL);
    if (rtc == NULL) return -1;

    rt_rtc_cntDwn_t value = { SLEEP_TIME, 0 };
    rt_rtc_control(rtc, RTC_CNTDOWN_SET, (void *)&value, NULL);
    rt_rtc_control(rtc, RTC_CNTDOWN_START, NULL, NULL);
    printf("GAP8 will wake up in %ds\n", SLEEP_TIME);

    rt_pm_state_switch(RT_PM_STATE_DEEP_SLEEP, RT_PM_STATE_FAST);
  }
  else
  {
    printf("Wakeup from deep sleep my_var=%d\n", my_var);
    if (my_var == 0)
    {
      printf("TEST SUCCESS\n");
      return 0;
    }
  }

  printf("TEST FAILURE\n");
  return -1;
}
