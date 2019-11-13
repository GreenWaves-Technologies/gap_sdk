/* PMSIS includes */
#include "pmsis.h"

/* Variables used. */
#define THRESHOLD_0 ( 0xFFFF0001 )
#define THRESHOLD_1 ( 0x88880001 )
#define THRESHOLD_2 ( 0x44440001 )
#define THRESHOLD_3 ( 0x22220001 )

struct pi_device pwm[ARCHI_NB_PWM];
struct pi_pwm_conf conf[ARCHI_NB_PWM];

static volatile uint8_t done = 0;

static void __pwm_callback(void *arg)
{
    struct pi_device *pwm = (struct pi_device *) arg;
    uint8_t pwm_id = (uint8_t) ((uint32_t) pwm->data);
    printf("PWM(%d) Callback called, counter value : %x\n",
           pwm_id, pi_pwm_counter_get(pwm));
    done |= 1 << pwm_id;
}

void test_pwm(void)
{
    printf("Entering main controller\n");

    uint32_t errors = 0;
    uint32_t threshold[ARCHI_NB_PWM] = {THRESHOLD_0, THRESHOLD_1, THRESHOLD_2, THRESHOLD_3};
    struct pi_pwm_ioctl_ch_config ch_conf[ARCHI_NB_PWM] = {0};
    struct pi_pwm_ioctl_evt evt[ARCHI_NB_PWM] = {0};
    pi_task_t cb_task[ARCHI_NB_PWM] = {0};

    for (uint32_t i = 0; i < (uint32_t) ARCHI_NB_PWM; i++)
    {
        pi_pwm_conf_init(&conf[i]);
        conf[i].pwm_id = i;
        pi_open_from_conf(&pwm[i], &conf[i]);
        if (pi_pwm_open(&pwm[i]))
        {
            printf("Error opening PWM(%d) !\n", i);
            pmsis_exit(-1 - i);
        }

        /* Setup Timer threshold. */
        pi_pwm_ioctl(&pwm[i], PI_PWM_TIMER_THRESH, (void *) threshold[i]);

        /* Setup channel : channel, threshold, mode. */
        ch_conf[i].ch_threshold = (threshold[i] >> 16) >> 1;
        ch_conf[i].channel = PI_PWM_CHANNEL0 + i;
        ch_conf[i].config = PI_PWM_TOGGLE_CLEAR;
        pi_pwm_ioctl(&pwm[i], PI_PWM_CH_CONFIG, (void *) &ch_conf[i]);

        /* Setup output event. */
        evt[i].evt_sel = PI_PWM_EVENT_SEL0 + i;
        //evt.evt_output = PI_PWM_EVT_PWM_0_CH_0;
        evt[i].evt_output = PI_PWM_EVENT_OUTPUT(i, i);
        pi_pwm_ioctl(&pwm[i], PI_PWM_EVENT_SET, (void *) &evt[i]);

        /* Attach callback on each PWM. */
        pi_task_callback_no_mutex(&cb_task[i], __pwm_callback, (void *) &pwm[i]);
        pi_pwm_ioctl(&pwm[i], PI_PWM_ATTACH_CB, (void *) &cb_task[i]);
    }

    /* Start PWM timer. */
    printf("Start PWM timer !\n");
    for (uint32_t i = 0; i < (uint32_t) ARCHI_NB_PWM; i++)
    {
        pi_pwm_timer_start(&pwm[i]);
    }

    for (uint32_t i = 0; i < (uint32_t) ARCHI_NB_PWM; i++)
    {
        printf("PWM(%d) : %x %x %x %x %x %x\n",
               i, pwm(i)->cmd, pwm(i)->config, pwm(i)->threshold,
               pwm(i)->ch_threshold[i], pwm_ctrl->event_cfg, pwm_ctrl->cg);
    }

    /* Delay current task for 5s. */
    printf("Sleeping for 5s.\n");
    pi_time_wait_us(5 * 1000 * 1000);
    printf("Coming back, closing PWM devices.\n");
    for (uint32_t i = 0; i < (uint32_t) ARCHI_NB_PWM; i++)
    {
        pi_pwm_close(&pwm[i]);
    }

    errors = (((1 << ARCHI_NB_PWM) - 1) != done);
    printf("\nTest %s with %ld error(s) !\n", (errors) ? "failed" : "success", errors);

    pmsis_exit(errors);
}

/* Program Entry. */
int main(void)
{
    printf("\n\n\t *** PMSIS PWM ***\n\n");
    return pmsis_kickoff((void *) test_pwm);
}

