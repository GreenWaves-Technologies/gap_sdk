/*
 * Copyright (C) 2020  GreenWaves Technologies, SAS
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 * 
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

/* 
 * Authors: Germain Haugou, GreenWaves Technologies (germain.haugou@greenwaves-technologies.com)
 */

#include <vp/vp.hpp>
#include <vp/itf/io.hpp>
#include <vp/itf/wire.hpp>
#include "vp/itf/clock.hpp"
#include <stdio.h>
#include <string.h>

#include "archi/vendors/dolphin/rtc.h"



class rtc : public vp::component
{

public:

  rtc(js::config *config);

  int build();
  void start();

  static vp::io_req_status_e req(void *__this, vp::io_req *req);

private:

  void reset(bool active);

  void soft_reset();
  void update_calendar();
  void check_interrupts();

  static void ref_clock_sync(void *__this, bool value);

  vp::io_req_status_e stat_req(int reg_offset, int size, bool is_write, uint8_t *data);
  vp::io_req_status_e ctrl_req(int reg_offset, int size, bool is_write, uint8_t *data);
  vp::io_req_status_e data_req(int reg_offset, int size, bool is_write, uint8_t *data);
  vp::io_req_status_e irq_ctrl_req(int reg_offset, int size, bool is_write, uint8_t *data);
  vp::io_req_status_e irq_mask_req(int reg_offset, int size, bool is_write, uint8_t *data);
  vp::io_req_status_e irq_flag_req(int reg_offset, int size, bool is_write, uint8_t *data);

  void handle_internal_access();

  void handle_status_access();
  void handle_ctrl_access();
  void handle_clk_access();
  void handle__access();
  void handle_irq_ctrl_access();
  void handle_irq_mask_access();
  void handle_irq_flag_access();
  void handle_calendar_ctrl_access();
  void handle_calendar_time_access();
  void handle_calendar_date_access();
  void handle_alarm_ctrl_access();
  void handle_alarm1_time_access();
  void handle_alarm1_date_access();
  void handle_cntdown_ctrl_access();
  void handle_cntdown1_init_access();
  void handle_cntdown1_timer_access();
  void handle_ckin_div1_access();
  void handle_ref_clk_conf_access();
  void handle_test_addr_access();


  vp::trace             trace;
  vp::io_slave          in;
  vp::wire_master<int>  event_itf;
  vp::wire_master<bool> irq_itf;
  vp::wire_master<bool> apb_irq_itf;
  vp::clock_slave       ref_clock_itf;

  Apb_rtc_ctrlT       apb_ctrl_reg;
  unsigned int        apb_data_reg;
  Apb_irq_ctrlT       apb_irq_ctrl_reg;
  Apb_irq_maskT       apb_irq_mask_reg;

  Rtc_ctrlT           ctrl_reg;
  Rtc_clk_ctrlT       clk_ctrl_reg;
  Rtc_irq_ctrlT       irq_ctrl_reg;
  Rtc_irq_maskT       irq_mask_reg;
  Rtc_irq_maskT       irq_flag_reg;
  Rtc_calend_ctrlT    calend_ctrl_reg;
  Rtc_calend_timeT    calendar_time;
  Rtc_calend_dateT    calendar_date;
  Rtc_alarm_ctrlT     alarm_ctrl_reg;
  Rtc_calend_timeT    alarm_time;
  Rtc_calend_dateT    alarm_date;
  Rtc_cntDwn_ctrlT    cntdwn_ctrl;
  unsigned int        cntdwn_init;
  int        cntdwn_timer;
  Rtc_CKIN_divT       ckin_div_reg;
  Rtc_ref_clkT        ref_clk_reg;

  int apb_irq_soc_event;
  int irq_soc_event;

  int ref_clock_cycles;
  
  unsigned int calendar_time_reset;
  unsigned int calendar_date_reset;

  unsigned int last_irq_state;
};



rtc::rtc(js::config *config)
: vp::component(config)
{

}



void rtc::check_interrupts()
{
  this->get_trace()->msg("Checking interrupts (flags: 0x%x, mask: 0x%x, last_state: 0x%x)\n",
    this->irq_flag_reg.raw, this->irq_mask_reg.raw, this->last_irq_state);

  unsigned int new_irq_state = this->irq_flag_reg.raw & ~this->irq_mask_reg.raw;
  if (new_irq_state != this->last_irq_state)
  {
    this->get_trace()->msg("Generating soc event (id: %d)\n", this->irq_soc_event);
    this->last_irq_state = new_irq_state;
    this->event_itf.sync(this->irq_soc_event);
    if (this->irq_itf.is_bound())
      this->irq_itf.sync(true);
  }
}

void rtc::update_calendar()
{
  this->get_trace()->msg("Updating calendar\n");

  this->calendar_time.second_0++;
  if (this->calendar_time.second_0 == 10)
  {
    this->calendar_time.second_0 = 0;
    this->calendar_time.second_1++;
    if (this->calendar_time.second_1 == 6)
    {
      this->calendar_time.second_1 = 0;
      this->calendar_time.minute_0++;
      if (this->calendar_time.minute_0 == 10)
      {
        this->calendar_time.minute_0 = 0;
        this->calendar_time.minute_1++;
        if (this->calendar_time.minute_1 == 6)
        {
          this->calendar_time.minute_1 = 0;
          this->calendar_time.hour_0++;

          if (this->calendar_time.hour_0 == 10)
          {
            this->calendar_time.hour_0 = 0;
            this->calendar_time.hour_1++;
          }

          int hour = this->calendar_time.hour_0 + this->calendar_time.hour_1 * 10;

          if (hour == 24)
          {
            this->calendar_time.hour_0 = 0;
            this->calendar_time.hour_1 = 0;

            int m = this->calendar_date.month_0 + this->calendar_date.month_1 * 10;
            int nb_days;
            if (m == 1 || m == 3 || m == 5 || m == 7 || m == 8 || m == 10 || m == 12)
              nb_days = 31;
            else if (m == 2)
              nb_days = 28;
            else
              nb_days = 30;

            this->calendar_date.day_0++;
            if (this->calendar_date.day_0 == 10)
            {
              this->calendar_date.day_0 = 0;
              this->calendar_date.day_1++;
            }
            int day = this->calendar_date.day_0 + this->calendar_date.day_1 * 10;
            if (day == nb_days + 1)
            {
              this->calendar_date.day_0 = 1;
              this->calendar_date.day_1 = 0;

              this->calendar_date.month_0++;
              if (this->calendar_date.month_0 == 10)
              {
                this->calendar_date.month_0 = 0;
                this->calendar_date.month_1++;
              }

              int month = this->calendar_date.month_0 + this->calendar_date.month_1 * 10;

              if (month == 13)
              {
                this->calendar_date.month_0 = 1;
                this->calendar_date.month_1 = 0;
                this->calendar_date.year_0++;
                if (this->calendar_date.year_0 == 10)
                {
                  this->calendar_date.year_0 = 0;
                  this->calendar_date.year_1++;
                }
              }
            }
          }
        }
      }
    }
  }

  if (!this->alarm_ctrl_reg.alarm1_En)
  {
    int reached = false;

    if (!this->alarm_ctrl_reg.alarm1_mode)
    {
      reached = this->calendar_date.raw == this->alarm_date.raw &&
        this->calendar_time.raw == this->alarm_time.raw;
    }
    else
    {
      reached = true;

      switch (this->alarm_ctrl_reg.alarm1_conf)
      {
        case 8: reached &= this->calendar_date.month_0 == this->alarm_date.month_0
          && this->calendar_date.month_1 == this->alarm_date.month_1;
        case 7: reached &= this->calendar_date.day_0 == this->alarm_date.day_0
          && this->calendar_date.day_1 == this->alarm_date.day_1;
        case 6: reached &= this->calendar_time.hour_0 == this->alarm_time.hour_0
          && this->calendar_time.hour_1 == this->alarm_time.hour_1;
        case 5: reached &= this->calendar_time.minute_0 == this->alarm_time.minute_0
          && this->calendar_time.minute_1 == this->alarm_time.minute_1;
        case 4: reached &= this->calendar_time.second_0 == this->alarm_time.second_0
          && this->calendar_time.second_1 == this->alarm_time.second_1;
          break;
      }
    }

    if (reached)
    {
      // Disabled the alarm in case we are in single-shot mode
      if (!this->alarm_ctrl_reg.alarm1_mode)
        this->alarm_ctrl_reg.alarm1_En = 1;

      // Register the interrupt in the flags and check if we must generate
      // soc event
      this->get_trace()->msg("Reached alarm, raising interrupt (mask: 0x%x)\n",
        RTC_Irq_Alarm1_Flag);
      this->irq_flag_reg.raw |= RTC_Irq_Alarm1_Flag;
      this->check_interrupts();
    }
  }

  if (!this->cntdwn_ctrl.cntDwn1_En)
  {
    this->cntdwn_timer--;
    if (this->cntdwn_timer == -1)    // Dolphin RTC is counting specified count + 1
    {
      this->get_trace()->msg("Reached countdown, raising interrupt (mask: 0x%x)\n",
        RTC_Irq_Timer1_Flag);
      this->irq_flag_reg.raw |= RTC_Irq_Timer1_Flag;
      this->check_interrupts();

      if (this->cntdwn_ctrl.cntDwn1_mode == 1)
        this->cntdwn_timer = this->cntdwn_init;
      else
        this->cntdwn_ctrl.cntDwn1_En = 1;
    }
  }
}



void rtc::ref_clock_sync(void *__this, bool value)
{
  rtc *_this = (rtc *)__this;

  if (value && !_this->ctrl_reg.rtc_sb)
  {
    _this->ref_clock_cycles++;
    if (_this->ref_clock_cycles >= _this->ckin_div_reg.divVal)
    {
      _this->ref_clock_cycles = 0;
      _this->update_calendar();
    }
  }
}



void rtc::soft_reset()
{
  this->get_trace()->msg("Soft reset\n");
  this->ref_clock_cycles = 0;
  this->ckin_div_reg.raw = 0x00008000;
  this->calend_ctrl_reg.raw = 0x00000000;
  this->calendar_time.raw = this->calendar_time_reset;
  this->calendar_date.raw = this->calendar_date_reset;
  this->alarm_ctrl_reg.raw = 0x00660003;
  this->alarm_time.raw = 0x00000000;
  this->alarm_date.raw = 0x00000000;
  this->cntdwn_ctrl.raw = 0x00000003;
  this->cntdwn_init = 0xffffffff;
  this->ctrl_reg.raw = 0;
}


void rtc::handle_status_access()
{
}



void rtc::handle_ctrl_access()
{
  if (!this->apb_ctrl_reg.apb_load)
    this->apb_data_reg = this->ctrl_reg.raw;
  else
  {
    this->ctrl_reg.raw = this->apb_data_reg;
    this->get_trace()->msg("Writing control register (rtc_sb: %d, calibration_en: %d, soft_rst: %d)\n",
      this->ctrl_reg.rtc_sb, this->ctrl_reg.cal_En, this->ctrl_reg.soft_rst);

    if (this->ctrl_reg.soft_rst)
    {
      this->soft_reset();
      this->ctrl_reg.soft_rst = 0;
    }
  }
}



void rtc::handle_clk_access()
{
  
}



void rtc::handle__access()
{
  
}



void rtc::handle_irq_ctrl_access()
{
  
}



void rtc::handle_irq_mask_access()
{
  if (!this->apb_ctrl_reg.apb_load)
    this->apb_data_reg = this->irq_mask_reg.raw;
  else
  {
    this->irq_mask_reg.raw = this->apb_data_reg;
    this->get_trace()->msg("Writing irq mask register (alarm_mask: %d, timer_mask: %d, calib_mask: %d)\n",
      this->irq_mask_reg.alarm1_masked, this->irq_mask_reg.alarm1_masked, this->irq_mask_reg.alarm1_masked);

    this->check_interrupts();
  }
}



void rtc::handle_irq_flag_access()
{
  if (!this->apb_ctrl_reg.apb_load)
    this->apb_data_reg = this->irq_flag_reg.raw;
  else
  {
    this->irq_flag_reg.raw &= ~this->apb_data_reg;
    this->last_irq_state &= ~this->apb_data_reg;
    this->get_trace()->msg("Writing irq flag register (alarm_mask: %d, timer_mask: %d, calib_mask: %d)\n",
      this->irq_flag_reg.alarm1_masked, this->irq_flag_reg.alarm1_masked, this->irq_flag_reg.alarm1_masked);
  }
}



void rtc::handle_calendar_ctrl_access()
{
  if (!this->apb_ctrl_reg.apb_load)
    this->apb_data_reg = this->calend_ctrl_reg.raw;
  else
  {
    this->calend_ctrl_reg.raw = this->apb_data_reg;
    this->get_trace()->msg("Writing calendar control register (raw: 0x%x, en: %d)\n",
      this->calend_ctrl_reg.raw, this->calend_ctrl_reg.calend_En);
  }
}



void rtc::handle_calendar_time_access()
{
  if (!this->apb_ctrl_reg.apb_load)
    this->apb_data_reg = this->calendar_time.raw;
  else
  {
    this->calendar_time.raw = this->apb_data_reg;
    this->get_trace()->msg("Writing calendar time register (value: 0x%x)\n",
      this->calendar_time.raw);
  }
}



void rtc::handle_calendar_date_access()
{
  if (!this->apb_ctrl_reg.apb_load)
    this->apb_data_reg = this->calendar_date.raw;
  else
  {
    this->calendar_date.raw = this->apb_data_reg;
    this->get_trace()->msg("Writing calendar date register (value: 0x%x)\n",
      this->calendar_date.raw);
  }
}



void rtc::handle_alarm_ctrl_access()
{
  if (!this->apb_ctrl_reg.apb_load)
    this->apb_data_reg = this->alarm_ctrl_reg.raw;
  else
  {
    this->alarm_ctrl_reg.raw = this->apb_data_reg;
    this->get_trace()->msg("Writing alarm control register (raw: 0x%x, en: %d, mode: %d, conf: %d)\n",
      this->alarm_ctrl_reg.raw, this->alarm_ctrl_reg.alarm1_En,
      this->alarm_ctrl_reg.alarm1_mode, this->alarm_ctrl_reg.alarm1_conf);
  }
}



void rtc::handle_alarm1_time_access()
{
  if (!this->apb_ctrl_reg.apb_load)
    this->apb_data_reg = this->alarm_time.raw;
  else
  {
    this->alarm_time.raw = this->apb_data_reg;
    this->get_trace()->msg("Writing alarm time register (value: 0x%x)\n",
      this->alarm_time);
  }
}



void rtc::handle_alarm1_date_access()
{
  if (!this->apb_ctrl_reg.apb_load)
    this->apb_data_reg = this->alarm_date.raw;
  else
  {
    this->alarm_date.raw = this->apb_data_reg;
    this->get_trace()->msg("Writing alarm date register (value: 0x%x)\n",
      this->alarm_date);
  }
}



void rtc::handle_cntdown_ctrl_access()
{
  if (!this->apb_ctrl_reg.apb_load)
    this->apb_data_reg = this->cntdwn_ctrl.raw;
  else
  {
    this->cntdwn_ctrl.raw = this->apb_data_reg;
    this->get_trace()->msg("Writing countdown control register (raw: 0x%x, en: %d, mode: %d)\n",
      this->cntdwn_ctrl.raw, this->cntdwn_ctrl.cntDwn1_En,
      this->cntdwn_ctrl.cntDwn1_mode);
    if (!this->cntdwn_ctrl.cntDwn1_En)
    {
      this->cntdwn_timer = this->cntdwn_init;
    }
  }
}



void rtc::handle_cntdown1_init_access()
{
  if (!this->apb_ctrl_reg.apb_load)
    this->apb_data_reg = this->cntdwn_init;
  else
  {
    this->cntdwn_init = this->apb_data_reg;
    this->get_trace()->msg("Writing countdown init register (raw: 0x%x)\n",
      this->cntdwn_init);
  }
}



void rtc::handle_cntdown1_timer_access()
{
  if (!this->apb_ctrl_reg.apb_load)
    this->apb_data_reg = this->cntdwn_timer;
}



void rtc::handle_ckin_div1_access()
{
  if (!this->apb_ctrl_reg.apb_load)
    this->apb_data_reg = this->ckin_div_reg.raw;
  else
  {
    this->ckin_div_reg.raw = this->apb_data_reg;
    this->get_trace()->msg("Writing ckin_div register (value: 0x%x, ckin_div: %d)\n",
      this->ckin_div_reg.raw, this->ckin_div_reg.divVal);
  }
}



void rtc::handle_ref_clk_conf_access()
{
  
}



void rtc::handle_test_addr_access()
{
  
}



void rtc::handle_internal_access()
{
  switch (this->apb_ctrl_reg.apb_addr)
  {
    case RTC_Status_Addr        : this->handle_status_access(); break;
    case RTC_Ctrl_Addr          : this->handle_ctrl_access(); break;
    case RTC_Clk_Ctrl_Addr      : this->handle_clk_access(); break;
    case RTC_IRQ_Ctrl_Addr      : this->handle_irq_ctrl_access(); break;
    case RTC_IRQ_Mask_Addr      : this->handle_irq_mask_access(); break;
    case RTC_IRQ_Flag_Addr      : this->handle_irq_flag_access(); break;
    case RTC_Calendar_Ctrl_Addr : this->handle_calendar_ctrl_access(); break;
    case RTC_Calendar_TIME_Addr : this->handle_calendar_time_access(); break;
    case RTC_Calendar_DATE_Addr : this->handle_calendar_date_access(); break;
    case RTC_Alarm_Ctrl_Addr    : this->handle_alarm_ctrl_access(); break;
    case RTC_Alarm1_TIME_Addr   : this->handle_alarm1_time_access(); break;
    case RTC_Alarm1_DATE_Addr   : this->handle_alarm1_date_access(); break;
    case RTC_CntDown_Ctrl_Addr  : this->handle_cntdown_ctrl_access(); break;
    case RTC_CntDown1_Init_Addr : this->handle_cntdown1_init_access(); break;
    case RTC_CntDown1_Timer_Addr: this->handle_cntdown1_timer_access(); break;
    case RTC_CKIN_DIV1_Addr     : this->handle_ckin_div1_access(); break;
    case RTC_Ref_Clk_Conf_Addr  : this->handle_ref_clk_conf_access(); break;
    case RTC_Test_Addr          : this->handle_test_addr_access(); break;
  }

  this->event_itf.sync(this->apb_irq_soc_event);
  if (this->apb_irq_itf.is_bound())
    this->apb_irq_itf.sync(true);
}

vp::io_req_status_e rtc::stat_req(int reg_offset, int size, bool is_write, uint8_t *data)
{
  return vp::io_req_status_e::IO_REQ_OK;
}



vp::io_req_status_e rtc::ctrl_req(int reg_offset, int size, bool is_write, uint8_t *data)
{
  if (!is_write)
  {
    memcpy(data, &(((uint8_t *)&this->apb_ctrl_reg.raw)[reg_offset]), size);
  }
  else
  {
    memcpy(&(((uint8_t *)&this->apb_ctrl_reg.raw)[reg_offset]), data, size);

    this->get_trace()->msg("Setting control register (addr: 0x%x, load: %d)\n",
      this->apb_ctrl_reg.apb_addr, this->apb_ctrl_reg.apb_load);

    this->handle_internal_access();
  }

  return vp::io_req_status_e::IO_REQ_OK;
}



vp::io_req_status_e rtc::data_req(int reg_offset, int size, bool is_write, uint8_t *data)
{
  if (!is_write)
  {
    memcpy(data, &(((uint8_t *)&this->apb_data_reg)[reg_offset]), size);
  }
  else
  {
    memcpy(&(((uint8_t *)&this->apb_data_reg)[reg_offset]), data, size);

    this->get_trace()->msg("Setting data register (value: 0x%x)\n",
      this->apb_data_reg);
  }
  return vp::io_req_status_e::IO_REQ_OK;
}



vp::io_req_status_e rtc::irq_ctrl_req(int reg_offset, int size, bool is_write, uint8_t *data)
{
  return vp::io_req_status_e::IO_REQ_OK;
}



vp::io_req_status_e rtc::irq_mask_req(int reg_offset, int size, bool is_write, uint8_t *data)
{
  return vp::io_req_status_e::IO_REQ_OK;
}



vp::io_req_status_e rtc::irq_flag_req(int reg_offset, int size, bool is_write, uint8_t *data)
{
  return vp::io_req_status_e::IO_REQ_OK;
}



vp::io_req_status_e rtc::req(void *__this, vp::io_req *req)
{
  rtc *_this = (rtc *)__this;

  vp::io_req_status_e err = vp::IO_REQ_INVALID;

  uint64_t offset = req->get_addr();
  uint8_t *data = req->get_data();
  uint64_t size = req->get_size();
  bool is_write = req->get_is_write();

  _this->trace.msg("RTC access (offset: 0x%x, size: 0x%x, is_write: %d)\n", offset, size, is_write);

  if (size != 4) return vp::IO_REQ_INVALID;

  int reg_id = offset / 4;
  int reg_offset = offset % 4;

  switch (reg_id) {
    case APB_RTC_STAT_OFFSET/4     : err = _this->stat_req(reg_offset, size, is_write, data); break;
    case APB_RTC_CTRL_OFFSET/4     : err = _this->ctrl_req(reg_offset, size, is_write, data); break;
    case APB_RTC_DATA_OFFSET/4     : err = _this->data_req(reg_offset, size, is_write, data); break;
    case APB_RTC_IRQ_CTRL_OFFSET/4 : err = _this->irq_ctrl_req(reg_offset, size, is_write, data); break;
    case APB_RTC_IRQ_MASK_OFFSET/4 : err = _this->irq_mask_req(reg_offset, size, is_write, data); break;
    case APB_RTC_IRQ_FLAG_OFFSET/4 : err = _this->irq_flag_req(reg_offset, size, is_write, data); break;
  }

  if (err != vp::IO_REQ_OK)
    goto error; 


  return vp::IO_REQ_OK;

error:
  _this->warning.force_warning("RTC invalid access (offset: 0x%x, size: 0x%x, is_write: %d)\n", offset, size, is_write);

  return vp::IO_REQ_INVALID;
}



int rtc::build()
{
  this->traces.new_trace("trace", &this->trace, vp::DEBUG);
  this->in.set_req_meth(&rtc::req);
  this->new_slave_port("input", &this->in);

  this->new_master_port("event", &this->event_itf);
  this->new_master_port("irq", &this->irq_itf);
  this->new_master_port("apb_irq", &this->apb_irq_itf);

  this->ref_clock_itf.set_sync_meth(&rtc::ref_clock_sync);
  this->new_slave_port("ref_clock", &this->ref_clock_itf);

  this->apb_irq_soc_event = this->get_js_config()->get_child_int("apb_irq_soc_event");
  this->irq_soc_event = this->get_js_config()->get_child_int("irq_soc_event");
  
  this->calendar_time_reset = this->get_js_config()->get_child_int("calendar_time");
  this->calendar_date_reset = this->get_js_config()->get_child_int("calendar_date");

  return 0;
}



void rtc::reset(bool active)
{
  if (active)
  {
    this->apb_ctrl_reg.raw = 0;
    this->irq_mask_reg.raw = 0x00001031;
    this->irq_flag_reg.raw = 0x00000000;
    this->last_irq_state = 0;
    this->soft_reset();
  }
}



void rtc::start()
{
}



extern "C" vp::component *vp_constructor(js::config *config)
{
  return new rtc(config);
}
