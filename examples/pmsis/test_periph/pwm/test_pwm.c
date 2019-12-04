/* PMSIS includes */
#include "pmsis.h"

/* Variables used. */
#define NB_PWM_USED      ( ARCHI_NB_PWM )

#define PWM0_FREQ        ( 200000 )
#define PWM1_FREQ        ( 800000 )
#define PWM2_FREQ        ( 3000000 )
#define PWM3_FREQ        ( 5000000 )

#define PWM0_DUTY_CYCLE  ( 20 )
#define PWM1_DUTY_CYCLE  ( 80 )
#define PWM2_DUTY_CYCLE  ( 30 )
#define PWM3_DUTY_CYCLE  ( 50 )


struct pi_device pwm[NB_PWM_USED];
struct pi_pwm_conf conf[NB_PWM_USED];

static volatile uint8_t done = 0;

static void __pwm_callback(void *arg)
{
    struct pi_device *pwm = (struct pi_device *) arg;
    uint8_t pwm_id = (uint8_t) ((uint32_t) pwm->data);
    /* printf("PWM(%d) Callback called, counter value : %x\n", */
    /*        pwm_id, pi_pwm_counter_get(pwm)); */
    done |= 1 << pwm_id;
}

void test_pwm(void)
{
    printf("Entering main controller\n");

    uint32_t errors = 0;
    uint32_t freq[NB_PWM_USED] = {PWM0_FREQ, PWM1_FREQ, PWM2_FREQ, PWM3_FREQ};
    uint32_t duty_cycle[NB_PWM_USED] = {PWM0_DUTY_CYCLE, PWM1_DUTY_CYCLE,
                                        PWM2_DUTY_CYCLE, PWM3_DUTY_CYCLE};
    struct pi_pwm_ioctl_ch_config ch_conf[NB_PWM_USED] = {0};
    struct pi_pwm_ioctl_evt evt[NB_PWM_USED] = {0};
    pi_task_t cb_task[NB_PWM_USED] = {0};

    for (uint32_t i = 0; i < (uint32_t) NB_PWM_USED; i++)
    {
        pi_pwm_conf_init(&conf[i]);
        conf[i].pwm_id = i;
        conf[i].ch_id = PI_PWM_CHANNEL0 + i;
        pi_open_from_conf(&pwm[i], &conf[i]);
        if (pi_pwm_open(&pwm[i]))
        {
            printf("Error opening PWM(%d) !\n", i);
            pmsis_exit(-1 - i);
        }

        pi_pwm_duty_cycle_set(&pwm[i], freq[i], 50);

        /* Setup output event. */
        evt[i].evt_sel = PI_PWM_EVENT_SEL0 + i;
        evt[i].evt_output = PI_PWM_EVENT_OUTPUT(i, (PI_PWM_CHANNEL0 + i));
        pi_pwm_ioctl(&pwm[i], PI_PWM_EVENT_SET, (void *) &evt[i]);

        /* Attach callback on each PWM. */
        pi_task_callback_no_mutex(&cb_task[i], __pwm_callback, (void *) &pwm[i]);
        pi_pwm_ioctl(&pwm[i], PI_PWM_ATTACH_CB, (void *) &cb_task[i]);
    }

    /* Start PWM timer. */
    for (uint32_t i = 0; i < (uint32_t) NB_PWM_USED; i++)
    {
        pi_pwm_timer_start(&pwm[i]);
        printf("Start PWM timer %d ! ", i);
        printf("PWM(%d) : %x %x %x %x %x %x\n",
               i, pwm(i)->cmd, pwm(i)->config, pwm(i)->threshold,
               pwm(i)->ch_threshold[i], pwm_ctrl->event_cfg, pwm_ctrl->cg);
    }

    /* Delay current task for 5s. */
    uint32_t sleep_us = 5 * 1000 * 1000;
    printf("Sleeping for %ds.\n", sleep_us/(1000 * 1000));
    pi_time_wait_us(sleep_us);
    printf("Coming back, closing PWM devices.\n");
    for (uint32_t i = 0; i < (uint32_t) NB_PWM_USED; i++)
    {
        pi_pwm_close(&pwm[i]);
    }

    errors = (((1 << NB_PWM_USED) - 1) != done);
    printf("\nTest %s with %ld error(s) !\n", (errors) ? "failed" : "success", errors);

    pmsis_exit(errors);
}

/* Program Entry. */
int main(void)
{
    printf("\n\n\t *** PMSIS PWM ***\n\n");
    return pmsis_kickoff((void *) test_pwm);
}

