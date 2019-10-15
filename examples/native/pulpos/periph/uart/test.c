#include <stdio.h>
#include <rt/rt_api.h>
#include "Gap8.h"
#include <stdint.h>

#define BUFFER_SIZE 4

RT_L2_DATA uint8_t rx_buffer[BUFFER_SIZE];

L2_MEM rt_event_sched_t sched;
rt_uart_t *uart;

volatile int flag = 0;
int error = 0;

void irq_handle(void *arg)
{
    for (int i=0; i<BUFFER_SIZE; i++)
    {
        printf("%d: %d\n", i, rx_buffer[i]);
    }
    rt_uart_write(uart, rx_buffer, BUFFER_SIZE, NULL);
    flag = 1;
}

int main()
{
  printf("Entered test\n");

  /* Initialize a event scheduler which controls the execution of events */
  rt_event_sched_init(&sched);

  /* Allocate 4 event elements in the scheduler */
  if (rt_event_alloc(&sched, 4)) {
      printf("Event Alloc failed\n");
      return -1;
  }

  rt_uart_conf_t conf;

  rt_uart_conf_init(&conf);

  // This will tell the driver to use the uart 0 interface (in case there are several)
  conf.itf = 0;
  conf.baudrate = 115200;

  uart = rt_uart_open(NULL, &conf, NULL);
  if (uart == NULL) {
      printf("Open failed\n");
      return -1;
  }
  printf("open successed, please enter 4 character\n");

  rt_event_t *event = rt_event_get(&sched, irq_handle, 0);

  // We first enqueue a read copy so that we can receive what we will send.
  // This must be asynchronous as the transfer cannot finish before
  // we enqueue the write.
  // The event used here is a blocking event, which means we must explicitely wait for it
  // to know when the transfer is finished.
  rt_uart_read(uart, rx_buffer, BUFFER_SIZE, event);

  // This one is not using any event and is thus blocking.

  /* A event scheduler will always wait and execute if any event coming to the queue */
  /* In this test, we have two TX transfers which cause two RX transfers, then the RX transfers count two times. */
  /* So, when all transfers are finished, it will exit */
  while(flag == 0) {
    rt_event_execute(&sched, 1);
  }

  if (error)
  {
    printf("Got %d errors\n", error);
  }
  else
  {
    printf("Got No errors\n");
  }

  return error;
}
