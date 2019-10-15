/*
 * Copyright (c) 2018, GreenWaves Technologies, Inc.
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

#include "gap_rtc.h"
#include <time.h>

/*******************************************************************************
 * Variables, macros, structures,... definition
 ******************************************************************************/

#define SECONDS_IN_A_DAY    (86400U)
#define SECONDS_IN_A_HOUR   (3600U)
#define SECONDS_IN_A_MINUTE (60U)
#define DAYS_IN_A_YEAR      (365U)
#define YEAR_RANGE_START    (2000U)
#define YEAR_RANGE_END      (2099U)

/*Year or Hour*/
#define YH_BCD_HIGH_BYTE_MASK        (0xF00000)
#define YH_BCD_HIGH_BYTE_SHIFT       (20U)
#define YH_BCD_HIGH_BYTE(x)          (((uint32_t)(((uint32_t)(x)) << YH_BCD_HIGH_BYTE_SHIFT)) & YH_BCD_HIGH_BYTE_MASK)
#define YH_BCD_HIGH_BYTE_READ(x)     (((uint32_t)(((uint32_t)(x)) & YH_BCD_HIGH_BYTE_MASK)) >> YH_BCD_HIGH_BYTE_SHIFT)

#define YH_BCD_LOW_BYTE_MASK         (0xF0000)
#define YH_BCD_LOW_BYTE_SHIFT        (16U)
#define YH_BCD_LOW_BYTE(x)           (((uint32_t)(((uint32_t)(x)) << YH_BCD_LOW_BYTE_SHIFT)) & YH_BCD_LOW_BYTE_MASK)
#define YH_BCD_LOW_BYTE_READ(x)      (((uint32_t)(((uint32_t)(x)) & YH_BCD_LOW_BYTE_MASK)) >> YH_BCD_LOW_BYTE_SHIFT)

/*Month or Minute*/
#define MM_BCD_HIGH_BYTE_MASK        (0xF000)
#define MM_BCD_HIGH_BYTE_SHIFT       (12U)
#define MM_BCD_HIGH_BYTE(x)          (((uint32_t)(((uint32_t)(x)) << MM_BCD_HIGH_BYTE_SHIFT)) & MM_BCD_HIGH_BYTE_MASK)
#define MM_BCD_HIGH_BYTE_READ(x)     (((uint32_t)(((uint32_t)(x)) & MM_BCD_HIGH_BYTE_MASK)) >> MM_BCD_HIGH_BYTE_SHIFT)

#define MM_BCD_LOW_BYTE_MASK         (0xF00)
#define MM_BCD_LOW_BYTE_SHIFT        (8U)
#define MM_BCD_LOW_BYTE(x)           (((uint32_t)(((uint32_t)(x)) << MM_BCD_LOW_BYTE_SHIFT)) & MM_BCD_LOW_BYTE_MASK)
#define MM_BCD_LOW_BYTE_READ(x)      (((uint32_t)(((uint32_t)(x)) & MM_BCD_LOW_BYTE_MASK)) >> MM_BCD_LOW_BYTE_SHIFT)

/*Day or Second*/
#define DS_BCD_HIGH_BYTE_MASK        (0xF0)
#define DS_BCD_HIGH_BYTE_SHIFT       (4U)
#define DS_BCD_HIGH_BYTE(x)          (((uint32_t)(((uint32_t)(x)) << DS_BCD_HIGH_BYTE_SHIFT)) & DS_BCD_HIGH_BYTE_MASK)
#define DS_BCD_HIGH_BYTE_READ(x)     (((uint32_t)(((uint32_t)(x)) & DS_BCD_HIGH_BYTE_MASK)) >> DS_BCD_HIGH_BYTE_SHIFT)

#define DS_BCD_LOW_BYTE_MASK         (0xF)
#define DS_BCD_LOW_BYTE_SHIFT        (0U)
#define DS_BCD_LOW_BYTE(x)           (((uint32_t)(((uint32_t)(x)) << DS_BCD_LOW_BYTE_SHIFT)) & DS_BCD_LOW_BYTE_MASK)
#define DS_BCD_LOW_BYTE_READ(x)      (((uint32_t)(((uint32_t)(x)) & DS_BCD_LOW_BYTE_MASK)) >> DS_BCD_LOW_BYTE_SHIFT)

static uint8_t rtc_apb_pending;
static uint8_t rtc_is_init = 0;
static rtc_handle_t rtc_irq_handler;

/*******************************************************************************
 * Function definition
 ******************************************************************************/

static void RTC_ConvertDatetimeToBCD(const rtc_datetime_t *datetime, rtc_datetime_bcd_t *datetime_bcd)
{
    /* Y:M:D */
    int high = (datetime->year - YEAR_RANGE_START) / 10;
    int low  = (datetime->year - YEAR_RANGE_START) % 10;
    uint32_t val = YH_BCD_HIGH_BYTE(high) | YH_BCD_LOW_BYTE(low);

    high = datetime->month / 10;
    low  = datetime->month % 10;
    val |= MM_BCD_HIGH_BYTE(high) | MM_BCD_LOW_BYTE(low);

    high = datetime->day / 10;
    low  = datetime->day % 10;
    val |= DS_BCD_HIGH_BYTE(high) | DS_BCD_LOW_BYTE(low);

    datetime_bcd->date = val;

    /* H:M:S */
    high = datetime->hour / 10;
    low  = datetime->hour % 10;
    val  = YH_BCD_HIGH_BYTE(high) | YH_BCD_LOW_BYTE(low);

    high = datetime->minute / 10;
    low  = datetime->minute % 10;
    val |= MM_BCD_HIGH_BYTE(high) | MM_BCD_LOW_BYTE(low);

    high = datetime->second / 10;
    low  = datetime->second % 10;
    val |= DS_BCD_HIGH_BYTE(high) | DS_BCD_LOW_BYTE(low);

    datetime_bcd->time = val;
}

static void RTC_ConvertBCDToDatetime(const rtc_datetime_bcd_t *datetime_bcd, rtc_datetime_t *datetime)
{
    uint32_t high = YH_BCD_HIGH_BYTE_READ(datetime_bcd->date);
    uint32_t low  = YH_BCD_LOW_BYTE_READ(datetime_bcd->date);
    datetime->year = high * 10 + low + YEAR_RANGE_START;

    high = MM_BCD_HIGH_BYTE_READ(datetime_bcd->date);
    low  = MM_BCD_LOW_BYTE_READ(datetime_bcd->date);
    datetime->month = high * 10 + low;

    high = DS_BCD_HIGH_BYTE_READ(datetime_bcd->date);
    low  = DS_BCD_LOW_BYTE_READ(datetime_bcd->date);
    datetime->day = high * 10 + low;

    high = YH_BCD_HIGH_BYTE_READ(datetime_bcd->time);
    low  = YH_BCD_LOW_BYTE_READ(datetime_bcd->time);
    datetime->hour = high * 10 + low;

    high = MM_BCD_HIGH_BYTE_READ(datetime_bcd->time);
    low  = MM_BCD_LOW_BYTE_READ(datetime_bcd->time);
    datetime->minute = high * 10 + low;

    high = DS_BCD_HIGH_BYTE_READ(datetime_bcd->time);
    low  = DS_BCD_LOW_BYTE_READ(datetime_bcd->time);
    datetime->second = high * 10 + low;
}

static bool RTC_CheckDatetimeFormat(const rtc_datetime_t *datetime)
{
    assert(datetime);

    /* Table of days in a month for a non leap year. First entry in the table is not used,
     * valid months start from 1
     */
    uint8_t daysPerMonth[] = {0U, 31U, 28U, 31U, 30U, 31U, 30U, 31U, 31U, 30U, 31U, 30U, 31U};

    /* Check year, month, hour, minute, seconds */
    if ((datetime->year <= YEAR_RANGE_START) || (datetime->year > YEAR_RANGE_END) || (datetime->month > 12U) ||
        (datetime->month < 1U) || (datetime->hour >= 24U) || (datetime->minute >= 60U) || (datetime->second >= 60U))
    {
        /* If not correct then error*/
        return false;
    }

    /* Adjust the days in February for a leap year */
    if ((((datetime->year & 3U) == 0) && (datetime->year % 100 != 0)) || (datetime->year % 400 == 0))
    {
        daysPerMonth[2] = 29U;
    }

    /* Check the validity of the day */
    if ((datetime->day > daysPerMonth[datetime->month]) || (datetime->day < 1U))
    {
        return false;
    }

    return true;
}

uint32_t RTC_ConvertDatetimeToSeconds(const rtc_datetime_t *datetime)
{
    assert(datetime);

    /* Number of days from begin of the non Leap-year*/
    uint16_t monthDays[] = {0U, 0U, 31U, 59U, 90U, 120U, 151U, 181U, 212U, 243U, 273U, 304U, 334U};
    uint32_t seconds;

    /* Compute number of days from 1970 till given year*/
    seconds = (datetime->year - 1970U) * DAYS_IN_A_YEAR;
    /* Add leap year days */
    seconds += ((datetime->year / 4) - (1970U / 4));
    /* Add number of days till given month*/
    seconds += monthDays[datetime->month];
    /* Add days in given month. We subtract the current day as it is
     * represented in the hours, minutes and seconds field*/
    seconds += (datetime->day - 1);
    /* For leap year if month less than or equal to Febraury, decrement day counter*/
    if ((!(datetime->year & 3U)) && (datetime->month <= 2U))
    {
        seconds--;
    }

    seconds = (seconds * SECONDS_IN_A_DAY) + (datetime->hour * SECONDS_IN_A_HOUR) +
              (datetime->minute * SECONDS_IN_A_MINUTE) + datetime->second;

    return seconds;
}

/*!
 * @name RTC Register indirect Access
 * @{
 */
static void RTC_APB_WAIT(void)
{
    while (*(volatile uint8_t *)&rtc_apb_pending)
    {
        EU_EVT_MaskWaitAndClr(1<<FC_SW_NOTIFY_EVENT);
    }
}

static uint32_t RTC_APB_REGRead(RTC_APB_Type *base, uint8_t addr)
{
    base->REQUEST = RTC_APB_REQUEST_ACCESS_ADDR(addr) | RTC_APB_REQUEST_ACCESS_RW(0);

    /* Set pending flag, which will be reset by the handler. */
    rtc_apb_pending = 1;

    /* Wait for INT_APB. */
    RTC_APB_WAIT();

    uint32_t val = base->DATA;

    /* Clear IRQ. */
    base->IRQ_FLAG = RTC_APB_IRQ_FLAG_READ(1);
    return val;
}

static void RTC_APB_REGWrite(RTC_APB_Type *base, uint8_t addr, uint32_t val)
{
    base->DATA = val;
    base->REQUEST = RTC_APB_REQUEST_ACCESS_ADDR(addr) | RTC_APB_REQUEST_ACCESS_RW(1);

    /* Set pending flag, which will be reset by the handler. */
    rtc_apb_pending = 1;

    /* Wait for INT_APB. */
    RTC_APB_WAIT();

    /* Clear IRQ. */
    base->IRQ_FLAG = RTC_APB_IRQ_FLAG_WRITE(1);
}
/*! @}*/

static void RTC_Enable(RTC_APB_Type *base)
{
    uint32_t val = RTC_APB_REGRead(base, RTC_CTRL_ADDR);
    val &= ~RTC_CR_STANDBY_MASK;
    RTC_APB_REGWrite(base, RTC_CTRL_ADDR, val);
}

static void RTC_Disable(RTC_APB_Type *base)
{
    uint32_t val = RTC_APB_REGRead(base, RTC_CTRL_ADDR);
    val |= RTC_CR_STANDBY(1);
    RTC_APB_REGWrite(base, RTC_CTRL_ADDR, val);
}

static void RTC_SetClockDiv(RTC_APB_Type *base, uint16_t div)
{
    /* Set CkInDiv1 to 0x80 => Clock = 32768/128 Hz, e.g 256 Hz, Period = 3.9 ms approx. */
    uint16_t val = RTC_CLKIN_DIV_VAL(div);
    RTC_APB_REGWrite(base, RTC_CLKIN_DIV_ADDR, val);

    /* Restart. */
    RTC_Disable(base);
    RTC_Enable(base);
}

void RTC_Init(RTC_APB_Type *base, const rtc_config_t *Config)
{
    assert(Config);

    SOC_EU_SetFCMask(RTC_APB_EVENT);
    SOC_EU_SetFCMask(RTC_EVENT);

    RTC_Reset(base);

    RTC_SetClockDiv(base, Config->clkDiv);

    rtc_is_init = 1;
}

void RTC_Deinit(RTC_APB_Type *base)
{
    if (rtc_is_init == 1)
    {
        RTC_Disable(base);
        SOC_EU_ClearFCMask(RTC_EVENT);
        SOC_EU_ClearFCMask(RTC_APB_EVENT);

        rtc_is_init = 0;
    }
}

void RTC_GetDefaultConfig(rtc_config_t *Config)
{
    assert(Config);
    Config->clkDiv  = 0x8000;
    Config->mode    = MODE_CALENDAR;
}

void RTC_Calibration(RTC_APB_Type *base)
{
    uint32_t val = RTC_APB_REGRead(base, RTC_CTRL_ADDR);
    val |= RTC_CR_CALIBRATION_EN(1);
    RTC_APB_REGWrite(base, RTC_CTRL_ADDR, val);

    RTC_ClearIRQFlags(RTC_APB, uRTC_CalibrationFlag);
}

uint32_t RTC_IsEnabled(RTC_APB_Type *base)
{
    if (rtc_is_init)
    {
        uint32_t val = RTC_APB_REGRead(base, RTC_CTRL_ADDR);
        return ((val & RTC_CR_STANDBY_MASK) == 0);
    }
    return 0;
}

void RTC_Reset(RTC_APB_Type *base)
{
    uint32_t val = RTC_APB_REGRead(base, RTC_CTRL_ADDR);
    val |= RTC_CR_SOFT_RST(1);
    RTC_APB_REGWrite(base, RTC_CTRL_ADDR, val);
}

status_t RTC_SetAlarm(RTC_APB_Type *base, const rtc_datetime_t *alarmTime, rtc_alarm_rpt_mode_t repeatmode)
{
    assert(alarmTime);

    rtc_datetime_t curr;
    rtc_datetime_bcd_t curr_bcd, alarmTime_bcd;

    /* Return error if the time provided is not valid. */
    if (!(RTC_CheckDatetimeFormat(alarmTime)))
    {
        return uStatus_InvalidArgument;
    }

    /* Get the current time. */
    RTC_GetCalendar(base, &curr);

    RTC_ConvertDatetimeToBCD(&curr, &curr_bcd);
    RTC_ConvertDatetimeToBCD(alarmTime, &alarmTime_bcd);

    if ((repeatmode < RPT_SEC) || (RPT_YEAR < repeatmode))
    {
        /* Return error if the alarm time has passed. */
        if(curr_bcd.date > alarmTime_bcd.date || ((curr_bcd.date == alarmTime_bcd.date) && (curr_bcd.time >= alarmTime_bcd.time)))
            return uStatus_InvalidArgument;
    }
    else
    {
        uint32_t val = RTC_APB_REGRead(base, RTC_ALARM_CTRL_ADDR);

        val &= ~RTC_ALARM_CTRL_MODE_MASK;
        val |=  RTC_ALARM_CTRL_MODE(1);

        val &= ~RTC_ALARM_CTRL_CONFIG_MASK;
        val |=  RTC_ALARM_CTRL_CONFIG(repeatmode);

        RTC_APB_REGWrite(base, RTC_ALARM_CTRL_ADDR, val);
    }
    /* Alarm IRQ enable. */
    RTC_EnableIRQ(base, uRTC_AlarmInterruptEnable);

    RTC_APB_REGWrite(base, RTC_ALARM_DATE_ADDR, alarmTime_bcd.date);
    RTC_APB_REGWrite(base, RTC_ALARM_TIME_ADDR, alarmTime_bcd.time);

    return uStatus_Success;
}

void RTC_GetAlarm(RTC_APB_Type *base, rtc_datetime_t *alarmTime)
{
    assert(alarmTime);

    rtc_datetime_bcd_t alarmTime_bcd;

    alarmTime_bcd.date = RTC_APB_REGRead(base, RTC_ALARM_DATE_ADDR);
    alarmTime_bcd.time = RTC_APB_REGRead(base, RTC_ALARM_TIME_ADDR);

    RTC_ConvertBCDToDatetime(&alarmTime_bcd, alarmTime);
}

void RTC_StartAlarm(RTC_APB_Type *base)
{
    uint32_t val = RTC_APB_REGRead(base, RTC_ALARM_CTRL_ADDR);

    val &= ~RTC_ALARM_CTRL_STANDBY_MASK;

    RTC_APB_REGWrite(base, RTC_ALARM_CTRL_ADDR, val);

    RTC_ClearIRQFlags(RTC_APB, uRTC_AlarmFlag);
}

void RTC_StopAlarm(RTC_APB_Type *base)
{
    uint32_t val = RTC_ALARM_CTRL_STANDBY(1);
    RTC_APB_REGWrite(base, RTC_ALARM_CTRL_ADDR, val);
}

status_t RTC_SetCalendar(RTC_APB_Type *base, const rtc_datetime_t *dateTime)
{
    assert(dateTime);

    /* Return error if the time provided is not valid. */
    if (!(RTC_CheckDatetimeFormat(dateTime)))
    {
        return uStatus_InvalidArgument;
    }

    rtc_datetime_bcd_t dateTime_bcd;

    RTC_ConvertDatetimeToBCD(dateTime, &dateTime_bcd);

    RTC_APB_REGWrite(base, RTC_CALENDAR_TIME_ADDR, dateTime_bcd.time);
    RTC_APB_REGWrite(base, RTC_CALENDAR_DATE_ADDR, dateTime_bcd.date);

    return uStatus_Success;
}

void RTC_GetCalendar(RTC_APB_Type *base, rtc_datetime_t *dateTime)
{
    assert(dateTime);

    rtc_datetime_bcd_t dateTime_bcd;

    dateTime_bcd.time = RTC_APB_REGRead(base, RTC_CALENDAR_TIME_ADDR);
    dateTime_bcd.date = RTC_APB_REGRead(base, RTC_CALENDAR_DATE_ADDR);

    RTC_ConvertBCDToDatetime(&dateTime_bcd, dateTime);
}

void RTC_StartCalendar(RTC_APB_Type *base)
{
    RTC_APB_REGWrite(base, RTC_CALENDAR_CTRL_ADDR, 0);
}

void RTC_StopCalendar(RTC_APB_Type *base)
{
    RTC_APB_REGWrite(base, RTC_CALENDAR_CTRL_ADDR, 1);
}

void RTC_SetTimer(RTC_APB_Type *base, const uint32_t count)
{
    /* Set timer inital value. */
    RTC_APB_REGWrite(base, RTC_TIMER_INIT_ADDR, count);

    /* Timer IRQ enable. */
    RTC_EnableIRQ(base, uRTC_TimerInterruptEnable);
}

uint32_t RTC_GetTimer(RTC_APB_Type *base)
{
    return RTC_APB_REGRead(base, RTC_TIMER_VALUE_ADDR);
}

void RTC_StartTimer(RTC_APB_Type *base, uint8_t repeat_en)
{
    uint32_t val = RTC_APB_REGRead(base, RTC_TIMER_CTRL_ADDR);
    val &= (~RTC_TIMER_STANDBY_MASK);
    val |= RTC_TIMER_STANDBY(0);

    val &= (~RTC_TIMER_MODE_MASK);
    val |= RTC_TIMER_MODE(repeat_en);

    RTC_APB_REGWrite(base, RTC_TIMER_CTRL_ADDR, val);

    RTC_ClearIRQFlags(RTC_APB, uRTC_TimerFlag);
}

void RTC_StopTimer(RTC_APB_Type *base)
{
    uint32_t val = RTC_APB_REGRead(base, RTC_TIMER_CTRL_ADDR);
    val &= (~RTC_TIMER_STANDBY_MASK);
    val |= RTC_TIMER_STANDBY(1);
    RTC_APB_REGWrite(base, RTC_TIMER_CTRL_ADDR, val);
}

uint32_t RTC_GetStatus(RTC_APB_Type *base)
{
    return RTC_APB_REGRead(base, RTC_STATUS_ADDR);
}

uint32_t RTC_GetIRQFlags(RTC_APB_Type *base)
{
    return RTC_APB_REGRead(base, RTC_IRQ_FLAG_ADDR);
}

void RTC_ClearIRQFlags(RTC_APB_Type *base, uint32_t mask)
{
    uint32_t val = RTC_APB_REGRead(base, RTC_IRQ_FLAG_ADDR);

    val |= mask;

    RTC_APB_REGWrite(base, RTC_IRQ_FLAG_ADDR, val);
}

uint32_t RTC_GetEnabledIRQ(RTC_APB_Type *base)
{
    return RTC_APB_REGRead(base, RTC_IRQ_MASK_ADDR);
}

void RTC_EnableIRQ(RTC_APB_Type *base, uint32_t mask)
{
    uint32_t val = RTC_APB_REGRead(base, RTC_IRQ_MASK_ADDR);
    val &= ~mask;

    RTC_APB_REGWrite(base, RTC_IRQ_MASK_ADDR, val);
}

void RTC_DisableIRQ(RTC_APB_Type *base, uint32_t mask)
{
    uint32_t val = RTC_APB_REGRead(base, RTC_IRQ_MASK_ADDR);
    val |= mask;

    RTC_APB_REGWrite(base, RTC_IRQ_MASK_ADDR, val);
}

void RTC_CreateHandler(rtc_callback_t callback, void *userData)
{
    rtc_irq_handler.callback = callback;
    rtc_irq_handler.userData = userData;
}

void RTC_IRQHandler(void)
{
    if (rtc_irq_handler.callback != NULL)
    {
        rtc_irq_handler.callback(rtc_irq_handler.userData);
    }
}

void RTC_APB_IRQHandler(void)
{
    rtc_apb_pending = 0;
}
