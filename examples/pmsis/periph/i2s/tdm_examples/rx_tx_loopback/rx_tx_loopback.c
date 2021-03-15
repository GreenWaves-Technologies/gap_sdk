/**
 * This example shows the loopback feature on I2S.
 * Incoming data on RX are sent back on TX. Received data will not be stored in
 * memory.
 */

#include "pmsis.h"
#include "bsp/bsp.h"

#define NB_CHANNELS        ( 4 )
#define NB_ACTIVE_CHANNELS ( 4 )

static struct pi_device i2s;

static int test_entry()
{
    int32_t errors = 0;

    printf("Entering main controller\n");

    // The configuration given when the driver is opened, is the global one
    // which applies to all channels like the number of channels, sampling rate
    // and so on.
    struct pi_i2s_conf i2s_conf = {0};
    pi_i2s_conf_init(&i2s_conf);

    i2s_conf.frame_clk_freq = 44100;
    i2s_conf.itf = 0;
    i2s_conf.word_size = 32;
    i2s_conf.channels = NB_CHANNELS;
    i2s_conf.options = PI_I2S_OPT_TDM | PI_I2S_OPT_FULL_DUPLEX;

    pi_open_from_conf(&i2s, &i2s_conf);
    errors = pi_i2s_open(&i2s);

    if (errors)
    {
        printf("Error opening i2s_%d : %lx\n", errors, i2s_conf.itf);
        return -1;
    }

    // Now that the global configuration is given, configure each channel
    // independently.
    for (uint8_t i=0; i<(uint8_t) NB_ACTIVE_CHANNELS; i++)
    {
        // Enabled TX for slot i with pingpong buffers
        i2s_conf.channel_id = i;
        i2s_conf.options = PI_I2S_OPT_IS_TX | PI_I2S_OPT_ENABLED | PI_I2S_OPT_LOOPBACK;
        pi_i2s_ioctl(&i2s, PI_I2S_IOCTL_CONF_SET, &i2s_conf);
    }

    printf("Start receiving and sending...\n");
    // Start sampling.
    // Starting from there, the driver will alternate between the 2 pingpong
    // buffers for each channel.
    if (pi_i2s_ioctl(&i2s, PI_I2S_IOCTL_START, NULL))
    {
        return -2;
    }

    while (1);

    // Sampling is done, close everything
    pi_i2s_ioctl(&i2s, PI_I2S_IOCTL_STOP, NULL);
    pi_i2s_close(&i2s);

    return errors;
}

void test_kickoff(void *arg)
{
    int ret = test_entry();
    pmsis_exit(ret);
}

int main()
{
    printf("PMSIS I2S TDM RX TX loopback example\n");
    return pmsis_kickoff((void *) test_kickoff);
}
