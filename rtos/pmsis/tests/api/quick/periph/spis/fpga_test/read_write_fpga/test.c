#include <stdio.h>
#include <pmsis.h>
#include <stdint.h>

#define BUFFER_SIZE 256
#define NB_ITER 4

static pi_device_t spi;
static pi_device_t spim;



static int test_entry()
{
    struct pi_spi_conf conf;
    struct pi_spi_conf spim_conf;
    pi_task_t task_spis;
    pi_task_t task_spim;

    printf("STA\n");

    pi_pad_set_function(PI_PAD_026, 0); //spi0_sck
    pi_pad_set_function(PI_PAD_027, 0); //spi0_cs
    pi_pad_set_function(PI_PAD_031, 0); //spi0_sdo
    pi_pad_set_function(PI_PAD_032, 0); //spi0_sdi

    pi_pad_set_function(PI_PAD_033, 0); //spi1_sck
    pi_pad_set_function(PI_PAD_034, 0); //spi1_cs
    pi_pad_set_function(PI_PAD_038, 0); //spi1_sdo
    pi_pad_set_function(PI_PAD_039, 0); //spi1_sdi

// Open SPI Master
    pi_spi_conf_init(&spim_conf);

    spim_conf.wordsize = PI_SPI_WORDSIZE_8;
    spim_conf.big_endian = 1;
    spim_conf.max_baudrate = 10000000;
    spim_conf.polarity = POLARITY;
    spim_conf.phase = PHASE;
    spim_conf.itf = SPIM_ITF;
    spim_conf.cs = SPIM_CS;

    pi_open_from_conf(&spim, &spim_conf);

    if (pi_spi_open(&spim))
        return -1;

#ifdef LOOPBACK_TEST
// Open SPI slave
    pi_spi_conf_init(&conf);

    conf.is_slave = 1;
    conf.wordsize = PI_SPI_WORDSIZE_8;
    conf.big_endian = 1;
    conf.max_baudrate = 10000000;
    conf.itf = SPIS_ITF;
    conf.polarity = POLARITY;
    conf.phase = PHASE;
    conf.cs = SPIS_CS;

    pi_open_from_conf(&spi, &conf);

    if (pi_spi_open(&spi))
        goto error;
#endif

    void *tx_buffer = pmsis_l2_malloc(BUFFER_SIZE);
    if (tx_buffer == NULL)
        goto error;

    void *rx_buffer = pmsis_l2_malloc(BUFFER_SIZE);
    if (rx_buffer == NULL)
        goto error;


    for (int i=0; i<BUFFER_SIZE; i++)
    {
        ((uint8_t *)tx_buffer)[i] = i;
    }

#ifdef LOOPBACK_TEST
    pi_spi_receive_async(&spi, rx_buffer, (BUFFER_SIZE-1)*8, 0, pi_task_block(&task_spis));
#endif
    pi_spi_send_async(&spim, tx_buffer, BUFFER_SIZE*8, 0, pi_task_block(&task_spim));

#ifdef LOOPBACK_TEST
    pi_task_wait_on(&task_spis);
    printf("slave done\n");
#endif

    pi_task_wait_on(&task_spim);
    printf("transfer done \n");

#ifdef LOOPBACK_TEST
    for (int i=0; i<BUFFER_SIZE; i++)
    {
        uint8_t expected = i+1;
        if (((uint8_t *)rx_buffer)[i] != expected)
        {
            printf("Error detected during RX (index: %d, value: %x, expected: %x)\n", i, ((uint8_t *)rx_buffer)[i], expected);
            goto error;
        }
    }
#endif

    printf("TOK\n");

    return 0;

error:
    printf("TKO\n");

    return -1;
}

static void test_kickoff(void *arg)
{
  printf("Test with frequency %d\n", CONFIG_FREQUENCY_FPGA);
  int ret = test_entry();
  pmsis_exit(ret);
}

int main()
{
  return pmsis_kickoff((void *)test_kickoff);
}
