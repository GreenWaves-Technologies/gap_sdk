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

#ifndef __TOOLS__SERVICES_H__
#define __TOOLS__SERVICES_H__

#define TRANSPORT_USER_FIRST_CHANNEL 1024

#define TRANSPORT_SERVICE_FRAME_STREAMER 1



#define NAME_LENGTH 64

typedef struct 
{
  uint32_t type;
  uint32_t channel;
  uint32_t width;
  uint32_t height;
  uint32_t depth;
  char name[NAME_LENGTH];
} frame_streamer_open_req_t;


#endif