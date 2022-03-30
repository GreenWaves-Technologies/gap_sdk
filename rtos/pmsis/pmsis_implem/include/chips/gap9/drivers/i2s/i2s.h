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

#pragma once

static inline int pi_i2s_frame_read(struct pi_device *dev, uint32_t frame, void **mem_block, size_t *size)
{
    return pi_i2s_channel_read(dev, __FF1(frame), mem_block, size);
}

static inline int pi_i2s_frame_read_async(struct pi_device *dev, uint32_t frame, pi_task_t *task)
{
    return pi_i2s_channel_read_async(dev, __FF1(frame), task);
}

int __pi_i2s_channel_conf_set(struct pi_device *device, uint32_t frame, int slot_id, struct pi_i2s_channel_conf *conf);

static inline int pi_i2s_channel_conf_set(struct pi_device *dev, int channel, struct pi_i2s_channel_conf *conf)
{
    return __pi_i2s_channel_conf_set(dev, 0, channel, conf);
}

static inline int pi_i2s_frame_channel_conf_set(struct pi_device *dev, uint32_t frame, int channel, struct pi_i2s_channel_conf *conf)
{
    return __pi_i2s_channel_conf_set(dev, frame, channel, conf);
}
