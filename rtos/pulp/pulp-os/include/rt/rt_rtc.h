
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

#ifndef __RT_RTC_H__
#define __RT_RTC_H__

/**
* @ingroup groupDrivers
*/



/**
 * @defgroup RTC RTC
 *
 * The RTC (real time clock) driver provides support for controlling our RTC device.
 *
 */

/**
 * @addtogroup RTC
 * @{
 */

/**@{*/

/** \enum rt_rtc_mode_e
 * \brief Mode of rtc device.
 *
 * This is used to make RTC works in which mode.
 */
typedef enum{
  MODE_CALENDAR,   /*!< Calendar mode.*/
  MODE_ALARM,      /*!< Alarm mode, work with calendar mode.*/
  MODE_CNTDOWN,    /*!< Count down mode.*/
  MODE_CALIBR      /*!< RTC Calibration.*/
}rt_rtc_mode_e;


/** \enum rt_rtc_cmd_e
 * \brief All the commands of rtc.
 *
 * This is used to control the RTC. It should be used with the api rt_rtc_control.
 */
typedef enum{
  RTC_START,            /*!< Start the RTC.*/
  RTC_STOP,             /*!< Stop the RTC.*/
  RTC_RESET,            /*!< Reset the RTC.*/
  RTC_CLK_DIV_SET,      /*!< Set the clock divider of RTC, 32kHz by default.*/
  RTC_ALARM_SET,        /*!< Set the time of alarm.*/
  RTC_ALARM_START,      /*!< Start the alarm.*/
  RTC_ALARM_STOP,       /*!< Pause the alarm.*/
  RTC_CALENDAR_SET,     /*!< Set the time of calendar.*/
  RTC_CALENDAR_START,   /*!< Start the calendar.*/
  RTC_CALENDAR_STOP,    /*!< Disable the calendar.*/
  RTC_CNTDOWN_SET,      /*!< Initialize the value of countdown timer.*/
  RTC_CNTDOWN_START,    /*!< Start the count down function.*/
  RTC_CNTDOWN_STOP,     /*!< Stop the count down function.*/
  RTC_CNTDOWN_VALUE,    /*!< Return the actual value in countdown mode.*/
  RTC_CALIBRATION,      /*!< RTC Calibration.*/
  RTC_GET_TIME          /*!< Get the actual time.*/
}rt_rtc_cmd_e;

/** \enum rt_alarm_mode_e
 * \brief mode of alarm.
 *
 * This is used to set the alarm repeat mode.
 */
typedef enum{
  PER_SEC = 3,          /*!< Repeat per second.*/
  PER_MIN = 4,          /*!< Repeat per minute*/
  PER_HOUR = 5,         /*!< Repeat per hour*/
  PER_DAY = 6,          /*!< Repeat per day*/
  PER_MON = 7,          /*!< Repeat per month*/
  PER_YEAR = 8          /*!< Repeat per year*/
}rt_alarm_mode_e;

/** \struct rt_time_date_t
 * \brief time and date structure.
 *
 * This structure is used to pass the time (32bits) and the date (32 bits).
 */
typedef struct{
  unsigned int      time;   /*!< Time coded in Hex, for example: 0x00124808 = 12H:45M:08S */
  unsigned int      date;   /*!< Date coded in Hex, for example: 0x00171228 = 28/12/2017 */
}rt_time_date_t;

/** \struct rt_rtc_calendar_t
 * \brief calendar configuration structure.
 *
 * This structure is used to pass the time (32bits) and the date (32 bits) to calendar. defined by rt_time_date_t
 */
typedef rt_time_date_t rt_rtc_calendar_t;

/** \struct rt_rtc_alarm_t
 * \brief alarm configuration structure.
 *
 * This structure is used for setting the alarm.
 */
typedef struct{
  rt_time_date_t       time_date; /*!< For setting the alarm time and date */
  unsigned int         repeat_mode; /*!< For setting the alarm repeat mode*/
}rt_rtc_alarm_t;

/** \struct rt_rtc_cntDwn_t
 * \brief countdown timer configuration structure.
 *
 * This structure is used for setting the countdown timer.
 */
typedef struct{
  unsigned int      value;      /*!< For initializing the value of countdown timer */
  unsigned int      repeat_en;  /*!< Trigger the repeat mode: 0-No, 1-Yes */
}rt_rtc_cntDwn_t;

/** \struct rt_rtc_conf_t
 * \brief main rtc configuration structure.
 *
 * This structure is used for configuring the rtc.
 */
typedef struct{
  unsigned int      mode;           /*!< configure the RTC working mode */
  unsigned int      clkDivider;     /*!< set the clock divider of rtc*/
  rt_rtc_calendar_t calendar;       /*!< calendar configuration structure*/
  rt_rtc_alarm_t    alarm;          /*!< alarm configuration structure*/
  rt_rtc_cntDwn_t   cntDwn;         /*!< countdown configuration structure*/
}rt_rtc_conf_t;

/// @cond IMPLEM
typedef struct{
  rt_event_t        *alarm_event;
  rt_event_t        *countdown_event;
  rt_event_t        *calib_event;
  rt_rtc_conf_t     conf;
  unsigned char     open_count;
}rt_rtc_t;
/// @endcond



/** \brief Initialize an RTC configuration with default values.
 *
 * This function can be called to get default values for all parameters before setting some of them.
 * The structure containing the configuration must be kept alive until the RTC is opened.
 *
 * \param conf A pointer to the SPI master configuration.
 */
void rt_rtc_conf_init(rt_rtc_conf_t *conf);



/** \brief Open the RTC (real time clock).
 *
 * This function must be called before the RTC can be used. It will do all the needed configuration to make it
 * usable and also return a handle used to refer to this opened rtc when calling other functions.
 * This operation is asynchronous and its termination can be managed through an event.
 *
 * \param rtc_conf      A pointer to the rtc configuration.
 * \param event     The event used for managing termination.
 * \return          NULL if the device is not found, or a handle identifying the rtc and which can be used with other functions.
 */
rt_rtc_t* rt_rtc_open(rt_rtc_conf_t *rtc_conf, rt_event_t *event);


/** \brief Close the opened RTC device.
 *
 * This function can be called to close the opened rtc once it is not needed anymore, in order to free all allocated resources. Once this function is called, the device is not accessible anymore and must be opened again before being used.
 * This operation is asynchronous and its termination can be managed through an event.
 *
 * \param handle    The handler of the rtc which was returned when the device was opened.
 * \param event     The event used for managing termination.
 */
void rt_rtc_close(rt_rtc_t *handle, rt_event_t *event);


/** \brief Control the opened RTC device.
 *
 * This function is used to control and configure the RTC device.
 * \param handle    The handle of the device camera which returned when the device was opened.
 * \param rtc_cmd       The command for controlling or configuring the camera. Check the description of rt_rtc_cmd_e for further information.
 * \param *arg      A pointer to the arguments of the command.
 * \param event     The event used for managing termination.
 */
void rt_rtc_control( rt_rtc_t *handle, rt_rtc_cmd_e rtc_cmd, void *arg, rt_event_t *event );

//!@}

/**
 * @} end of RTC master
 */


/// @cond IMPLEM

typedef enum{
  INT_CALIBRATION = 0,
  INT_CNTDOWN,
  INT_ALARM
}rt_irq_rtc_e;

/// @endcond

#endif
