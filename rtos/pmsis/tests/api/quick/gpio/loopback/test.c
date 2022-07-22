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

#define NB_GPIO_ITER 20


static int gpio_loopbacks[][2] = {
#ifdef GAP9_WLCSP
    { 48, 49 },
    { 86, 87 },
    { -1, -1 }
#else
    { 14, 15 },
    { 48, 49 },
    { 78, 79 },
#if defined(__GAP9_5__)
    { 90, 91 },
#else
    { 86, 87 },
#endif
    { -1, -1 }
#endif
};



int check_gpio_loopback(pi_device_t *bench, int output, int input)
{
    int errors = 0;

    printf("Checking GPIO loopback (output: %d, input: %d)\n", output, input);

    struct pi_gpio_conf gpio_conf;
    pi_device_t gpio_in;
    pi_device_t gpio_out;

    pi_gpio_conf_init(&gpio_conf);

    gpio_conf.port = input / 32;

    pi_open_from_conf(&gpio_in, &gpio_conf);

    if (pi_gpio_open(&gpio_in))
        return -1;

    gpio_conf.port = output / 32;

    pi_open_from_conf(&gpio_out, &gpio_conf);

    if (pi_gpio_open(&gpio_out))
        return -1;

    
    if (pi_gpio_pin_configure(&gpio_in, input, PI_GPIO_INPUT))
        return -1;

    if (pi_gpio_pin_configure(&gpio_out, output, PI_GPIO_OUTPUT))
        return -1;

    // First drive value from testbench to avoid 'X' propagation
    // when setting GPIO pads
    pi_testbench_gpio_loopback(bench, output, input, 1);

    pi_pad_set_function(input, 1);
    pi_pad_set_function(output, 1);

    int current_val = 1;

    for (int i=0; i<NB_GPIO_ITER; i++)
    {
        pi_gpio_pin_write(&gpio_out, output, current_val);

        uint32_t value;
        pi_gpio_pin_read(&gpio_in, input, &value);

        if (value != current_val)
        {
            printf("Read wrong GPIO value (expected %d, got %d)\n", current_val, value);
            errors++;
            break;
        }

        current_val ^= 1;
    }


    pi_testbench_gpio_loopback(bench, output, input, 0);

    return errors;
}


static int test_entry()
{
    printf("Entering test\n");

    int errors = 0;
    int index = 0;

    struct pi_testbench_conf conf;
    pi_device_t bench;

    pi_testbench_conf_init(&conf);

    pi_open_from_conf(&bench, &conf);

    if (pi_testbench_open(&bench))
        return -1;

    while(gpio_loopbacks[index][0] != -1)
    {
        int output = gpio_loopbacks[index][0];
        int input = gpio_loopbacks[index][1];

        errors += check_gpio_loopback(&bench, output, input);

        index++;
    } 

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