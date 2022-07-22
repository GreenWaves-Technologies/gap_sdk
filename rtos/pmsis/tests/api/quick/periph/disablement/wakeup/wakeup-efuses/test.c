#include <stdio.h>
#include <pmsis.h>
#include "testbench.h"
//#include "efuse.h"

#if defined(__PLATFORM_RTL__)

#define RTC_DIV 0x6
#ifdef LONG_PERIOD
#define RTC_COUNT 0x100
#else
#define RTC_COUNT 0x10
#endif
#else
#define RTC_DIV 0x8000
#define RTC_COUNT 1
#endif

#if TEST_DURATION >= 100
#define NB_EVENTS 600

#elif TEST_DURATION >= 75
#define NB_EVENTS 100

#elif TEST_DURATION >= 50
#define NB_EVENTS 20

#elif TEST_DURATION >= 25
#define NB_EVENTS 5

#else
#define NB_EVENTS 1
#endif


//#define VERBOSE 1


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


int main()
{
    // Setup the Pads now to avoid triggering some random communication with the testbench
    testbench_prepare_pads();

    // And then release the outputs that we forced in case we come back from deep sleep
    pi_pad_sleep_cfg_force(0);

    if (pi_pmu_get_prev_state(PI_PMU_DOMAIN_CHIP) == PI_PMU_DOMAIN_STATE_RESET)
    {
        printf("STA\n");

        //apb_soc_ctrl_feature_disablement_disable_crypto_set(apb_soc_ctrl,1);
        
        // efuse tests -> set ro
        // use page 8

        start_time = testbench_get_time();

        efuse_start_read();
        // read first efuse of page 4
        int efuse_val = efuse_read(64);
        printf("efuse val=%x\n",efuse_val);
        efuse_sleep();
        printf("%s:%d\n",__func__,__LINE__);
        efuse_start_program();
        efuse_program(64, efuse_val | 1<<0);
        efuse_sleep();
        efuse_start_read();
        // read first efuse of page 4
        efuse_val = efuse_read(64);
        printf("efuse val=%x\n",efuse_val);
        efuse_sleep();
        printf("%s:%d\n",__func__,__LINE__);
        
        efuse_prot_page(4);
        efuse_prot_page_lock();
        //efuse_ctrl->page_protect = (1<<4) | (1<<31);
        printf("%s:%d\n",__func__,__LINE__);
        efuse_start_read();
        printf("%s:%d\n",__func__,__LINE__);
        efuse_val = efuse_read(64);
        printf("%s:%d\n",__func__,__LINE__);
        printf("efuse val=%x\n",efuse_val);
        efuse_sleep();
        efuse_start_program();
        efuse_program(64, efuse_val | 1<<1);
        efuse_sleep();
        efuse_start_read();
        efuse_val = efuse_read(64);
        printf("efuse val=%x\n",efuse_val);
        efuse_sleep();
        

        // We'll try to shift the wakeup at each iteration so that we cover a full RTC period
        // For that compute the number of cycles of 1 RTC period and divide by the number of iterations
        int freq = pi_freq_get(PI_FREQ_DOMAIN_FC);

        int period_cycles = freq / 32768 * RTC_DIV * RTC_COUNT * 2;

    #if NB_EVENTS != 1
        step_cycles = period_cycles / (NB_EVENTS - 1);
    #endif

        struct pi_rtc_conf conf;
        uint32_t countdown;
        struct pi_device rtc;
        pi_rtc_conf_init(&conf);

        conf.clk_div = RTC_DIV;
        countdown = RTC_COUNT;

        pi_open_from_conf(&rtc, &conf);

        if (pi_rtc_open(&rtc))
            return -1;

        pi_rtc_timer_set(&rtc, countdown);
        pi_rtc_ioctl(&rtc, PI_RTC_TIMER_START, (void *)1);

        // Force outputs during deep sleep to avoid random communication
        pi_pad_sleep_cfg_force(1);

        pi_pmu_wakeup_control(PI_PMU_WAKEUP_RTC, 0);

        pi_pmu_set_state(PI_PMU_DOMAIN_CHIP, PI_PMU_DOMAIN_STATE_DEEP_SLEEP_RETENTIVE, 0);
    }
    else
    {
        pi_pmu_wakeup_control(0, 0);

        //printf("Wakeup from deep sleep my_var=%d\n", my_var);
        my_var--;

        if (my_var == 0)
        {
            uint64_t total_time = testbench_get_time() - start_time;
            uint64_t min_time = (uint64_t)1000000000000 / 32768 * RTC_DIV * RTC_COUNT * NB_EVENTS;

#ifdef VERBOSE
            printf("soc_ctrl: %x\n",apb_soc_ctrl->feature_disablement_soc);
#endif

            printf("%s:%d\n",__func__,__LINE__);
            efuse_start_read();
            printf("%s:%d\n",__func__,__LINE__);
            int efuse_val = efuse_read(64);
            printf("%s:%d\n",__func__,__LINE__);
            printf("efuse val=%x\n",efuse_val);
            efuse_sleep();
            if (efuse_val != 0x1)
            {
                printf("TKO\n");
                testbench_exit(-1);
            }
            efuse_start_program();
            efuse_program(64, efuse_val | 1<<1);
            efuse_sleep();
            efuse_start_read();
            efuse_val = efuse_read(64);
            printf("efuse val=%x\n",efuse_val);
            efuse_sleep();
            if (efuse_val == 0x3)
            {
                printf("TOK\n");
                testbench_exit(0);
            }
            else
            {
                printf("TKO\n");
                testbench_exit(-1);
            }
        }

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

        pi_pad_sleep_cfg_force(1);

        pi_pmu_wakeup_control(PI_PMU_WAKEUP_RTC, 0);

        pi_pmu_set_state(PI_PMU_DOMAIN_CHIP, PI_PMU_DOMAIN_STATE_DEEP_SLEEP_RETENTIVE, 0);
    }

error:
    printf("TKO\n");
    testbench_exit(-1);
}
