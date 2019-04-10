#include <stdio.h>
#include "rt/rt_api.h"

#define GPIO_DELAY 33000000
//#define GPIO_DELAY 2000000

typedef union {
  struct {
    int value:8;
    int id:16;
    int cmd:8;
    int delay:32;
  };
  uint64_t raw;
} cmd_gpio_set_t;

RT_L2_DATA cmd_gpio_set_t cmd_buffer;


static RT_L2_RET_DATA int my_var = 0;

int main()
{
  if (rt_pm_wakeup_state() == RT_PM_WAKEUP_COLD)
  {
    printf("Entering test from cold boot my_var=%d\n", my_var);
    rt_spim_conf_t conf;

    rt_spim_conf_init(&conf);
    conf.wordsize = RT_SPIM_WORDSIZE_32;
    conf.big_endian = 1;
    conf.id = SPIM_ITF;
    conf.cs = SPIM_CS;

    rt_spim_t *spim = rt_spim_open(NULL, &conf, NULL);
    if (spim == NULL) return -1;

    hal_apb_soc_pad_set_function(4, 1);

    rt_gpio_init(0, 0);
    rt_gpio_set_dir(0, 1, RT_GPIO_IS_IN);

    rt_pm_wakeup_gpio_conf(1, 0, RT_PM_WAKEUP_GPIO_RISING_EDGE);
    rt_pm_wakeup_clear_all();

    cmd_buffer.cmd = 4;
    cmd_buffer.id = 0;

    cmd_buffer.value = 0;
    cmd_buffer.delay = 0;
    rt_spim_send(spim, &cmd_buffer, sizeof(cmd_buffer)*8, RT_SPIM_CS_AUTO, NULL);

    cmd_buffer.value = 1;
    cmd_buffer.delay = GPIO_DELAY;
    rt_spim_send(spim, &cmd_buffer, sizeof(cmd_buffer)*8, RT_SPIM_CS_AUTO, NULL);

    my_var++;

    rt_pm_state_switch(RT_PM_STATE_SLEEP, RT_PM_STATE_FAST);
  }
  else
  {
    printf("Wakeup from deep sleep my_var=%d\n", my_var);

    my_var++;

    rt_spim_conf_t conf;

    rt_spim_conf_init(&conf);
    conf.wordsize = RT_SPIM_WORDSIZE_32;
    conf.big_endian = 1;
    conf.id = SPIM_ITF;
    conf.cs = SPIM_CS;

    rt_spim_t *spim = rt_spim_open(NULL, &conf, NULL);
    if (spim == NULL) return -1;

    hal_apb_soc_pad_set_function(4, 1);


    rt_gpio_init(0, 0);
    rt_gpio_set_dir(0, 1, RT_GPIO_IS_IN);


    cmd_buffer.cmd = 4;
    cmd_buffer.id = 0;

    cmd_buffer.value = 0;
    cmd_buffer.delay = 0;
    rt_spim_send(spim, &cmd_buffer, sizeof(cmd_buffer)*8, RT_SPIM_CS_AUTO, NULL);

    cmd_buffer.value = 1;
    cmd_buffer.delay = GPIO_DELAY;
    rt_spim_send(spim, &cmd_buffer, sizeof(cmd_buffer)*8, RT_SPIM_CS_AUTO, NULL);


    rt_pm_state_switch(RT_PM_STATE_SLEEP, RT_PM_STATE_FAST);

    if (my_var == 1)
    {
      printf("TEST SUCCESS");
      return 0;
    }
  }

  printf("TEST FAILURE");
  return -1;
}
