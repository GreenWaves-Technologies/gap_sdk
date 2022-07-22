#include <stdio.h>
#include <pmsis.h>
#include <stdint.h>

#ifdef __GAP9__
#include "testbench.h"
#endif

#if !defined(SYNC_CS_AUTO) && !defined(ASYNC_CS_AUTO) && !defined(SYNC_CS_KEEP) && !defined(ASYNC_CS_KEEP)
#define SYNC_CS_AUTO 1
#endif


#if TEST_DURATION >= 100
#define TOTAL_SIZE 4096
#define NB_BUFFERS 4

#elif TEST_DURATION >= 75
#define TOTAL_SIZE 2048
#define NB_BUFFERS 4

#elif TEST_DURATION >= 50
#define TOTAL_SIZE 1024
#define NB_BUFFERS 4

#elif TEST_DURATION >= 25
#define TOTAL_SIZE 64
#define NB_BUFFERS 1

#else
#define TOTAL_SIZE 1024
#define NB_BUFFERS 4
#endif


PI_L2 int32_t cmd_buffer[4][2];

PI_L2 uint8_t *tx_buffer;

PI_L2 uint8_t *rx_buffer;

static pi_task_t cmd_event[NB_BUFFERS];
static pi_task_t buf_event[NB_BUFFERS];


static inline void get_info(int *buffer_size)
{
  *buffer_size = TOTAL_SIZE;
}


#ifdef __GAP9__
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
#endif


static void set_spim_verif_command(struct pi_device *spim, int cmd, int addr, int size, int32_t *cmd_buffer, pi_task_t *event)
{
  cmd_buffer[0] = (cmd << 24) | (size*8);
  cmd_buffer[1] = addr;
  if (event == NULL)
    pi_spi_send(spim, cmd_buffer, 8*8, PI_SPI_CS_AUTO);
  else
    pi_spi_send_async(spim, cmd_buffer, 8*8, PI_SPI_CS_AUTO, event);
}

static int test_entry()
{
  struct pi_spi_conf conf;
  struct pi_device spim;

  int total_size;
  get_info(&total_size);
  int buffer_size = total_size / NB_BUFFERS;

#ifdef __GAP9__
  pi_pad_set_function(PI_PAD_026, 0);
  pi_pad_set_function(PI_PAD_027, 0);
  pi_pad_set_function(PI_PAD_028, 0);
  pi_pad_set_function(PI_PAD_029, 0);
  pi_pad_set_function(PI_PAD_030, 0);
  pi_pad_set_function(PI_PAD_031, 0);
  pi_pad_set_function(PI_PAD_032, 0);

  spim_verif_enable(SPIM_ITF, SPIM_CS);
#endif

  pi_spi_conf_init(&conf);
  conf.wordsize = PI_SPI_WORDSIZE_32;
  conf.big_endian = 1;
  conf.max_baudrate = 10000000;
  conf.polarity = 0;
  conf.phase = 0;
  conf.itf = SPIM_ITF;
  conf.cs = SPIM_CS;

  pi_open_from_conf(&spim, &conf);

  if (pi_spi_open(&spim))
    return -1;

  tx_buffer = pmsis_l2_malloc(total_size);
  if (tx_buffer == NULL) return -1;

  rx_buffer = pmsis_l2_malloc(total_size);
  if (rx_buffer == NULL) return -1;

  for (int i=0; i<total_size; i++)
  {
    tx_buffer[i] = i & 0x7f;
  }

  set_spim_verif_command(&spim, 0x1, 0, total_size, cmd_buffer[0], NULL);
  pi_spi_send(&spim, tx_buffer, total_size*8, PI_SPI_CS_AUTO);



  for (int i=0; i<total_size; i++)
  {
    tx_buffer[i] = i | 0x80;
  }

#if defined(SYNC_CS_AUTO)

  for (int i=0; i<NB_BUFFERS; i++)
  {
    set_spim_verif_command(&spim, 0x3, buffer_size*i, buffer_size, cmd_buffer[i], NULL);
    pi_spi_transfer(&spim, tx_buffer+buffer_size*i, rx_buffer+buffer_size*i, buffer_size*8, PI_SPI_CS_AUTO);
  }

#elif defined(ASYNC_CS_AUTO)

  for (int i=0; i<NB_BUFFERS; i++)
  {
    set_spim_verif_command(&spim, 0x3, buffer_size*i, buffer_size, cmd_buffer[i], pi_task_block(&cmd_event[i]));
    pi_spi_transfer_async(&spim, tx_buffer+buffer_size*i, rx_buffer+buffer_size*i, buffer_size*8, PI_SPI_CS_AUTO, pi_task_block(&buf_event[i]));
  }

  for (int i=0; i<NB_BUFFERS; i++)
  {
    pi_task_wait_on(&cmd_event[i]);
    pi_task_wait_on(&buf_event[i]);
  }

#elif defined(SYNC_CS_KEEP)

  set_spim_verif_command(&spim, 0x3, 0, total_size, cmd_buffer[0], NULL);

  for (int i=0; i<NB_BUFFERS; i++)
  {
    int mode = i == NB_BUFFERS - 1 ? PI_SPI_CS_AUTO : PI_SPI_CS_KEEP;
    pi_spi_transfer(&spim, tx_buffer+buffer_size*i, rx_buffer+buffer_size*i, buffer_size*8, mode);
  }

#elif defined(ASYNC_CS_KEEP)

  set_spim_verif_command(&spim, 0x3, 0, total_size, cmd_buffer[0], NULL);

  for (int i=0; i<NB_BUFFERS; i++)
  {
    int mode = i == NB_BUFFERS - 1 ? PI_SPI_CS_AUTO : PI_SPI_CS_KEEP;
    pi_spi_transfer_async(&spim, tx_buffer+buffer_size*i, rx_buffer+buffer_size*i, buffer_size*8, mode, pi_task_block(&buf_event[i]));
  }

  for (int i=0; i<NB_BUFFERS; i++)
  {
    pi_task_wait_on(&buf_event[i]);
  }

#endif


  int error = 0;
  for (int i=0; i<total_size; i++)
  {
    //printf("%d: @%p: %x @%p: %x\n", i, &rx_buffer[i], rx_buffer[i], &tx_buffer[i], tx_buffer[i]);
    int expected = i & 0x7f;
    if (rx_buffer[i] != expected)
    {
      printf("RX error at index %d, expected 0x%x, got 0x%x\n", i, expected, rx_buffer[i]);
      error++;
    }
  }


  set_spim_verif_command(&spim, 0x2, 0, total_size, cmd_buffer[0], NULL);
  pi_spi_receive(&spim, rx_buffer, total_size*8, PI_SPI_CS_AUTO);

  for (int i=0; i<total_size; i++)
  {
    //printf("%d: @%p: %x @%p: %x\n", i, &rx_buffer[i], rx_buffer[i], &tx_buffer[i], tx_buffer[i]);
    int expected = (i | 0x80) & 0xff;
    if (rx_buffer[i] != expected)
    {
      printf("TX error at index %d, expected 0x%x, got 0x%x\n", i, expected, rx_buffer[i]);
      error++;
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
