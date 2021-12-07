#ifndef __RT_API_PWM_H__
#define __RT_API_PWM_H__

enum {
    ADV_TIMER_EVENT0 = ARCHI_SOC_EVENT_ADV_TIMER0,
    ADV_TIMER_EVENT1,
    ADV_TIMER_EVENT2,
    ADV_TIMER_EVENT3,
};

#define PWM_EVT_OFFSET(timer,ch)         (4*timer+ch)

void gap_pwm_timer_channel_en(numPwmTimerT nTimer, PwmTimerLutChannelT channel);
void gap_pwm_timer_channel_all_en(numPwmTimerT nTimer);
void gap_pwm_timer_channel_disable(numPwmTimerT nTimer, PwmTimerLutChannelT channel);
void gap_pwm_timer_channel_all_disable(numPwmTimerT nTimer);

void gap_pwm_timer_conf(numPwmTimerT nTimer, unsigned int inputSource, unsigned int enableIn, unsigned int FllOrRTC, unsigned int IncThenDec, unsigned int PreScaler);
void gap_pwm_timer_channel_config(numPwmTimerT nTimer, PwmTimerLutChannelT channel, unsigned int thCh, unsigned int actCh);
void gap_pwm_timer_all_channel_config(numPwmTimerT nTimer, unsigned int thCh0, unsigned int actCh0,
                                                   unsigned int thCh1, unsigned int actCh1,
                                                   unsigned int thCh2, unsigned int actCh2,
                                                   unsigned int thCh3, unsigned int actCh3);
int gap_pwm_timer_alloc();
void gap_pwm_timer_free(numPwmTimerT nTimer);

void gap_pwm_timer_event_set (unsigned char nEvt, numPwmTimerT nTimer, PwmTimerLutChannelT channel);
void gap_pwm_timer_int_wait_and_clear(unsigned char nEvt);
void gap_pwm_timer_event_disable (unsigned char nEvt, numPwmTimerT nTimer, PwmTimerLutChannelT channel);
void gap_pwm_timer_event_all_disable();

#endif

