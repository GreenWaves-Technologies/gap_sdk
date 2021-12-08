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
 * Authors: Germain Haugou, ETH (germain.haugou@iis.ee.ethz.ch)
 */

#include "rt/rt_api.h"
#include <stdint.h>

static RT_FC_DATA rt_spim_t *__rt_spim_first = NULL;
static RT_FC_DATA int open_count = 0;



static uint16_t swap_uint16( uint16_t val ) 
{
    return (val << 8) | (val >> 8 );
}

static uint16_t swap_uint32_16_8( uint16_t value ) 
{
  return (swap_uint16(value & 0xffff) << 16) | (swap_uint16(value >> 16));
}

static uint16_t swap_uint32_16( uint16_t value ) 
{
  return (value << 16) | (value >> 16);
}

static uint32_t swap_uint32( uint32_t val )
{
    val = ((val << 8) & 0xFF00FF00 ) | ((val >> 8) & 0xFF00FF ); 
    return (val << 16) | (val >> 16);
}

void __rt_spim_v0_set_cs(rt_spim_t *spim, int cs)
{
  if (spim->cs_gpio != -1)
  {
    rt_gpio_set_value(0, 1<<spim->cs_gpio, cs);
  }
}

void __rt_spim_v0_receive(
  rt_spim_t *handle, void *buffer, int len, int qspi, rt_spim_cs_e mode, rt_event_t *event)
{
  rt_spim_t *spim = (rt_spim_t *)handle;

  // There is a HW issue when we receive after some bits were sent
  // The HW FIFO still contains the last byte sent and keeps sending it
  // on the MOSI line while we are receiving.
  // As this can disturb the master, we push empty data to the FIFO so that
  // it sent 0.
  // This can only work with a CS GPIO as we must send these dummy bis with
  // the CS low and we have no control over the SPI CS.
  // In case the KEEP cs mode is used, some HW logic must be added to keep
  // the clock at the same level while these dummy bits are being sent.
  if (spim->cs_gpio != -1)
  {
    int workaround_buff = 0;
    rt_spim_send(spim, &workaround_buff, sizeof(workaround_buff)*8, RT_SPIM_CS_NONE, NULL);
  }

  int full_size = (len + 7) / 8;
  int size = full_size;
  unsigned int spiBase = pulp_spi_base();
  uint32_t *data = (uint32_t *)buffer;
  int wordsize = handle->wordsize;
  int big_endian = handle->big_endian;

  unsigned int cmd = qspi ? PULP_SPI_CMD_QRD : PULP_SPI_CMD_RD;

  if (mode != RT_SPIM_CS_NONE)
    __rt_spim_v0_set_cs(spim, 0);

  // Apply the computed divider to get requested SPIM frequency as several devices
  // with different frequencies can be used.
  pulp_spi_clkdiv(pulp_spi_base(), spim->div);

  // Set the length of the whole transaction, this will control the CS
  pulp_spi_set_datalen(spiBase, len);
  
  // Just in case there is a spurious event
#if EU_VERSION >= 3
  eu_evt_clr(1<<ARCHI_EVT_SPIM0);
#else
  pulp_gpevt_clear(ARCHI_EVT_SPIM0);
#endif

  // The transaction is started now so that data start being pushed
  // to the FIFO.
  pulp_spi_start_transaction(spiBase, cmd, PULP_SPI_CSN0);

  // Now iterate everytime at least 4 elements are ready so that we have time
  // pop them while the SPI can still push data.
  int iter_size = (PULP_SPI_FIFO_SIZE/2)*4;

  while(size)
  {
    if (size < iter_size) iter_size = size;
    int nb_elems = (iter_size + 3)/ 4;

    // Wait first until the 4 elemets are ready
    eu_evt_maskWaitAndClr((1<<ARCHI_EVT_SPIM0) | (1<<ARCHI_EVT_SPIM1));

    // And pop them
    if (wordsize == RT_SPIM_WORDSIZE_32 && big_endian)
    {      
      for (int i=0; i<nb_elems; i++)
      {
        data[i] = pulp_spi_fifo_pop(spiBase);
      }
    }
    else if ((wordsize == RT_SPIM_WORDSIZE_32 && !big_endian) || wordsize == RT_SPIM_WORDSIZE_8)
    {  
      for (int i=0; i<nb_elems; i++)
      {
        data[i] = swap_uint32(pulp_spi_fifo_pop(spiBase));
      }
    }
    else
    {
      if (!big_endian)
      {
        for (int i=0; i<nb_elems; i++)
        {
          data[i] = swap_uint32_16_8(pulp_spi_fifo_pop(spiBase));
        }
      }
      else
      {
        for (int i=0; i<nb_elems; i++)
        {
          data[i] = swap_uint32_16(pulp_spi_fifo_pop(spiBase));
        }
      }
    }

    size -= iter_size;
    data += nb_elems;
  }

  if (wordsize == RT_SPIM_WORDSIZE_8)
  {
    uint32_t *last_word_address = (uint32_t*)(buffer+(full_size & ~0x3));
    *last_word_address = (*last_word_address)>>(((-full_size)&0x03)<<3);
   }

  if (mode == RT_SPIM_CS_AUTO)  
    __rt_spim_v0_set_cs(handle, 1);

}

void __rt_spim_v0_send(
  rt_spim_t *handle, void *buffer, int len, int qspi, rt_spim_cs_e mode, rt_event_t *event)
{
  int size = (len + 7) / 8;
  unsigned int spiBase = pulp_spi_base();
  uint32_t *data = (uint32_t *)buffer;
  int wordsize = handle->wordsize;
  int big_endian = handle->big_endian;
  rt_spim_t *spim = (rt_spim_t *)handle;

  if (mode != RT_SPIM_CS_NONE)
    __rt_spim_v0_set_cs(spim, 0);

  // Apply the computed divider to get requested SPIM frequency as several devices
  // with different frequencies can be used.
  pulp_spi_clkdiv(pulp_spi_base(), spim->div);

  // Set the length of the whole transaction, this will control the CS
  pulp_spi_set_datalen(spiBase, len);
  
  // Just in case there is a spurious event
#if EU_VERSION >= 3
  eu_evt_clr(1<<ARCHI_EVT_SPIM0);
#else
  pulp_gpevt_clear(ARCHI_EVT_SPIM0);
#endif

  // Now we need to iterate in order to fill the FIFO without overwriting
  // any existing value.
  // For that first push enough elements to fill the fifo and then
  // wait until half the FIFO has been sent before reenqueuing enough
  // elements to fill the FIFO.
  int iter_size = PULP_SPI_FIFO_SIZE*4;
  int is_first = 1;

  unsigned int cmd = qspi ? PULP_SPI_CMD_QWR : PULP_SPI_CMD_WR;

  while(1)
  {
    if (size < iter_size) iter_size = size;
    int nb_elems = (iter_size + 3)/ 4;

    // We have to be careful to push the data in the right order
    // depending on the wordsize and the byte order.

    if (wordsize == RT_SPIM_WORDSIZE_32 && big_endian)
    {      
      for (int i=0; i<nb_elems; i++)
      {
        pulp_spi_fifo_push(spiBase, data[i]);
      }
    }
    else if ((wordsize == RT_SPIM_WORDSIZE_32 && !big_endian) || wordsize == RT_SPIM_WORDSIZE_8)
    {  
      for (int i=0; i<nb_elems; i++)
      {
        pulp_spi_fifo_push(spiBase, swap_uint32(data[i]));
      }
    }
    else
    {
      if (!big_endian)
      {
        for (int i=0; i<nb_elems; i++)
        {
          pulp_spi_fifo_push(spiBase, swap_uint32_16_8(data[i]));
        }
      }
      else
      {
        for (int i=0; i<nb_elems; i++)
        {
          pulp_spi_fifo_push(spiBase, swap_uint32_16(data[i]));
        }
      }
    }

    // The first time we push elements, we need to start the transaction
    // as it can not be started before a few elemets have been pushed.
    if (is_first)
    {
      is_first = 0;
      pulp_spi_start_transaction(spiBase, cmd, PULP_SPI_CSN0);
    }

    size -= iter_size;
    data += nb_elems;

    if (size == 0) break;

    // We haven't reached the end of the transfer. Wait until half the FIFO
    // has been sent and reiterate to fill again the FIFO.
    // This is important that the FIFO is never empty otherwise the SPI will
    // send wrong data as there is no synchronization between the SPI and the
    // FIFO.
    eu_evt_maskWaitAndClr(1<<ARCHI_EVT_SPIM0);
    iter_size = (PULP_SPI_FIFO_SIZE/2)*4;

  }


  /// Finally wait until all our elements have been sent
  while ((pulp_spi_status(spiBase) & 0xFFFF) != 1)
  {
    eu_evt_maskWaitAndClr(1<<ARCHI_EVT_SPIM1); 
  }

  if (mode == RT_SPIM_CS_AUTO)  
    __rt_spim_v0_set_cs(handle, 1);

}

static int __rt_spim_get_div(int spi_freq, int periph_freq)
{
  if (spi_freq > periph_freq) return -1;

  int div = pulp_spi_get_div(periph_freq / spi_freq);
  if (div >= 256) return -1;

  return div;
}

static int __rt_spim_setfreq_before(void *arg)
{
  // In case the periph frequency is being changed, we have to complain
  // in case the new divider does not fit.
  rt_spim_t *spim = __rt_spim_first;
  while (spim)
  {
    int div = __rt_spim_get_div(spim->max_baudrate, __rt_freq_periph_get_next());
    if (div == -1) return -1;
    spim = spim->next;
  }

  return 0;
}


static int __rt_spim_setfreq_after(void *arg)
{
  // Now that the frequency has been changed, apply the new divider.
  // In case we reported an error during the first callback,
  // the new frequency is actually the same.
  rt_spim_t *spim = __rt_spim_first;
  while (spim)
  {
    spim->div = __rt_spim_get_div(spim->max_baudrate, __rt_freq_periph_get());
    spim = spim->next;
  }

  return 0;
}

rt_spim_t *rt_spim_open(char *dev_name, rt_spim_conf_t *conf, rt_event_t *event)
{
  rt_spim_conf_t def_conf;

  // Check that the required frequency is correct.
  // It must be below the periph frequency as we can just apply a divider
  // and the divider must fits 8 bits.
  int div = __rt_spim_get_div(conf->max_baudrate, __rt_freq_periph_get());
  if (div == -1) goto error0;

  rt_spim_t *spim = rt_alloc(RT_ALLOC_FC_DATA, sizeof(rt_spim_t));
  if (spim == NULL) goto error0;

  if (open_count == 0)
  {
    // Insert callbacks in case the periph frequency is dynamically modified
    if (__rt_cbsys_add(RT_CBSYS_PERIPH_SETFREQ_BEFORE, __rt_spim_setfreq_before, NULL))
    {
      goto error1;
    }

    if (__rt_cbsys_add(RT_CBSYS_PERIPH_SETFREQ_AFTER, __rt_spim_setfreq_after, NULL))
    {
      goto error2;
    }
  }

  // Now that all checks are done, we can start applying the configuration
  open_count++;

  // In case a GPIO is specified for the CS, configure it as an output
  if (conf->cs_gpio >= 0)
  {
    rt_gpio_set_dir(0, 1<<conf->cs_gpio, 1);
    rt_gpio_set_value(0, 1<<conf->cs_gpio, 1);
  }

  // Insert the new descriptor in the list
  spim->next = __rt_spim_first;
  __rt_spim_first = spim->next;

  // Apply the computed divider to get requested SPIM frequency
  spim->div = div;

  // Remember the part of the configuration that we need afterwards as the
  // configuration structure must be released.
  spim->max_baudrate = conf->max_baudrate;
  spim->wordsize = conf->wordsize;
  spim->big_endian = conf->big_endian;
  spim->cs_gpio = conf->cs_gpio;

  // Configure the pads for SPIM
#ifdef PIN_MSPI_SIO0
  rt_pad_config(PIN_MSPI_SIO0, PIN_MSPI_SIO0_FUN);
  rt_pad_config(PIN_MSPI_SIO1, PIN_MSPI_SIO1_FUN);
  rt_pad_config(PIN_MSPI_SIO2, PIN_MSPI_SIO2_FUN);
  rt_pad_config(PIN_MSPI_SIO3, PIN_MSPI_SIO3_FUN);
  rt_pad_config(PIN_MSPI_SCK, PIN_MSPI_SCK_FUN);
  rt_pad_config(PIN_MSPI_CSN0, PIN_MSPI_CSN0_FUN);
#endif

  // Setup command and address to zero as this driver is just receiving and
  // sending raw spi bitstream.
  pulp_spi_setup_cmd_addr(pulp_spi_base(), 0, 0, 0, 0);

  // Setup interrupt generation based on threshold.
  // We will receive an interrupt everytime 4 elements (half the fifo) are
  // transfered.
  pulp_spi_intcfg(pulp_spi_base(), PULP_SPI_FIFO_SIZE, 0, PULP_SPI_FIFO_SIZE/2,
   PULP_SPI_FIFO_SIZE/2, 1, 1);

  // Finally handle the user notification
  if (event) rt_event_enqueue(event);

  return (rt_spim_t *)spim;


error2:
  __rt_cbsys_del(RT_CBSYS_PERIPH_SETFREQ_BEFORE, __rt_spim_setfreq_before, NULL);

error1:
  rt_free(RT_ALLOC_FC_DATA, spim, sizeof(rt_spim_t));

error0:
  return NULL;
}

void __rt_spim_control(rt_spim_t *handle, rt_spim_control_e cmd, uint32_t arg)
{
  //if (cmd & RT_SPIM_CONTROL_CS)
  //{
  //  rt_gpio_set_value(0, 1<<handle->cs_gpio, arg);
  //}

}

void rt_spim_close(rt_spim_t *handle, rt_event_t *event)
{
  rt_spim_t *spim = (rt_spim_t *)handle;

  // Decrement opened dirver count and cancel the callbacks if this is the last
  // opened driver
  open_count--;
  if (open_count == 0)
  {
    __rt_cbsys_del(RT_CBSYS_PERIPH_SETFREQ_BEFORE, __rt_spim_setfreq_before, NULL);
    __rt_cbsys_del(RT_CBSYS_PERIPH_SETFREQ_AFTER, __rt_spim_setfreq_after, NULL);
  }

  // Remove it from the list of drivers
  rt_spim_t *current = __rt_spim_first, *prev = NULL;
  while (current && current != spim)
  {
    prev = current;
    current = current->next;
  }
  if (prev) prev->next = spim->next;
  else __rt_spim_first = spim->next;

  // And free it
  rt_free(RT_ALLOC_FC_DATA, handle, sizeof(rt_spim_t));
}

void rt_spim_conf_init(rt_spim_conf_t *conf)
{
  conf->wordsize = RT_SPIM_WORDSIZE_8;
  conf->big_endian = 1;
  conf->max_baudrate = 10000000;
  conf->cs_gpio = -1;
}

