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



#ifdef QUAD
#define WRITE_CMD 4
#define READ_CMD  5
#define SPI_FLAGS (PI_SPI_LINES_QUAD)
#else
#define WRITE_CMD 1
#define READ_CMD  2
#define SPI_FLAGS 0
#endif


static inline void get_info(int *buffer_size)
{
  *buffer_size = TOTAL_SIZE;
}




PI_L2 int32_t cmd_buffer[4][2];
PI_L2 int32_t rx_cmd_buffer[4][2];

PI_L2 uint8_t *tx_buffer;

PI_L2 uint8_t *rx_buffer;

static pi_task_t buf_event[NB_BUFFERS];
static pi_task_t rx_buf_event[NB_BUFFERS];
static pi_task_t cmd_event[NB_BUFFERS];
static pi_task_t rx_cmd_event[NB_BUFFERS];


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

  if (pi_testbench_spim_verif_setup(&bench, 1, itf, cs, POLARITY, PHASE, 16))
    return -1;

    return 0;
}
#endif


static void set_spim_verif_command(struct pi_device *spim, int cmd, int addr, int size, int32_t *cmd_buffer, pi_task_t *task)
{
  cmd_buffer[0] = (cmd << 24) | (size*8);
  cmd_buffer[1] = addr;

  if (task)
    pi_spi_send_async(spim, cmd_buffer, 8*8, PI_SPI_CS_AUTO, task);
  else
    pi_spi_send(spim, cmd_buffer, 8*8, PI_SPI_CS_AUTO);
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
  pi_pad_set_function(PI_PAD_046, 3);
  pi_pad_set_function(PI_PAD_047, 3);
  pi_pad_set_function(PI_PAD_053, 3);
  pi_pad_set_function(PI_PAD_054, 3);
  pi_pad_set_function(PI_PAD_055, 3);
  pi_pad_set_function(PI_PAD_056, 3);
  pi_pad_set_function(PI_PAD_057, 3);
  pi_pad_set_function(PI_PAD_058, 3);
  pi_pad_set_function(PI_PAD_059, 3);
  pi_pad_set_function(PI_PAD_070, 3);
  pi_pad_set_function(PI_PAD_071, 3);
  pi_pad_set_function(PI_PAD_072, 3);
  pi_pad_set_function(PI_PAD_073, 3);
  pi_pad_set_function(PI_PAD_074, 3);
  pi_pad_set_function(PI_PAD_075, 3);
  pi_pad_set_function(PI_PAD_076, 3);

  spim_verif_enable(SPIM_ITF, SPIM_CS);
#endif

  pi_spi_conf_init(&conf);

  conf.wordsize = PI_SPI_WORDSIZE_32;
  conf.big_endian = 1;
  conf.max_baudrate = 10000000;
  conf.polarity = POLARITY;
  conf.phase = PHASE;
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
    tx_buffer[i] = i;
  }


#if defined(SYNC_CS_AUTO)

  for (int i=0; i<NB_BUFFERS; i++)
  {
    set_spim_verif_command(&spim, WRITE_CMD, buffer_size*i, buffer_size, cmd_buffer[i], NULL);
    pi_spi_send(&spim, tx_buffer+buffer_size*i, buffer_size*8, PI_SPI_CS_AUTO | SPI_FLAGS);  

    set_spim_verif_command(&spim, READ_CMD, buffer_size*i, buffer_size, cmd_buffer[i], NULL);
    pi_spi_receive(&spim, rx_buffer+buffer_size*i, buffer_size*8, PI_SPI_CS_AUTO | SPI_FLAGS);
  }

#elif defined(ASYNC_CS_AUTO)

  for (int i=0; i<NB_BUFFERS; i++)
  {
    set_spim_verif_command(&spim, WRITE_CMD, buffer_size*i, buffer_size, cmd_buffer[i], pi_task_block(&cmd_event[i]));
    pi_spi_send_async(&spim, tx_buffer+buffer_size*i, buffer_size*8, PI_SPI_CS_AUTO | SPI_FLAGS, pi_task_block(&buf_event[i]));
  }

  for (int i=0; i<NB_BUFFERS; i++)
  {
    set_spim_verif_command(&spim, READ_CMD, buffer_size*i, buffer_size, rx_cmd_buffer[i], pi_task_block(&rx_cmd_event[i]));
    pi_spi_receive_async(&spim, rx_buffer+buffer_size*i, buffer_size*8, PI_SPI_CS_AUTO | SPI_FLAGS, pi_task_block(&rx_buf_event[i]));
  }

  for (int i=0; i<NB_BUFFERS; i++)
  {
    pi_task_wait_on(&cmd_event[i]);
    pi_task_wait_on(&buf_event[i]);
    pi_task_wait_on(&rx_cmd_event[i]);
    pi_task_wait_on(&rx_buf_event[i]);
  }

#elif defined(SYNC_CS_KEEP)

  set_spim_verif_command(&spim, WRITE_CMD, 0, total_size, cmd_buffer[0], NULL);

  for (int i=0; i<NB_BUFFERS; i++)
  {
    int mode = i == NB_BUFFERS - 1 ? PI_SPI_CS_AUTO : PI_SPI_CS_KEEP;
    pi_spi_send(&spim, tx_buffer+buffer_size*i, buffer_size*8, mode | SPI_FLAGS);
  }

  set_spim_verif_command(&spim, READ_CMD, 0, total_size, cmd_buffer[0], NULL);

  for (int i=0; i<NB_BUFFERS; i++)
  {
    int mode = i == NB_BUFFERS - 1 ? PI_SPI_CS_AUTO : PI_SPI_CS_KEEP;
    pi_spi_receive(&spim, rx_buffer+buffer_size*i, buffer_size*8, mode | SPI_FLAGS);
  }

#elif defined(ASYNC_CS_KEEP)

  set_spim_verif_command(&spim, WRITE_CMD, 0, total_size, cmd_buffer[0], NULL);

  for (int i=0; i<NB_BUFFERS; i++)
  {
    int mode = i == NB_BUFFERS - 1 ? PI_SPI_CS_AUTO : PI_SPI_CS_KEEP;
    pi_spi_send_async(&spim, tx_buffer+buffer_size*i, buffer_size*8, mode | SPI_FLAGS, pi_task_block(&buf_event[i]));
  }

  set_spim_verif_command(&spim, READ_CMD, 0, total_size, cmd_buffer[0], pi_task_block(&cmd_event[0]));

  for (int i=0; i<NB_BUFFERS; i++)
  {
    int mode = i == NB_BUFFERS - 1 ? PI_SPI_CS_AUTO : PI_SPI_CS_KEEP;
    pi_spi_receive_async(&spim, rx_buffer+buffer_size*i, buffer_size*8, mode | SPI_FLAGS, pi_task_block(&rx_buf_event[i]));
  }

  pi_task_wait_on(&cmd_event[0]);

  for (int i=0; i<NB_BUFFERS; i++)
  {
    pi_task_wait_on(&buf_event[i]);
    pi_task_wait_on(&rx_buf_event[i]);
  }

#endif



  int error = 0;
  for (int i=0; i<total_size; i++)
  {
    //printf("%d: @%p: %x @%p: %x\n", i, &rx_buffer[i], rx_buffer[i], &tx_buffer[i], tx_buffer[i]);
    if (rx_buffer[i] != tx_buffer[i])
    {
      if (error == 0)
        printf("First error at index %d, expected 0x%x, got 0x%x at %p\n", i, tx_buffer[i], rx_buffer[i], &rx_buffer[i]);
      error++;
      return -1;
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
  pi_spi_close(&spim);
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
