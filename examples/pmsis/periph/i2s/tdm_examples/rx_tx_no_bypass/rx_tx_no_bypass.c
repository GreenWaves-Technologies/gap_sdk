/**
 * This example shows how to use I2S TDM to sample data from multiple channels.
 * It receives the samples from I2S channels and send them to the hyperrram to
 * respect real-time constraints. Once the sampling is done, it reads back
 * the samples from hyperram and send them to a file on the workstation through
 * JTAG.
 * When sampling data on RX, different data can be sent on TX from memory. Received data
 * will be stored in memory.
 */

#include "pmsis.h"
#include "bsp/bsp.h"
#include "wav_out.h"

#define SAVE_SAMPLES 1
//#define DUMP_SAMPLES 1
#define DUMP_WAV 1

#define NB_ELEM            ( 256 )
#define WHOLE_SIZE         ( NB_ELEM * 256 * 16 )

#define ELEM_SIZE          ( sizeof(uint32_t) )
#define BUFF_SIZE          ( NB_ELEM * ELEM_SIZE )

#define NB_CHANNELS        ( 4 )
#define NB_ACTIVE_CHANNELS ( 4 )

static struct pi_device i2s;

PI_L2 static uint8_t ch_buff_rx[NB_CHANNELS][2][BUFF_SIZE];
PI_L2 static uint8_t ch_buff_tx[NB_CHANNELS][2][BUFF_SIZE];

static void buffer_init(void *buffer, uint32_t size, uint8_t slot_id)
{
    uint8_t *buff = (uint8_t *) buffer;
    for (uint32_t i=0; i<size; i++)
    {
        buff[i] = i + 0x55 + slot_id;
    }
}

static int test_entry()
{
    int32_t errors = 0;

    printf("Entering main controller\n");

    #if SAVE_SAMPLES
    struct pi_device ram;
    struct pi_hyperram_conf ram_conf;
    pi_hyperram_conf_init(&ram_conf);
    pi_open_from_conf(&ram, &ram_conf);
    if (pi_ram_open(&ram))
    {
        printf("Error ram open !\n");
        return -1;
    }
    uint32_t hyper_buff[NB_ACTIVE_CHANNELS] = {0};
    uint32_t cur_addr[NB_ACTIVE_CHANNELS] = {0};
    for (uint8_t i=0; i<(uint8_t) NB_ACTIVE_CHANNELS; i++)
    {
        if (pi_ram_alloc(&ram, &hyper_buff[i], (uint32_t) WHOLE_SIZE))
        {
            printf("Ram malloc failed !\n");
            return -2;
        }
        else
        {
            printf("Ram allocated : %lx %ld.\n", hyper_buff[i], (uint32_t) WHOLE_SIZE);
            cur_addr[i] = hyper_buff[i];
        }
    }
    #endif  /* SAVE_SAMPLES */

    // Then prepare the I2S channel
    pi_bsp_init();

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
        return -3;
    }

    // Now that the global configuration is given, configure each channel
    // independently.
    for (uint8_t i=0; i<(uint8_t) NB_ACTIVE_CHANNELS; i++)
    {
        // Enabled RX for slot i with pingpong buffers
        i2s_conf.channel_id = i;
        i2s_conf.options = PI_I2S_OPT_IS_RX | PI_I2S_OPT_PINGPONG | PI_I2S_OPT_ENABLED;
        i2s_conf.block_size = BUFF_SIZE;
        i2s_conf.pingpong_buffers[0] = ch_buff_rx[i][0];
        i2s_conf.pingpong_buffers[1] = ch_buff_rx[i][1];
        pi_i2s_ioctl(&i2s, PI_I2S_IOCTL_CONF_SET, &i2s_conf);
    }

    for (uint8_t i=0; i<(uint8_t) NB_ACTIVE_CHANNELS; i++)
    {
        i2s_conf.channel_id = i;
        i2s_conf.options = PI_I2S_OPT_IS_TX | PI_I2S_OPT_PINGPONG | PI_I2S_OPT_ENABLED;
        i2s_conf.block_size = BUFF_SIZE;
        i2s_conf.pingpong_buffers[0] = ch_buff_tx[i][0];
        i2s_conf.pingpong_buffers[1] = ch_buff_tx[i][1];
        pi_i2s_ioctl(&i2s, PI_I2S_IOCTL_CONF_SET, &i2s_conf);

        /* Init data buffer to send. */
        buffer_init(ch_buff_tx[i][0], BUFF_SIZE, i2s_conf.channel_id);
        buffer_init(ch_buff_tx[i][1], BUFF_SIZE, i2s_conf.channel_id);
    }

    printf("Start receiving and sending...\n");
    // Start sampling.
    // Starting from there, the driver will alternate between the 2 pingpong
    // buffers for each channel.
    if (pi_i2s_ioctl(&i2s, PI_I2S_IOCTL_START, NULL))
    {
        return -4;
    }

    for (uint8_t i=0; i<(uint8_t) NB_ACTIVE_CHANNELS; i++)
    {
        /* Buffer arg is NULL because we are using pingpong buffers. */
        pi_i2s_channel_write(&i2s, i, NULL, BUFF_SIZE);
    }

    #if SAVE_SAMPLES
    uint16_t *chunk;
    uint32_t size;
    for (int j=0; j<WHOLE_SIZE; j++)
    {
        for (int i=0; i<(int) NB_ACTIVE_CHANNELS; i++)
        {
            // For each channel, wait for one buffer of sample and immediately
            // send it to flash. Another buffer is already being filled in the
            // meantime.
            pi_i2s_channel_read(&i2s, i, (void **)&chunk, (size_t *)&size);
            pi_ram_write(&ram, cur_addr[i], chunk, size);

            cur_addr[i] += size;
        }
        j += size;
    }
    for (uint8_t i=0; i<(uint8_t) NB_ACTIVE_CHANNELS; i++)
    {
        cur_addr[i] = hyper_buff[i];
    }
    printf("Finished sampling\n");

    // Sampling is done, close everything
    pi_i2s_ioctl(&i2s, PI_I2S_IOCTL_STOP, NULL);
    pi_i2s_close(&i2s);

    // Finally, read back the samples from flash and send them to files
    // through JTAG
    uint16_t *temp_buff = (uint16_t *) ch_buff_rx[0][0];
    for (int chan=0; chan<(int) NB_ACTIVE_CHANNELS; chan++)
    {
        #ifdef DUMP_WAV
        char path[32];
        sprintf(path, "out_file_%d.wav", chan);
        dump_wav_open(path, i2s_conf.word_size, i2s_conf.frame_clk_freq,
                      1, WHOLE_SIZE);
        printf("Dumping chan=%d in %s\n", chan, path);
        #endif  /* DUMP_WAV */

        int whole_size = WHOLE_SIZE - 2048;

        while (whole_size > 0)
        {
            int iter_size = BUFF_SIZE;
            if (iter_size > whole_size)
            {
                iter_size = whole_size;
            }

            pi_ram_read(&ram, cur_addr[chan], temp_buff, iter_size);

            #ifdef DUMP_SAMPLES
            printf("Interface %d / channel %d\n", i2s_conf.itf, chan);
            for (int i=0; i<4; i++)
            {
                printf("  Sample %d: %lx\n", i, temp_buff[i + 32]);
            }
            #endif  /* DUMP_SAMPLES */

            #ifdef DUMP_WAV
            dump_wav_write(temp_buff, iter_size);
            #endif  /* DUMP_WAV */

            cur_addr[chan] += iter_size;
            whole_size -= iter_size;
        }

        #ifdef DUMP_WAV
        dump_wav_close();
        printf("Dump chan=%d in %s done\n", chan, path);
        #endif  /* DUMP_WAV */
    }
    #else
    while (1);

    // Sampling is done, close everything
    pi_i2s_ioctl(&i2s, PI_I2S_IOCTL_STOP, NULL);
    pi_i2s_close(&i2s);
    #endif  /* SAVE_SAMPLES */

    return errors;
}

void test_kickoff(void *arg)
{
    int ret = test_entry();
    pmsis_exit(ret);
}

int main()
{
    printf("PMSIS I2S TDM RX TX no bypass example\n");
    return pmsis_kickoff((void *) test_kickoff);
}
