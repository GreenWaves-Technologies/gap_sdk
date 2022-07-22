#include <stdio.h>
#include <pmsis.h>
#include <stdint.h>
#include "testbench.h"



#if TEST_DURATION >= 100
#define BUFFER_SIZE 4096
#define NB_ITER 4

#elif TEST_DURATION >= 75
#define BUFFER_SIZE 2048
#define NB_ITER 4

#elif TEST_DURATION >= 50
#define BUFFER_SIZE 1024
#define NB_ITER 4

#elif TEST_DURATION >= 25
#define BUFFER_SIZE 64
#define NB_ITER 1

#else
#define BUFFER_SIZE 1024
#define NB_ITER 4
#endif

static pi_device_t bench;
static pi_device_t spi;


static int spim_verif_enable(int itf, int cs)
{
  struct pi_testbench_conf bench_conf;
  pi_testbench_conf_init(&bench_conf);

  bench_conf.uart_id = CONFIG_TESTBENCH_UART_ID;
  bench_conf.uart_baudrate = CONFIG_TESTBENCH_UART_BAUDRATE;

  pi_open_from_conf(&bench, &bench_conf);

  if (pi_testbench_open(&bench))
      return -1;

  pi_testbench_req_spim_verif_setup_t config = {
    .enabled=1, .itf=itf, .cs=cs, .is_master=1, .mem_size_log2=16
  };
  if (pi_testbench_spim_verif_setup(&bench, &config))
    return -1;

    return 0;
}


static int test_entry()
{
    struct pi_spi_conf conf;
    pi_task_t task;

    printf("STA\n");

    for (int j=0; j<NB_ITER; j++)
    {
        spim_verif_enable(SPI_ITF, SPI_CS);
    }

    pi_spi_conf_init(&conf);

    conf.is_slave = 1;
    conf.itf = SPI_ITF;

    pi_open_from_conf(&spi, &conf);

    if (pi_spi_open(&spi))
        goto error;

    void *tx_buffer = pmsis_l2_malloc(BUFFER_SIZE);
    if (tx_buffer == NULL)
        goto error;

    void *rx_buffer = pmsis_l2_malloc(BUFFER_SIZE);
    if (rx_buffer == NULL)
        goto error;

    for (int j=0; j<NB_ITER; j++)
    {

        for (int i=0; i<BUFFER_SIZE; i++)
        {
          ((uint8_t *)tx_buffer)[i] = 0x3C + i;
        }

        pi_spi_send_async(&spi, tx_buffer, BUFFER_SIZE*8, 0, pi_task_block(&task));

        pi_testbench_req_spim_verif_transfer_t config = {
            .frequency=10000000, .address=0, .size=BUFFER_SIZE, .is_master=0, .is_rx=1, .is_duplex=0,
        };

        pi_testbench_spim_verif_transfer(&bench, SPI_ITF, SPI_CS, &config);

        pi_task_wait_on(&task);


        pi_spi_receive_async(&spi, rx_buffer, BUFFER_SIZE*8, 0, pi_task_block(&task));

        pi_testbench_req_spim_verif_transfer_t config2 = {
            .frequency=10000000, .address=0, .size=BUFFER_SIZE, .is_master=0, .is_rx=0, .is_duplex=0,
        };

        pi_testbench_spim_verif_transfer(&bench, SPI_ITF, SPI_CS, &config2);

        pi_task_wait_on(&task);

        for (int i=0; i<BUFFER_SIZE; i++)
        {
            uint8_t expected = 0x3C + i;
            if (((uint8_t *)rx_buffer)[i] != expected)
            {
                printf("Error detected during RX (iter: %d, index: %d, value: %x, expected: %x)\n", j, i, ((uint8_t *)rx_buffer)[i], expected);
                goto error;
            }
        }


        for (int i=0; i<BUFFER_SIZE; i++)
        {
          ((uint8_t *)rx_buffer)[i] = 0;
        }

        for (int i=0; i<BUFFER_SIZE; i++)
        {
          ((uint8_t *)tx_buffer)[i] = 0x3C + i*3;
        }


        // Do full duplex transfer
        pi_spi_transfer_async(&spi, tx_buffer, rx_buffer, BUFFER_SIZE*8, 0, pi_task_block(&task));

        pi_testbench_req_spim_verif_transfer_t config3 = {
            .frequency=10000000, .address=0, .size=BUFFER_SIZE, .is_master=0, .is_rx=0, .is_duplex=1,
        };

        pi_testbench_spim_verif_transfer(&bench, SPI_ITF, SPI_CS, &config3);

        pi_task_wait_on(&task);

        // Check that what we received during the full duplex transfer is matching the previous transfer
        for (int i=0; i<BUFFER_SIZE; i++)
        {
            uint8_t expected = 0x3C + i;
            if (((uint8_t *)rx_buffer)[i] != expected)
            {
                printf("Error detected during full duplex on RX (iter: %d, index: %d, value: %x, expected: %x)\n", j, i, ((uint8_t *)rx_buffer)[i], expected);
                goto error;
            }
        }

        // Read the new content
        pi_spi_receive_async(&spi, rx_buffer, BUFFER_SIZE*8, 0, pi_task_block(&task));

        pi_testbench_req_spim_verif_transfer_t config4 = {
            .frequency=10000000, .address=0, .size=BUFFER_SIZE, .is_master=0, .is_rx=0, .is_duplex=0,
        };

        pi_testbench_spim_verif_transfer(&bench, SPI_ITF, SPI_CS, &config4);

        pi_task_wait_on(&task);


        // And check that it is matching what we sent during full duplex transfer
        for (int i=0; i<BUFFER_SIZE; i++)
        {
            uint8_t expected = 0x3C + i*3;
            if (((uint8_t *)rx_buffer)[i] != expected)
            {
                printf("Error detected during full duplex on TX (iter: %d, index: %d, value: %x, expected: %x)\n", j, i, ((uint8_t *)rx_buffer)[i], expected);
                goto error;
            }
        }
    }

    printf("TOK\n");

    return 0;

error:
    printf("TKO\n");

    return -1;
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
