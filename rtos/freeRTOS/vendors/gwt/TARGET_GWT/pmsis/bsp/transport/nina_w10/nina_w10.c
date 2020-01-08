/*
 * Copyright (C) 2019 GreenWaves Technologies
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
 * Authors:  Esteban Gougeon, GreenWaves Technologies (esteban.gougeon@greenwaves-technologies.com)
 *           Germain Haugou, GreenWaves Technologies (germain.haugou@greenwaves-technologies.com)
 */

#include "pmsis.h"
#include "bsp/transport/nina_w10.h"
#include "bsp/bsp.h"
#include "bsp/transport.h"


#define NINA_W10_CMD_SETUP        0x80
#define NINA_W10_CMD_SEND_PACKET  0x81



typedef struct
{
  uint32_t type;
  uint32_t size;
} __attribute__((packed)) nina_req_t;


typedef struct
{
  struct pi_device spim;
  struct pi_device gpio_ready;
  pi_task_t task;
  uint32_t pending_size;
  uint8_t *pending_packet;
  pi_task_t *pending_task;
  pi_task_t *pending_first;
  pi_task_t *pending_last;
  int setup_size;
  uint8_t *setup_command;
  nina_req_t req;
  int access_done;
} nina_t;



static int __nina_w10_send_packet(nina_t *nina, uint8_t *packet, int size, pi_task_t *task);



static int __nina_w10_send_command(nina_t *nina, uint8_t *command, int size, pi_task_t *task)
{
  if (nina->access_done)
  {
    // Nina module is generating a rising edge when it is ready to receive a command
    while(pi_gpio_pin_notif_get(&nina->gpio_ready, CONFIG_NINA_GPIO_NINA_ACK) == 0)
    {
      pi_yield();
    }
  }

  pi_gpio_pin_notif_clear(&nina->gpio_ready, CONFIG_NINA_GPIO_NINA_ACK);

  // TODO this is needed when using JPEG which generates sizes which are not a multiple of 4, but then is not compatible
  // with the streamer which contains a packet size in the request which is sent which is then not aligned with the
  // real size sent. Try to see on the silicon why sending a non-mutiple of 4 is an issue if we have aligned buffers.
  //pi_spi_send_async(&nina->spim, (void *)command, ((size + 3) & ~0x3)*8, PI_SPI_CS_AUTO, task);
  pi_spi_send_async(&nina->spim, (void *)command, size*8, PI_SPI_CS_AUTO, task);

  nina->access_done = 1;

  return 0;
}




static int __nina_w10_get_response(nina_t *nina, uint8_t *response, int size, pi_task_t *task)
{
  if (nina->access_done)
  {
    // Nina module is generating a rising edge when it is ready to receive a command
    while(pi_gpio_pin_notif_get(&nina->gpio_ready, CONFIG_NINA_GPIO_NINA_ACK) == 0)
    {
      pi_yield();
    }
  }

  pi_gpio_pin_notif_clear(&nina->gpio_ready, CONFIG_NINA_GPIO_NINA_ACK);
 
  //pi_spi_receive_async(&nina->spim, (void *)response, ((size + 3) & ~0x3)*8, PI_SPI_CS_AUTO, task);
  pi_spi_receive_async(&nina->spim, (void *)response, size*8, PI_SPI_CS_AUTO, task);

  nina->access_done = 1;

  return 0;
}



static int __nina_w10_append_string(uint8_t *buffer, const char *str)
{
  int index = 0;
  while(1)
  {
    buffer[index] = str[index];
    if (str[index] == 0)
      break;
    index++;
  }
  return index + 1;
}



static int __nina_w10_append_uint32(uint8_t *buffer, uint32_t value)
{
  *(uint32_t *)buffer = value;
  return 4;
}



static void __nina_w10_setup_resume(void *arg)
{
  nina_t *nina = (nina_t *)arg;

  pmsis_l2_malloc_free(nina->setup_command, nina->setup_size);

  pi_task_t *task = nina->pending_task;
  nina->pending_task = NULL;

  __nina_w10_get_response(nina, (uint8_t *)&nina->req, sizeof(nina_req_t), task);
}



static int __nina_w10_setup(nina_t *nina, struct pi_nina_w10_conf *conf, pi_task_t *task)
{
  int setup_size = sizeof(nina_req_t) + strlen(conf->ip_addr) + 1 + strlen(conf->ssid) + 1 + strlen(conf->passwd) + 1;
  uint8_t *setup_command = pmsis_l2_malloc(setup_size);
  if (setup_command == NULL)
    return -1;

  uint8_t *current = setup_command;

  nina_req_t *req = (nina_req_t *)current;

  req->type = NINA_W10_CMD_SETUP;

  nina->pending_task = task;

  current += sizeof(nina_req_t);

  current += __nina_w10_append_string(current, conf->ssid);
  current += __nina_w10_append_string(current, conf->passwd);
  current += __nina_w10_append_string(current, conf->ip_addr);
  current += __nina_w10_append_uint32(current, conf->port);

  nina->setup_size = setup_size;
  nina->setup_command = setup_command;

  // TODO workaround until SPI driver non-multiple of 4 for the size
  int size = ((current - setup_command) + 3) & ~3;

  __nina_w10_send_command(nina, setup_command, size, pi_task_callback(&nina->task, __nina_w10_setup_resume, (void *)nina));

  return 0;
}



static void __nina_w10_send_packet_end(void *arg)
{
  nina_t *nina = (nina_t *)arg;

  int irq = disable_irq();

  pi_task_push(nina->pending_task);
  nina->pending_task = NULL;

  if (nina->pending_first)
  {
    pi_task_t *task = nina->pending_first;
    nina->pending_first = task->implem.next;
    __nina_w10_send_packet(nina, (uint8_t *)task->implem.data[0], task->implem.data[1], task);
  }

  restore_irq(irq);
}



static void __nina_w10_send_packet_resume(void *arg)
{
  nina_t *nina = (nina_t *)arg;

  if (nina->pending_size)
  {
    uint32_t iter_size = 1024;
    if (iter_size > nina->pending_size)
      iter_size = nina->pending_size;

    __nina_w10_send_command(nina, nina->pending_packet, iter_size, pi_task_callback(&nina->task, __nina_w10_send_packet_resume, nina));

    nina->pending_size -= iter_size;
    nina->pending_packet += iter_size;
  }
  else
  {
    __nina_w10_get_response(nina, (uint8_t *)&nina->req, sizeof(nina_req_t), pi_task_callback(&nina->task, __nina_w10_send_packet_end, nina));
  }
}



static int __nina_w10_send_packet(nina_t *nina, uint8_t *packet, int size, pi_task_t *task)
{
  nina_req_t *req = &nina->req;

  req->type = NINA_W10_CMD_SEND_PACKET;
  req->size = size;

  nina->pending_task = task;
  nina->pending_size = size;
  nina->pending_packet = packet;

  __nina_w10_send_command(nina, (uint8_t *)&nina->req, sizeof(nina_req_t), pi_task_callback(&nina->task, __nina_w10_send_packet_resume, nina));

  return 0;
}



int __nina_w10_open(struct pi_device *device)
{
  struct pi_nina_w10_conf *conf = (struct pi_nina_w10_conf *)device->config;

  nina_t *nina = (nina_t *)pmsis_l2_malloc(sizeof(nina_t));
  if (nina == NULL) return -1;

  struct pi_gpio_conf gpio_conf;
  pi_gpio_conf_init(&gpio_conf);

  pi_open_from_conf(&nina->gpio_ready, &gpio_conf);
  pi_gpio_open(&nina->gpio_ready);

  pi_gpio_pin_configure(&nina->gpio_ready, CONFIG_NINA_GPIO_NINA_ACK, PI_GPIO_INPUT);
  pi_gpio_pin_notif_configure(&nina->gpio_ready, CONFIG_NINA_GPIO_NINA_ACK, PI_GPIO_NOTIF_RISE);

  pi_pad_set_function(CONFIG_NINA_GPIO_NINA_ACK_PAD, CONFIG_NINA_GPIO_NINA_ACK_PAD_FUNC);


  struct pi_spi_conf spi_conf;
  pi_spi_conf_init(&spi_conf);
  spi_conf.itf = conf->spi_itf;
  spi_conf.cs = conf->spi_cs;

  spi_conf.wordsize = PI_SPI_WORDSIZE_8;
  spi_conf.big_endian = 1;
  spi_conf.max_baudrate = 30000000;
  spi_conf.polarity = 0;
  spi_conf.phase = 0;

  pi_open_from_conf(&nina->spim, &spi_conf);

  if (pi_spi_open(&nina->spim))
    goto error;

  device->data = (void *)nina;

  nina->pending_task = NULL;
  nina->pending_first = NULL;

  do {
    uint32_t value;
    pi_gpio_pin_read(&nina->gpio_ready, CONFIG_NINA_GPIO_NINA_ACK, &value);
    if (value == 1)
      break;
    pi_time_wait_us(10);
  } while(1);

  nina->access_done = 0;

  pi_task_t task;
  __nina_w10_setup(nina, conf, pi_task_block(&task));
  pi_task_wait_on(&task);

  return 0;

error:
  pmsis_l2_malloc_free(nina, sizeof(nina_t));
  return -1;
}



int __nina_w10_connect(struct pi_device *device, int channel, void (*rcv_callback(void *arg, struct pi_transport_header)), void *arg)
{
  return 0;
}



void __nina_w10_close(struct pi_device *device)
{

}



int __nina_w10_send_async(struct pi_device *device, void *buffer, size_t size, pi_task_t *task)
{
  nina_t *nina = (nina_t *)device->data;

  int irq = disable_irq();

  if (nina->pending_task)
  {
    if (nina->pending_first)
      nina->pending_last->implem.next = task;
    else
      nina->pending_first = task;

    nina->pending_last = task;
    task->implem.next = NULL;

    task->implem.data[0] = (int)buffer;
    task->implem.data[1] = size;
  }
  else
  {
    __nina_w10_send_packet(nina, buffer, size, task);
  }

  restore_irq(irq);

  return 0;
}



int __nina_w10_receive_async(struct pi_device *device, void *buffer, size_t size, pi_task_t *task)
{
  return 0;
}



static pi_transport_api_t nina_w10_api =
{
  .open              = &__nina_w10_open,
  .connect           = &__nina_w10_connect,
  .send_async        = &__nina_w10_send_async,
  .receive_async     = &__nina_w10_receive_async,
  .close             = &__nina_w10_close,
};


void pi_nina_w10_conf_init(struct pi_nina_w10_conf *conf)
{
  conf->transport.api = &nina_w10_api;
  bsp_nina_w10_conf_init(conf);
}

