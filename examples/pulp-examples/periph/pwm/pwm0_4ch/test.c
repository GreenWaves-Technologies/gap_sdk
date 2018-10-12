#include "Gap8.h"

#define TH      0xFFFF0000

int main(){
    int err=0;
    unsigned int timeNow0 = 0;
    unsigned int timer0 = 0;
    PwmCounterCmdT cmd;

    printf("test start\n");
    timer0 = gap_pwm_timer_alloc();
    gap_pwm_timer_threshold_set(timer0, TH);

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
 *                      ADV_TIMER_USEREF            0x1 //32KHz ref clk
 *                      ADV_TIMER_USESIG            0x0 //FLL
 *      - IncThenDec: action when reaching the TH:
 *                      ADV_TIMER_SAW:     /|/|/| (SAW)
 *                      ADV_TIMER_NOSAW:   /\/\/\ (NOSAW)
 *      - PreScaler
 */

    gap_pwm_timer_conf(timer0, 0,    // choose the input source ? (if the rule is AO, this arg is not important)
            ADV_TIMER_INMODE_AO,    // define the enable rules
            ADV_TIMER_USEREF,       // FLL or RTC
            ADV_TIMER_SAW,          // when reaching the TH: /|/|/| (SAW) or /\/\/\ (NOSAW)
            0);                     // prescaler

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
    gap_pwm_timer_channel_config(timer0, PWM_TIMER_TH_CHANNEL0, 0x2FFF, ADV_TIMER_OP_TOG);
    gap_pwm_timer_channel_config(timer0, PWM_TIMER_TH_CHANNEL1, 0x6FFF, ADV_TIMER_OP_TOG);
    gap_pwm_timer_channel_config(timer0, PWM_TIMER_TH_CHANNEL2, 0x8FFF, ADV_TIMER_OP_TOG);
    gap_pwm_timer_channel_config(timer0, PWM_TIMER_TH_CHANNEL3, 0xBFFF, ADV_TIMER_OP_TOG);

    gap_pwm_timer_event_set (pwm_timer_event0, timer0, PWM_TIMER_TH_CHANNEL0);
    gap_pwm_timer_event_set (pwm_timer_event1, timer0, PWM_TIMER_TH_CHANNEL1);
    gap_pwm_timer_event_set (pwm_timer_event2, timer0, PWM_TIMER_TH_CHANNEL2);
    gap_pwm_timer_event_set (pwm_timer_event3, timer0, PWM_TIMER_TH_CHANNEL3);

    cmd = PWM_TIMER_CMD_START;
    gap_pwm_timer_cmd(timer0, cmd);

    printf("pwm started\n");
    while(1){
        gap_pwm_timer_int_wait_and_clear(pwm_timer_event0);
        timeNow0 = gap_pwm_timer_read_counter(timer0);
        printf("time now: %X\n", timeNow0);
        printf("------------------------------------\n");
        gap_pwm_timer_int_wait_and_clear(pwm_timer_event1);
        timeNow0 = gap_pwm_timer_read_counter(timer0);
        printf("time now: %X\n", timeNow0);
        printf("------------------------------------\n");
        gap_pwm_timer_int_wait_and_clear(pwm_timer_event2);
        timeNow0 = gap_pwm_timer_read_counter(timer0);
        printf("time now: %X\n", timeNow0);
        printf("------------------------------------\n");
        gap_pwm_timer_int_wait_and_clear(pwm_timer_event3);
        timeNow0 = gap_pwm_timer_read_counter(timer0);
        printf("time now: %X\n", timeNow0);
        printf("------------------------------------\n");
    }

    cmd = PWM_TIMER_CMD_STOP;
    gap_pwm_timer_cmd(timer0, cmd);
    return err;
}

