#include <stdio.h>
#include <pmsis.h>
#include <stdint.h>
#include "testbench.h"

#if TEST_DURATION >= 100
#define BUFFER_SIZE 4096
#define NB_BUFFERS 4

#elif TEST_DURATION >= 75
#define BUFFER_SIZE 2048
#define NB_BUFFERS 4

#elif TEST_DURATION >= 50
#define BUFFER_SIZE 1024
#define NB_BUFFERS 4

#elif TEST_DURATION >= 25
#define BUFFER_SIZE 64
#define NB_BUFFERS 1

#else
#define BUFFER_SIZE 1024
#define NB_BUFFERS 4
#endif

#define NB_BUFFERS 4
#define TRANSFER_SIZE (BUFFER_SIZE/NB_BUFFERS)


#define NB_ITF  4
#define NB_CS   4


typedef struct
{
    int id;
    int itf;
    int cs;
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
    pi_task_t end_task;
    int buffer_size;
    int transfer_size;
    int current_size;
} spi_dev_t;

static spi_dev_t dev[4][4];

static void spi_dev_handler(void *arg);


static int spim_verif_enable(int itf, int cs)
{
  static pi_device_t bench;
  struct pi_testbench_conf bench_conf;
  pi_testbench_conf_init(&bench_conf);

  bench_conf.uart_id = TESTBENCH_UART_ID;
  bench_conf.uart_baudrate = TESTBENCH_UART_BAUDRATE;

  pi_open_from_conf(&bench, &bench_conf);

  if (pi_testbench_open(&bench))
      return -1;

  if (pi_testbench_spim_verif_setup(&bench, 1, itf, cs, 0, 0, 20))
    return -1;

  return 0;
}


static int spi_dev_init(spi_dev_t *dev, int itf, int cs, int buffer_size, int transfer_size)
{
    static int id = 0;
    struct pi_spi_conf conf;
    dev->step = 0;
    dev->id = id++;
    dev->buffer_id = id % 3;
    dev->itf = itf;
    dev->cs = cs;
    dev->buffer_size = buffer_size;
    dev->transfer_size = transfer_size;
    dev->tx_buffer = pi_l2_malloc(buffer_size);
    dev->rx_buffer = pi_l2_malloc(buffer_size);
    pi_task_block(&dev->end_task);

    if (spim_verif_enable(itf, cs))
      return -1;

    pi_spi_conf_init(&conf);
    conf.wordsize = PI_SPI_WORDSIZE_32;
    conf.big_endian = 1;
    conf.max_baudrate = 10000000;
    conf.polarity = 0;
    conf.phase = 0;
    conf.itf = itf;
    conf.cs = cs;

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
    uint32_t irq = pi_irq_disable();
    spi_dev_t *dev = (spi_dev_t *)arg;

    switch (dev->step)
    {
        case 0:
            if (dev->current_size == 0)
            {
                set_spim_verif_command(&dev->spim, 0x1, 0, dev->transfer_size, dev->cmd_buffer, pi_task_callback(&dev->task, spi_dev_handler, (void *)dev));
            }
            else
            {
                set_spim_verif_command(&dev->spim, 0x3, 0, dev->transfer_size, dev->cmd_buffer, pi_task_callback(&dev->task, spi_dev_handler, (void *)dev));
            }
            dev->step = 1;
            break;

        case 1:
            if (dev->current_size == 0)
            {
                pi_spi_send_async(&dev->spim, dev->current_tx_buffer, dev->transfer_size*8, PI_SPI_CS_AUTO, pi_task_callback(&dev->task, spi_dev_handler, (void *)dev));
            }
            else
            {
                pi_spi_transfer_async(&dev->spim, dev->current_tx_buffer, dev->current_rx_buffer, dev->transfer_size*8, PI_SPI_CS_AUTO, pi_task_callback(&dev->task, spi_dev_handler, (void *)dev));
                dev->current_rx_buffer += dev->transfer_size;
            }
            dev->current_tx_buffer += dev->transfer_size;
            dev->current_size += dev->transfer_size;
            if (dev->current_size == dev->buffer_size)
            {
                dev->step = 2;
            }
            else
            {
                dev->step = 0;
            }
            break;

        case 2:
            set_spim_verif_command(&dev->spim, 0x2, 0, dev->transfer_size, dev->cmd_buffer, pi_task_callback(&dev->task, spi_dev_handler, (void *)dev));
            dev->step = 3;
            break;

        case 3:
            pi_spi_receive_async(&dev->spim, dev->current_rx_buffer, dev->transfer_size*8, PI_SPI_CS_AUTO, pi_task_callback(&dev->task, spi_dev_handler, (void *)dev));
            dev->step = 4;
            break;

        case 4:
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
    pi_irq_restore(irq);
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
        for (int j=0; j<NB_CS; j++)
        {
            if (i == 1 && j == 0)
                continue;

            if (spi_dev_init(&dev[i][j], i, j, BUFFER_SIZE, TRANSFER_SIZE))
                return -1;
        }
    }

    for (int i=0; i<NB_ITF; i++)
    {
        for (int j=0; j<NB_CS; j++)
        {
            if (i == 1 && j == 0)
                continue;

            spi_dev_start(&dev[i][j]);
        }
    }

    for (int i=0; i<NB_ITF; i++)
    {
        for (int j=0; j<NB_CS; j++)
        {
            if (i == 1 && j == 0)
                continue;

            error += spi_dev_wait(&dev[i][j]);
        }
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
