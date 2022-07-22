#include <stdio.h>
#include <pmsis.h>
#include "testbench.h"
#include <bsp/ram/hyperram.h>

#define TB_FREQ 50000000
#define TB_GPIO_PULSE_CYCLES 150000

#if defined(__PLATFORM_RTL__)
#define NB_EVENTS 3
#else
#define NB_EVENTS 5
#endif

void testbench_prepare_pads()
{
    struct pi_testbench_conf conf;
    pi_testbench_conf_init(&conf);

    pi_testbench_prepare_pads(&conf);
}

void testbench_exit(int status)
{
    static pi_device_t bench;
    struct pi_testbench_conf conf;
    pi_testbench_conf_init(&conf);

    pi_open_from_conf(&bench, &conf);

    if (pi_testbench_open(&bench))
        return;

    pi_testbench_set_status(&bench, status);
    while (1);
}

static int open_ram(struct pi_device *ram)
{
    struct pi_hyperram_conf ram_conf;
    pi_hyperram_conf_init(&ram_conf);

    pi_open_from_conf(ram, &ram_conf);

    if (pi_ram_open(ram))
        return -1;

    return 0;
}

int test_entry()
{
    struct pi_device ram;

    // Setup the Pads now to avoid triggering some random communication with the testbench
    pi_testbench_get();

    // And then release the outputs that we forced in case we come back from deep sleep
    pi_pad_sleep_cfg_force(0);

    if (open_ram(&ram))
        return -1;

    if (pi_pmu_boot_state_get() == PI_PMU_DOMAIN_STATE_OFF)
    {
        printf("STA\n");

        int count = 0;
        pi_ram_write(&ram, 0x00080000, &count, 4);

        #ifndef CONFIG_BOOT_MODE_SPISLAVE
        pi_testbench_req_spim_verif_setup_t setup_config = { .enabled=1, .itf=4, .cs=0, .is_master=1, .mem_size_log2=16 };
        if (pi_testbench_spim_verif_setup(pi_testbench_get(), &setup_config))
            return -1;

        pi_testbench_req_spim_verif_spi_wakeup_t config = {.delay = 1000000000, .frequency=50000000, .mode = PI_TESTBENCH_REQ_SPIM_VERIF_SPI_WAKEUP_MODE_CMD, .spi_reload=0};
        pi_testbench_spim_verif_spi_wakeup(pi_testbench_get(), 4, 0, &config);

        #else

        pi_testbench_req_spim_verif_spi_wakeup_t config = {.delay = 1000000000, .frequency=50000000, .mode = PI_TESTBENCH_REQ_SPIM_VERIF_SPI_WAKEUP_MODE_CMD, .spi_reload=1};
        pi_testbench_spim_verif_spi_wakeup(pi_testbench_get(), 4, 0, &config);
        #endif

        // Force outputs during deep sleep to avoid random communication
        pi_pad_sleep_cfg_force(1);

        pi_pmu_wakeup_control(PI_PMU_WAKEUP_SPISLAVE, 0);

        pi_pmu_domain_state_change(PI_PMU_DOMAIN_CHIP, PI_PMU_DOMAIN_STATE_DEEP_SLEEP, 0);
    }
    else
    {
        pi_pmu_wakeup_control(0, 0);

        int count;
        pi_ram_read(&ram, 0x00080000, &count, 4);
        count++;

        #ifdef VERBOSE
        printf("Wakeup from deep sleep count=%d\n", count);
        #endif

        if (count == NB_EVENTS)
        {
            printf("TOK\n");
            testbench_exit(0);
            return 0;
        }

        pi_ram_write(&ram, 0x00080000, &count, 4);

        #ifndef CONFIG_BOOT_MODE_SPISLAVE
        pi_testbench_req_spim_verif_setup_t setup_config = { .enabled=1, .itf=4, .cs=0, .is_master=1, .mem_size_log2=16 };
        if (pi_testbench_spim_verif_setup(pi_testbench_get(), &setup_config))
            return -1;

        pi_testbench_req_spim_verif_spi_wakeup_t config = {.delay = 1000000000, .frequency=50000000, .mode = PI_TESTBENCH_REQ_SPIM_VERIF_SPI_WAKEUP_MODE_CMD, .spi_reload=0};
        pi_testbench_spim_verif_spi_wakeup(pi_testbench_get(), 4, 0, &config);

        #else

        pi_testbench_req_spim_verif_spi_wakeup_t config = {.delay = 1000000000, .frequency=50000000, .mode = PI_TESTBENCH_REQ_SPIM_VERIF_SPI_WAKEUP_MODE_CMD, .spi_reload=1};
        pi_testbench_spim_verif_spi_wakeup(pi_testbench_get(), 4, 0, &config);
        #endif

        // Ref clock edge is 15 us, wait a different number of edges at each iteration
        pi_time_wait_us(count == 0 ? 1 : (count % 10) * 200);

        //for (volatile int i=0; i<count*6000; i++);

        pi_pad_sleep_cfg_force(1);

        pi_pmu_wakeup_control(PI_PMU_WAKEUP_SPISLAVE, 0);

        pi_pmu_domain_state_change(PI_PMU_DOMAIN_CHIP, PI_PMU_DOMAIN_STATE_DEEP_SLEEP, 0);
    }

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
