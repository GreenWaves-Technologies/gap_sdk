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

#ifndef __DRIVERS__BUFFER_H__
#define __DRIVERS__BUFFER_H__

#include <stdint.h>
#include <pmsis.h>

typedef enum {
  PI_BUFFER_TYPE_L2,
  PI_BUFFER_TYPE_FC_TCDM,
  PI_BUFFER_TYPE_L1,
  PI_BUFFER_TYPE_HYPERRAM
} pi_buffer_type_e;


typedef enum {
  PI_BUFFER_FORMAT_GRAY,
  PI_BUFFER_FORMAT_RGB565
} pi_buffer_format_e;

typedef struct {
  pi_buffer_type_e type;
  void *data;
  uint32_t width;
  uint32_t height;
  uint32_t channels;
  uint32_t stride;
  uint32_t size;
  pi_buffer_format_e format;
} pi_buffer_t;

static inline int pi_buffer_size(pi_buffer_t *buffer)
{
  return buffer->width*buffer->height*buffer->channels;
}

static inline void pi_buffer_init(pi_buffer_t *buffer, pi_buffer_type_e type, void *data);

static inline void pi_buffer_set_stride(pi_buffer_t *buffer, uint32_t stride);

static inline void pi_buffer_set_format(pi_buffer_t *buffer, uint32_t width, uint32_t height, uint32_t channels, pi_buffer_format_e format);


static inline void pi_buffer_init(pi_buffer_t *buffer, pi_buffer_type_e type, void *data)
{
  buffer->type = type;
  buffer->stride = 0;
  buffer->data = data;
}

static inline void pi_buffer_set_stride(pi_buffer_t *buffer, uint32_t stride)
{
  buffer->stride = stride;
}

static inline void pi_buffer_set_format(pi_buffer_t *buffer, uint32_t width, uint32_t height, uint32_t channels, pi_buffer_format_e format)
{
  buffer->format = format;
  buffer->width = width;
  buffer->height = height;
  buffer->channels = channels;
}

#endif
