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

#ifndef __BSP_TRANSPORT_NINA_W10_H__
#define __BSP_TRANSPORT_NINA_W10_H__

#include "bsp/transport.h"

struct pi_nina_w10_conf
{
  struct pi_transport_conf transport;
  int spi_itf;
  int spi_cs;
  const char *ssid;
  const char *passwd;
  const char *ip_addr;
  uint32_t port;
};

void pi_nina_w10_conf_init(struct pi_nina_w10_conf *conf);

#endif
