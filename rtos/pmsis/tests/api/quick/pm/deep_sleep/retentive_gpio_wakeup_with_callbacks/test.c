#include <stdio.h>
#include <pmsis.h>
#include "testbench.h"



#if TEST_DURATION >= 100
#define NB_EVENTS 600

#elif TEST_DURATION >= 75
#define NB_EVENTS 100

#elif TEST_DURATION >= 50
#define NB_EVENTS 5

#elif TEST_DURATION >= 25
#define NB_EVENTS 3

#else
#define NB_EVENTS 1
#endif


#define TB_FREQ 50000000
#define TB_GPIO_PULSE_CYCLES 120000ULL
#define TB_GPIO_PULSE_WIDTH 2500
#define TB_GPIO_PULSE_TIME_US (TB_GPIO_PULSE_CYCLES * 1000000 / TB_FREQ)

#define RTC_DIV 0x6
#define RTC_COUNT 0x10


#define GPIO_ID 42
#define GPIO_PORT (GPIO_ID / 32)
#define GPIO_PORT_ID (GPIO_ID % 32)

#define VERBOSE 1


typedef struct {
    int id;
    int nb_irq;
    int reconf_irq;
    pi_gpio_callback_t gpio_callback;
} gpio_test_t;

static gpio_test_t gpios[1];


static int my_var = NB_EVENTS;
static int step_cycles;
static pi_device_t bench;
static uint64_t start_time;
static struct pi_device gpio_dev[3];
static volatile int pending_gpio_reconf;
static pi_task_t pulse_task;


static void gen_pulse(void *arg)
{
    static pi_device_t bench;
    struct pi_testbench_conf conf;
    pi_testbench_conf_init(&conf);

    conf.uart_id = TESTBENCH_UART_ID;
    conf.uart_baudrate = TESTBENCH_UART_BAUDRATE;

    pi_open_from_conf(&bench, &conf);

    pi_testbench_open(&bench);

    pi_testbench_gpio_pulse_gen(&bench, GPIO_ID, 1, TB_GPIO_PULSE_CYCLES, TB_GPIO_PULSE_WIDTH, 0);
}


static void gpio_callback_handler(void *arg)
{
    gpio_test_t *gpio = (gpio_test_t *)arg;

    if (pending_gpio_reconf)
    {
        gpio->reconf_irq = 1;
    }
    else
    {
        gpio->nb_irq++;
        #ifdef ONE_SHOT
        pi_task_push(pi_task_callback(&pulse_task, gen_pulse, NULL));
        #endif
    }
}


static int gpio_init(gpio_test_t *gpio, int gpio_id)
{
    gpio->id = gpio_id;
    gpio->nb_irq = 0;
    gpio->reconf_irq = 0;

    return 0;
}


static int gpio_enable(gpio_test_t *gpio)
{
    struct pi_gpio_conf gpio_conf;
    int gpio_id = gpio->id;
    int gpio_port = gpio_id / 32;
    int gpio_port_id = gpio_id % 32;
    struct pi_device *dev = &gpio_dev[gpio_port];

    pi_gpio_conf_init(&gpio_conf);
    gpio_conf.port = gpio_port;
    pi_open_from_conf(dev, &gpio_conf);

    if (pi_gpio_open(dev))
    {
        return -1;
    }

    if (pi_gpio_pin_configure(dev, gpio_port_id, PI_GPIO_INPUT))
        return -1;

    pi_gpio_pin_notif_configure(dev, 1<<gpio_port_id, PI_GPIO_NOTIF_RISE);
    pi_gpio_callback_init(&gpio->gpio_callback, 1<<gpio_port_id, gpio_callback_handler, (void *)gpio);
    pi_gpio_callback_add(dev, &gpio->gpio_callback);

    pi_pad_set_function(gpio_id, PI_PAD_FUNC1);

    return 0;
}


static int gpio_disable(gpio_test_t *gpio)
{
    int gpio_id = gpio->id;
    int gpio_port = gpio_id / 32;
    int gpio_port_id = gpio_id % 32;
    struct pi_device *dev = &gpio_dev[gpio_port];
    pi_gpio_pin_notif_configure(dev, 1<<gpio_port_id, PI_GPIO_NOTIF_NONE);
}


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
    struct pi_device gpio;

    // Setup the Pads now to avoid triggering some random communication with the testbench
    testbench_prepare_pads();

    // And then release the outputs that we forced in case we come back from deep sleep
    pi_pad_sleep_cfg_force(0);

    if (pi_pmu_boot_state_get() == PI_PMU_DOMAIN_STATE_OFF)
    {
        printf("STA\n");

        // We'll try to shift the wakeup at each iteration so that we cover a full RTC period
        // For that compute the number of cycles of 1 RTC period and divide by the number of iterations
        int64_t freq = pi_freq_get(PI_FREQ_DOMAIN_FC);

        int period_cycles = freq * TB_GPIO_PULSE_CYCLES / TB_FREQ;

        #if NB_EVENTS != 1
        // Increase by 1.5 to cover a bit more than 1 period
        step_cycles = period_cycles / (NB_EVENTS - 1) * 5;
        #endif

        #ifdef USE_RTC
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
        #endif

        struct pi_testbench_conf tb_conf;

        pi_testbench_conf_init(&tb_conf);

        tb_conf.uart_id = TESTBENCH_UART_ID;
        tb_conf.uart_baudrate = TESTBENCH_UART_BAUDRATE;

        pi_open_from_conf(&bench, &tb_conf);

        if (pi_testbench_open(&bench))
            return -1;

        #ifndef ONE_SHOT
        // printf("Testbench GPIO pulse gen start\n");
        pi_testbench_gpio_pulse_gen(&bench, GPIO_ID, 1, TB_GPIO_PULSE_CYCLES, TB_GPIO_PULSE_WIDTH, TB_GPIO_PULSE_CYCLES);
        // printf("Testbench GPIO pulse gen finish\n");
        #endif

        start_time = testbench_get_time();

        #ifdef ONE_SHOT
        // printf("Gen pulse null start\n");
        gen_pulse(NULL);
        // printf("Gen pulse null finish\n");
        #endif
        if (gpio_init(&gpios[0], GPIO_ID))
            return -1;

        if (gpio_enable(&gpios[0]))
            goto error;

        // Force outputs during deep sleep to avoid random communication
        pi_pad_sleep_cfg_force(1);

        pending_gpio_reconf = 1;

        pi_pmu_wakeup_control(PI_PMU_WAKEUP_GPIO | PI_PMU_WAKEUP_RTC, 1);

        pi_pmu_domain_state_change(PI_PMU_DOMAIN_CHIP, PI_PMU_DOMAIN_STATE_DEEP_SLEEP_RETENTIVE, 0);
    }
    else
    {
        #ifdef VERBOSE
        //printf("Wakeup from deep sleep my_var=%d\n", my_var);
        #endif

        // During the period where we switch from wakeup mechanism to classic gpio, we may caught the gpio
        // pulse in both systens. Make sure it is accounted only once when the switch is done

        if (gpio_enable(&gpios[0]))
            goto error;

        pi_pmu_wakeup_control(0, 0);

        // Here we should wait for a time equal to the gpio pulse width, to be sure that
        // we are not capturing the same pulse as the one caught by the wakeup system.

        // Now switch to normal gpio handling since we are sure it's a new pulse
        pending_gpio_reconf = 0;

        // And account what was caght during the switch as only one pulse

        int reconf_irq = 0;
        if (gpios[0].reconf_irq)
        {
            reconf_irq = 1;
            gpios[0].reconf_irq = 0;
        }

        //if (pi_pmu_wakeup_reason() != 2)
        //printf("WAKEUP REASON %x gpio %x\n", pi_pmu_wakeup_reason(), pi_pmu_gpio_wakeup_pins());

        if (pi_pmu_wakeup_reason() & PI_PMU_WAKEUP_GPIO)
        {
            uint32_t gpio_wakeup = pi_pmu_gpio_wakeup_pins();
            //printf("GPIO WAKEUP %x\n", gpio_wakeup);
            if (gpio_wakeup != 0)
            {
                if (((gpio_wakeup >> 0) & 1) == 0)
                {
                    #ifndef USE_RTC
                    goto error;
                    #endif
                }
                else
                {
                    reconf_irq = 1;
                }
            }
        }

        if (reconf_irq)
        {
            // A pulse was caught during the wakeup or during the switch, account it. Be careful to lock IRQs since this is also modified by the interrupt handler
            int irq = pi_irq_disable();
            gpios[0].nb_irq++;
            #ifdef ONE_SHOT
            printf("Gen pulse 2 null start\n");
            gen_pulse(NULL);
            printf("Gen pulse 2 null finish\n");
            #endif
            pi_irq_restore(irq);
        }


        my_var--;

        #if 0
        {
            uint64_t total_time = testbench_get_time() - start_time;
            int nb_expected_pulses = total_time / TB_GPIO_PULSE_TIME_US / 1000000;

            if (nb_expected_pulses != gpios[0].nb_irq)
                printf("Finished with time %lld nb_irq %d nb_expected_pulses %d\n", total_time, gpios[0].nb_irq, nb_expected_pulses);
        }
        #endif

        if (my_var == 0)
        {
            gpio_disable(&gpios[0]);

            #ifdef ONE_SHOT
            printf("Get testbench time start\n");
            uint64_t total_time = testbench_get_time() - start_time;
            uint64_t min_time = (uint64_t)1000000000000 / TB_FREQ * TB_GPIO_PULSE_CYCLES * NB_EVENTS;
            printf("Get testbench time end\n");
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
            #else
            uint64_t total_time = testbench_get_time() - start_time;
            uint64_t min_time = (uint64_t)1000000000000 / TB_FREQ * TB_GPIO_PULSE_CYCLES * NB_EVENTS;
            int nb_pulses = gpios[0].nb_irq;
            int nb_expected_pulses = total_time / TB_GPIO_PULSE_TIME_US / 1000000;
            int nb_pulse_diff = nb_pulses - nb_expected_pulses;

            #ifdef VERBOSE
            printf("Finished with time %lld min time %lld nb_irq %d nb_expected_pulses %d\n", total_time, min_time, gpios[0].nb_irq, nb_expected_pulses);
            #endif

            if (total_time >= min_time && nb_pulse_diff >= -1 && nb_pulse_diff <= 1)
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
            #endif
        }

        #if !defined(__FREERTOS__)
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
        #endif  /* __FREERTOS__ */

        pending_gpio_reconf = 1;

        #if !defined(__FREERTOS__)
        // FLush pending tasks as there may be a pending task for generating a new pulse
        pos_task_flush();
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
