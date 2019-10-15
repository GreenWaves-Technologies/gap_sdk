#include "rt/rt_api.h"

#define CKIN_DIV_VAL    0x8000
#define DATE            0x00180423
#define TIME            0x00000000
#define ALARM_TIME      0x00000005
#define byteExtract(off, value)     ((value >> off) & 0xFF)
RT_FC_TINY_DATA rt_rtc_t *rtc;
RT_FC_TINY_DATA rt_rtc_conf_t *rtc_conf;
RT_FC_TINY_DATA unsigned int timeStart = 0;
RT_FC_TINY_DATA unsigned int timeStop = 0;
static int done = 0;


void dumpTime(rt_rtc_calendar_t time){
    unsigned int YY, MM, DD, hh, mm, ss;
    YY = __BITEXTRACT(time.date, 8, 16);
    MM = __BITEXTRACT(time.date, 8, 8);
    DD = __BITEXTRACT(time.date, 8, 0);
    hh = __BITEXTRACT(time.time, 8, 16);
    mm = __BITEXTRACT(time.time, 8, 8);
    ss = __BITEXTRACT(time.time, 8, 0);
    printf ("%x/%x/%x - %x:%x:%x\n", YY, MM, DD, hh, mm, ss);
}

void times_up(void *arg){
    timeStop = rt_time_get_us();
    rt_rtc_calendar_t timeNow;
    rt_rtc_t *rtc = (rt_rtc_t*)arg;
    rt_rtc_control(rtc, RTC_GET_TIME, &timeNow, NULL);
    printf ("TIMES UP!!! Now is:");
    dumpTime(timeNow);
    printf("start: %d us, stop: %d us, diff: %d us = %d s\n", timeStart, timeStop, (timeStop-timeStart),(timeStop-timeStart)/1000000 );
    done = 1;
}

void rtc_conf_init(rt_rtc_conf_t *rtc_conf){
    rtc_conf->mode = MODE_CALENDAR;
    rtc_conf->clkDivider = CKIN_DIV_VAL;
    rtc_conf->calendar.date = DATE;
    rtc_conf->calendar.time = TIME;
}

int main(){
    rt_rtc_calendar_t timeNow;
    printf("RTC test start\n");
    if (rt_event_alloc(NULL, 8)) return -1;
    rtc_conf = rt_alloc(RT_ALLOC_FC_DATA, sizeof(rt_rtc_conf_t));
    rtc_conf_init(rtc_conf);
    timeStart = rt_time_get_us();
    rtc = rt_rtc_open(rtc_conf, NULL);
    if (rtc == NULL) return -1;
    printf("RTC opened\n");
    rt_free(RT_ALLOC_FC_DATA, rtc_conf, sizeof(rt_rtc_conf_t));
    rt_rtc_control(rtc, RTC_CALENDAR_START, NULL, NULL);

    // Calendar and alarm test
    rt_rtc_control(rtc, RTC_GET_TIME, &timeNow, NULL);
    printf ("Calendar is set!!! Now is :");
    dumpTime(timeNow);
    timeNow.date = DATE;
    timeNow.time = ALARM_TIME;
    printf ("The alarm is set to: ");
    dumpTime(timeNow);

    rt_rtc_alarm_t alarm;
    alarm.time_date.date = DATE;
    alarm.time_date.time = ALARM_TIME;
    alarm.repeat_mode = 0;
    rt_rtc_control(rtc, RTC_ALARM_SET, &alarm, rt_event_get(NULL, times_up, rtc));
    rt_rtc_control(rtc, RTC_ALARM_START, NULL, NULL);

    while(!done)
        rt_event_execute(NULL, 1);

    rt_rtc_close(rtc, NULL);
    int timediff_s = (timeStop-timeStart)/1000000;
    if (timediff_s != (ALARM_TIME-TIME))
        return -1;

    printf ("Test success\n");
    return 0;
}
