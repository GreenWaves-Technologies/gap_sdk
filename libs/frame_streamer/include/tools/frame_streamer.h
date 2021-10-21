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

#ifndef __TOOLS__FRAME_STREAMER_H__
#define __TOOLS__FRAME_STREAMER_H__

#include <bsp/buffer.h>
#include <tools/services.h>
#include <tools/frame_streamer_constants.h>

struct frame_streamer_conf {
  struct pi_device *transport;
  frame_streamer_format_e format;
  uint32_t width;
  uint32_t height;
  uint32_t depth;
  char *name;
};

typedef struct frame_streamer_s frame_streamer_t;


int frame_streamer_conf_init(struct frame_streamer_conf *conf);

frame_streamer_t *frame_streamer_open(struct frame_streamer_conf *conf);

int frame_streamer_send_async(frame_streamer_t *streamer, pi_buffer_t *buffer, pi_task_t *task);

int frame_streamer_send(frame_streamer_t *streamer, pi_buffer_t *buffer);

#endif