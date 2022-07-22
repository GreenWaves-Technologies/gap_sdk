#include <stdio.h>
#include <stdint.h>
#include <pmsis.h>

#define BUFFER_SIZE 256

PI_L2 uint8_t tx_buffer[BUFFER_SIZE];

PI_L2 uint8_t rx_buffer[BUFFER_SIZE];

struct pi_uart_conf conf;
struct pi_device uart;


static int test()
{
  printf("Entered test\n");

  pi_uart_conf_init(&conf);

  conf.enable_tx = 1;
  conf.enable_rx = 1;
  conf.uart_id = 0;

  pi_open_from_conf(&uart, &conf);

  if (pi_uart_open(&uart))
    return -1;

  for (int i=0; i<BUFFER_SIZE; i++)
  {
    tx_buffer[i] = i;
  }

  pi_task_t task;

  // We first enqueue a read copy so that we can receive what we will send.
  // This must be asynchronous as the transfer cannot finish before
  // we enqueue the write.
  // The event used here is a blocking event, which means we must explicitely wait for it
  // to know when the transfer is finished.
  pi_uart_read_async(&uart, rx_buffer, BUFFER_SIZE, pi_task_block(&task));

  // This one is not using any event and is thus blocking.
  pi_uart_write(&uart, tx_buffer, BUFFER_SIZE);

  // This will block execution until we have received the whole buffer.
  pi_task_wait_on(&task);

  int error = 0;
  for (int i=0; i<BUFFER_SIZE; i++)
  {
    //printf("%d: @%p: %x @%p: %x\n", i, &rx_buffer[i], rx_buffer[i], &tx_buffer[i], tx_buffer[i]);
    if (rx_buffer[i] != tx_buffer[i])
    {
      printf("Error at index %d, expected 0x%x, got 0x%x\n", i, tx_buffer[i], rx_buffer[i]);
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

static void test_entry()
{
  int status = test();
  pmsis_exit(status);
}

int main(void)
{
    return pmsis_kickoff((void *)test_entry);
}
