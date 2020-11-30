/**
 * GPIO input test using push button on Gapuino.
 * Before using check your board configuration.
 */

/* PMSIS includes */
#include "pmsis.h"

#define DELAY_MS 500

/* Variables used. */
struct pi_device gpio_a1;
struct pi_device gpio_led;
struct pi_gpio_conf gpio_conf;

void test_gpio(void)
{
    int32_t errors = 0;
    uint32_t value = 0;
    //Setting pad to alternate 1
    //GPIO A1
    pi_pad_set_function(PI_PAD_12_A3_RF_PACTRL0, PI_PAD_12_A3_GPIO_A0_FUNC1);
    //GPIO LED (A3)
    pi_pad_set_function(PI_PAD_15_B1_RF_PACTRL3, PI_PAD_FUNC1);
    
    pi_gpio_e gpio_out_a1 = PI_GPIO_A0_PAD_12_A3;
    pi_gpio_e gpio_out_led = PI_GPIO_A3_PAD_15_B1;

    /* Configure gpio output. */
    pi_gpio_flags_e cfg_flags = PI_GPIO_OUTPUT;
    pi_gpio_pin_configure(&gpio_a1, gpio_out_a1, cfg_flags);
    pi_gpio_pin_configure(&gpio_led, gpio_out_led, cfg_flags);
    //printf("mamma\n");
    while (1)
    {
        pi_time_wait_us(DELAY_MS*1000);
        pi_gpio_pin_write(&gpio_a1, gpio_out_a1, 0);
        pi_gpio_pin_write(&gpio_led, gpio_out_led, 0);
        pi_time_wait_us(DELAY_MS*1000);
        pi_gpio_pin_write(&gpio_a1, gpio_out_a1, 1);
        pi_gpio_pin_write(&gpio_led, gpio_out_led, 1);
    }
    pmsis_exit(errors);
}

/* Program Entry. */
int main(void)
{
    printf("\n\n\t *** PMSIS GPIO Output + LED ***\n\n");
    return pmsis_kickoff((void *) test_gpio);
}

