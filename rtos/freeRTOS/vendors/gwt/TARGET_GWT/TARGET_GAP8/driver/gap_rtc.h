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

#ifndef _GAP_RTC_H_
#define _GAP_RTC_H_

#include "gap_udma.h"

/*!
 * @addtogroup RTC
 * @{
 */

/*******************************************************************************
 * Variables, macros, structures,... definitions
 ******************************************************************************/

/*! @brief RTC Clock default frequency. */
#define RTC_CLK_FRE_DEFAUT (32768) /*!< Default frequency : 32768Hz. */

/*! @brief RTC IRQ enable. */
typedef enum _rtc_irq_enable
{
    uRTC_AlarmInterruptEnable       = RTC_IMR_ALARM_MASK,      /*!< Alarm interrupt. */
    uRTC_TimerInterruptEnable       = RTC_IMR_TIMER_MASK,      /*!< Timer interrupt. */
    uRTC_CalibrationInterruptEnable = RTC_IMR_CALIBRATION_MASK /*!< Calibration interrupt. */
} rtc_irq_enable_t;

/*! @brief RTC IRQ flags. */
typedef enum _rtc_irq_flags
{
    uRTC_AlarmFlag       = RTC_IFR_ALARM_MASK,      /*!< Alarm flag. */
    uRTC_TimerFlag       = RTC_IFR_TIMER_MASK,      /*!< Timer flag. */
    uRTC_CalibrationFlag = RTC_IFR_CALIBRATION_MASK /*!< Calibration flag. */
} rtc_irq_flags_t;

/*! @brief  RTC module status. */
enum _rtc_status
{
    uStatus_RTC_TimeSuccess  = MAKE_STATUS(uStatusGroup_RTC, 0), /*!< RTC success. */
    uStatus_RTC_TimeInvalid  = MAKE_STATUS(uStatusGroup_RTC, 1), /*!< RTC time invalid. */
    uStatus_RTC_TimeOverflow = MAKE_STATUS(uStatusGroup_RTC, 2), /*!< RTC time overflow. */
    uStatus_RTC_AlarmPassed  = MAKE_STATUS(uStatusGroup_RTC, 3)  /*!< RTC time alarm passed. */
};

/*! @brief RTC module modes. */
typedef enum
{
    MODE_CALENDAR = 0, /*!< Calendar mode. */
    MODE_ALARM,        /*!< Alarm mode, work with calendar mode. */
    MODE_TIMER,        /*!< Timer mode. */
    MODE_CALIBR        /*!< RTC Calibration. */
} rtc_mode_t;

/*! @brief RTC alarm repeat mode. */
typedef enum
{
    RPT_SEC  = 3,  /*!< Repeat every second. */
    RPT_MIN  = 4,  /*!< Repeat every minute. */
    RPT_HOUR = 5,  /*!< Repeat every hour. */
    RPT_DAY  = 6,  /*!< Repeat every day. */
    RPT_MON  = 7,  /*!< Repeat every month. */
    RPT_YEAR = 8   /*!< Repeat every year. */
} rtc_alarm_rpt_mode_t;

/*! @brief Structure used to hold date and time, using BCD format. */
typedef struct _rtc_datetime_bcd
{
    uint32_t time;  /*!< Time coded in Bcd, for example: 0x00124508 = 12H:45M:08S. */
    uint32_t date;  /*!< Date coded in Bcd, for example: 0x00171228 = 2017/12/28. */
} rtc_datetime_bcd_t;

/*! @brief Structure used to hold date and time, in separate fields. */
typedef struct _rtc_datetime
{
    uint16_t year;   /*!< Range from 2001 to 2099. */
    uint8_t  month;  /*!< Range from 1 to 12. */
    uint8_t  day;    /*!< Range from 1 to 31(depending on month). */
    uint8_t  hour;   /*!< Range from 0 to 23. */
    uint8_t  minute; /*!< Range from 0 to 59. */
    uint8_t  second; /*!< Range from 0 to 59. */
} rtc_datetime_t;

/*! @brief RTC configuration structure. */
typedef struct _rtc_config
{
    rtc_mode_t mode;    /*!< RTC module working mode. */
    uint32_t   clkDiv;  /*!< Baud rate configuration of RTC peripheral. */
} rtc_config_t;

/*!
 * @brief Completion callback function pointer type.
 *
 * When an asynchronous is completed, the handler calls this callback function.
 *
 * @param userData  Parameter passed to the callback function by the user.
 */
typedef void (*rtc_callback_t)(void *useData);

/*!
 * @brief RTC handler structure.
 *
 * This structure holds information to handle IRQ.
 */
typedef struct _rtc_handle
{
    rtc_callback_t  callback;   /*!< A callback function called when the transfer is finished. */
    void           *userData;   /*!< A callback parameter passed to the callback function. */
} rtc_handle_t;

/*******************************************************************************
 * APIs
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/*!
 * @name RTC module configuration.
 * @{
 */

/*!
 * @brief Convert datetime to second
 *
 * This function returns the number of seconds for a the given date and time.
 *
 * @param datetime       Pointer to rtc_datetime_t structure.
 *
 * @return Number of seconds.
 */
uint32_t RTC_ConvertDatetimeToSeconds(const rtc_datetime_t *datetime);

/*!
 * @brief Initialize the RTC module.
 *
 * This function initializes the RTC module.
 * This is an example use case.
 *  @code
 *   rtc_config_t  Config;
 *   Config.mode = 100000;
 *   Config.clkDiv = 100000;
 *   RTC_Init(base, &Config);
 *  @endcode
 *
 * @param base           RTC base pointer.
 * @param Config         Pointer to rtc_config_t structure.
 */
void RTC_Init(RTC_APB_Type *base, const rtc_config_t *Config);

/*!
 * @brief De-initialize the RTC module.
 *
 * A call to this function gates the RTC clock.
 *
 * @param base           RTC base pointer.
 */
void RTC_Deinit(RTC_APB_Type *base);

/*!
 * @brief Get the default settings of the RTC module.
 *
 * The default values are as follows.
 * @code
 *    Config->mode   = MODE_CALENDAR;
 *    Config->clkDiv = 2;
 * @endcode
 *
 * @param Config         Pointer to rtc_config_t structure.
 */
void RTC_GetDefaultConfig(rtc_config_t *Config);

/*!
 * @brief RTC calibration.
 *
 * @param base           RTC base pointer.
 */
void RTC_Calibration(RTC_APB_Type *base);

/*!
 * @brief Check if RTC is enabled.
 *
 * @param base           RTC base pointer.
 *
 * @return 1 if the RTC module is enabled, 0 otherwise.
 */
uint32_t RTC_IsEnabled(RTC_APB_Type *base);

/*!
 * @brief Software reset of the RTC module.
 *
 * This function resets all RTC registers except for the SWR bit, and the RTC_WAR and RTC_RAR
 * registers. The SWR bit is cleared by software explicitly.
 *
 * @param base           RTC base pointer.
 */
void RTC_Reset(RTC_APB_Type *base);

/* @} */

/*!
 * @name Alarm, Calendar and Timer(count down) operations.
 * @{
 */

/*!
 * @brief Set the RTC alarm.
 *
 * The function sets the RTC alarm according to date and time provided
 * in rtc_datetime_t structure.
 * Checks are performed to verify the format of the given date and time and its validity.
 * The IRQ corresponding to the alarm is enabled.
 *
 * The last parameter allows user to define if the alarm should be reloaded or not.
 * It should be one of the rtc_alarm_rpt_mode_t value.
 *
 * @note
 *       - When the repeat mode is disabled, all fields of the alarm date and time need to be set.
 *       - When the repeat mode is enabled, only the required fields need to be set.
 *
 * @param base           RTC base pointer.
 * @param alarmTime      Pointer to rtc_datetime_t structure.
 * @param repeatmode     Repeat mode.
 *
 * @retval uStatus_RTC_TimeSuccess Successfully set the RTC alarm.
 * @retval uStatus_RTC_TimeInvalid Error : the alarm datetime format is incorrect.
 * @retval uStatus_RTC_AlarmPassed Error : the alarm time has already passed.
 */
status_t RTC_SetAlarm(RTC_APB_Type *base, const rtc_datetime_t *alarmTime, rtc_alarm_rpt_mode_t repeatmode);

/*!
 * @brief Get the RTC alarm time.
 *
 * This function returns RTC alarm date and time, stored in rtc_datetime_t structure.
 *
 * @param base           RTC base pointer.
 * @param alarmTime      Pointer to rtc_datetime_t structure.
 */
void RTC_GetAlarm(RTC_APB_Type *base, rtc_datetime_t *alarmTime);

/*!
 * @brief Start the RTC alarm.
 *
 * This function starts the RTC alarm.
 * The alarm can also be repeated, see rtc_alarm_rpt_mode_t, by setting the second parameter.
 * This value should be equal to one of the rtc_alarm_rpt_mode_t value.
 *
 * @param base           RTC base pointer.
 */
void RTC_StartAlarm(RTC_APB_Type *base);

/*!
 * @brief Stop the RTC alarm.
 *
 * @param base           RTC base pointer.
 */
void RTC_StopAlarm(RTC_APB_Type *base);

/*!
 * @brief Set the RTC clock.
 *
 * The function sets the RTC clock's date and time according to date and time provided
 * in rtc_datetime_t structure.
 * A check is performed to verify the format of the given date and time.
 *
 * @note The RTC counter must be stopped prior to calling this function
 *       because writes to the RTC seconds register fail if the RTC counter is running.
 *
 * @param base           RTC base pointer.
 * @param dateTime       Pointer to rtc_datetime_t structure.
 *
 * @retval uStatus_RTC_TimeSuccess Successfully set the RTC alarm.
 * @retval uStatus_RTC_TimeInvalid Error : the datetime format is incorrect.
 */
status_t RTC_SetCalendar(RTC_APB_Type *base, const rtc_datetime_t *dateTime);

/*!
 * @brief Get the RTC time.
 *
 * This function returns RTC clock's current date and time, stored in rtc_datetime_t structure.
 *
 * @param base           RTC base pointer.
 * @param dateTime       Pointer to rtc_datetime_t structure.
 */
void RTC_GetCalendar(RTC_APB_Type *base, rtc_datetime_t *dateTime);

/*!
 * @brief Start the RTC clock.
 *
 * @param base           RTC base pointer.
 */
void RTC_StartCalendar(RTC_APB_Type *base);

/*!
 * @brief Stop the RTC clock.
 *
 * @param base           RTC base pointer.
 */
void RTC_StopCalendar(RTC_APB_Type *base);

/*!
 * @brief Set the RTC timer.
 *
 * This function sets the timer of the RTC module. It is the count down function.
 * Once the timer reached zero, an IRQ is sent by the RTC module.
 * The IRQ corresponding to the timer is enabled.
 *
 * @param base           RTC base pointer.
 * @param count          Count down value.
 */
void RTC_SetTimer(RTC_APB_Type *base, const uint32_t count);

/*!
 * @brief Get the RTC timer.
 *
 * This function returns current value of the RTC timer.
 *
 * @param base           RTC base pointer.
 *
 * @return Counter value
 */
uint32_t RTC_GetTimer(RTC_APB_Type *base);

/*!
 * @brief Start the RTC timer.
 *
 * This function starts the RTC timer. The timer can be set to repeat the count down
 * if the second parameter is set.
 *
 * @param base           RTC base pointer.
 * @param repeat_en      RTC timer repeat enable.
 */
void RTC_StartTimer(RTC_APB_Type *base, uint8_t repeat_en);

/*!
 * @brief Stop the RTC timer.
 *
 * @param base           RTC base pointer.
 */
void RTC_StopTimer(RTC_APB_Type *base);

/*! @}*/

/*!
 * @name IRQ Handler.
 * @{
 */

/*!
 * @brief Get RTC Status.
 *
 * This function returns the content of the internal RTC Status Register(SR).
 *
 * @param base           RTC base pointer.
 *
 * @return The content of RTC SR.
 */
uint32_t RTC_GetStatus(RTC_APB_Type *base);

/*!
 * @brief Get the RTC IRQ flags.
 *
 * This function returns the content of the internal RTC Interruption Flag Register(IFR).
 * This register indicates active events(IRQ) : an event has occured if a bit is set to '1'.
 *
 * @param base           RTC base pointer.
 *
 * @return The content of RTC IFR.
 */
uint32_t RTC_GetIRQFlags(RTC_APB_Type *base);

/*!
 * @brief Clear the RTC IRQ flags.
 *
 * This function resets the IRQ flags, in RTC IFR, corresponding to the given mask.
 *
 * @param base           RTC base pointer.
 * @param mask           Mask of the IRQ to clear. This is a logical OR of rtc_irq_flags_t members.
 */
void RTC_ClearIRQFlags(RTC_APB_Type *base, uint32_t mask);

/*!
 * @brief Get the enabled RTC IRQ.
 *
 * This functions returns the content of the internal RTC Interruption Mask Regsiter(IMR).
 * This register indicates masked and non masked IRQ.
 *
 * @param base           RTC base pointer.
 *
 * @return Enabled IRQ of the RTC module.
 */
uint32_t RTC_GetEnabledIRQ(RTC_APB_Type *base);

/*!
 * @brief Enable RTC IRQ.
 *
 * This function clears the RTC IMR to enable IRQ generation for the given IRQ mask.
 *
 *@note For a given mode, if its bit is set in RTC IMR, when an event from this mode occurs,
 *      an IRQ signal will be generated and the corresponding bit will be set in RTC IFR.
 *      If it is not set, the RTC IFR will be set but an IRQ will not be generated.
 *
 * @param base           RTC base pointer.
 * @param mask           Mask of the IRQ to enable. This is a logical OR of rtc_irq_enable_t members.
 */
void RTC_EnableIRQ(RTC_APB_Type *base, uint32_t mask);

/*!
 * @brief Disable RTC IRQ.
 *
 * This function sets the RTC Interruption Mask Regsiter(IMR) to disable IRQ generation for the given IRQ mask.
 *
 * @param base           RTC base pointer.
 * @param mask           Mask of the IRQ to disable. This is a logical OR of rtc_irq_enable_t members.
 */
void RTC_DisableIRQ(RTC_APB_Type *base, uint32_t mask);

/*!
 * @brief Create RTC IRQ handler.
 *
 * This function allows the user to add a callback function, which will be called
 * when an IRQ from RTC occurs.
 *
 * @param callback       Callback function.
 * @param userData       Parameter passed to the callback function.
 */
void RTC_CreateHandler(rtc_callback_t callback, void *userData);

/*!
 * @brief RTC IRQ handler.
 *
 * This function is called to handle IRQ coming from the RTC module.
 */
void RTC_IRQHandler(void);

/*!
 * @brief RTC APB IRQ handler.
 *
 * This function is called to handle IRQ coming from APB, due to indirect access to some RTC module's registers.
 */
void RTC_APB_IRQHandler(void);

/*! @}*/

#if defined(__cplusplus)
}
#endif /* __cplusplus */

/* @} */

#endif /*_GAP_RTC_H_*/
