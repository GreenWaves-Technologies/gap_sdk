/*
      PWM driver: configurations of 4 PWM Timer, 16 channels
      January 2016.
      Author: Yao ZHANG, GreenWaves Technologies. yao.zhang@greenwaves-technologies.com
*/

#include "rt/rt_api.h"

RT_FC_TINY_DATA unsigned int pwmEventsStatus;
static unsigned char pwmTimerAlloc = 0;   // TODO: to identify whether the timer is allocted

// TODO: this macro should be removed after Eric committed.
//#define gap8_bitinsert_r(dst, src, size, off)   __builtin_pulp_binsert_r((dst), (src), SO((size), (off)))

/*
 * Enable the specific timer
 */
void gap_pwm_timer_en(numPwmTimerT nTimer){
    unsigned int ch_en_val = gap_pwm_timer_status();
    ch_en_val |= PWM_TIMER_EN(nTimer);
    gap_pwm_timer_en_reg(ch_en_val);
}

/*
 * Enable all the timers
 */
void gap_pwm_timer_all_en(){
    gap_pwm_timer_en_reg(PWM_TIMER_EN_ALL);
}

/*
 * Disable the specific timer
 */
void gap_pwm_timer_disable(numPwmTimerT nTimer){
    unsigned int ch_en_val = gap_pwm_timer_status();
    ch_en_val &= PWM_TIMER_DISABLE(nTimer);
    gap_pwm_timer_en_reg(ch_en_val);
}

/*
 * Enable all the timers
 */
void gap_pwm_timer_all_disable(){
    gap_pwm_timer_en_reg(PWM_TIMER_DISABLE_ALL);
}

/*
 * PWM timer configuration
 * Arguments:
 *      - nTimer: number of timer
 *      - inputSource: Select counting source
 *                      0-31 gpio
 *                      32-35 out of timer 0
 *                      ...
 *                      44-47 out of timer 3
 *      - enableIn: count rules
 *                      ADV_TIMER_INMODE_AO         0x0 //event at each clock cycle
 *                      ADV_TIMER_INMODE_IN0        0x1 //event if input signal is 0
 *                      ADV_TIMER_INMODE_IN1        0x2 //event if input signal is 1
 *                      ADV_TIMER_INMODE_RISE       0x3 //event on input signal rising edge
 *                      ADV_TIMER_INMODE_FALL       0x4 //event on input signal falling edge
 *                      ADV_TIMER_INMODE_EDGE       0x5 //event on input signal rising or falling edge
 *                      ADV_TIMER_INMODE_ARM_RISE   0x6 //event on input signal rising edge when armed
 *                      ADV_TIMER_INMODE_ARM_FALL   0x7 //event on input signal falling edge when armed
 *      - FllorRTC:
 *                      ADV_TIMER_USEREF            0x1
 *                      ADV_TIMER_USESIG            0x0
 *      - IncThenDec: action when reaching the TH:
 *                      ADV_TIMER_SAW:     /|/|/| (SAW)
 *                      ADV_TIMER_NOSAW:   /\/\/\ (NOSAW)
 *      - PreScaler
 */
void gap_pwm_timer_conf(numPwmTimerT nTimer, unsigned int inputSource, unsigned int enableIn, unsigned int FllOrRTC, unsigned int IncThenDec, unsigned int PreScaler){
    gap_pwm_timer_t timer;
    timer.timerConf.SelectInputSource = inputSource;
    timer.timerConf.InputEnableIn = enableIn;
    timer.timerConf.FllOrRTC = FllOrRTC;
    timer.timerConf.IncThenDec = IncThenDec;
    timer.timerConf.PreScaler = PreScaler;
    gap_pwm_timer_config(nTimer, timer.Raw);              // Write the configs, set start bit
}

/*
 *  Configuration the 1 of 16 PWM channels:
 *  Arguments:
 *      - nTimer:   number of timer
 *      - channel:  number of channel
 *      - thCh:     threshold
 *      - actCh:    action of channel
 *                    ADV_TIMER_OP_SET    0       //   	000: Set
 *                    ADV_TIMER_OP_TOGRST 1       //   	001: Toggle then next is Clear
 *                    ADV_TIMER_OP_SETRST 2       //   	010: Set then Clear
 *                    ADV_TIMER_OP_TOG    3       //   	011: Toggle
 *                    ADV_TIMER_OP_RST    4       //   	100: Clear
 *                    ADV_TIMER_OP_TOGSET 5       //   	101: Toggle then next is Set
 *                    ADV_TIMER_OP_RSTSET 6       //   	110: Clear then Set
 *
 */
void gap_pwm_timer_channel_config(numPwmTimerT nTimer, PwmTimerLutChannelT channel, unsigned int thCh, unsigned int actCh){
    gap_pwm_timer_t timer;
    timer.ch_ThConfig.chThreshold = thCh;
    timer.ch_ThConfig.chAction = actCh;
    gap_pwm_timer_th_channel_set(nTimer, channel, timer.Raw);
}

/*
 *  Configuration of all 4 channels of 1 timer:
 *  Arguments:
 *      - nTimer:   number of timer
 *      - thCh(x):  threshold
 *      - actCh(x): action of channel
 *                    ADV_TIMER_OP_SET    0       //   	000: Set
 *                    ADV_TIMER_OP_TOGRST 1       //   	001: Toggle then next is Clear
 *                    ADV_TIMER_OP_SETRST 2       //   	010: Set then Clear
 *                    ADV_TIMER_OP_TOG    3       //   	011: Toggle
 *                    ADV_TIMER_OP_RST    4       //   	100: Clear
 *                    ADV_TIMER_OP_TOGSET 5       //   	101: Toggle then next is Set
 *                    ADV_TIMER_OP_RSTSET 6       //   	110: Clear then Set
 *
 */
void gap_pwm_timer_all_channel_config(numPwmTimerT nTimer, unsigned int thCh0, unsigned int actCh0,
                                                   unsigned int thCh1, unsigned int actCh1,
                                                   unsigned int thCh2, unsigned int actCh2,
                                                   unsigned int thCh3, unsigned int actCh3){

    gap_pwm_timer_channel_config(nTimer, PWM_TIMER_TH_CHANNEL0, thCh0, actCh0);
    gap_pwm_timer_channel_config(nTimer, PWM_TIMER_TH_CHANNEL1, thCh1, actCh1);
    gap_pwm_timer_channel_config(nTimer, PWM_TIMER_TH_CHANNEL2, thCh2, actCh2);
    gap_pwm_timer_channel_config(nTimer, PWM_TIMER_TH_CHANNEL3, thCh3, actCh3);
}

/*
 *  Allocation a pwm timer
 *  return: the number of timer
 */
int gap_pwm_timer_alloc(){
    numPwmTimerT nTimer;
    if (pwmTimerAlloc == 0xF) return -1;
    nTimer = __builtin_fl1((~pwmTimerAlloc)&0xF);
    pwmTimerAlloc |= (0x1 << nTimer);
    nTimer = 3-nTimer;
    gap_pwm_timer_en(nTimer);
    return (nTimer);
}

/*
 *  Free a pwm timer
 *  Argument: the number of timer
 */
void gap_pwm_timer_free(numPwmTimerT nTimer){
    gap_pwm_timer_cmd(nTimer, PWM_TIMER_CMD_STOP);
    pwmTimerAlloc &= (~(0x1 << (3-nTimer)));
    gap_pwm_timer_disable(nTimer);
}

/*
 *  Set 1 of 4 events for a pwm timer channel
 *  Argument:
 *          nEvt:   the number of event
 *          nTimer: the number of timer
 *          channel: the number of channel
 */
void gap_pwm_timer_event_set (unsigned char nEvt, numPwmTimerT nTimer, PwmTimerLutChannelT channel){
    unsigned int Val = soc_eu_eventMask_get(SOC_FC_MASK_MSB);
    gap_pwm_timer_t timer;
    timer.Raw = gap_pwm_timer_evt_reg_read();
    timer.timerEvt.evt_en |= (0x1 << nEvt);
    switch (nEvt){
        case pwm_timer_event0:
            Val = Val & ~(1<<(ADV_TIMER_EVENT0-32));
            timer.timerEvt.evt0_sel = (nTimer*4 + channel);
            break;
        case pwm_timer_event1:
            Val = Val & ~(1<<(ADV_TIMER_EVENT1-32));
            timer.timerEvt.evt1_sel = (nTimer*4 + channel);
            break;
        case pwm_timer_event2:
            Val = Val & ~(1<<(ADV_TIMER_EVENT2-32));
            timer.timerEvt.evt2_sel = (nTimer*4 + channel);
            break;
        case pwm_timer_event3:
            Val = Val & ~(1<<(ADV_TIMER_EVENT3-32));
            timer.timerEvt.evt3_sel = (nTimer*4 + channel);
            break;
    }
    soc_eu_eventMask_set(SOC_FC_MASK_MSB, Val);
    gap_pwm_timer_evt_reg_cfg(timer.Raw);
}

/*
 *  Wait and clear 1 event
 *  Argument:
 *          nEvt:   the number of event
 */
void gap_pwm_timer_int_wait_and_clear(unsigned char nEvt){
    volatile unsigned int * Evt = &pwmEventsStatus;
    while ((*Evt&(0x1<<nEvt)) ==  0) eu_evt_maskWaitAndClr(1<<PLP_RT_NOTIF_EVENT);
    pwmEventsStatus = __builtin_bitinsert_r(pwmEventsStatus, 0, 1, nEvt);
}

/*
 *  Disable 1 of 4 events for a pwm timer channel
 *  Argument:
 *          nEvt:   the number of event
 *          nTimer: the number of timer
 *          channel: the number of channel
 */
void gap_pwm_timer_event_disable (unsigned char nEvt, numPwmTimerT nTimer, PwmTimerLutChannelT channel){
    unsigned int Val = soc_eu_eventMask_get(SOC_FC_MASK_MSB);
    gap_pwm_timer_t timer;
    timer.Raw = gap_pwm_timer_evt_reg_read();
    timer.timerEvt.evt_en &= ~(0x1 << nEvt);
    switch (nEvt){
        case pwm_timer_event0:
            Val = Val | (1<<(ADV_TIMER_EVENT0-32));
            timer.timerEvt.evt0_sel = (nTimer*4 + channel);
            break;
        case pwm_timer_event1:
            Val = Val | (1<<(ADV_TIMER_EVENT1-32));
            timer.timerEvt.evt1_sel = (nTimer*4 + channel);
            break;
        case pwm_timer_event2:
            Val = Val | (1<<(ADV_TIMER_EVENT2-32));
            timer.timerEvt.evt2_sel = (nTimer*4 + channel);
            break;
        case pwm_timer_event3:
            Val = Val | (1<<(ADV_TIMER_EVENT3-32));
            timer.timerEvt.evt3_sel = (nTimer*4 + channel);
            break;
    }
    soc_eu_eventMask_set(SOC_FC_MASK_MSB, Val);
    gap_pwm_timer_evt_reg_cfg(timer.Raw);
}

/*
 *  Disable all these 4 events
 */
void gap_pwm_timer_event_all_disable(){
    unsigned int Val = soc_eu_eventMask_get(SOC_FC_MASK_MSB);
    gap_pwm_timer_t timer;
    timer.timerEvt.evt_en = 0;
    Val = (Val | (0xF<<(ADV_TIMER_EVENT0-32)));
    soc_eu_eventMask_set(SOC_FC_MASK_MSB, Val);
    gap_pwm_timer_evt_reg_cfg(timer.Raw);
}


