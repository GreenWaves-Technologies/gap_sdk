/*
 * Copyright (C) 2019 GreenWaves Technologies
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

#ifndef __PI_DRIVERS_RTC_H__
#define __PI_DRIVERS_RTC_H__

#include "pmsis/pmsis_types.h"
#include <time.h>

/**
 * @ingroup groupDrivers
 */

/**
 * @defgroup RTC RTC
 */

/**
 * @addtogroup RTC
 * @{
 */

/**
 * \enum pi_rtc_mode_e
 * \brief RTC working mode
 */
typedef enum
{
    PI_RTC_MODE_CALENDAR    = 0x1, /*!< RTC calendar mode. */
    PI_RTC_MODE_ALARM       = 0x2, /*!< RTC alarm mode. */
    PI_RTC_MODE_TIMER       = 0x4, /*!< RTC countdown mode. */
    PI_RTC_MODE_CALIBRATION = 0x8  /*!< RTC calibration mode. */
} pi_rtc_mode_e;

/**
 * \enum pi_rtc_alarm_repeat_e
 * \brief RTC alarm repeat mode
 */
typedef enum
{
    PI_RTC_ALARM_RPT_NONE = 0x0, /*!< Alarm not repeated. */
    PI_RTC_ALARM_RPT_SEC  = 0x3, /*!< Alarm repeated every seconds. */
    PI_RTC_ALARM_RPT_MIN  = 0x4, /*!< Alarm repeated every minutes. */
    PI_RTC_ALARM_RPT_HOUR = 0x5, /*!< Alarm repeated every hours. */
    PI_RTC_ALARM_RPT_DAY  = 0x6, /*!< Alarm repeated every days. */
    PI_RTC_ALARM_RPT_MON  = 0x7, /*!< Alarm repeated every months. */
    PI_RTC_ALARM_RPT_YEAR = 0x8  /*!< Alarm repeated every years. */
} pi_rtc_alarm_repeat_e;

/**
 * \struct pi_rtc_conf
 * \brief RTC configuration structure.
 */
struct pi_rtc_conf
{
    uint8_t rtc_id;             /*!< RTC device ID. */
    pi_rtc_mode_e mode;         /*!< RTC mode. */
    struct tm time;             /*!< Current time. */
    struct tm alarm;            /*!< Alarm to set. */
    uint32_t counter;           /*!< Counter initial value. */
};

/**
 * \enum pi_rtc_ioctl_cmd_e
 * \brief RTC ioctl commands
 */
typedef enum
{
    PI_RTC_CALENDAR_START = 0,  /*!< Start RTC device. */
    PI_RTC_CALENDAR_STOP  = 1,  /*!< Stop RTC device. */
    PI_RTC_ALARM_START    = 2,  /*!< Start alarm function. */
    PI_RTC_ALARM_STOP     = 3,  /*!< Stop alarm function. */
    PI_RTC_TIMER_START    = 4,  /*!< Start countdown. */
    PI_RTC_TIMER_STOP     = 5   /*!< Stop countdown. */
} pi_rtc_ioctl_cmd_e;

/**
 * \brief Initialize a RTC configuration structure.
 *
 * This function initializes a RTC configuration structure with default values.
 *
 * \param conf           RTC configuration structure.
 */
void pi_rtc_conf_init(struct pi_rtc_conf *conf);

/**
 * \brief Open a RTC device.
 *
 * This function opens a RTC device.
 *
 * \param device         Device structure.
 *
 * \retval 0             If operation is successful.
 * \retval ERRNO         An error code otherwise.
 */
int pi_rtc_open(struct pi_device *device);

/**
 * \brief Close a RTC device.
 *
 * This function closes a RTC device.
 *
 * \param device         Device structure.
 */
void pi_rtc_close(struct pi_device *device);

/**
 * \brief Set date and time.
 *
 * This function is used to set current date and current time.
 *
 * \param device         Device structure.
 * \param time           Current date and time to set.
 *
 * \retval 0             If operation is successful.
 * \retval ERRNO         An error code otherwise.
 *
 * \note Standard is not used to set the struct tm here.\n
 *       Ex: 20/12/2019 15:20:45\n
 *       time->tm_year = 2019;\n
 *       time->tm_mon  = 12;\n
 *       time->tm_date = 20;\n
 *       time->tm_hour = 15;\n
 *       time->tm_min  = 20;\n
 *       time->tm_sec  = 45;\n
 *       If this struct needs to be used with standarg functions,
 *       copy this struct into a new one, adjust members according to standards.
 */
int pi_rtc_datetime_set(struct pi_device *device, struct tm *time);

/**
 * \brief Get current date ans time.
 *
 * This function retrieves current date and time.
 *
 * \param device         Device structure.
 * \param time           Current date and time to store.
 *
 * \retval 0             If operation is successful.
 * \retval ERRNO         An error code otherwise.
 */
int pi_rtc_datetime_get(struct pi_device *device, struct tm *time);

/**
 * \brief Set alaram.
 *
 * This function is used to set alarm. The alarm can be one shot or rearmed.
 * An IRQ is triggered when the alarm date and time are reached.
 *
 * \param device         Device structure.
 * \param alarm          Date and time to set alarm.
 *
 * \retval 0             If operation is successful.
 * \retval ERRNO         An error code otherwise.
 */
int pi_rtc_alarm_set(struct pi_device *device, struct tm *alarm);

/**
 * \brief Get current alaram.
 *
 * This function retrieves alarm date and time, if set.
 *
 * \param device         Device structure.
 * \param alarm          Curremt alarm date and time to store.
 *
 * \retval 0             If operation is successful.
 * \retval ERRNO         An error code otherwise.
 */
int pi_rtc_alarm_get(struct pi_device *device, struct tm *alarm);

/**
 * \brief Set counter.
 *
 * This function is used to set counter.
 * The timer counts down from the given till 0, and triggers an IRQ.
 *
 * \param device         Device structure.
 * \param countdown      Countdown value.
 *
 * \retval 0             If operation is successful.
 * \retval ERRNO         An error code otherwise.
 */
int pi_rtc_timer_set(struct pi_device *device, uint32_t countdown);

/**
 * \brief Get current counter value.
 *
 * This function returns current timer value.
 *
 * \param device         Device structure.
 *
 * \return Value         Current timer value..
 */
uint32_t pi_rtc_timer_get(struct pi_device *device);

/**
 * \brief Ioctl command.
 *
 * This function allows to send different commands to RTC device.
 * The commands are listed above in pi_rtc_ioctl_cmd_e.
 *
 * \param device         Device structure.
 * \param cmd            Ioctl command.
 * \param arg            Ioctl command args.
 *
 * \retval -1            If wrong ioctl command.
 * \retval Value         Otherwise return value depending on ioctl command.
 */
int pi_rtc_ioctl(struct pi_device *device, uint32_t cmd, void *arg);

/**
 * @} end of RTC
 */

#endif  /* __PI_DRIVERS_RTC_H__ */
