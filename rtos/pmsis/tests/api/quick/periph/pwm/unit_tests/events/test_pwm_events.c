/* PMSIS includes */
#include "pmsis.h"

const uint32_t pwm_freq = 3000;
const uint32_t pwm_duty_cycle = 20;

struct pi_device pwm;
struct pi_pwm_conf conf;

static volatile uint32_t handler_counter = 0;

void pin_handler(void* arg)
{
    handler_counter++;
}

void test_pwm(void)
{
    printf("Entering main controller\n");

    int32_t errors = 0;

    for(int pwm_id = 0; pwm_id < 4; pwm_id++)
    {
        printf("Testing callback for PWM %d\n", pwm_id);
        struct pi_pwm_ioctl_ch_config ch_conf = {0};
        struct pi_pwm_ioctl_evt evt = {0};

        /* reset global counter */
        handler_counter = 0;

        pi_pwm_conf_init(&conf);
        conf.pwm_id = pwm_id;
        conf.ch_id = PI_PWM_CHANNEL0;
#if CLK_SOURCE_REF_32K == 0
        printf("Using ref_clk_fast\n");
#else
        conf.timer_conf = PI_PWM_EVT_EACH_CLK_CYCLE |
                          PI_PWM_CLKSEL_REFCLK_32K |
                          PI_PWM_UPDOWNSEL_RESET;
        printf("Using ref_clk_32k\n");
#endif
        pi_open_from_conf(&pwm, &conf);
        errors = pi_pwm_open(&pwm);
        if (errors)
        {
            printf("Error opening PWM(%d) : %ld!\n", pwm_id, errors);
            pmsis_exit(-1);
        }

        pi_pwm_duty_cycle_set(&pwm, pwm_freq, pwm_duty_cycle);

        pi_task_t cb_task = {0};
        pi_task_callback(&cb_task, pin_handler, NULL);

        /* Setup output event. */
        evt.evt_sel = PI_PWM_EVENT_SEL0 + pwm_id;
        evt.evt_output = PI_PWM_EVENT_OUTPUT(pwm_id, (PI_PWM_CHANNEL0));
        pi_pwm_ioctl(&pwm, PI_PWM_EVENT_SET, (void *) &evt);
        pi_pwm_ioctl(&pwm, PI_PWM_ATTACH_CB, (void*) &cb_task); // takes a pi_task as args

        /* Start PWM timer. */
        pi_pwm_timer_start(&pwm);
        printf("Start PWM timer !\n");

#if CLK_SOURCE_REF_32K == 0
        uint32_t sleep_us = 2 * 1000;
#else
        uint32_t sleep_us = 5 * 1000;
#endif
        printf("Sleeping for %ld us.\n", sleep_us);
        pi_time_wait_us(sleep_us);

        errors = (handler_counter == 0);
        if (errors)
        {
            printf("Handler not called, but it should have been\n");
            pmsis_exit(-2);
        }

        printf("Disabling callback\n");
        pi_pwm_ioctl(&pwm, PI_PWM_ATTACH_CB, NULL);// NULL cancels callback
        handler_counter = 0;
        printf("Sleeping for %ld us.\n", sleep_us);
        pi_time_wait_us(sleep_us);

        errors = (handler_counter != 0);
        if (errors)
        {
            printf("Handler called, but it should not have been\n");
            pmsis_exit(-2);
        }

        printf("Coming back, closing PWM devices.\n");
        pi_pwm_close(&pwm);
    }

    pmsis_exit(errors);
}

/* Program Entry. */
int main(void)
{
    printf("\n\n\t *** PMSIS PWM ***\n\n");
    return pmsis_kickoff((void *) test_pwm);
}

