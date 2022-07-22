#include <stdio.h>
#include <pmsis.h>
#include "testbench.h"


#if TEST_DURATION >= 100
#ifdef LONG_PERIOD
#define NB_EVENTS 150
#else
#define NB_EVENTS 600
#endif

#elif TEST_DURATION >= 75
#ifdef LONG_PERIOD
#define NB_EVENTS 25
#else
#define NB_EVENTS 100
#endif

#elif TEST_DURATION >= 50
#ifdef LONG_PERIOD
#define NB_EVENTS 5
#else
#define NB_EVENTS 20
#endif

#elif TEST_DURATION >= 25
#ifdef LONG_PERIOD
#define NB_EVENTS 2
#else
#define NB_EVENTS 5
#endif

#else
#define NB_EVENTS 1
#endif


#define TB_FREQ 50000000
#ifdef LONG_PERIOD
#define TB_GPIO_PULSE_CYCLES 1500000
#else
#define TB_GPIO_PULSE_CYCLES 150000
#endif
#define TB_GPIO_PULSE_WIDTH 2500


static int my_var = NB_EVENTS;
static int step_cycles;
static pi_device_t bench;
static uint64_t start_time;


void testbench_prepare_pads()
{
    struct pi_testbench_conf conf;
    pi_testbench_conf_init(&conf);

    conf.uart_id = TESTBENCH_UART_ID;
    conf.uart_baudrate = TESTBENCH_UART_BAUDRATE;

    pi_testbench_prepare_pads(&conf);

}


uint64_t testbench_get_time()
{
    static pi_device_t bench;
    struct pi_testbench_conf conf;
    pi_testbench_conf_init(&conf);

    conf.uart_id = TESTBENCH_UART_ID;
    conf.uart_baudrate = TESTBENCH_UART_BAUDRATE;

    pi_open_from_conf(&bench, &conf);

    if (pi_testbench_open(&bench))
        return 0;

    return pi_testbench_get_time_ps(&bench);
}

void testbench_exit(int status)
{
    static pi_device_t bench;
    struct pi_testbench_conf conf;
    pi_testbench_conf_init(&conf);

    conf.uart_id = TESTBENCH_UART_ID;
    conf.uart_baudrate = TESTBENCH_UART_BAUDRATE;

    pi_open_from_conf(&bench, &conf);

    if (pi_testbench_open(&bench))
        return;

    pi_testbench_set_status(&bench, status);
    while(1);
}


int test_entry()
{
    // Setup the Pads now to avoid triggering some random communication with the testbench
    testbench_prepare_pads();

    // And then release the outputs that we forced in case we come back from deep sleep
    pi_pad_sleep_cfg_force(0);

    if (pi_pmu_boot_state_get() == PI_PMU_DOMAIN_STATE_OFF)
    {
      printf("STA, my_var=%d\n", my_var);

        start_time = testbench_get_time();

        // We'll try to shift the wakeup at each iteration so that we cover a full RTC period
        // For that compute the number of cycles of 1 RTC period and divide by the number of iterations
        int64_t freq = pi_freq_get(PI_FREQ_DOMAIN_FC);

        int period_cycles = freq * TB_GPIO_PULSE_CYCLES / TB_FREQ;

        #if NB_EVENTS != 1
        // Increase by 1.5 to cover a bit more than 1 period
        step_cycles = period_cycles / (NB_EVENTS - 1) * 3 / 2;
        #endif

        struct pi_testbench_conf tb_conf;

        pi_testbench_conf_init(&tb_conf);

        tb_conf.uart_id = TESTBENCH_UART_ID;
        tb_conf.uart_baudrate = TESTBENCH_UART_BAUDRATE;

        pi_open_from_conf(&bench, &tb_conf);

        if (pi_testbench_open(&bench))
            return -1;

        pi_testbench_gpio_pulse_gen(&bench, 42, 1, TB_GPIO_PULSE_CYCLES, TB_GPIO_PULSE_WIDTH, TB_GPIO_PULSE_CYCLES);

        // Force outputs during deep sleep to avoid random communication
        pi_pad_sleep_cfg_force(1);

        pi_pmu_wakeup_control(PI_PMU_WAKEUP_GPIO | PI_PMU_WAKEUP_RTC, 1);

        pi_pmu_domain_state_change(PI_PMU_DOMAIN_CHIP, PI_PMU_DOMAIN_STATE_DEEP_SLEEP_RETENTIVE, 0);
    }
    else
    {
        pi_pmu_wakeup_control(0, 0);

        my_var--;

        if (my_var == 0)
        {
            uint64_t total_time = testbench_get_time() - start_time;
            uint64_t min_time = (uint64_t)1000000000000 / TB_FREQ * TB_GPIO_PULSE_CYCLES * NB_EVENTS;

            #ifdef VERBOSE
            printf("Finished with time %lld min time %lld\n", total_time, min_time);
            #endif
            if (total_time >= min_time)
            {
                printf("TOK\n");
                testbench_exit(0);
                return 0;
            }
            else
            {
                printf("TKO\n");
                testbench_exit(-1);
                return -1;
            }
        }

        #if !defined(__FREERTOS__)
        #ifndef LONG_PERIOD
        timer_cmp_set(timer_base_fc(1, 0),
            step_cycles*(NB_EVENTS-my_var)
        );

        timer_conf_set(timer_base_fc(1, 0),
            TIMER_CFG_LO_ENABLE(1) |
            TIMER_CFG_LO_RESET(1) |
            TIMER_CFG_LO_IRQEN(1) |
            TIMER_CFG_LO_ONE_S(1)
        );

        pos_wait_for_event(1 << ARCHI_FC_EVT_TIMER1_LO);
        #endif
        #endif  /* __FREERTOS__ */

        pi_pad_sleep_cfg_force(1);

        // After this call, GPIOs will be caught by wakeup system, not anymore by classic GPIO handling
        pi_pmu_wakeup_control(PI_PMU_WAKEUP_GPIO | PI_PMU_WAKEUP_RTC, 1);

        pi_pmu_domain_state_change(PI_PMU_DOMAIN_CHIP, PI_PMU_DOMAIN_STATE_DEEP_SLEEP_RETENTIVE, 0);
    }

error:
    printf("TKO\n");
    testbench_exit(-1);
    return -1;
}

static void test_kickoff(void *arg)
{
    int ret = test_entry();
    pmsis_exit(ret);
}

int main()
{
    return pmsis_kickoff((void *) test_kickoff);
}
