/* mbed Microcontroller Library
 * Copyright (c) 2006-2013 ARM Limited
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
#include "rtc_api.h"

#if DEVICE_RTC

#include "mbed_assert.h"
#include "pinmap.h"
#include "gap_rtc.h"
#include "PeripheralPins.h"
#include <time.h>

void rtc_init(void)
{
    rtc_config_t rtcConfig;

    RTC_GetDefaultConfig(&rtcConfig);

    /* Initial RTC 32KHz oscillator */
    RTC_Init(RTC_APB, &rtcConfig);

    RTC_StartCalendar(RTC_APB);
}

void rtc_free(void)
{
    RTC_Deinit(RTC_APB);
}

/*
 * Little check routine to see if the RTC has been enabled
 * 0 = Disabled, 1 = Enabled
 */
int rtc_isenabled(void)
{
    return RTC_IsEnabled(RTC_APB);
}

time_t rtc_read(void)
{
    rtc_datetime_t datetime;
    RTC_GetCalendar(RTC_APB, &datetime);

    return RTC_ConvertDatetimeToSeconds(&datetime);
}

void rtc_write(time_t t)
{
    if (t == 0) {
        t = 1;
    }

    rtc_datetime_t datetime;

    RTC_StopCalendar(RTC_APB);

    struct tm *temp = localtime(&t);

    datetime.year   = temp->tm_year + 1900;
    datetime.month  = temp->tm_mon + 1;
    datetime.day    = temp->tm_mday;
    datetime.hour   = temp->tm_hour;
    datetime.minute = temp->tm_min;
    datetime.second = temp->tm_sec;

    /* RTC only support 2001 - 2099, set 2001 if less than 2001 */
    if(datetime.year < 2001) {
        datetime.year = 2001;
    }

    RTC_SetCalendar(RTC_APB, &datetime);

    RTC_StartCalendar(RTC_APB);
}

#endif
