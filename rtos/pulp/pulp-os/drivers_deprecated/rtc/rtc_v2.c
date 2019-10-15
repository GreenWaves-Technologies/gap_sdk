
/*
 * Copyright (C) 2018 ETH Zurich, University of Bologna and
 * GreenWaves Technologies
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * Authors: Germain Haugou, ETH (germain.haugou@iis.ee.ethz.ch)
 */

#include "rt/rt_api.h"

extern void __rt_rtc_handler();

RT_FC_TINY_DATA rt_rtc_t dev_rtc;
RT_FC_TINY_DATA rt_event_t *__rtc_handler;
RT_FC_TINY_DATA unsigned int __rt_rtc_timer_target;




void rt_rtc_conf_init(rt_rtc_conf_t *conf)
{
}



rt_rtc_t* rt_rtc_open(rt_rtc_conf_t *conf, rt_event_t *event)
{
  int irq = rt_irq_disable();

  rt_rtc_conf_t def_conf;

  if (dev_rtc.open_count)
    goto error;

  if (conf == NULL)
  {
    conf = &def_conf;
    rt_rtc_conf_init(conf);
  }

  rt_irq_set_handler(ARCHI_FC_EVT_RTC, __rt_rtc_handler);
  rt_irq_mask_set(1<<ARCHI_FC_EVT_RTC);

  if (event) __rt_event_enqueue(event);
  dev_rtc.open_count++;

  rt_irq_restore(irq);

  return &dev_rtc;

error:
  rt_warning("[RTC] Failed to open rtc\n");
  rt_irq_restore(irq);

  return NULL;
}



void rt_rtc_close(rt_rtc_t *rtc, rt_event_t *event)
{
  int irq = rt_irq_disable();
  rt_irq_mask_set(1<<ARCHI_FC_EVT_RTC);
  if (event) __rt_event_enqueue(event);
  rt_irq_restore(irq);
}



void rt_rtc_control( rt_rtc_t *rtc, rt_rtc_cmd_e rtc_cmd, void *value, rt_event_t *event )
{
  int irq = rt_irq_disable();

  if (event)
    __rtc_handler = event;

  switch (rtc_cmd){
    case RTC_START:
      break;
    case RTC_STOP:
      break;
    case RTC_RESET:
      break;
    case RTC_CLK_DIV_SET:
      break;
    case RTC_ALARM_SET:
      break;
    case RTC_ALARM_START:
      break;
    case RTC_ALARM_STOP:
      break;
    case RTC_CALENDAR_SET:
      break;
    case RTC_CALENDAR_START:
      break;
    case RTC_CALENDAR_STOP:
      break;
    case RTC_CNTDOWN_SET:
    {
      __rt_rtc_timer_target = (*(uint32_t *)value);
      break;
    }
    case RTC_CNTDOWN_START:
    {
      rtc_timer_t timer = { .raw = 0 };
      timer.retrig = 1;
      timer.enable = 1;
      timer.target = __rt_rtc_timer_target;
      rtc_timer_set(rtc_base(), timer.raw);
      break;
    }
      break;
    case RTC_CNTDOWN_STOP:
    {
      rtc_timer_t timer = { .raw = 0 };
      timer.enable = 0;
      rtc_timer_set(rtc_base(), timer.raw);
      break;
    }
      break;
    case RTC_CALIBRATION:
      break;
    case RTC_GET_TIME:
      break;
    case RTC_CNTDOWN_VALUE:
      break;
  }
  rt_irq_restore(irq);
}


RT_FC_BOOT_CODE void __attribute__((constructor)) __rt_rtc_init()
{
  dev_rtc.open_count = 0;
}
