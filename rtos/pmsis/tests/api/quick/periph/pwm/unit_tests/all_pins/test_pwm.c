/* PMSIS includes */
#include "pmsis.h"

/* Variables used. */
#define NB_PWM_USED      (8)

#define PWM0_FREQ        (200000)
#define PWM1_FREQ        (800000)
#define PWM2_FREQ        (3000000)
#define PWM3_FREQ        (5000000)

#define PWM0_CH0_DUTY_CYCLE  (20)
#define PWM1_CH0_DUTY_CYCLE  (80)
#define PWM2_CH0_DUTY_CYCLE  (30)
#define PWM3_CH0_DUTY_CYCLE  (50)

#define PWM0_CH1_DUTY_CYCLE  (10)
#define PWM1_CH1_DUTY_CYCLE  (70)
#define PWM2_CH1_DUTY_CYCLE  (90)
#define PWM3_CH1_DUTY_CYCLE  (40)

struct pi_device pwm[NB_PWM_USED];
struct pi_pwm_conf conf[NB_PWM_USED];

static int32_t pwm_configurations[NB_PWM_USED][4] = {
    /* pwm_id, pwm_ch_id, pwm_freq, pwm_duty_cycle */
    {0, 0, PWM0_FREQ, PWM0_CH0_DUTY_CYCLE},
    {2, 0, PWM2_FREQ, PWM2_CH0_DUTY_CYCLE},
    {1, 0, PWM1_FREQ, PWM1_CH0_DUTY_CYCLE},
    {3, 0, PWM3_FREQ, PWM3_CH0_DUTY_CYCLE},
    {0, 1, PWM0_FREQ, PWM0_CH1_DUTY_CYCLE},
    {2, 1, PWM2_FREQ, PWM2_CH1_DUTY_CYCLE},
    {1, 1, PWM1_FREQ, PWM1_CH1_DUTY_CYCLE},
    {3, 1, PWM3_FREQ, PWM3_CH1_DUTY_CYCLE},
};

void test_pwm(void)
{
    printf("Entering main controller\n");

    int32_t errors = 0;

    struct pi_pwm_ioctl_ch_config ch_conf[NB_PWM_USED] = {0};
    struct pi_pwm_ioctl_evt evt[NB_PWM_USED] = {0};

    pi_task_t cb_task[NB_PWM_USED] = {0};

    for (uint32_t i = 0; i < (uint32_t) NB_PWM_USED; i++)
    {
        pi_pwm_conf_init(&conf[i]);
        conf[i].pwm_id = pwm_configurations[i][0];
        conf[i].ch_id = pwm_configurations[i][1];
        pi_open_from_conf(&pwm[i], &conf[i]);
        errors = pi_pwm_open(&pwm[i]);
        if (errors)
        {
            printf("Error opening PWM(%ld) : %ld!\n", i, errors);
            pmsis_exit(-1 - i);
        }

        pi_pwm_duty_cycle_set(&pwm[i], pwm_configurations[i][2], pwm_configurations[i][3]);

        /* Setup output event. */
        evt[i].evt_sel = PI_PWM_EVENT_SEL0 + i;
        evt[i].evt_output = PI_PWM_EVENT_OUTPUT(i, (PI_PWM_CHANNEL0 + i));
        pi_pwm_ioctl(&pwm[i], PI_PWM_EVENT_SET, (void *) &evt[i]);
    }

    /* Start PWM timer. */
    for (uint32_t i = 0; i < (uint32_t) NB_PWM_USED; i++)
    {
        pi_pwm_timer_start(&pwm[i]);
        printf("Start PWM timer %ld !\n", i);
    }

    /* Delay current task */
    uint32_t sleep_us = 10 * 1000;
    printf("Sleeping for %ldms.\n", sleep_us/1000);
    pi_time_wait_us(sleep_us);
    printf("Coming back, closing PWM devices.\n");
    for (uint32_t i = 0; i < (uint32_t) NB_PWM_USED; i++)
    {
        pi_pwm_close(&pwm[i]);
    }

    pmsis_exit(errors);
}

/* Program Entry. */
int main(void)
{
    printf("\n\n\t *** PMSIS PWM ***\n\n");
    return pmsis_kickoff((void *) test_pwm);
}

