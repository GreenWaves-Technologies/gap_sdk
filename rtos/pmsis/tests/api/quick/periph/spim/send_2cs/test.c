#include <stdio.h>
#include <pmsis.h>
#include <stdint.h>

#ifdef __GAP9__
#include "testbench.h"
#endif

#define BUFFER_SIZE 1024
#define NB_BUFFERS 4
#define SIZE (BUFFER_SIZE/NB_BUFFERS)

PI_L2 int32_t cmd_buffer[4][2];

PI_L2 uint8_t tx_buffer[2][BUFFER_SIZE];

PI_L2 uint8_t rx_buffer[2][BUFFER_SIZE];

static void set_spim_verif_command(struct pi_device *spim, int cmd, int addr, int size, int32_t *cmd_buffer, pi_task_t *event)
{
  cmd_buffer[0] = (cmd << 24) | (size*8);
  cmd_buffer[1] = addr;
  if (event)
    pi_spi_send_async(spim, cmd_buffer, 8*8, PI_SPI_CS_AUTO, event);
  else
    pi_spi_send(spim, cmd_buffer, 8*8, PI_SPI_CS_AUTO);
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


static int test_entry()
{
  struct pi_spi_conf conf;
  struct pi_device spim0, spim1;

#ifdef __GAP9__
  pi_pad_set_function(PI_PAD_026, 0);
  pi_pad_set_function(PI_PAD_027, 0);
  pi_pad_set_function(PI_PAD_028, 0);
  pi_pad_set_function(PI_PAD_029, 0);
  pi_pad_set_function(PI_PAD_030, 0);
  pi_pad_set_function(PI_PAD_031, 0);
  pi_pad_set_function(PI_PAD_032, 0);

  spim_verif_enable(SPIM_ITF, 0);
  spim_verif_enable(SPIM_ITF, 1);
#endif

  pi_spi_conf_init(&conf);
  conf.wordsize = PI_SPI_WORDSIZE_32;
  conf.big_endian = 1;
  conf.max_baudrate = 10000000;
  conf.polarity = 0;
  conf.phase = 0;
  conf.itf = SPIM_ITF;
  conf.cs = 0;

  pi_open_from_conf(&spim0, &conf);
  if (pi_spi_open(&spim0))
    return -1;

  conf.cs = 1;

  pi_open_from_conf(&spim1, &conf);
  if (pi_spi_open(&spim1))
    return -1;

  for (int i=0; i<BUFFER_SIZE; i++)
  {
    tx_buffer[0][i] = i & 0x7f;
    tx_buffer[1][i] = i | 0x80;
  }

  set_spim_verif_command(&spim0, 0x1, 0, BUFFER_SIZE, cmd_buffer[0], NULL);
  set_spim_verif_command(&spim1, 0x1, 0, BUFFER_SIZE, cmd_buffer[0], NULL);

  pi_task_t event0;
  pi_task_t event1;

  pi_spi_send_async(&spim0, tx_buffer[0], BUFFER_SIZE*8, PI_SPI_CS_AUTO, pi_task_block(&event0));
  pi_spi_send_async(&spim1, tx_buffer[1], BUFFER_SIZE*8, PI_SPI_CS_AUTO, pi_task_block(&event1));

  pi_task_wait_on(&event0);
  pi_task_wait_on(&event1);

  set_spim_verif_command(&spim0, 0x2, 0, BUFFER_SIZE, cmd_buffer[0], NULL);
  set_spim_verif_command(&spim1, 0x2, 0, BUFFER_SIZE, cmd_buffer[0], NULL);

  pi_spi_receive_async(&spim0, rx_buffer[0], BUFFER_SIZE*8, PI_SPI_CS_AUTO, pi_task_block(&event0));
  pi_spi_receive_async(&spim1, rx_buffer[1], BUFFER_SIZE*8, PI_SPI_CS_AUTO, pi_task_block(&event1));

  pi_task_wait_on(&event0);
  pi_task_wait_on(&event1);

  int error = 0;
  for (int j=0; j<2; j++)
  {
    for (int i=0; i<BUFFER_SIZE; i++)
    {
      //printf("%d: @%p: %x @%p: %x\n", i, &rx_buffer[i], rx_buffer[i], &tx_buffer[i], tx_buffer[i]);
      int expected = j == 0 ? i & 0x7f : (i | 0x80) & 0xff;
      if (rx_buffer[j][i] != expected)
      {
        printf("Error at buffer %d index %d, expected 0x%x, got 0x%x\n", j, i, expected, rx_buffer[j][i]);
        error++;
      }
    }
  }

  pi_spi_close(&spim0);
  pi_spi_close(&spim1);

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
