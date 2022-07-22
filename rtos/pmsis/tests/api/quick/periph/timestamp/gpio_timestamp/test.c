/* 
 * Copyright (C) 2017 GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 */
#include "pmsis.h"
#include "testbench.h"

#define NB_GPIO_ITER    5
#define NB_PAIR_GPIO    4
#define NB_FIFO_SIZE    (16*NB_GPIO_ITER) 
#define wait_us(x)  	for(volatile int i=0; i<(10*x); i++);

static pi_device_t ts_fifo_device = {0};
static pi_udma_fifo_conf_t ts_fifo_conf = {0};
static uint32_t timestamp_val[NB_FIFO_SIZE];
static pi_device_t gpio_timestamp;
static pi_timestamp_input_t ts_gpio[8];

pi_device_t gpio_in[NB_PAIR_GPIO];
pi_device_t gpio_out[NB_PAIR_GPIO];

static int gpio_loopbacks[NB_PAIR_GPIO][2] = {
#ifdef GAP9_WLCSP
    { 38, 39 },
#else
    { 14, 15 },
    { 16, 17 },
    { 18, 19 },
//    { 39, 40 },
//    { 3, 5 },
//    { 22, 26 },
    { 24, 48 },
//    { 43, 44 },
#endif
};

void timestamp_reg_dump()
{
#define TS_BASE	( UDMA_TS_ADDR(0) )
    printf("TS BASE: 0x%8x \n",   TS_BASE);
    printf("reg CNT: %x \n",                *(volatile unsigned int *)(long)(TS_BASE+0x4));
    printf("reg REG_SETUP_CH4: %x \n",      *(volatile unsigned int *)(long)(TS_BASE+0x18));
    printf("reg REG_SETUP_CH5: %x \n",      *(volatile unsigned int *)(long)(TS_BASE+0x1C));
    printf("reg REG_SETUP_CH6: %x \n",      *(volatile unsigned int *)(long)(TS_BASE+0x20));
    printf("reg REG_SETUP_CH7: %x \n",      *(volatile unsigned int *)(long)(TS_BASE+0x24));
    printf("reg EVT ID: %x \n",             *(volatile unsigned int *)(long)(TS_BASE+0x28));
    printf("reg CLK CFG: %x \n",            *(volatile unsigned int *)(long)(TS_BASE+0x2C));
}

static uint32_t init_timestamp_fifo(pi_device_t *fifo_timestamp, pi_udma_fifo_conf_t *conf)
{
    uint32_t fifo_id = 0;
    uint32_t timestamp_size = sizeof(uint32_t) * NB_FIFO_SIZE;

    pi_udma_fifo_conf_init(conf);
    pi_open_from_conf(fifo_timestamp, conf);
    if (0 != pi_udma_fifo_open(fifo_timestamp))
    {
        printf("Could not open udma_fifo\n");
        return -1;
    }

    pi_udma_fifo_ioctl(fifo_timestamp, PI_UDMA_FIFO_GET_ID, &fifo_id);
    pi_udma_fifo_ioctl(fifo_timestamp, PI_UDMA_FIFO_SET_SIZE, &timestamp_size);

    return fifo_id;
}

static pi_device_t* timestamp_init( uint32_t * fifo_id )
{
    /* Set up timestamp */
    struct pi_timestamp_conf ts_conf;

    pi_timestamp_conf_init(&ts_conf);
#ifdef SOC_CLK
    ts_conf.cnt_src = PI_TIMESTAMP_CNT_SOC_CLK;
#endif
#ifdef TS_PRESCALER
    ts_conf.prescaler = TS_PRESCALER? TS_PRESCALER-1:0;
#endif
    pi_open_from_conf(&gpio_timestamp, &ts_conf);
    pi_udma_timestamp_open(&gpio_timestamp);
    pi_udma_timestamp_ioctl( &gpio_timestamp, PI_UDMA_TIMESTAMP_IOCTL_CLR, NULL);

    uint32_t timestamp_fifo_id;
    if (*fifo_id == 0xFF)
    {
        /* Set up a new fifo */
        timestamp_fifo_id = init_timestamp_fifo(&ts_fifo_device, &ts_fifo_conf);
        if (!timestamp_fifo_id)
        {
            printf("Timestamp fifo open failed\n");
            return NULL;
        }
        *fifo_id = timestamp_fifo_id;
    }
    else
    {
        /* Use previous fifo */
        timestamp_fifo_id = *fifo_id;
    }

    return &gpio_timestamp;
}

static void timestamp_gpio_input_init(uint32_t fifo_id)
{
    uint8_t index=0;
    pi_device_t * timestamp = &gpio_timestamp; //This is for setting the timestamp input, not dedicate for this timestamp[0]

    /* ONLY the input gpio can trigger timestamp */
    for (index=0; index<NB_PAIR_GPIO; index++)
    {
        ts_gpio[index].dest_id = fifo_id;
        ts_gpio[index].input_sel = gpio_loopbacks[index][1];
        ts_gpio[index].input_type = PI_TIMESTAMP_GPIO_BOTH_EDGE;

        pi_udma_timestamp_ioctl(timestamp, PI_UDMA_TIMESTAMP_IOCTL_SET_INPUT, &ts_gpio[index]);
    }
}

int init_gpio_loopback(pi_device_t *bench, int output, int input, int index)
{
    int errors = 0;

    struct pi_gpio_conf gpio_conf;

    pi_gpio_conf_init(&gpio_conf);

    gpio_conf.port = input >> 5;

    pi_open_from_conf(&gpio_in[index], &gpio_conf);

    if (pi_gpio_open(&gpio_in[index]))
        return -1;

    gpio_conf.port = output >> 5;

    pi_open_from_conf(&gpio_out[index], &gpio_conf);

    if (pi_gpio_open(&gpio_out[index]))
        return -1;

    if (pi_gpio_pin_configure(&gpio_in[index], input, PI_GPIO_INPUT))
        return -1;

    if (pi_gpio_pin_configure(&gpio_out[index], output, PI_GPIO_OUTPUT))
        return -1;

    return errors;
}

int run_gpio_test(pi_device_t *bench)
{
    int err = 0;
    int index = 0;
    int output[NB_PAIR_GPIO];
    int input[NB_PAIR_GPIO];
    int ts_diff[NB_PAIR_GPIO];
    uint32_t fifo_size = 0;

    for (int i=0; i<NB_PAIR_GPIO; i++)
    {
        output[i] = gpio_loopbacks[i][0];
        input[i] = gpio_loopbacks[i][1];

        // First drive value from testbench to avoid 'X' propagation
        // when setting GPIO pads
        pi_testbench_gpio_loopback(bench, output[i], input[i], 1);
        pi_pad_set_function(input[i], 1);
        pi_pad_set_function(output[i], 1);
    }

    int current_val = 1;

    /* Remove things in timestamps fifo*/
    pi_udma_fifo_ioctl(&ts_fifo_device, PI_UDMA_FIFO_GET_LEVEL, &fifo_size);
    for (uint32_t i=0; i<(fifo_size/4); i++)
    {
        pi_udma_fifo_ioctl(&ts_fifo_device, PI_UDMA_FIFO_POP32, &timestamp_val[i]);
    }

    pi_udma_fifo_ioctl(&ts_fifo_device, PI_UDMA_FIFO_GET_LEVEL, &fifo_size);

    //for (int i=0; i<NB_GPIO_ITER; i++)
    {
        pi_gpio_pin_write(&gpio_out[0], output[0], 1);
        wait_us(5);
        pi_gpio_pin_write(&gpio_out[0], output[0], 0);
        wait_us(5);

        pi_gpio_pin_write(&gpio_out[1], output[1], 1);
        wait_us(5);
        pi_gpio_pin_write(&gpio_out[1], output[1], 0);
        wait_us(5);

        pi_gpio_pin_write(&gpio_out[2], output[2], 1);
        wait_us(5);
        pi_gpio_pin_write(&gpio_out[2], output[2], 0);
        wait_us(5);
        
        pi_gpio_pin_write(&gpio_out[3], output[3], 1);
        wait_us(5);
        pi_gpio_pin_write(&gpio_out[3], output[3], 0);
        /*
           pi_gpio_pin_write(&gpio_out[4], output[4], current_val);
           wait_us(10);
           pi_gpio_pin_write(&gpio_out[5], output[5], current_val);
           */

        pi_udma_fifo_ioctl(&ts_fifo_device, PI_UDMA_FIFO_GET_LEVEL, &fifo_size);

        for (uint32_t i=0; i<(fifo_size/4); i++)
        {
            pi_udma_fifo_ioctl(&ts_fifo_device, PI_UDMA_FIFO_POP32, &timestamp_val[i]);
        }

        pi_udma_fifo_ioctl(&ts_fifo_device, PI_UDMA_FIFO_GET_LEVEL, &fifo_size);

#ifdef TH_TEST
        int test_diff[4];

        /* Calcul the diff between 2 timestamps */
        ts_diff[0] = timestamp_val[1] - timestamp_val[0];
        ts_diff[1] = timestamp_val[3] - timestamp_val[2];
        ts_diff[2] = timestamp_val[5] - timestamp_val[4];
        ts_diff[3] = timestamp_val[7] - timestamp_val[6];

        /* Calcul the diff precision */
        test_diff[0] = __builtin_pulp_abs((ts_diff[1] - ts_diff[0]));
        test_diff[1] = __builtin_pulp_abs((ts_diff[2] - ts_diff[1]));
        test_diff[2] = __builtin_pulp_abs((ts_diff[3] - ts_diff[2]));

        if ( test_diff[0] > TH_TEST || test_diff[1] > TH_TEST || test_diff[2] > TH_TEST)
        {
            //printf("test diff 0: %d 1: %d, 2: %d\n", test_diff[0], test_diff[1], test_diff[2]);
            err++;
        }

#endif

        current_val ^= 1;
        fifo_size = 0;
    }

    for (int i=0; i<NB_PAIR_GPIO; i++)
    {
        output[i] = gpio_loopbacks[i][0];
        input[i] = gpio_loopbacks[i][1];

        pi_testbench_gpio_loopback(bench, output[i], input[i], 0);
    } 

    return err;
}


static int test_entry()
{
    int errors = 0;
    int index = 0;

    struct pi_testbench_conf conf;
    pi_device_t bench;

    /* Init 1st timestamp with a new fifo */
    uint32_t fifo_id = 0xFF;
    timestamp_init(&fifo_id);

    pi_testbench_conf_init(&conf);

    pi_open_from_conf(&bench, &conf);

    if (pi_testbench_open(&bench))
        return -1;

    for (int i=0; i<NB_PAIR_GPIO; i++)
    {
        int output = gpio_loopbacks[index][0];
        int input = gpio_loopbacks[index][1];

        errors += init_gpio_loopback(&bench, output, input, index);

        index++;
    } 

    timestamp_gpio_input_init(fifo_id);

    errors = run_gpio_test(&bench);

    pi_udma_timestamp_close(&gpio_timestamp);

    if(errors)
        printf("TKO\n");
    else
        printf("TOK\n");

    return errors;
}

static void test_kickoff(void *arg)
{
    int ret = test_entry();
    pmsis_exit(ret);
}

int main()
{
    return pmsis_kickoff((void *)test_kickoff);
}
