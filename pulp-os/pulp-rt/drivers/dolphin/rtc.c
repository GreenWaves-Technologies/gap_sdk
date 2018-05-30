
/*
 * Copyright (C) 2018 GreenWaves Technologies
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
 *Author: Yao ZHANG, GreenWaves Technologies. yao.zhang@greenwaves-technologies.com
 */

#include "rt/rt_api.h"

RT_FC_TINY_DATA unsigned int rtcEventsStatus = 0;
RT_FC_TINY_DATA rt_rtc_t dev_rtc;
RT_FC_TINY_DATA rt_event_t *__rtc_handler;

static void  rt_rtc_alarm_stop();
static void  rt_rtc_cntDwn_stop();

static void  rt_apb_wait_int_apb(){
  volatile unsigned int * Evt = &rtcEventsStatus;

  // Wait till INT_APB = 1, will take approx 10 to 12 32KHz periods
  while ((*Evt&0x1) ==  0) eu_evt_maskWaitAndClr(1<<PLP_RT_NOTIF_EVENT);
  rtcEventsStatus = __BITINSERT(rtcEventsStatus, 0, 1, 0);
}

static void  rt_apb_int_apb_clear(unsigned char write){
  if (write){
    rt_apb_rtc_reg_write(APB_RTC_IRQ_FLAG_REG_ADDR, APB_W_Flag);                   // Write 0x02 in APB_IFR
  }else{
    rt_apb_rtc_reg_write(APB_RTC_IRQ_FLAG_REG_ADDR, APB_R_Flag);                   // Write 0x01 in APB_IFR
  }
}

// Execute a read request through APB interface
static unsigned int rt_rtc_reg_read(unsigned char iAddr){
  unsigned int read_res, i;
  Apb_rtcT apb_rtc;
  apb_rtc.rtcCtrl.apb_addr = iAddr;                           // Write iAddr to APB_CR
  apb_rtc.rtcCtrl.apb_load = APB_RTC_READ;                    // Write read cmd to APB_CR
  rt_apb_rtc_reg_write(APB_RTC_CTRL_REG_ADDR,(unsigned int) apb_rtc.Raw);    // Write APB_CR
  rt_apb_wait_int_apb();                     // Wait for INT_APB
  read_res = rt_apb_rtc_reg_read(APB_RTC_DATA_REG_ADDR);     // Read data from APB_DR
  rt_apb_int_apb_clear(APB_RTC_READ);
  return read_res;
}

// Execute a write request through APB interface
static void  rt_rtc_reg_config(unsigned char iAddr, unsigned int conf){
  Apb_rtcT apb_rtc;
  rt_apb_rtc_reg_write(APB_RTC_DATA_REG_ADDR, conf);         // Write conf value to APB_DR
  apb_rtc.rtcCtrl.apb_addr = iAddr;                           // Write iAddr to APB_CR
  apb_rtc.rtcCtrl.apb_load = APB_RTC_WRITE;                   // Write write cmd to APB_CR
  rt_apb_rtc_reg_write(APB_RTC_CTRL_REG_ADDR, (unsigned int) apb_rtc.Raw);   // Write APB_CR
  rt_apb_wait_int_apb();                    // Wait for end of Tx, and write 0x01 in APB_IFR
  rt_apb_int_apb_clear(APB_RTC_WRITE);
}

void __rt_int_rtc_handler(void *arg){
  rt_rtc_t *rtc = (rt_rtc_t *) arg;
  rt_rtc_reg_read(RTC_IRQ_Flag_Addr);
  switch (rtc->conf.mode){
    case MODE_CALIBR:
      rt_rtc_reg_config(RTC_IRQ_Flag_Addr, RTC_Irq_Calibration_Flag);
      break;
    case MODE_CNTDOWN:
      rt_rtc_reg_config(RTC_IRQ_Flag_Addr, RTC_Irq_Timer1_Flag);
      break;
    case MODE_ALARM:
      rt_rtc_reg_config(RTC_IRQ_Flag_Addr, RTC_Irq_Alarm1_Flag);
      break;
  }
  __rt_event_enqueue(rtc->event);
}

static void  rt_rtc_enable(){
  RtcT rtc;
  rtc.Raw = rt_rtc_reg_read(RTC_Ctrl_Addr);
  rtc.conf.rtc_sb = RTC_Active_Mode;
  rt_rtc_reg_config(RTC_Ctrl_Addr, (unsigned int) rtc.Raw);
}

static void  rt_rtc_disable(){
  RtcT rtc;
  rtc.Raw = rt_rtc_reg_read(RTC_Ctrl_Addr);
  rtc.conf.rtc_sb = RTC_Standby_Mode;
  rt_rtc_reg_config(RTC_Ctrl_Addr, (unsigned int) rtc.Raw);
}
static void  rt_rtc_set_clk(unsigned int value){
  RtcT rtc;
  /* Set CkInDiv1 to 0x80 => Clock = 32768/128 Hz, e.g 256 Hz, Period = 3.9 ms approx */
  rtc.ckinDiv.divVal = value;                                         // 0x8000 as init.Set the divider at 2, so 32KHz right shift 1
  rt_rtc_reg_config(RTC_CKIN_DIV1_Addr, (unsigned int) rtc.Raw);     // Enable the countdown timer 1 interruption
  rt_rtc_disable();
  rt_rtc_enable();
}

static unsigned int rt_rtc_status_get(){
  return rt_rtc_reg_read(RTC_Status_Addr);
}

static void  rt_rtc_reset(){
  RtcT rtc;
  rtc.Raw = rt_rtc_reg_read(RTC_Ctrl_Addr);
  rtc.conf.soft_rst = RTC_Soft_Reset;
  rt_rtc_reg_config(RTC_Ctrl_Addr, (unsigned int) rtc.Raw);
}

static void  rt_rtc_calibration(){
  RtcT rtc;
  rtc.Raw = rt_rtc_reg_read(RTC_Ctrl_Addr);
  rtc.conf.cal_En = RTC_Calibration_Enable;
  rt_rtc_reg_config(RTC_Ctrl_Addr, (unsigned int) rtc.Raw);
}

static void rt_rtc_cntDwn_Value(unsigned int *value){
  *value = rt_rtc_reg_read(RTC_CntDown1_Timer_Addr);
}

static void  rt_rtc_calendar_start(){
  rt_rtc_cntDwn_stop();
  rt_rtc_alarm_stop();
  rt_rtc_reg_config(RTC_Calendar_Ctrl_Addr, ~RTC_Calendar_Inactive);
}

static void  rt_rtc_calendar_stop(){
  rt_rtc_reg_config(RTC_Calendar_Ctrl_Addr, RTC_Calendar_Inactive);
}

static void  rt_rtc_calendar(rt_rtc_calendar_t *calendar){
  rt_rtc_reg_config(RTC_Calendar_TIME_Addr, calendar->time);
  rt_rtc_reg_config(RTC_Calendar_DATE_Addr, calendar->date);
}

static void  rt_rtc_getTime(rt_rtc_calendar_t *calendar){
  calendar->time = rt_rtc_reg_read(RTC_Calendar_TIME_Addr);
  calendar->date = rt_rtc_reg_read(RTC_Calendar_DATE_Addr);
}

static void  rt_rtc_cntDwn_start(rt_rtc_cntDwn_t *cntDwn){
  RtcT rtc;
  rt_rtc_calendar_stop();
  rt_rtc_alarm_stop();
  rtc.Raw = rt_rtc_reg_read(RTC_CntDown_Ctrl_Addr);
  if (cntDwn->repeat_en) rtc.cntDwnCtrl.cntDwn1_mode = RTC_CountDown1_Rpt_Mode; // Set to repeat mode, if repeat = 1
  rtc.cntDwnCtrl.cntDwn1_En = RTC_CountDown1_Active;                 // Start the CountDown timer
  rt_rtc_reg_config(RTC_CntDown_Ctrl_Addr, (unsigned int) rtc.Raw);
}

static void  rt_rtc_cntDwn_stop(){
  RtcT rtc;
  rtc.Raw = rt_rtc_reg_read(RTC_CntDown_Ctrl_Addr);
  rtc.cntDwnCtrl.cntDwn1_En = RTC_CountDown1_Inactive;                 // Start the CountDown timer
  rt_rtc_reg_config(RTC_CntDown_Ctrl_Addr, (unsigned int) rtc.Raw);
}

static void  rt_rtc_countDown(rt_rtc_cntDwn_t *cntDwn){
  RtcT rtc;
  rt_rtc_reg_config(RTC_CntDown1_Init_Addr, cntDwn->value);                 // Configure the init value

  rtc.Raw = rt_rtc_reg_read(RTC_IRQ_Mask_Addr);
  rtc.irqMask.timer1_masked = RTC_Timer1_Irq_Enable;
  rt_rtc_reg_config(RTC_IRQ_Mask_Addr, (unsigned int) rtc.Raw);     // Enable the countdown timer 1 interruption
}

static void  rt_rtc_alarm_start(rt_rtc_alarm_t * alarm){
  RtcT rtc;
  unsigned int repeat = 0;
  unsigned int mode = alarm->repeat_mode;
  if (mode) repeat = 1;
  rtc.Raw = rt_rtc_reg_read(RTC_Alarm_Ctrl_Addr);
  rtc.alarmCtrl.alarm1_mode = repeat;
  rtc.alarmCtrl.alarm1_conf = mode;
  rtc.alarmCtrl.alarm1_En = RTC_Alarm_ON;
  rt_rtc_reg_config(RTC_Alarm_Ctrl_Addr, rtc.Raw);
}

static void  rt_rtc_alarm_stop(){
  RtcT rtc;
  rtc.alarmCtrl.alarm1_En = RTC_Alarm_OFF;
  rt_rtc_reg_config(RTC_Alarm_Ctrl_Addr, rtc.Raw);
}

static void  rt_rtc_set_alarm(rt_rtc_alarm_t* alarm){
  RtcT rtc;
  rtc.Raw = rt_rtc_reg_read(RTC_IRQ_Mask_Addr);
  rtc.irqMask.alarm1_masked = RTC_Alarm1_Irq_Enable;
  rt_rtc_reg_config(RTC_IRQ_Mask_Addr, (unsigned int) rtc.Raw);     // Enable the Alarm interruption

  if (alarm->time_date.date) rt_rtc_reg_config(RTC_Alarm1_DATE_Addr, alarm->time_date.date);
  rt_rtc_reg_config(RTC_Alarm1_TIME_Addr, alarm->time_date.time);
}

static void rt_rtc_init(rt_rtc_t *rtc, rt_rtc_conf_t *rtc_conf)
{
  unsigned int Val = soc_eu_eventMask_get(SOC_FC_MASK_MSB);
  Val = Val & ~((1<<(RTC_RTC_INT_EVENT-32)) | (1<<(RTC_RTC_APB_EVENT-32)));
  soc_eu_eventMask_set(SOC_FC_MASK_MSB, Val);
  rt_rtc_reset();
  rtc->conf.mode = MODE_CALENDAR;
  memcpy(&rtc->conf, rtc_conf, sizeof(rt_rtc_conf_t));
  // config the RTC in calendar mode.
  rt_rtc_set_clk(rtc->conf.clkDivider);
  rt_rtc_calendar(&rtc->conf.calendar);
  rtc->conf.mode = MODE_CALENDAR;
}

rt_rtc_t* rt_rtc_open(rt_rtc_conf_t *rtc_conf, rt_event_t *event)
{
  if(dev_rtc.open_count) goto error;
  rt_rtc_init(&dev_rtc, rtc_conf);
  if (event) __rt_event_enqueue(event);
  dev_rtc.open_count++;
  return &dev_rtc;

error:
  rt_warning("[RTC] Failed to open rtc\n");
  return NULL;
}

void rt_rtc_close(rt_rtc_t *rtc, rt_event_t *event)
{
  rt_rtc_disable();
  unsigned int Val = soc_eu_eventMask_get(SOC_FC_MASK_MSB);
  Val = Val | ((1<<(RTC_RTC_INT_EVENT-32)) | (1<<(RTC_RTC_APB_EVENT-32)));
  soc_eu_eventMask_set(SOC_FC_MASK_MSB, Val);
  if (event) __rt_event_enqueue(event);
}

void rt_rtc_control( rt_rtc_t *rtc, rt_rtc_cmd_e rtc_cmd, void *value, rt_event_t *event )
{
  if (event){
      rt_event_sched_t *sched = event->sched;
      rtc->event = event;
      __rtc_handler = rt_event_get(sched,  __rt_int_rtc_handler, rtc);
  }
  switch (rtc_cmd){
    case RTC_START:
      rt_rtc_enable();
      break;
    case RTC_STOP:
      rt_rtc_disable();
      break;
    case RTC_RESET:
      rt_rtc_reset();
      rtc->conf.mode = MODE_CALENDAR;
      break;
    case RTC_CLK_DIV_SET:
      rtc->conf.clkDivider = *((unsigned int *) value);
      rt_rtc_set_clk(rtc->conf.clkDivider);
      break;
    case RTC_ALARM_SET:
      memcpy(&rtc->conf.alarm, value, sizeof(rt_rtc_alarm_t));
      rt_rtc_set_alarm(&rtc->conf.alarm);
      break;
    case RTC_ALARM_START:
      if (rtc->conf.mode != MODE_CALENDAR)
          rt_rtc_control(rtc, RTC_CALENDAR_START, NULL, NULL);
      rt_rtc_alarm_start(&rtc->conf.alarm);
      rtc->conf.mode = MODE_ALARM;
      break;
    case RTC_ALARM_STOP:
      rt_rtc_alarm_stop();
      break;
    case RTC_CALENDAR_SET:
      memcpy(&rtc->conf.calendar, value, sizeof(rt_rtc_calendar_t));
      rt_rtc_calendar(&rtc->conf.calendar);
      break;
    case RTC_CALENDAR_START:
      rt_rtc_calendar_start();
      rtc->conf.mode = MODE_CALENDAR;
      break;
    case RTC_CALENDAR_STOP:
      rt_rtc_calendar_stop();
      break;
    case RTC_CNTDOWN_SET:
      memcpy(&rtc->conf.cntDwn, value, sizeof(rt_rtc_cntDwn_t));
      rt_rtc_countDown(&rtc->conf.cntDwn);
      break;
    case RTC_CNTDOWN_START:
      rt_rtc_cntDwn_start(&rtc->conf.cntDwn);
      rtc->conf.mode = MODE_CNTDOWN;
      break;
    case RTC_CNTDOWN_STOP:
      rt_rtc_cntDwn_stop();
      break;
    case RTC_CALIBRATION:
      rt_rtc_calibration();
      break;
    case RTC_GET_TIME:
      rt_rtc_getTime(value);
      break;
    case RTC_CNTDOWN_VALUE:
      rt_rtc_cntDwn_Value(value);
  }
}

