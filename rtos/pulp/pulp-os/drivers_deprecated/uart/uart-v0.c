/*
 * Copyright (C) 2018 ETH Zurich and University of Bologna
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * Copyright (C) 2018 GreenWaves Technologies
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/* 
 * Authors: Germain Haugou, ETH (germain.haugou@iis.ee.ethz.ch)
 */


#include "rt/rt_api.h"

#define __RT_UART_BAUDRATE 115200



#define UART_REG_RBR ( ARCHI_UART_ADDR + 0x00)
#define UART_REG_DLL ( ARCHI_UART_ADDR + 0x00)
#define UART_REG_THR ( ARCHI_UART_ADDR + 0x00)
#define UART_REG_DLM ( ARCHI_UART_ADDR + 0x04)
#define UART_REG_IER ( ARCHI_UART_ADDR + 0x04)
#define UART_REG_IIR ( ARCHI_UART_ADDR + 0x08)
#define UART_REG_FCR ( ARCHI_UART_ADDR + 0x08)
#define UART_REG_LCR ( ARCHI_UART_ADDR + 0x0C)
#define UART_REG_MCR ( ARCHI_UART_ADDR + 0x10)
#define UART_REG_LSR ( ARCHI_UART_ADDR + 0x14)
#define UART_REG_MSR ( ARCHI_UART_ADDR + 0x18)
#define UART_REG_SCR ( ARCHI_UART_ADDR + 0x1C)



static rt_uart_t __rt_uart;



void rt_uart_conf_init(rt_uart_conf_t *conf)
{
  conf->baudrate = __RT_UART_BAUDRATE;
}



static void __rt_uart_wait_tx_done(rt_uart_t *uart)
{
  // Wait until the fifo is empty.
  // There is no other way than actively polling the register as the interrupt
  // is generated before bit 6 of LSR becomes 1, as it is generated when the
  // FIFO is empty but this bit tells when the whole stream is flushed.
  while( (*(volatile unsigned int*)(UART_REG_LSR) & 0x40) == 0);
}



static void __rt_uart_setup(rt_uart_t *uart)
{
  int div =  __rt_freq_periph_get() / uart->baudrate / 16 - 1;

/*
 * clk_counter  Clock counter value that is used to derive the UART clock.
 *              There is a prescaler in place that already divides the SoC
 *              clock by 16.  Since this is a counter, a value of 0 means that
 *              the SoC clock divided by 16 is used. A value of 31 would mean
 *              that we use the SoC clock divided by 16*32 = 512.
 */

  *(volatile unsigned int*)(UART_REG_LCR) = 0x83; //sets 8N1 and set DLAB to 1
  *(volatile unsigned int*)(UART_REG_DLM) = (div >> 8) & 0xFF;
  *(volatile unsigned int*)(UART_REG_DLL) =  div       & 0xFF;
  *(volatile unsigned int*)(UART_REG_FCR) = 0xA7; //enables 16byte FIFO and clear FIFOs
  *(volatile unsigned int*)(UART_REG_LCR) = 0x03; //sets 8N1 and set DLAB to 0

  *(volatile unsigned int*)(UART_REG_IER) = ((*(volatile unsigned int*)(UART_REG_IER)) & 0xF0) | 0x02; // set IER (interrupt enable register) on UART
  *(volatile unsigned int*)(UART_REG_MCR) = 0x20; //enable automatic flow control
}



static int __rt_uart_setfreq_before(void *arg)
{
  // On this architecture, we don't have any choice than deactivating the uart as
  // we cannot buffer the bytes which would be pushed between the 2 callbacks
  // Just deactivate the uart until the other callback
  __rt_uart.active = 0;

  // Wait until everything is flushed so that the new frequency can be safely
  // applied.
  __rt_uart_wait_tx_done(&__rt_uart);

  return 0;
}



static int __rt_uart_setfreq_after(void *arg)
{
  // Impact the divider with the new frequency.
  __rt_uart_setup(&__rt_uart);

  // And reactivate the uart.
  __rt_uart.active = 0;

  return 0;
}


rt_uart_t* __rt_uart_open(int channel, rt_uart_conf_t *conf, rt_event_t *event, char *name)
{
  int irq = rt_irq_disable();

  int baudrate = __RT_UART_BAUDRATE;
  if (conf) baudrate = conf->baudrate;

  if (name) rt_trace(RT_TRACE_DEV_CTRL, "[UART] Opening uart device (name: %s, baudrate: %d)\n", name, baudrate);
  else rt_trace(RT_TRACE_DEV_CTRL, "[UART] Opening uart device (channel: %d, baudrate: %d)\n", channel, baudrate);

  rt_uart_t *uart = &__rt_uart;

  // In case the uart is already openened, return the same handle if the 
  // configuration is the same or return an error
  if (uart->open_count)
  {
    if (conf && conf->baudrate != uart->baudrate) return NULL;
    uart->open_count++;
    return uart;
  }

  uart->open_count++;
  uart->baudrate = baudrate;
  uart->active = 1;

  // Then set it up
  __rt_uart_setup(uart);

  rt_trace(RT_TRACE_DEV_CTRL, "[UART] Successfully opened uart device (handle: %p)\n", uart);

  rt_irq_restore(irq);

  return uart;
}


rt_uart_t* rt_uart_open(char *dev_name, rt_uart_conf_t *conf, rt_event_t *event)
{

  rt_trace(RT_TRACE_DEV_CTRL, "[UART] Opening uart device (name: %s)\n", dev_name);

  rt_dev_t *dev = rt_dev_get(dev_name);
  if (dev == NULL) goto error;

  return __rt_uart_open(0, conf, event, dev_name);
  
error:
  rt_warning("[UART] Failed to open uart device\n");
  return NULL;
}



void rt_uart_close(rt_uart_t *uart, rt_event_t *event)
{
  int irq = rt_irq_disable();

  rt_trace(RT_TRACE_DEV_CTRL, "[UART] Closing uart device (handle: %p)\n", uart);

  uart->open_count--;

  if (uart->open_count == 0)
  {
      // Wait for pending transfers to finish before stoppping uart in case
      // some printf are still pending
      __rt_uart_wait_tx_done(uart);
  }

  rt_irq_restore(irq);
}


RT_FC_BOOT_CODE void __attribute__((constructor)) __rt_uart_init()
{

  // In case the peripheral clock can dynamically change, we need to be notified
  // when this happens so that we flush pending transfers before updating the frequency
  int err = 0;

  err |= __rt_cbsys_add(RT_CBSYS_PERIPH_SETFREQ_BEFORE, __rt_uart_setfreq_before, NULL);

  err |= __rt_cbsys_add(RT_CBSYS_PERIPH_SETFREQ_AFTER, __rt_uart_setfreq_after, NULL);


  __rt_uart.open_count = 0;

  if (err) rt_fatal("Unable to initialize uart driver\n");
}



void __rt_uart_write(rt_uart_t *uart, void *buffer, size_t len, rt_event_t *event)
{
  // In case the frequency is being changed, we cannot push anything, just
  // drop it.
  if (!uart->active) return;

  unsigned char *str = buffer;

  while(len > 0) {
    // Wait until we can push one character
    while( (*(volatile unsigned int*)(UART_REG_LSR) & 0x20) == 0)
    {
#if defined(ARCHI_EVT_UART)
      eu_evt_maskWaitAndClr(1<<ARCHI_EVT_UART);
#endif
      *(volatile unsigned int*)(UART_REG_IIR);
    }

    *(volatile unsigned int*)(UART_REG_THR) = *str++;

    len--;
  }

  if (event) rt_event_enqueue(event);
}

