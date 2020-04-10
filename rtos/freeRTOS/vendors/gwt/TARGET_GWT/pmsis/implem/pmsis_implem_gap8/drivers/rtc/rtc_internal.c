/*
 * Copyright (c) 2019, GreenWaves Technologies, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of GreenWaves Technologies, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "pmsis/implem/drivers/rtc/rtc_internal.h"
#include "pmsis/implem/drivers/fc_event/fc_event.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

#define PI_RTC_YEAR_RANGE_START     ( 2000 )
#define PI_RTC_YEAR_RANGE_END       ( 2099 )

#define PI_RTC_IFR_ALARM_FLAG       ( RTC_RTC_IFR_ALARM_FLAG_MASK )
#define PI_RTC_IFR_TIMER_FLAG       ( RTC_RTC_IFR_TIMER_FLAG_MASK )
#define PI_RTC_IFR_CAL_FLAG         ( RTC_RTC_IFR_CAL_FLAG_MASK )

#define PI_RTC_CLK_DIV_DEFAULT      ( 0x8000 )

typedef struct
{
    uint32_t date;
    uint32_t time;
} pi_rtc_bcd_datetime_t;

/*******************************************************************************
 * Driver data
 ******************************************************************************/

static pi_task_t g_irq_task[3] = {0};

static volatile uint8_t g_rtc_pending = 0;

/*******************************************************************************
 * Function declaration
 ******************************************************************************/

/*
 * Function to check date and time format. Standard is not used to set the
 * structure here.
 * Ex: 20/12/2019 15:20:45
 * time->tm_year = 2019;     time->tm_hour = 15;
 * time->tm_mon  = 12;       time->tm_min  = 20;
 * time->tm_date = 20;       time->tm_sec  = 45;
 * If this struct needs to be used with standarg functions, copy this struct
 * into a new one. Adjust members according to standards.
 */
static int8_t __pi_rtc_datetime_check(struct tm *time);

/*! RTC_APB IRQ handler. Not used. */
static void __pi_rtc_apb_handler(void *arg);

/*! RTC IRQ handler. IRQ triggered when alarm/timer/calibration ends. */
static void __pi_rtc_handler(void *arg);
/*! Function called by the handler to clear IRQ flags. */
static void __pi_rtc_irq_type_handle(void *arg);

/*! Wait for APB IRQ. Used to read/write RTC internal registers. */
static void __pi_rtc_apb_reg_wait(uint8_t rtc_id);

/*! Read from RTC internal registers. */
static uint32_t __pi_rtc_apb_reg_read(uint8_t rtc_id, uint8_t addr);

/*! Write to RTC internal regisers. */
static void __pi_rtc_apb_reg_write(uint8_t rtc_id, uint8_t addr, uint32_t data);

/*! Disable RTC standby mode.  */
static void __pi_rtc_enable(uint8_t rtc_id);

/*! Enable RTC standby mode. */
static void __pi_rtc_disable(uint8_t rtc_id);

/*! Reset RTC device after configs. */
static void __pi_rtc_soft_reset(uint8_t rtc_id);

/*! Set up clock divider. Used when RTC should output a signal. */
static void __pi_rtc_clock_div_set(uint8_t rtc_id, uint16_t clk_div);

/*! Start calendar. */
static void __pi_rtc_calendar_start(uint8_t rtc_id);

/*! Stop calendar. */
static void __pi_rtc_calendar_stop(uint8_t rtc_id);

/*! Start alarm. */
static void __pi_rtc_alarm_start(uint8_t rtc_id, pi_rtc_alarm_repeat_e mode);

/*! Stop alarm. */
static void __pi_rtc_alarm_stop(uint8_t rtc_id);

/*! Start timer. */
static void __pi_rtc_timer_start(uint8_t rtc_id, uint8_t repeat);

/*! Stop timer. */
static void __pi_rtc_timer_stop(uint8_t rtc_id);

/*! Read RTC IRQ status. */
static uint32_t __pi_rtc_status_get(uint8_t rtc_id);

/*! Enable IRQ for alarm/timer/calibration. */
static void __pi_rtc_irq_mask_set(uint8_t rtc_id, uint32_t mask);

/*! Disable IRQ for alarm/timer/calibration. */
static void __pi_rtc_irq_mask_clear(uint8_t rtc_id, uint32_t mask);

/*! Read IRQ flags. */
static uint32_t __pi_rtc_irq_flag_get(uint8_t rtc_id);

/*!
 * Acknowledge received IRQ, need to be cleared to get other IRQ.
 * Clear IRQ flags. IRQ flags need to be cleared before in order to receive
 * next ones.
 */
static void __pi_rtc_irq_flag_clear(uint8_t rtc_id, uint32_t mask);

/*******************************************************************************
 * Internal functions
 ******************************************************************************/

static int8_t __pi_rtc_datetime_check(struct tm *time)
{
    /* First month is for padding. */
    uint8_t days_month[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    if ((time->tm_year <= (uint16_t) PI_RTC_YEAR_RANGE_START) ||
        (time->tm_year >  (uint16_t) PI_RTC_YEAR_RANGE_END)   ||
        (time->tm_mon  < 1) || (time->tm_mon  > 12)           ||
        (time->tm_mday < 1) || (time->tm_mday > 31)           )
    {
        printf("Wrong date format %d/%d/%d!\n", time->tm_mday, time->tm_mon, time->tm_year);
        return -1;
    }
    if ((time->tm_hour < 0) || (time->tm_hour > 24) ||
        (time->tm_min  < 0) || (time->tm_min  > 59) ||
        (time->tm_sec  < 0) || (time->tm_sec  > 59) )
    {
        printf("Wrong time format %d:%d:%d!\n", time->tm_hour, time->tm_min, time->tm_sec);
        return -2;
    }
    /* Leap year check. */
    if ((((time->tm_year & 0x3) == 0) && ((time->tm_year % 100) != 0)) ||
        ((time->tm_year % 400) == 0))
    {
        days_month[2] = 29;
    }
    if (time->tm_mday > days_month[time->tm_mon])
    {
        return -3;
    }
    return 0;
}

static void __pi_rtc_datetime_to_bcd(struct tm *time, pi_rtc_bcd_datetime_t *datetime)
{
    rtc_calendar_date_t _date = {0};
    rtc_calendar_time_t _time = {0};
    _date.field.yea1 = (time->tm_year - (int) PI_RTC_YEAR_RANGE_START) / 10;
    _date.field.yea0 = (time->tm_year - (int) PI_RTC_YEAR_RANGE_START) % 10;
    _date.field.mon1 = time->tm_mon / 10;
    _date.field.mon0 = time->tm_mon % 10;
    _date.field.day1 = time->tm_mday / 10;
    _date.field.day0 = time->tm_mday % 10;

    _time.field.hou1 = time->tm_hour / 10;
    _time.field.hou0 = time->tm_hour % 10;
    _time.field.min1 = time->tm_min / 10;
    _time.field.min0 = time->tm_min % 10;
    _time.field.sec1 = time->tm_sec / 10;
    _time.field.sec0 = time->tm_sec % 10;

    datetime->date = _date.word;
    datetime->time = _time.word;
}

static void __pi_rtc_bcd_to_datetime(pi_rtc_bcd_datetime_t *datetime, struct tm *time)
{
    rtc_calendar_date_t _date = {0};
    rtc_calendar_time_t _time = {0};
    _date.word = datetime->date;
    _time.word = datetime->time;
    time->tm_year = _date.field.yea1 * 10 + _date.field.yea0 + (int) PI_RTC_YEAR_RANGE_START;
    time->tm_mon = _date.field.mon1 * 10 + _date.field.mon0;
    time->tm_mday = _date.field.day1 * 10 + _date.field.day0;
    time->tm_hour = _time.field.hou1 * 10 + _time.field.hou0;
    time->tm_min = _time.field.min1 * 10 + _time.field.min0;
    time->tm_sec = _time.field.sec1 * 10 + _time.field.sec0;
}

static void __pi_rtc_irq_type_handle(void *arg)
{
    struct pi_task *task = (struct pi_task *) arg;
    uint8_t rtc_id = task->data[2];
    uint32_t flags = __pi_rtc_irq_flag_get(rtc_id);
    callback_t func = NULL;
    if (flags & PI_RTC_IFR_CAL_FLAG)
    {
        __pi_rtc_irq_flag_clear(rtc_id, PI_RTC_IFR_CAL_FLAG);
        func = (callback_t) g_irq_task[0].arg[0];
        if (func != NULL)
        {
            func((void *) g_irq_task[0].arg[1]);
        }
    }
    if (flags & PI_RTC_IFR_ALARM_FLAG)
    {
        __pi_rtc_irq_flag_clear(rtc_id, PI_RTC_IFR_ALARM_FLAG);
        func = (callback_t) g_irq_task[1].arg[0];
        if (func != NULL)
        {
            func((void *) g_irq_task[1].arg[1]);
        }
    }
    if (flags & PI_RTC_IFR_TIMER_FLAG)
    {
        __pi_rtc_irq_flag_clear(rtc_id, PI_RTC_IFR_TIMER_FLAG);
        func = (callback_t) g_irq_task[2].arg[0];
        if (func != NULL)
        {
            func((void *) g_irq_task[2].arg[1]);
        }
    }
    pi_l2_free(task, sizeof(struct pi_task));
}

static void __pi_rtc_apb_handler(void *arg)
{
    g_rtc_pending = 0;
}

static void __pi_rtc_handler(void *arg)
{
    uint32_t event = (uint32_t) arg;
    uint8_t rtc_id = 0;
    struct pi_task *task = (struct pi_task *) pi_l2_malloc(sizeof(struct pi_task));
    if (task == NULL)
    {
        printf("Task is NULL !\n");
        return;
    }
    pi_task_callback(task, __pi_rtc_irq_type_handle, task);
    task->data[2] = rtc_id;
    pmsis_event_push(pmsis_event_get_default_scheduler(), task);
}

/*
 * Using active wait to avoid overhead due to IRQ handling.
 * This way should be faster.
 */
static void __pi_rtc_apb_reg_wait(uint8_t rtc_id)
{
    while (g_rtc_pending == 1)
    {
        hal_eu_evt_mask_wait_and_clr(1 << FC_SW_NOTIFY_EVENT);
    }
}

static uint32_t __pi_rtc_apb_reg_read(uint8_t rtc_id, uint8_t addr)
{
    uint32_t data = 0;
    hal_rtc_request_set(rtc_id, addr, 0);
    /* Wait IRQ. */
    g_rtc_pending = 1;
    __pi_rtc_apb_reg_wait(rtc_id);
    /* Read data. */
    data = hal_rtc_data_get(rtc_id);
    /* Reset Read IRQ. */
    hal_rtc_irq_flag_read_reset(rtc_id);
    return data;
}

static void __pi_rtc_apb_reg_write(uint8_t rtc_id, uint8_t addr, uint32_t data)
{
    hal_rtc_data_set(rtc_id, data);
    hal_rtc_request_set(rtc_id, addr, 1);
    /* Wait IRQ. */
    g_rtc_pending = 1;
    __pi_rtc_apb_reg_wait(rtc_id);
    /* Reset Write IRQ. */
    hal_rtc_irq_flag_write_reset(rtc_id);
}

static void __pi_rtc_enable(uint8_t rtc_id)
{
    uint32_t data = __pi_rtc_apb_reg_read(rtc_id, (uint8_t) RTC_CR_OFFSET);
    data &= ~RTC_RTC_CR_SB_MASK;
    __pi_rtc_apb_reg_write(rtc_id, (uint8_t) RTC_CR_OFFSET, data);
}

static void __pi_rtc_disable(uint8_t rtc_id)
{
    uint32_t data = __pi_rtc_apb_reg_read(rtc_id, (uint8_t) RTC_CR_OFFSET);
    data |= RTC_RTC_CR_SB(1);
    __pi_rtc_apb_reg_write(rtc_id, (uint8_t) RTC_CR_OFFSET, data);
}

static void __pi_rtc_soft_reset(uint8_t rtc_id)
{
    uint32_t data = __pi_rtc_apb_reg_read(rtc_id, (uint8_t) RTC_CR_OFFSET);
    data |= RTC_RTC_CR_SOFT_RST(1);
    __pi_rtc_apb_reg_write(rtc_id, (uint8_t) RTC_CR_OFFSET, data);
}

static void __pi_rtc_clock_div_set(uint8_t rtc_id, uint16_t clk_div)
{
    __pi_rtc_apb_reg_write(rtc_id, (uint8_t) CKIN_DIV1_OFFSET, clk_div);
    __pi_rtc_disable(rtc_id);
    __pi_rtc_enable(rtc_id);
}

static void __pi_rtc_calendar_start(uint8_t rtc_id)
{
    uint32_t data = RTC_CALENDAR_CONTROL_SB(0);
    __pi_rtc_apb_reg_write(rtc_id, (uint8_t) CALENDAR_CONTROL_OFFSET, data);
}

static void __pi_rtc_calendar_stop(uint8_t rtc_id)
{
    uint32_t data = RTC_CALENDAR_CONTROL_SB(1);
    __pi_rtc_apb_reg_write(rtc_id, (uint8_t) CALENDAR_CONTROL_OFFSET, data);
}

static void __pi_rtc_alarm_start(uint8_t rtc_id, pi_rtc_alarm_repeat_e mode)
{
    /* Enable alarm IRQ. */
    __pi_rtc_irq_mask_set(rtc_id, PI_RTC_IFR_ALARM_FLAG);
    /* Clear alarm IRQ flag. */
    __pi_rtc_irq_flag_clear(rtc_id, PI_RTC_IFR_ALARM_FLAG);
    uint8_t repeat = (mode & 0xF) ? 1 : 0;
    uint32_t data = RTC_ALARM_CONTROL_MODE(repeat) |
                    RTC_ALARM_CONTROL_CFG(mode) |
                    RTC_ALARM_CONTROL_SB(0);
    __pi_rtc_apb_reg_write(rtc_id, (uint8_t) ALARM_CONTROL_OFFSET, data);
}

static void __pi_rtc_alarm_stop(uint8_t rtc_id)
{
    uint32_t data = RTC_ALARM_CONTROL_SB(1);
    __pi_rtc_apb_reg_write(rtc_id, (uint8_t) ALARM_CONTROL_OFFSET, data);
}

static uint32_t __pi_rtc_irq_mask_get(uint8_t rtc_id);

static void __pi_rtc_timer_start(uint8_t rtc_id, uint8_t repeat)
{
    /* Enable timer IRQ. */
    __pi_rtc_irq_mask_set(rtc_id, PI_RTC_IFR_TIMER_FLAG);
    /* Clear timer IRQ flag. */
    __pi_rtc_irq_flag_clear(rtc_id, PI_RTC_IFR_TIMER_FLAG);
    uint32_t data = RTC_COUNTDOWN_CONTROL_MODE(repeat) |
                    RTC_COUNTDOWN_CONTROL_SB(0);
    __pi_rtc_apb_reg_write(rtc_id, (uint8_t) COUNTDOWN_CONTROL_OFFSET, data);
}

static void __pi_rtc_timer_stop(uint8_t rtc_id)
{
    uint32_t data = RTC_COUNTDOWN_CONTROL_SB(1);
    __pi_rtc_apb_reg_write(rtc_id, (uint8_t) COUNTDOWN_CONTROL_OFFSET, data);
}

static uint32_t __pi_rtc_status_get(uint8_t rtc_id)
{
    return __pi_rtc_apb_reg_read(rtc_id, (uint8_t) RTC_SR_OFFSET);
}

static uint32_t __pi_rtc_irq_mask_get(uint8_t rtc_id)
{
    return __pi_rtc_apb_reg_read(rtc_id, (uint8_t) RTC_IMR_OFFSET);
}

static void __pi_rtc_irq_mask_set(uint8_t rtc_id, uint32_t mask)
{
    uint32_t data = __pi_rtc_apb_reg_read(rtc_id, (uint8_t) RTC_IMR_OFFSET);
    data &= ~mask;
    __pi_rtc_apb_reg_write(rtc_id, (uint8_t) RTC_IMR_OFFSET, data);
}

static void __pi_rtc_irq_mask_clear(uint8_t rtc_id, uint32_t mask)
{
    uint32_t data = __pi_rtc_apb_reg_read(rtc_id, (uint8_t) RTC_IMR_OFFSET);
    data |= mask;
    __pi_rtc_apb_reg_write(rtc_id, (uint8_t) RTC_IMR_OFFSET, data);
}

static uint32_t __pi_rtc_irq_flag_get(uint8_t rtc_id)
{
    uint32_t data = __pi_rtc_apb_reg_read(rtc_id, (uint8_t) RTC_IFR_OFFSET);
    return data;
}

static void __pi_rtc_irq_flag_clear(uint8_t rtc_id, uint32_t mask)
{
    uint32_t data = __pi_rtc_apb_reg_read(rtc_id, (uint8_t) RTC_IFR_OFFSET);
    data |= mask;
    __pi_rtc_apb_reg_write(rtc_id, (uint8_t) RTC_IFR_OFFSET, data);
}

static void __pi_rtc_cb_attach(uint8_t rtc_id, uint32_t mask, pi_task_t *task)
{
    switch (mask)
    {
    case PI_RTC_IFR_CAL_FLAG :
        memcpy(&g_irq_task[0], task, sizeof(pi_task_t));
        break;
    case PI_RTC_IFR_ALARM_FLAG :
        memcpy(&g_irq_task[1], task, sizeof(pi_task_t));
        break;
    case PI_RTC_IFR_TIMER_FLAG :
        memcpy(&g_irq_task[2], task, sizeof(pi_task_t));
        break;
    }
}

/*******************************************************************************
 * API implementation
/ ******************************************************************************/

void __pi_rtc_conf_init(struct pi_rtc_conf *conf)
{
    conf->rtc_id = 0;
    conf->mode = PI_RTC_MODE_CALENDAR;
    conf->time.tm_year = 2019;
    conf->time.tm_mon  = 12;
    conf->time.tm_mday = 31;
    conf->time.tm_hour = 23;
    conf->time.tm_min  = 59;
    conf->time.tm_sec  = 55;
    memset(&(conf->alarm), 0, sizeof(struct tm));
    conf->counter = 0;
}

int32_t __pi_rtc_open(uint8_t rtc_id, struct pi_rtc_conf *conf)
{
    int32_t errors = 0;
    /* Set handler. */
    pi_fc_event_handler_set(SOC_EVENT_RTC_APB, __pi_rtc_apb_handler);
    pi_fc_event_handler_set(SOC_EVENT_RTC, __pi_rtc_handler);
    /* Enable SOC event propagation to FC. */
    hal_soc_eu_set_fc_mask(SOC_EVENT_RTC_APB);
    hal_soc_eu_set_fc_mask(SOC_EVENT_RTC);

    /* Reset RTC. */
    __pi_rtc_soft_reset(rtc_id);

    /* Set clock divider. */
    __pi_rtc_clock_div_set(rtc_id, (uint32_t) PI_RTC_CLK_DIV_DEFAULT);

    if (conf->mode & PI_RTC_MODE_CALENDAR)
    {
        errors = __pi_rtc_datetime_set(rtc_id, &(conf->time));
        if (errors)
        {
            return errors;
        }
    }
    if (conf->mode & PI_RTC_MODE_ALARM)
    {
        errors = __pi_rtc_alarm_set(rtc_id, &(conf->alarm));
        if (errors)
        {
            return errors;
        }
    }
    if (conf->mode & PI_RTC_MODE_TIMER)
    {
        errors = __pi_rtc_timer_set(rtc_id, conf->counter);
        if (errors)
        {
            return errors;
        }
    }
    return errors;
}

void __pi_rtc_close(uint8_t rtc_id)
{
    /* Disable RTC. */
    __pi_rtc_disable(rtc_id);

    /* Clear handler. */
    /* SOC_EVENT_RTC_APB events are handled using IRQ. */
    //pi_fc_event_handler_clear(SOC_EVENT_RTC_APB);
    pi_fc_event_handler_clear(SOC_EVENT_RTC);
    /* Disable SOC event propagation to FC. */
    hal_soc_eu_clear_fc_mask(SOC_EVENT_RTC_APB);
    hal_soc_eu_clear_fc_mask(SOC_EVENT_RTC);
}

int32_t __pi_rtc_datetime_set(uint8_t rtc_id, struct tm *time)
{
    int32_t errors = 0;
    errors = __pi_rtc_datetime_check(time);
    if (errors)
    {
        return errors;
    }

    pi_rtc_bcd_datetime_t datetime = {0};
    __pi_rtc_datetime_to_bcd(time, &datetime);
    __pi_rtc_apb_reg_write(rtc_id, (uint8_t) CALENDAR_DATE_OFFSET, datetime.date);
    __pi_rtc_apb_reg_write(rtc_id, (uint8_t) CALENDAR_TIME_OFFSET, datetime.time);
    return errors;
}

int32_t __pi_rtc_datetime_get(uint8_t rtc_id, struct tm *time)
{
    pi_rtc_bcd_datetime_t datetime = {0};
    datetime.date = __pi_rtc_apb_reg_read(rtc_id, (uint8_t) CALENDAR_DATE_OFFSET);
    datetime.time = __pi_rtc_apb_reg_read(rtc_id, (uint8_t) CALENDAR_TIME_OFFSET);
    __pi_rtc_bcd_to_datetime(&datetime, time);
    return 0;
}

int32_t __pi_rtc_alarm_set(uint8_t rtc_id, struct tm *alarm)
{
    int32_t errors = 0;
    errors = __pi_rtc_datetime_check(alarm);
    if (errors)
    {
        return errors;
    }

    pi_rtc_bcd_datetime_t datetime = {0};
    __pi_rtc_datetime_to_bcd(alarm, &datetime);

    __pi_rtc_apb_reg_write(rtc_id, (uint8_t) ALARM1_DATE_OFFSET, datetime.date);
    __pi_rtc_apb_reg_write(rtc_id, (uint8_t) ALARM1_TIME_OFFSET, datetime.time);
    return errors;
}

int32_t __pi_rtc_alarm_get(uint8_t rtc_id, struct tm *alarm)
{
    pi_rtc_bcd_datetime_t datetime = {0};
    datetime.date = __pi_rtc_apb_reg_read(rtc_id, (uint8_t) ALARM1_DATE_OFFSET);
    datetime.time = __pi_rtc_apb_reg_read(rtc_id, (uint8_t) ALARM1_TIME_OFFSET);
    __pi_rtc_bcd_to_datetime(&datetime, alarm);
    return 0;
}

int32_t __pi_rtc_timer_set(uint8_t rtc_id, uint32_t countdown)
{
    if (!countdown)
    {
        return -1;
    }
    __pi_rtc_apb_reg_write(rtc_id, (uint8_t) COUNTDOWN1_INIT_OFFSET, countdown);
    return 0;
}

uint32_t __pi_rtc_timer_get(uint8_t rtc_id)
{
    return __pi_rtc_apb_reg_read(rtc_id, (uint8_t) COUNTDOWN1_TIMER_OFFSET);
}

int32_t __pi_rtc_ioctl(uint8_t rtc_id, uint32_t cmd, void *arg)
{
    switch (cmd)
    {
    case PI_RTC_CALENDAR_START :
        __pi_rtc_calendar_start(rtc_id);
        break;
    case PI_RTC_CALENDAR_STOP :
        __pi_rtc_calendar_stop(rtc_id);
        break;
    case PI_RTC_ALARM_START :
    {
        pi_rtc_alarm_repeat_e repeat = (pi_rtc_alarm_repeat_e) arg;
        __pi_rtc_alarm_start(rtc_id, repeat);
    }
        break;
    case PI_RTC_ALARM_STOP :
        __pi_rtc_alarm_stop(rtc_id);
        break;
    case PI_RTC_TIMER_START :
    {
        uint8_t repeat = (uint32_t) arg;
        __pi_rtc_timer_start(rtc_id, repeat);
    }
        break;
    case PI_RTC_TIMER_STOP :
        __pi_rtc_timer_stop(rtc_id);
        break;
    case PI_RTC_ATTACH_CB_CAL :
    {
        pi_task_t *task = (pi_task_t *) arg;
        __pi_rtc_cb_attach(rtc_id, PI_RTC_IFR_CAL_FLAG, task);
    }
        break;
    case PI_RTC_ATTACH_CB_ALARM :
    {
        pi_task_t *task = (pi_task_t *) arg;
        __pi_rtc_cb_attach(rtc_id, PI_RTC_IFR_ALARM_FLAG, task);
    }
        break;
    case PI_RTC_ATTACH_CB_TIMER :
    {
        pi_task_t *task = (pi_task_t *) arg;
        __pi_rtc_cb_attach(rtc_id, PI_RTC_IFR_TIMER_FLAG, task);
    }
        break;
    default :
        return -199;
    }
    return 0;
}
