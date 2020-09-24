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

#include "archi/rtc/rtc_v2.h"



class rtc : public vp::component
{

public:

  rtc(js::config *config);

  int build();
  void start();
  void reset(bool active);

private:

  void update_calendar();
  void raise_interrupt();
  void check_state();

  static void ref_clock_sync(void *__this, bool value);

  static vp::io_req_status_e req(void *__this, vp::io_req *req);

  vp::io_req_status_e clock_req(int reg_offset, int size, bool is_write, uint8_t *data);
  vp::io_req_status_e alarm_req(int reg_offset, int size, bool is_write, uint8_t *data);
  vp::io_req_status_e timer_req(int reg_offset, int size, bool is_write, uint8_t *data);
  vp::io_req_status_e date_req(int reg_offset, int size, bool is_write, uint8_t *data);


  vp::trace             trace;
  vp::io_slave          in;
  vp::wire_master<int>  event_itf;
  vp::wire_master<bool> irq_itf;
  vp::clock_slave       ref_clock_itf;

  vp_rtc_clock          r_clock;
  vp_rtc_alarm          r_alarm;
  vp_rtc_timer          r_timer;
  vp_rtc_date           r_date;

  vp::reg_32            ref_clock_cycles;
  vp::reg_32            timer_count;

  int soc_event;

  unsigned int last_irq_state;
};



rtc::rtc(js::config *config)
: vp::component(config)
{

}



void rtc::raise_interrupt()
{
  if (this->soc_event != -1)
  {
    this->get_trace()->msg("Raising soc event (soc_event: %d)\n", this->soc_event);
    this->event_itf.sync(this->soc_event);
  }

  if (this->irq_itf.is_bound())
  {
    this->get_trace()->msg("Raising interrupt\n");
    this->irq_itf.sync(true);
  }
}



void rtc::update_calendar()
{
  this->get_trace()->msg("Updating calendar\n");

  this->r_clock.seconds_0_set(this->r_clock.seconds_0_get() + 1);

  if (this->r_clock.seconds_0_get() == 10)
  {
    this->r_clock.seconds_0_set(0);
    this->r_clock.seconds_1_set(this->r_clock.seconds_1_get() + 1);

    if (this->r_clock.seconds_1_get() == 6)
    {
      this->r_clock.seconds_1_set(0);
      this->r_clock.minutes_0_set(this->r_clock.minutes_0_get() + 1);

      if (this->r_clock.minutes_0_get() == 10)
      {
        this->r_clock.minutes_0_set(0);
        this->r_clock.minutes_1_set(this->r_clock.minutes_1_get() + 1);

        if (this->r_clock.minutes_1_get() == 6)
        {
          this->r_clock.minutes_1_set(0);
          this->r_clock.hours_0_set(this->r_clock.hours_0_get() + 1);

          if (this->r_clock.hours_0_get() == 10)
          {
            this->r_clock.hours_0_set(0);
            this->r_clock.hours_1_set(this->r_clock.hours_1_get() + 1);
          }

          int hour = this->r_clock.hours_0_get() + this->r_clock.hours_1_get() * 10;

          if (hour == 24)
          {
            this->r_clock.hours_0_set(0);
            this->r_clock.hours_1_set(0);

            int m = this->r_date.month_0_get() + this->r_date.month_1_get() * 10;
            int nb_days;
            if (m == 1 || m == 3 || m == 5 || m == 7 || m == 8 || m == 10 || m == 12)
              nb_days = 31;
            else if (m == 2)
              nb_days = 28;
            else
              nb_days = 30;

            this->r_date.day_0_set(this->r_date.day_0_get() + 1);

            if (this->r_date.day_0_get() == 10)
            {
              this->r_date.day_0_set(0);
              this->r_date.day_1_set(this->r_date.day_1_get() + 1);
            }
            int day = this->r_date.day_0_get() + this->r_date.day_1_get() * 10;
            if (day == nb_days + 1)
            {
              this->r_date.day_0_set(1);
              this->r_date.day_1_set(0);

              this->r_date.month_0_set(this->r_date.month_0_get() + 1);
              if (this->r_date.month_0_get() == 10)
              {
                this->r_date.month_0_set(0);
                this->r_date.month_1_set(this->r_date.month_1_get() + 1);
              }

              int month = this->r_date.month_0_get() + this->r_date.month_1_get() * 10;

              if (month == 13)
              {
                this->r_date.month_0_set(1);
                this->r_date.month_1_set(0);
                this->r_date.year_0_set(this->r_date.year_0_get() + 1);
                if (this->r_date.year_0_get() == 10)
                {
                  this->r_date.year_0_set(0);
                  this->r_date.year_1_set(this->r_date.year_1_get() + 1);
                }
              }
            }
          }
        }
      }
    }
  }

#if 0
  if (~this->alarm_ctrl_reg.alarm1_En)
  {
    int reached = false;

    if (!this->alarm_ctrl_reg.alarm1_mode)
    {
      reached = this->r_date.raw == this->alarm_date.raw &&
        this->r_clock.raw == this->alarm_time.raw;
    }
    else
    {
      reached = true;

      switch (this->alarm_ctrl_reg.alarm1_conf)
      {
        case 8: reached &= this->r_date.month_0 == this->alarm_date.month_0
          && this->r_date.month_1 == this->alarm_date.month_1;
        case 7: reached &= this->r_date.day_0 == this->alarm_date.day_0
          && this->r_date.day_1 == this->alarm_date.day_1;
        case 6: reached &= this->r_clock.hour_0 == this->alarm_time.hour_0
          && this->r_clock.hour_1 == this->alarm_time.hour_1;
        case 5: reached &= this->r_clock.minute_0 == this->alarm_time.minute_0
          && this->r_clock.minute_1 == this->alarm_time.minute_1;
        case 4: reached &= this->r_clock.second_0 == this->alarm_time.second_0
          && this->r_clock.second_1 == this->alarm_time.second_1;
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

  if (~this->cntdwn_ctrl.cntDwn1_En)
  {
    this->cntdwn_timer--;
    if (this->cntdwn_timer == 0)
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
  #endif
}



void rtc::check_state()
{



}


void rtc::ref_clock_sync(void *__this, bool value)
{
  rtc *_this = (rtc *)__this;

  if (value == 0)
    return;

  uint32_t cycles = _this->ref_clock_cycles.get() + 1;

  if (_this->r_timer.enable_get())
  {
    _this->timer_count.set(_this->timer_count.get() + 1);

    if (_this->timer_count.get() == _this->r_timer.target_get())
    {
      _this->get_trace()->msg("Timer reached target (target: %d)\n", _this->r_timer.target_get());

      _this->timer_count.set(0);

      if (!_this->r_timer.retrig_get())
        _this->r_timer.enable_set(0);

      _this->raise_interrupt();
    }
  }

  if (cycles == 0x8000)
  {
    _this->ref_clock_cycles.set(0);
    _this->update_calendar();
  }
  else
  {
    _this->ref_clock_cycles.set(cycles);
  }
}



vp::io_req_status_e rtc::clock_req(int reg_offset, int size, bool is_write, uint8_t *data)
{
  if (is_write)
  {
    this->r_clock.write(reg_offset, size, data);
    this->ref_clock_cycles.set(this->r_clock.init_sec_count_get());
  }
  else
  {
    this->r_clock.read(reg_offset, size, data);
  }

  return vp::io_req_status_e::IO_REQ_OK;
}



vp::io_req_status_e rtc::alarm_req(int reg_offset, int size, bool is_write, uint8_t *data)
{
  if (is_write)
  {
    this->r_alarm.write(reg_offset, size, data);
    this->check_state();
  }
  else
  {
    this->r_alarm.read(reg_offset, size, data);
  }

  return vp::io_req_status_e::IO_REQ_OK;
}



vp::io_req_status_e rtc::timer_req(int reg_offset, int size, bool is_write, uint8_t *data)
{
  if (is_write)
  {
    this->r_timer.write(reg_offset, size, data);
    this->timer_count.set(0);
    this->check_state();
  }
  else
  {
    this->r_timer.read(reg_offset, size, data);
  }

  return vp::io_req_status_e::IO_REQ_OK;
}



vp::io_req_status_e rtc::date_req(int reg_offset, int size, bool is_write, uint8_t *data)
{
  if (is_write)
  {
    this->r_date.write(reg_offset, size, data);
    this->check_state();
  }
  else
  {
    this->r_date.read(reg_offset, size, data);
  }

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
    case RTC_CLOCK_OFFSET/4:
      err = _this->clock_req(reg_offset, size, is_write, data);
      break;

    case RTC_ALARM_OFFSET/4:
      err = _this->alarm_req(reg_offset, size, is_write, data);
      break;

    case RTC_TIMER_OFFSET/4:
      err = _this->timer_req(reg_offset, size, is_write, data);
      break;

    case RTC_DATE_OFFSET/4:
      err = _this->date_req(reg_offset, size, is_write, data);
      break;
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

  this->ref_clock_itf.set_sync_meth(&rtc::ref_clock_sync);
  this->new_slave_port("ref_clock", &this->ref_clock_itf);

  this->soc_event = this->get_js_config()->get_child_int("soc_event");

  this->new_reg("clock", &this->r_clock, 0);
  this->new_reg("alarm", &this->r_alarm, 0);
  this->new_reg("timer", &this->r_timer, 0);
  this->new_reg("date", &this->r_date, 0);

  this->new_reg("ref_clock_cycles", &this->ref_clock_cycles, 0);
  this->new_reg("timer_count", &this->timer_count, 0);

  return 0;
}



void rtc::reset(bool active)
{
}



void rtc::start()
{
}



extern "C" vp::component *vp_constructor(js::config *config)
{
  return new rtc(config);
}
