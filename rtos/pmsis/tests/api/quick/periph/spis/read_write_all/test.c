#include <stdio.h>
#include <pmsis.h>
#include <stdint.h>
#include "testbench.h"

#define BUFFER_SIZE 1024
#define NB_BUFFERS 4
#define TRANSFER_SIZE (BUFFER_SIZE/NB_BUFFERS)


#define NB_ITF  4
#define NB_CS   1


typedef struct
{
    int id;
    int itf;
    int buffer_id;
    int step;
    int error;
    struct pi_device spim;
    int32_t cmd_buffer[2];
    uint8_t *tx_buffer;
    uint8_t *rx_buffer;
    uint8_t *current_tx_buffer;
    uint8_t *current_rx_buffer;
    pi_task_t task;
    pi_task_t task2;
    pi_task_t end_task;
    int buffer_size;
    int transfer_size;
    int current_size;
    pi_testbench_req_spim_verif_transfer_t config;
} spi_dev_t;

static pi_device_t bench;
static spi_dev_t dev[4];

static void spi_dev_handler(void *arg);


static int spim_verif_enable(int itf)
{
  struct pi_testbench_conf bench_conf;
  pi_testbench_conf_init(&bench_conf);

  bench_conf.uart_id = CONFIG_TESTBENCH_UART_ID;
  bench_conf.uart_baudrate = CONFIG_TESTBENCH_UART_BAUDRATE;

  pi_open_from_conf(&bench, &bench_conf);

  if (pi_testbench_open(&bench))
      return -1;

  pi_testbench_req_spim_verif_setup_t config = {
    .enabled=1, .itf=itf, .cs=0, .is_master=1, .mem_size_log2=16
  };
  if (pi_testbench_spim_verif_setup(&bench, &config))
    return -1;

    return 0;
}


static int spi_dev_init(spi_dev_t *dev, int itf, int buffer_size, int transfer_size)
{
    static int id = 0;
    struct pi_spi_conf conf;
    dev->step = 0;
    dev->id = id++;
    dev->buffer_id = id % 3;
    dev->itf = itf;
    dev->buffer_size = buffer_size;
    dev->transfer_size = transfer_size;
    dev->tx_buffer = pi_l2_malloc(buffer_size);
    dev->rx_buffer = pi_l2_malloc(buffer_size);
    pi_task_block(&dev->end_task);

    if (spim_verif_enable(itf))
      return -1;

    pi_spi_conf_init(&conf);
    conf.is_slave = 1;
    conf.itf = itf;

    pi_open_from_conf(&dev->spim, &conf);
    if (pi_spi_open(&dev->spim))
      return 1;

    uint32_t bias = dev->buffer_id * 0x40;

    for (int i=0; i<buffer_size; i++)
    {
      dev->tx_buffer[i] = (i & 0x3f) | bias;
    }

    return 0;
}


static void spi_dev_start(spi_dev_t *dev)
{
    dev->current_tx_buffer = dev->tx_buffer;
    dev->current_rx_buffer = dev->rx_buffer;
    dev->current_size = 0;
    spi_dev_handler((void *)dev);
}



static int spi_dev_wait(spi_dev_t *dev)
{
    pi_task_wait_on(&dev->end_task);
    pi_spi_close(&dev->spim);
    return dev->error;
}


static void set_spim_verif_command(struct pi_device *spim, int cmd, int addr, int size, int32_t *cmd_buffer, pi_task_t *event)
{
  cmd_buffer[0] = (cmd << 24) | (size*8);
  cmd_buffer[1] = addr;
  if (event)
    pi_spi_send_async(spim, cmd_buffer, 8*8, PI_SPI_CS_AUTO, event);
  else
    pi_spi_send(spim, cmd_buffer, 8*8, PI_SPI_CS_AUTO);
}


static void spi_dev_handler(void *arg)
{
    spi_dev_t *dev = (spi_dev_t *)arg;

    //printf("STAR DEV %p\n", dev);
    switch (dev->step)
    {
        case 0:
            if (dev->current_size == 0)
            {
                pi_spi_send_async(&dev->spim, dev->current_tx_buffer, dev->transfer_size*8, 0, pi_task_callback(&dev->task, spi_dev_handler, (void *)dev));

                dev->config.frequency = 10000000;
                dev->config.address = 0;
                dev->config.size = dev->transfer_size;
                dev->config.is_rx = 1;
                dev->config.is_duplex = 0;
                pi_testbench_spim_verif_transfer_async(&bench, dev->itf, 0, &dev->config, pi_task_block(&dev->task2));
            }
            else
            {
                pi_spi_transfer_async(&dev->spim, dev->current_tx_buffer, dev->current_rx_buffer, dev->transfer_size*8, 0, pi_task_callback(&dev->task, spi_dev_handler, (void *)dev));

                dev->config.frequency = 10000000;
                dev->config.address = 0;
                dev->config.size = dev->transfer_size;
                dev->config.is_rx = 0;
                dev->config.is_duplex = 1;
                pi_testbench_spim_verif_transfer_async(&bench, dev->itf, 0, &dev->config, pi_task_block(&dev->task2));
                dev->current_rx_buffer += dev->transfer_size;
            }
            dev->current_tx_buffer += dev->transfer_size;
            dev->current_size += dev->transfer_size;
            if (dev->current_size == dev->buffer_size)
            {
                dev->step = 1;
            }
            else
            {
                dev->step = 0;
            }
            break;

        case 1:
        {
            pi_spi_receive_async(&dev->spim, dev->current_rx_buffer, dev->transfer_size*8, 0, pi_task_callback(&dev->task, spi_dev_handler, (void *)dev));

            dev->config.frequency = 10000000;
            dev->config.address = 0;
            dev->config.size = dev->transfer_size;
            dev->config.is_rx = 0;
            dev->config.is_duplex = 0;
            pi_testbench_spim_verif_transfer_async(&bench, dev->itf, 0, &dev->config, pi_task_block(&dev->task2));
            dev->step = 2;
            break;
        }

        case 2:
        {

            int error = 0;
            for (int i=0; i<dev->buffer_size; i++)
            {

                int expected = dev->buffer_id == 0 ? i & 0x3f : dev->buffer_id == 1 ? (i & 0x3f) | 0x40 : (i & 0x3f) | 0x80;
                if (dev->rx_buffer[i] != expected)
                {
                    printf("Error at index %d, expected 0x%x, got 0x%x\n", i, expected, dev->rx_buffer[i]);
                    error++;
                }
            }

            dev->error = error;
            pi_task_push(&dev->end_task);
            break;
        }
    }
}



static int test_entry()
{
    int error = 0;

#ifdef __GAP9__
    pi_pad_set_function(PI_PAD_026, 0);
    pi_pad_set_function(PI_PAD_027, 0);
    pi_pad_set_function(PI_PAD_028, 0);
    pi_pad_set_function(PI_PAD_029, 0);
    pi_pad_set_function(PI_PAD_030, 0);
    pi_pad_set_function(PI_PAD_031, 0);
    pi_pad_set_function(PI_PAD_032, 0);
    pi_pad_set_function(PI_PAD_033, 0);
    pi_pad_set_function(PI_PAD_034, 0);
    pi_pad_set_function(PI_PAD_035, 0);
    pi_pad_set_function(PI_PAD_036, 0);
    pi_pad_set_function(PI_PAD_037, 0);
    pi_pad_set_function(PI_PAD_038, 0);
    pi_pad_set_function(PI_PAD_039, 0);
    pi_pad_set_function(PI_PAD_053, 2);
    pi_pad_set_function(PI_PAD_054, 2);
    pi_pad_set_function(PI_PAD_055, 2);
    pi_pad_set_function(PI_PAD_056, 2);
    pi_pad_set_function(PI_PAD_057, 2);
    pi_pad_set_function(PI_PAD_058, 2);
    pi_pad_set_function(PI_PAD_059, 2);
#ifdef __GAP9_5__
    pi_pad_set_function(PI_PAD_074, 2);
    pi_pad_set_function(PI_PAD_075, 2);
    pi_pad_set_function(PI_PAD_076, 2);
    pi_pad_set_function(PI_PAD_077, 2);
    pi_pad_set_function(PI_PAD_078, 2);
    pi_pad_set_function(PI_PAD_079, 2);
    pi_pad_set_function(PI_PAD_080, 2);
#else
    pi_pad_set_function(PI_PAD_070, 2);
    pi_pad_set_function(PI_PAD_071, 2);
    pi_pad_set_function(PI_PAD_072, 2);
    pi_pad_set_function(PI_PAD_073, 2);
    pi_pad_set_function(PI_PAD_074, 2);
    pi_pad_set_function(PI_PAD_075, 2);
    pi_pad_set_function(PI_PAD_076, 2);
#endif
#endif

    for (int i=0; i<NB_ITF; i++)
    {
        if (i == 1)
            continue;

        if (spi_dev_init(&dev[i], i, BUFFER_SIZE, TRANSFER_SIZE))
            return -1;
    }

    for (int i=0; i<NB_ITF; i++)
    {
        if (i == 1)
            continue;

        spi_dev_start(&dev[i]);
    }

    for (int i=0; i<NB_ITF; i++)
    {
        if (i == 1)
            continue;

        error += spi_dev_wait(&dev[i]);
    }

    if (error)
    {
        printf("Got %d errors\n", error);
    }
    else
    {
      printf("Test success\n");
    }

    return error;
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
