#include <stdio.h>
#include <pmsis.h>
#include <stdint.h>

#define BUFFER_SIZE 256
#define NB_ITER 4

static pi_device_t spi;

static int test_entry()
{
    struct pi_spi_conf conf;
    pi_task_t task_spis;

    printf("STA\n");

#if 0
    pi_pad_set_function(PI_PAD_026, 0); //spi0_sck
    pi_pad_set_function(PI_PAD_027, 0); //spi0_cs
    pi_pad_set_function(PI_PAD_031, 0); //spi0_sdo
    pi_pad_set_function(PI_PAD_032, 0); //spi0_sdi
#endif

    pi_pad_set_function(PI_PAD_033, 0); //spi1_sck
    pi_pad_set_function(PI_PAD_034, 0); //spi1_cs
    pi_pad_set_function(PI_PAD_038, 0); //spi1_sdo
    pi_pad_set_function(PI_PAD_039, 0); //spi1_sdi

// Open SPI slave
    pi_spi_conf_init(&conf);

    conf.is_slave = 1;
    conf.wordsize = PI_SPI_WORDSIZE_8;
    conf.big_endian = 1;
    conf.max_baudrate = 500000;
    conf.itf = SPIS_ITF;
    conf.polarity = POLARITY;
    conf.phase = PHASE;
    conf.cs = SPIS_CS;

    pi_open_from_conf(&spi, &conf);

    if (pi_spi_open(&spi))
        goto error;

    void *rx_buffer = pmsis_l2_malloc(BUFFER_SIZE);
    if (rx_buffer == NULL)
        goto error;


    for (int i=0; i<BUFFER_SIZE; i++)
    {
        ((uint8_t *)rx_buffer)[i] = 0x5C;
    }

    printf("slave rx start\n");
    pi_spi_receive_async(&spi, rx_buffer, (BUFFER_SIZE-1)*8, 0, pi_task_block(&task_spis));
    pi_task_wait_on(&task_spis);
    printf("slave done\n");


    for (int i=0; i<BUFFER_SIZE; i++)
    {
        printf("RX (index: %d, value: %x)\n", i, ((uint8_t *)rx_buffer)[i]);
    }

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
