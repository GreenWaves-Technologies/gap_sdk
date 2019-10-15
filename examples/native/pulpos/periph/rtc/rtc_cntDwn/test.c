#include "rt/rt_api.h"

#define DATE            0x00180423
#define TIME            0x00000000
#define CKIN_DIV_VAL    0x8000
#define COUNT           10
#define byteExtract(off, value)     ((value >> off) & 0xFF)
RT_FC_TINY_DATA rt_rtc_t *rtc;
RT_FC_TINY_DATA rt_rtc_conf_t *rtc_conf;
RT_FC_TINY_DATA unsigned int timeStart = 0;
RT_FC_TINY_DATA unsigned int timeStop = 0;
static int done = 0;

void times_up(void *arg){
    timeStop = rt_time_get_us();
    printf ("Count Down Finished!\n");
    printf("start: %d us, stop: %d us, diff: %d us\n", timeStart, timeStop, (timeStop-timeStart));
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
    rtc = rt_rtc_open(rtc_conf, NULL);
    if (rtc == NULL) return -1;
    printf("RTC opened\n");
    rt_free(RT_ALLOC_FC_DATA, rtc_conf, sizeof(rt_rtc_conf_t));

    rt_rtc_cntDwn_t cntDwn;
    cntDwn.value = COUNT;
    cntDwn.repeat_en = 0;
    rt_rtc_control(rtc, RTC_CNTDOWN_SET, &cntDwn, rt_event_get(NULL, times_up, 0));
    rt_rtc_control(rtc, RTC_CNTDOWN_START, NULL, NULL);
    timeStart = rt_time_get_us();
    printf("Count down start, count for %d(s)\n", COUNT);

    while(!done)
        rt_event_execute(NULL, 1);

    rt_rtc_close(rtc, NULL);

    int timediff_s = (timeStop-timeStart)/1000000;
    if (timediff_s != (COUNT))
        return -1;

    printf ("Test success\n");

    return 0;
}
