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

#include <pmsis.h>
#include <string.h>

#define I2S_NB_SLOTS 16

typedef struct
{
    void **buffer;
    uint8_t nb_elem;
    uint8_t head;
    uint8_t tail;
    uint8_t current_nb_elem;
} __pi_ringbuffer_t;

typedef struct __pi_i2s_s __pi_i2s_t;

typedef struct
{
    __pi_i2s_t *i2s;
    uint32_t channel_base;
    pi_task_t *waiting_first;
    pi_task_t *waiting_last;
    size_t block_size;
    void *pingpong_buffers[2];
    pi_mem_slab_t *mem_slab;
    __pi_ringbuffer_t ring_buffer;
    pi_task_t *tx_task0;
    pi_task_t *tx_task1;
    void *tx_buffer0;
    void *tx_buffer1;
    uint16_t frame;
    uint8_t nb_ready_buffer;
    uint8_t udma_channel;
    uint8_t reenqueue;
    uint8_t current_buffer;
    uint8_t error_id;
    uint8_t is_rx;
    uint8_t error;
    uint8_t loopback;
    uint8_t ignore_first_error;    // This flag is set to 1 after the slot is enabled, since we can get a false error at
                                   // the very beginning of the transfer, in order to ignore it. This false error can happen
                                   // in some rare cases depending on the activity because the DC fifo take more time
                                   // than expected to propagate the first sample.
} __pi_i2s_slot_t;


typedef struct __pi_i2s_s
{
    struct pi_i2s_conf conf;
    uint8_t open_count;
    int i2s_freq;
    __pi_i2s_slot_t *rx_slots[I2S_NB_SLOTS];
    __pi_i2s_slot_t *tx_slots[I2S_NB_SLOTS];
    uint32_t cfg;
    uint32_t clk_cfg;
    uint32_t slot_en;
    uint32_t base;
    uint32_t errors;
    uint32_t frame_period_us;
    // TODO this is used when stopping the slot to wait for 2 clock periods
    // This consumes a lot of memory, this could be done with callbacks if the
    // structure of callbacks is having what is needed for tasks.
    pi_task_t task;
    uint32_t pending_stop;
    uint32_t waiting_stop;
    pi_task_t *stop_pending_tasks;
    pi_task_t *stop_waiting_tasks;
    uint8_t itf;
} __pi_i2s_t;


PI_FC_L1 static __pi_i2s_t __pi_i2s[ARCHI_UDMA_NB_I2S];


static inline void __pi_i2s_enqueue_buffer(__pi_i2s_slot_t *slot, void *buffer);
static inline void __pi_i2s_enqueue_buffer_for_frame(__pi_i2s_slot_t *slot, uint32_t buffer);


void pi_i2s_setup(uint32_t flags)
{
}


void pi_i2s_conf_init(struct pi_i2s_conf *conf)
{
    conf->itf = 0;
    conf->frame_clk_freq = 44100;
    conf->format = PI_I2S_CH_FMT_DATA_ORDER_MSB |
        PI_I2S_CH_FMT_DATA_ALIGN_LEFT |
        PI_I2S_CH_FMT_DATA_SIGN_NO_EXTEND;
    conf->options = PI_I2S_OPT_PINGPONG | PI_I2S_OPT_IS_RX | PI_I2S_OPT_ENABLED;
    conf->word_size = 16;
    conf->ws_delay = 1;
    conf->mem_word_size = -1;
    conf->channels = 1;
    conf->pingpong_buffers[0] = NULL;
    conf->pingpong_buffers[1] = NULL;
    conf->mem_slab = NULL;
    conf->pdm_polarity=0;
    conf->pdm_diff=0;
    conf->channel_id = 0;
    conf->asrc_channel = -1;
    conf->ws_type = 0;
}


static void *__pi_i2s_ring_buffer_pop(__pi_i2s_slot_t *slot)
{
    if (slot->ring_buffer.current_nb_elem == 0)
        return NULL;

    void *buffer = slot->ring_buffer.buffer[slot->ring_buffer.tail++];
    if (slot->ring_buffer.tail == slot->ring_buffer.nb_elem)
        slot->ring_buffer.tail = 0;

    slot->ring_buffer.current_nb_elem--;

    return buffer;
}


static void __pi_i2s_handle_rx_frame(uint32_t event, void *arg)
{
    __pi_i2s_slot_t *frame_slot = (__pi_i2s_slot_t *)arg;
    __pi_i2s_t *i2s = frame_slot->i2s;
    uint32_t frame = frame_slot->frame;
    int block_size = frame_slot->block_size;

    // Get error code. Be careful that reading it is clearing the error code for all
    // slots. Save it for all slots to not lose the information.
    i2s->errors |= udma_i2s_err_status_get(i2s->base);
    uint32_t errors = i2s->errors & frame_slot->frame;

    if (!errors)
    {
        if (frame_slot->is_rx)
        {
            void *buffer;
            if (frame_slot->mem_slab)
            {
                pi_mem_slab_alloc(frame_slot->mem_slab, (void **)&buffer, 0);
            }
            else
            {
                // Reenqueue pingpong buffer in UDMA
                int buffer_index = frame_slot->current_buffer;
                buffer = frame_slot->pingpong_buffers[buffer_index];
                frame_slot->current_buffer = buffer_index ^ 1;
            }

            if (buffer)
            {
                __pi_i2s_enqueue_buffer_for_frame(frame_slot, (uint32_t)buffer);
            }

            //And check if we give the finished buffer to a waiting task or we just flag it
            pi_task_t *waiting = frame_slot->waiting_first;

            if (waiting)
            {
                waiting->data[1] = (int)__pi_i2s_ring_buffer_pop(frame_slot);
                frame_slot->waiting_first = waiting->next;
                pi_task_push_irq_safe(waiting);
            }
            else
            {
                frame_slot->nb_ready_buffer++;
            }
        }
        else
        {
            if (frame_slot->mem_slab)
            {
                pi_mem_slab_free(frame_slot->mem_slab, &frame_slot->tx_buffer0);
            }

            frame_slot->tx_buffer0 = frame_slot->tx_buffer1;
            frame_slot->tx_buffer1 = __pi_i2s_ring_buffer_pop(frame_slot);

            if (frame_slot->tx_buffer1)
            {
                uint32_t frame = frame_slot->frame;
                int block_size = frame_slot->block_size;
                uint32_t buffer = (uint32_t)frame_slot->tx_buffer1;

                while (frame)
                {
                    int slot_id = __FF1(frame);

                    frame = __BITCLR_R(frame, 1, slot_id);

                    __pi_i2s_slot_t *slot = frame_slot->i2s->tx_slots[slot_id];
                    uint32_t base = slot->channel_base;

                    udma_core_lin_addrgen_cfg_sa_buf0_set(base, buffer);

                    udma_core_lin_addrgen_cfg_ctrl_set(base, UDMA_CORE_LIN_ADDRGEN_CFG_CTRL_EN(1));

                    buffer += block_size;
                }
            }

            pi_task_t *waiting = frame_slot->waiting_first;

            if (waiting)
            {
                frame_slot->waiting_first = waiting->next;
                pi_task_push_irq_safe(waiting);
            }
        }
    }
    else
    {
        // In case of errors, cancel all pending tasks and return an error
        pi_task_t *waiting;

        i2s->errors = __BITCLR_R(i2s->errors, 16, errors);

        while ((waiting = frame_slot->waiting_first) != NULL)
        {
            waiting->data[0] = -1;
            frame_slot->waiting_first = waiting->next;
            pi_task_push_irq_safe(waiting);
        }
    }
}

static void __pi_i2s_handle_tx_frame(uint32_t event, void *arg)
{
    __pi_i2s_slot_t *frame_slot = (__pi_i2s_slot_t *)arg;
    __pi_i2s_t *i2s = frame_slot->i2s;
    uint32_t frame = frame_slot->frame;
    int block_size = frame_slot->block_size;

    // Get error code. Be careful that reading it is clearing the error code for all
    // slots. Save it for all slots to not lose the information.
    i2s->errors |= udma_i2s_err_status_get(i2s->base);
    uint32_t errors = i2s->errors & frame_slot->frame;
    i2s->errors = __BITCLR_R(i2s->errors, 16, errors);

    // Check if we get the error because this is the beginning of the transfer.
    if (frame_slot->ignore_first_error)
    {
        // Always clear it, since we ignore it only for the first transfer, we need to detected the following ones.
        frame_slot->ignore_first_error = 0;
        errors = 0;
    }

    pi_task_t *waiting = frame_slot->tx_task0;

    if (frame_slot->mem_slab)
    {
        void *buffer = (void *)waiting->data[3];
        pi_mem_slab_free(frame_slot->mem_slab, &buffer);
    }

    frame_slot->tx_task0 = frame_slot->tx_task1;
    frame_slot->tx_task1 = frame_slot->waiting_first;
    if (frame_slot->waiting_first)
    {
        frame_slot->waiting_first = frame_slot->waiting_first->next;
    }

    if (frame_slot->tx_task1)
    {
        uint32_t frame = frame_slot->frame;
        int block_size = frame_slot->block_size;
        uint32_t buffer = frame_slot->tx_task1->data[3];

        while (frame)
        {
            int slot_id = __FF1(frame);

            frame = __BITCLR_R(frame, 1, slot_id);

            __pi_i2s_slot_t *slot = frame_slot->i2s->tx_slots[slot_id];
            uint32_t base = slot->channel_base;

            udma_core_lin_addrgen_cfg_sa_buf0_set(base, buffer);

            udma_core_lin_addrgen_cfg_ctrl_set(base, UDMA_CORE_LIN_ADDRGEN_CFG_CTRL_EN(1));

            buffer += block_size;
        }
    }

    waiting->data[0] = errors;
    pi_task_push_irq_safe(waiting);
}


static void __pi_i2s_handle_rx_channel(uint32_t event, void *arg)
{
    __pi_i2s_slot_t *slot = (__pi_i2s_slot_t *)arg;
    __pi_i2s_t *i2s = slot->i2s;
    int task_done = 1;
    void *buffer;

    // Get error code. Be careful that reading it is clearing the error code for all
    // slots. Save it for all slots to not lose the information.
    i2s->errors |= udma_i2s_err_status_get(i2s->base);
    slot->error = __BITEXTRACTU_R(i2s->errors, 1, slot->error_id);

    // Check if we get the error because this is the beginning of the transfer.
    if (slot->ignore_first_error)
    {
        // Always clear it, since we ignore it only for the first transfer, we need to detected the following ones.
        slot->ignore_first_error = 0;
        slot->error = 0;
        i2s->errors = __BITCLR_R(i2s->errors, 1, slot->error_id);
    }

    // Reenqueue pingpong buffer in UDMA
    if (!slot->error)
    {
        if (slot->is_rx)
        {
            if (slot->mem_slab)
            {
                pi_mem_slab_alloc(slot->mem_slab, (void **)&buffer, 0);
            }
            else
            {
                int buffer_index = slot->current_buffer;
                buffer = slot->pingpong_buffers[buffer_index];
                slot->current_buffer = buffer_index ^ 1;
            }

            if (buffer)
            {
                __pi_i2s_enqueue_buffer(slot, buffer);
            }

            //And check if we give the finished buffer to a waiting task or we just flag it
            pi_task_t *waiting = slot->waiting_first;

            if (waiting)
            {
                waiting->data[1] = (int)__pi_i2s_ring_buffer_pop(slot);
                slot->waiting_first = waiting->next;
                pi_task_push_irq_safe(waiting);
            }
            else
            {
                slot->nb_ready_buffer++;
            }
        }
        else
        {
            if (slot->mem_slab)
            {
                pi_mem_slab_free(slot->mem_slab, &slot->tx_buffer0);
            }

            slot->tx_buffer0 = slot->tx_buffer1;
            slot->tx_buffer1 = __pi_i2s_ring_buffer_pop(slot);

            if (slot->tx_buffer1)
            {
                uint32_t base = slot->channel_base;
                udma_core_lin_addrgen_cfg_sa_buf0_set(base, (int)slot->tx_buffer1);
                udma_core_lin_addrgen_cfg_ctrl_set(base, UDMA_CORE_LIN_ADDRGEN_CFG_CTRL_EN(1));
            }

            pi_task_t *waiting = slot->waiting_first;

            if (waiting)
            {
                slot->waiting_first = waiting->next;
                pi_task_push_irq_safe(waiting);
            }
        }

    }
    else
    {
        // In case of errors, cancel all pending tasks and return an error
        pi_task_t *waiting;

        i2s->errors = __BITCLR_R(i2s->errors, 1, slot->error_id);

        while ((waiting = slot->waiting_first) != NULL)
        {
            waiting->data[0] = -1;
            slot->waiting_first = waiting->next;
            pi_task_push_irq_safe(waiting);
        }
    }
}

static void __pi_i2s_handle_tx_channel(uint32_t event, void *arg)
{
    __pi_i2s_slot_t *slot = (__pi_i2s_slot_t *)arg;
    __pi_i2s_t *i2s = slot->i2s;

    // Get error code. Be careful that reading it is clearing the error code for all
    // slots. Save it for all slots to not lose the information.
    i2s->errors |= udma_i2s_err_status_get(i2s->base);
    int error = __BITEXTRACTU_R(i2s->errors, 1, slot->error_id);
    i2s->errors = __BITCLR_R(i2s->errors, 1, slot->error_id);

    // Check if we get the error because this is the beginning of the transfer.
    if (slot->ignore_first_error)
    {
        // Always clear it, since we ignore it only for the first transfer, we need to detected the following ones.
        slot->ignore_first_error = 0;
        error = 0;
    }

    pi_task_t *waiting = slot->tx_task0;

    // Reenqueue pingpong buffer in UDMA
    if (slot->mem_slab)
    {
        void *buffer = (void *)waiting->data[3];
        pi_mem_slab_free(slot->mem_slab, &buffer);
    }

    slot->tx_task0 = slot->tx_task1;
    slot->tx_task1 = slot->waiting_first;
    if (slot->waiting_first)
    {
        slot->waiting_first = slot->waiting_first->next;
    }

    if (slot->tx_task1)
    {
        uint32_t base = slot->channel_base;
        uint32_t buffer = slot->tx_task1->data[3];
        udma_core_lin_addrgen_cfg_sa_buf0_set(base, buffer);
        udma_core_lin_addrgen_cfg_ctrl_set(base, UDMA_CORE_LIN_ADDRGEN_CFG_CTRL_EN(1));
    }

    waiting->data[0] = error;
    pi_task_push_irq_safe(waiting);
}


static void __pi_i2s_slot_free(__pi_i2s_slot_t *slot)
{
    if (slot->pingpong_buffers[0] && !slot->loopback)
    {
        udma_core_lin_addrgen_cfg_ctrl_set(slot->channel_base,
            UDMA_CORE_LIN_ADDRGEN_CFG_CTRL_STOP(1)
        );

        pi_udma_core_lin_free(slot->udma_channel);
        soc_eu_fc_mask_set_set(SOC_EU_ADDR, ARCHI_SOC_EVENT_LINEAR_CHANNEL_FIRST + slot->udma_channel);
    }

    pi_fc_l1_free(slot, sizeof(__pi_i2s_slot_t));
}


static inline void __pi_i2s_enqueue_buffer(__pi_i2s_slot_t *slot, void *buffer)
{
    uint32_t base = slot->channel_base;

    udma_core_lin_addrgen_cfg_sa_buf0_set(base, (int)buffer);

    udma_core_lin_addrgen_cfg_ctrl_set(base,
        UDMA_CORE_LIN_ADDRGEN_CFG_CTRL_EN(1)
    );

    slot->ring_buffer.buffer[slot->ring_buffer.head++] = buffer;
    if (slot->ring_buffer.head == slot->ring_buffer.nb_elem)
        slot->ring_buffer.head = 0;

    slot->ring_buffer.current_nb_elem++;
}


static void __pi_i2s_slot_enqueue_buffers(__pi_i2s_slot_t *slot)
{
    void *buffer0;
    void *buffer1;

    if (slot->mem_slab)
    {
        pi_mem_slab_alloc(slot->mem_slab, (void **)&buffer0, 0);
        pi_mem_slab_alloc(slot->mem_slab, (void **)&buffer1, 0);
    }
    else
    {
        buffer0 = slot->pingpong_buffers[0];
        buffer1 = slot->pingpong_buffers[1];
    }

    if (buffer0)
    {
        __pi_i2s_enqueue_buffer(slot, buffer0);
    }
    if (buffer1)
    {
        __pi_i2s_enqueue_buffer(slot, buffer1);
    }
}


static inline void __pi_i2s_enqueue_buffer_for_frame(__pi_i2s_slot_t *first_slot, uint32_t buffer)
{
    uint32_t frame = first_slot->frame;
    int block_size = first_slot->block_size;

    first_slot->ring_buffer.buffer[first_slot->ring_buffer.head++] = (void *)buffer;
    if (first_slot->ring_buffer.head == first_slot->ring_buffer.nb_elem)
        first_slot->ring_buffer.head = 0;

    first_slot->ring_buffer.current_nb_elem++;

    while (frame)
    {
        int slot_id = __FF1(frame);
        frame = __BITCLR_R(frame, 1, slot_id);

        __pi_i2s_slot_t *slot = first_slot->is_rx ? first_slot->i2s->rx_slots[slot_id] : first_slot->i2s->tx_slots[slot_id];
        uint32_t base = slot->channel_base;

        udma_core_lin_addrgen_cfg_sa_buf0_set(base, buffer);

        udma_core_lin_addrgen_cfg_ctrl_set(base, UDMA_CORE_LIN_ADDRGEN_CFG_CTRL_EN(1));

        buffer += block_size;
    }
}


static void __pi_i2s_slot_enqueue_buffers_for_frame(__pi_i2s_slot_t *slot)
{
    void *buffer0;
    void *buffer1;

    if (slot->mem_slab)
    {
        pi_mem_slab_alloc(slot->mem_slab, (void **)&buffer0, 0);
        pi_mem_slab_alloc(slot->mem_slab, (void **)&buffer1, 0);
    }
    else
    {
        buffer0 = slot->pingpong_buffers[0];
        buffer1 = slot->pingpong_buffers[1];
    }

    __pi_i2s_enqueue_buffer_for_frame(slot, (uint32_t)buffer0);
    __pi_i2s_enqueue_buffer_for_frame(slot, (uint32_t)buffer1);
}


void pi_i2s_channel_conf_init(struct pi_i2s_channel_conf *conf)
{
    conf->format = PI_I2S_CH_FMT_DATA_ORDER_MSB |
        PI_I2S_CH_FMT_DATA_ALIGN_LEFT |
        PI_I2S_CH_FMT_DATA_SIGN_NO_EXTEND;
    conf->options = PI_I2S_OPT_PINGPONG | PI_I2S_OPT_IS_RX | PI_I2S_OPT_ENABLED;
    conf->word_size = 16;
    conf->mem_word_size = -1;
    conf->pingpong_buffers[0] = NULL;
    conf->pingpong_buffers[1] = NULL;
    conf->mem_slab = NULL;
    conf->asrc_channel = -1;
    conf->slot_enable = 1;
}


int __pi_i2s_channel_conf_set(struct pi_device *device, uint32_t frame, int slot_id, struct pi_i2s_channel_conf *conf)
{
    unsigned int is_rx = __BITEXTRACTU(conf->options, 1, PI_I2S_OPT_RX_TX_SHIFT);
    __pi_i2s_slot_t *slot;
    uint32_t new_cfg = 0;
    __pi_i2s_t *i2s = (__pi_i2s_t *)device->data;

    int irq = pi_irq_disable();

    unsigned int is_enabled = __BITEXTRACTU(conf->options, 1, PI_I2S_OPT_CH_ENABLE_SHIFT);

    if (!is_enabled)
    {
        //I2S_INF("Disabling I2S channel (itf: %d, channel: %d, is_rx: %d)\n", i2s->itf, slot_id, is_rx);
        slot = is_rx ? i2s->rx_slots[slot_id] : i2s->tx_slots[slot_id];
        __pi_i2s_slot_free(slot);
        slot = NULL;
        new_cfg = 0xff;
        if (is_rx)
        {
            i2s->rx_slots[slot_id] = NULL;
        }
        else
        {
            i2s->tx_slots[slot_id] = NULL;
        }

    }
    else
    {
        // Allocate slot and channel
        unsigned int loopback = __BITEXTRACTU(conf->options, 1, PI_I2S_OPT_LOOPBACK_ENA_SHIFT);
        int ring_buffer_nb_elem = conf->mem_slab ? conf->mem_slab->num_blocks : 2;
        int ring_buffer_size = sizeof(void *) * ring_buffer_nb_elem;

        slot = pi_fc_l1_malloc(sizeof(__pi_i2s_slot_t) + ring_buffer_size);
        if (slot == NULL)
        {
            I2S_WNG("Failed to allocate memory\n");
            goto error;
        }

        if (is_rx)
        {
            i2s->rx_slots[slot_id] = slot;
        }
        else
        {
            i2s->tx_slots[slot_id] = slot;
        }

        // Copy configuration
        slot->pingpong_buffers[0] = conf->pingpong_buffers[0];
        slot->pingpong_buffers[1] = conf->pingpong_buffers[1];
        slot->mem_slab = conf->mem_slab;
        slot->error_id = is_rx ? slot_id : slot_id + 16;
        slot->error = 0;
        slot->i2s = i2s;
        slot->is_rx = is_rx;
        slot->loopback = loopback;
        slot->frame = frame;
        slot->tx_buffer0 = NULL;
        slot->tx_buffer1 = NULL;
        slot->tx_task0 = NULL;
        slot->tx_task1 = NULL;
        slot->ignore_first_error = 0;

        if (conf->asrc_channel == -1 && loopback == 0)
        {
            int channel = pi_udma_core_lin_alloc();
            if (channel == -1)
            {
                I2S_WNG("Failed to allocate channel\n");
                pi_fc_l1_free(slot, sizeof(__pi_i2s_slot_t));
                goto error;
            }

            slot->udma_channel = channel;

            slot->ring_buffer.nb_elem = ring_buffer_nb_elem;
            slot->ring_buffer.current_nb_elem = 0;
            slot->ring_buffer.head = 0;
            slot->ring_buffer.tail = 0;
            slot->ring_buffer.buffer = (void *)((uint32_t)slot + sizeof(__pi_i2s_slot_t));

            slot->block_size = conf->block_size;
            slot->current_buffer = 0;

            slot->nb_ready_buffer = 0;
            slot->waiting_first = NULL;

            slot->channel_base = pi_udma_core_lin_addr_get(slot->udma_channel);
            int event = ARCHI_SOC_EVENT_LINEAR_CHANNEL_FIRST + slot->udma_channel;

            if (frame == 0)
            {
                pi_fc_event_handler_set(event, is_rx ? __pi_i2s_handle_rx_channel : __pi_i2s_handle_tx_channel, (void *)slot);
                soc_eu_fc_mask_clr_set(SOC_EU_ADDR, event);
                udma_core_lin_addrgen_cfg_size_set(slot->channel_base, slot->block_size);
                if (is_rx)
                {
                    __pi_i2s_slot_enqueue_buffers(slot);
                }
            }
            else
            {
                int last_slot_id = __FL1(frame);
                int first_slot_id = __FF1(frame);
                __pi_i2s_slot_t *first_slot = is_rx ? i2s->rx_slots[first_slot_id] : i2s->tx_slots[first_slot_id];
                udma_core_lin_addrgen_cfg_size_set(slot->channel_base, slot->block_size);

                if (slot_id == last_slot_id)
                {
                    pi_fc_event_handler_set(event, is_rx ? __pi_i2s_handle_rx_frame : __pi_i2s_handle_tx_frame, (void *)first_slot);
                    soc_eu_fc_mask_clr_set(SOC_EU_ADDR, event);
                    if (is_rx)
                    {
                        __pi_i2s_slot_enqueue_buffers_for_frame(first_slot);
                    }
                }
            }
        }
        else if (conf->asrc_channel != -1)
        {
            slot->udma_channel = 0xe0 + conf->asrc_channel;
        }
        else
        {
            slot->udma_channel = 0;
        }

        // Store the word size
        // There are 2 slots per register
        int reg_offset = (slot_id >> 1) << 2;
        // Slot 0 RX at 0, slot 0 TX at 1, slot 1 RX at 16 and slot 1 TX at 24
        int bit = (slot_id & 1) << 4;
        if (!is_rx)
        {
            bit += 8;
        }

        uint32_t reg_value = udma_i2s_word_size_0_1_get(i2s->base + reg_offset);
        reg_value = GAP_BINSERT_R(reg_value, conf->word_size-1, UDMA_I2S_WORD_SIZE_0_1_WORD_SIZE_RX_0_WIDTH, bit);
        udma_i2s_word_size_0_1_set(i2s->base + reg_offset, reg_value);


        // Now compute slot configuration
        //I2S_INF("Configuring I2S channel (itf: %d, channel: %d, is_rx: %d, enabled: 1, word_size: %d)\n",
        //    i2s->itf, slot_id, is_rx, conf->word_size);

        unsigned int is_pdm = __BITEXTRACTU(conf->format, 1, PI_I2S_FMT_DATA_FORMAT_SHIFT);

        if (!is_pdm)
        {
            int mem_word_size = conf->mem_word_size;
            if (mem_word_size == -1) mem_word_size = i2s->conf.word_size;

            unsigned int udma_size = (mem_word_size / 8) - 1;
            unsigned int left_aligned = !__BITEXTRACTU(conf->format, 1, PI_I2S_CH_FMT_DATA_ALIGN_SHIFT);
            unsigned int msb_first = !__BITEXTRACTU(conf->format, 1, PI_I2S_CH_FMT_DATA_ORDER_SHIFT);
            unsigned int sign = __BITEXTRACTU(conf->format, 1, PI_I2S_CH_FMT_DATA_SIGN_SHIFT);
            unsigned int loopback = __BITEXTRACTU(conf->options, 1, PI_I2S_OPT_LOOPBACK_ENA_SHIFT);

            new_cfg =
                UDMA_I2S_SLOT_CFG_0_ID_RX(slot->udma_channel) |
                UDMA_I2S_SLOT_CFG_0_RX_MSB_FIRST(msb_first) |
                UDMA_I2S_SLOT_CFG_0_RX_LEFT_ALIGN(left_aligned) |
                UDMA_I2S_SLOT_CFG_0_RX_DSIZE(udma_size) |
                UDMA_I2S_SLOT_CFG_0_RX_SIGN(sign) |
                UDMA_I2S_SLOT_CFG_0_RX_EN(1) |
                loopback << 15;
        }

    }

    // Finally update the config in the HW register
    uint32_t reg_offset = i2s->base + (slot_id << 2);
    uint32_t cfg = udma_i2s_slot_cfg_0_get(reg_offset);
    cfg = GAP_BINSERT_R(cfg, new_cfg, 16, (!is_rx)*16);

    udma_i2s_slot_cfg_0_set(reg_offset, cfg);

    if (slot && is_enabled && conf->slot_enable)
    {
        pi_i2s_slots_enable(device, 1<<slot_id, 1);
    }

    pi_irq_restore(irq);
    return 0;

error:
    pi_irq_restore(irq);
    return -1;
}


int pi_i2s_open(struct pi_device *device)
{
    struct pi_i2s_conf *conf = (struct pi_i2s_conf *)device->config;
    int itf_id = conf->itf;
    __pi_i2s_t *i2s = &__pi_i2s[itf_id];
    int is_tdm = (conf->options & PI_I2S_OPT_TDM) != 0;

    int is_pdm = (conf->format & PI_I2S_FMT_DATA_FORMAT_PDM)>>PI_I2S_FMT_DATA_FORMAT_SHIFT;

    //I2S_INF("Opening I2S driver (itf: %d)\n", conf->itf);

    device->data = (void *)i2s;

    memcpy(&i2s->conf, conf, sizeof(struct pi_i2s_conf));

    i2s->open_count++;
    if (i2s->open_count == 1)
    {
        int periph_id = UDMA_I2S_ID(itf_id);
        udma_ctrl_cfg_rstn_set_set(UDMA_CTRL_ADDR, 1 << periph_id);
        udma_ctrl_cfg_cg_set_set(UDMA_CTRL_ADDR, 1 << periph_id);

        i2s->itf = itf_id;
        i2s->errors = 0;
        i2s->pending_stop = 0;
        i2s->waiting_stop = 0;
        i2s->stop_pending_tasks = NULL;
        i2s->stop_waiting_tasks = NULL;

        for (int i=0; i<I2S_NB_SLOTS; i++)
        {
            i2s->rx_slots[i] = NULL;
            i2s->tx_slots[i] = NULL;
        }
    }

    i2s->slot_en = 0;
    i2s->base = UDMA_I2S_ADDR(itf_id);

    unsigned int ext_clk = __BITEXTRACTU(conf->options, 1, PI_I2S_OPT_CLK_SRC_SHIFT);
    unsigned int ext_ws = __BITEXTRACTU(conf->options, 1, PI_I2S_OPT_WS_SRC_SHIFT);
    unsigned int ext_clk_int = __BITEXTRACTU(conf->options, 1, PI_I2S_OPT_CLK_INT_ROUTED_SHIFT);
    unsigned int ext_ws_int = __BITEXTRACTU(conf->options, 1, PI_I2S_OPT_WS_INT_ROUTED_SHIFT);
    unsigned int clk_polarity = __BITEXTRACTU(conf->options, 1, PI_I2S_OPT_CLK_POLARITY_SHIFT);
    unsigned int ws_polarity = __BITEXTRACTU(conf->options, 1, PI_I2S_OPT_WS_POLARITY_SHIFT);
    unsigned int clk_gen_en = (!ext_clk) && (!ext_clk_int);
    //I2S_INF("ext_clk (%x), ext_ws (%x), ext_clk_int(%x), ext_ws_int(%x), clk_gen_en (%x)\n", ext_clk, ext_ws, ext_clk_int, ext_ws_int, clk_gen_en);

    i2s->clk_cfg = UDMA_I2S_CLKCFG_SETUP_CLK_EN(clk_gen_en) |
        UDMA_I2S_CLKCFG_SETUP_CLK_SRC(ext_clk) |
        UDMA_I2S_CLKCFG_SETUP_CLK_EXT_SRC(ext_clk_int) |
        UDMA_I2S_CLKCFG_SETUP_CLK_EDGE(clk_polarity) |
        UDMA_I2S_CLKCFG_SETUP_WS_SRC(ext_ws) |
        UDMA_I2S_CLKCFG_SETUP_WS_EXT_SRC (ext_ws_int) |
        UDMA_I2S_CLKCFG_SETUP_WS_EDGE(ws_polarity) |
        UDMA_I2S_CLKCFG_SETUP_WS_TYPE(conf->ws_type);

    i2s->cfg = UDMA_I2S_GLB_SETUP_FRAME_LENGTH(conf->channels-1) |
        UDMA_I2S_GLB_SETUP_SLOT_WIDTH(is_pdm? 0 : (conf->word_size-1)) |
        UDMA_I2S_GLB_SETUP_WS_DELAY(is_pdm? 0 :(conf->ws_delay)) |
        UDMA_I2S_GLB_SETUP_FULL_DUPLEX_EN(0) |
        UDMA_I2S_GLB_SETUP_PDM_EN(is_pdm) |
        UDMA_I2S_GLB_SETUP_PDM_POL(conf->pdm_polarity) |
        UDMA_I2S_GLB_SETUP_PDM_DIFF(conf->pdm_diff);

    if (conf->options & PI_I2S_OPT_FULL_DUPLEX)
    {
        i2s->cfg |= UDMA_I2S_GLB_SETUP_FULL_DUPLEX_EN(1);
    }

    /* TODO: correct the frequency calculation for PDM */
    if (is_pdm)
        i2s->i2s_freq = conf->frame_clk_freq;
    else
        i2s->i2s_freq = conf->frame_clk_freq * conf->word_size * conf->channels;

    i2s->frame_period_us = (1000000 + conf->frame_clk_freq - 1) / conf->frame_clk_freq;

    if (!is_tdm)
    {
        struct pi_i2s_channel_conf channel_conf = {
            .block_size = conf->block_size, .mem_slab=conf->mem_slab,
            .pingpong_buffers[0] = conf->pingpong_buffers[0], .pingpong_buffers[1] = conf->pingpong_buffers[1],
            .format = conf->format, .word_size=conf->word_size, .mem_word_size=conf->mem_word_size,
            .options = conf->options | PI_I2S_OPT_ENABLED | PI_I2S_OPT_IS_RX, .slot_enable=1
        };

        pi_i2s_channel_conf_set(device, 0, &channel_conf);

        struct pi_i2s_channel_conf channel_conf_tx = {
            .block_size = conf->block_size, .mem_slab=conf->mem_slab,
            .pingpong_buffers[0] = conf->pingpong_buffers[0], .pingpong_buffers[1] = conf->pingpong_buffers[1],
            .format = conf->format, .word_size=conf->word_size, .mem_word_size=conf->mem_word_size,
            .options = conf->options | PI_I2S_OPT_ENABLED | PI_I2S_OPT_IS_TX, .slot_enable=1
        };

        pi_i2s_channel_conf_set(device, 0, &channel_conf_tx);
    }

    //unsigned int base = hal_udma_channel_base(i2s->channel);
    int periph_freq = 0;
    if (i2s->conf.options & PI_I2S_OPT_REF_CLK_FAST)
    {
        periph_freq = CONFIG_FAST_OSC_FREQUENCY;
        udma_i2s_clk_fast_set(UDMA_I2S_ADDR(i2s->conf.itf), UDMA_I2S_CLK_FAST_FAST_EN(1));
    }
    else
    {
        periph_freq = pi_freq_get(PI_FREQ_DOMAIN_PERIPH);
    }
    int div = periph_freq / i2s->i2s_freq;

    udma_i2s_clkcfg_setup_set(UDMA_I2S_ADDR(i2s->conf.itf),
        UDMA_I2S_CLKCFG_SETUP_CLK_DIV(div) |
        i2s->clk_cfg
    );

    return 0;
}


static __attribute__((noinline)) void __pi_i2s_suspend(__pi_i2s_t *i2s)
{
    // Stop the clock now
    udma_i2s_glb_setup_set(UDMA_I2S_ADDR(i2s->conf.itf),
        UDMA_I2S_GLB_SETUP_GLOBAL_EN(0)
    );

    // Once the clock is down, the udma can still write samples until the end
    // of the current frame. Wait for one frame period to be sure
    // everything is flushed
    pi_time_wait_us(i2s->frame_period_us);
}


void pi_i2s_close(struct pi_device *device)
{
    __pi_i2s_t *i2s = (__pi_i2s_t *)device->data;

    i2s->open_count--;
    if (i2s->open_count == 0)
    {
        // Close all active slots
        udma_i2s_slot_en_set(i2s->base, 0);

        for (int i=0; i<I2S_NB_SLOTS; i++)
        {
            if (i2s->rx_slots[i])
            {
                __pi_i2s_slot_free(i2s->rx_slots[i]);
            }
            if (i2s->tx_slots[i])
            {
                __pi_i2s_slot_free(i2s->tx_slots[i]);
            }
        }

        // And deactivated device
        int periph_id = UDMA_I2S_ID(i2s->itf);
        udma_ctrl_cfg_rstn_clr_set(UDMA_CTRL_ADDR, 1 << periph_id);
        udma_ctrl_cfg_cg_clr_set(UDMA_CTRL_ADDR, 1 << periph_id);
    }
}


static void __pi_i2s_slots_enable(__pi_i2s_t *i2s, uint32_t channels, int enabled)
{
    if (enabled)
    {
        i2s->slot_en |= channels;

        while(channels)
        {
            int channel_id = __FF1(channels);
            channels = __BITCLR_R(channels, 1, channel_id);

            // Due the HW issue with first transfer, we need to remember to ignore errors on the first transfer.
            __pi_i2s_slot_t *slot = i2s->tx_slots[channel_id];
            if (slot)
            {
                slot->ignore_first_error = 1;
            }
        }
    }
    else
    {
        i2s->slot_en &= ~channels;
    }
    udma_i2s_slot_en_set(i2s->base, i2s->slot_en);
}



int pi_i2s_slots_enable(struct pi_device *device, uint32_t channels, int enabled)
{
    __pi_i2s_t *i2s = (__pi_i2s_t *)device->data;

    int irq = pi_irq_disable();
    __pi_i2s_slots_enable(i2s, channels, enabled);
    pi_irq_restore(irq);

    return 0;
}


static void __pi_i2s_slot_stop(__pi_i2s_slot_t *slot)
{
    if (slot == NULL)
    {
        return;
    }

    // Be careful to read the size left before stoppping, otherwise it goes to zero
    int size_left = udma_core_lin_addrgen_cfg_bytes_left_get(slot->channel_base);

    udma_core_lin_addrgen_cfg_ctrl_set(slot->channel_base, UDMA_CORE_LIN_ADDRGEN_CFG_CTRL_STOP(1));

    if (slot->waiting_first)
    {
        slot->waiting_first->data[2] = slot->block_size - size_left;
    }

    if (slot->is_rx)
    {
        __pi_i2s_handle_rx_channel(0, slot);
    }
    else
    {
        __pi_i2s_handle_tx_channel(0, slot);
    }
}


static void __pi_i2s_channel_stop(void *arg)
{
    __pi_i2s_t *i2s = (__pi_i2s_t *)arg;

    int irq = pi_irq_disable();

    uint32_t pending_stop = i2s->pending_stop;

    // Stop all specified slots
    while (pending_stop)
    {
        int slot = __FF1(pending_stop);
        __pi_i2s_slot_stop(i2s->rx_slots[slot]);
        __pi_i2s_slot_stop(i2s->tx_slots[slot]);
        pending_stop = __BITCLR_R(pending_stop, 1, slot);
    }

    // Unclock all tasks waiting for this stop
    pi_task_t *task = i2s->stop_pending_tasks;
    while(task)
    {
        pi_task_push_irq_safe(task);
        task = task->next;
    }

    // In case another set of tasks were waiting for stopping channels,
    // Start now their delay to be sure they wait full 2 frame clocks.
    if (i2s->waiting_stop)
    {
        i2s->pending_stop = i2s->waiting_stop;
        i2s->waiting_stop = 0;
        i2s->stop_pending_tasks = i2s->stop_waiting_tasks;
        i2s->stop_waiting_tasks = NULL;
        // UDMA needs one full frame period to disable a lot, so we must wait 2 periods
        pi_task_push_delayed_us(pi_task_callback(&i2s->task, __pi_i2s_channel_stop, (void *)i2s), 2*i2s->frame_period_us);
    }
    else
    {
        i2s->pending_stop = 0;
    }

    pi_irq_restore(irq);
}


int pi_i2s_slots_stop_async(struct pi_device *device, uint32_t channel_mask, pi_task_t *task)
{
    __pi_i2s_t *i2s = (__pi_i2s_t *)device->data;

    int irq = pi_irq_disable();

    // First disable the slots
    __pi_i2s_slots_enable(i2s, channel_mask, 0);

    // Don't know why, the udma should stop after 1 frame period, but it actually continues a bit,
    // probably a HW issue, just wait one more period.
    if (i2s->pending_stop)
    {
        // If a stop is already pending, just remember that we need to wait another 2 frame clocks.
        // In case we are already waiting for a stop, this is also fine, because we wait 2 frame clocks
        // at least starting from now.
        i2s->waiting_stop |= channel_mask;
        task->next = i2s->stop_waiting_tasks;
        i2s->stop_waiting_tasks = task;
    }
    else
    {
        i2s->pending_stop = channel_mask;
        task->next = i2s->stop_pending_tasks;
        i2s->stop_pending_tasks = task;
        // UDMA needs one full frame period to disable a lot, so we must wait 2 periods
        pi_task_push_delayed_us(pi_task_callback(&i2s->task, __pi_i2s_channel_stop, (void *)i2s), 2*i2s->frame_period_us);
    }

    pi_irq_restore(irq);

    return 0;
}


int pi_i2s_slots_stop(struct pi_device *device, uint32_t channel_mask)
{
    pi_task_t task;
    if (pi_i2s_slots_stop_async(device, channel_mask, pi_task_block(&task)))
    {
        return -1;
    }
    pi_task_wait_on(&task);

    return 0;
}


static void __pi_i2s_resume(struct pi_device *device, __pi_i2s_t *i2s)
{
    //I2S_INF("Resuming (i2s: %d)\n", i2s->conf.itf);

    udma_i2s_glb_setup_set(i2s->base,
        UDMA_I2S_GLB_SETUP_GLOBAL_EN(1) |
        i2s->cfg
    );
}


static void __pi_i2s_sync(struct pi_device *device, __pi_i2s_t *i2s, unsigned int team)
{
    uint32_t value = udma_i2s_glb_setup_get(i2s->base);
    value |= UDMA_I2S_GLB_SETUP_BLOCK_COMMIT(team);
    udma_i2s_glb_setup_set(i2s->base, value);
}



static void __pi_i2s_ch_conf_get(__pi_i2s_t *i2s, struct pi_i2s_conf *conf)
{
    //__pi_i2s_slot_t *slot = &i2s->slots[conf->channel_id];
    //struct pi_i2s_conf *ch_conf = conf->options & PI_I2S_OPT_IS_RX ? &slot->rx_conf : &slot->tx_conf;
    //memcpy(conf, ch_conf, sizeof(struct pi_i2s_conf));
}


int pi_i2s_channel_timestamp_set(struct pi_device *device, int slot_id, struct pi_i2s_channel_conf *conf)
{
    __pi_i2s_t *i2s = (__pi_i2s_t *)device->data;

    unsigned int is_rx = __BITEXTRACTU(conf->options, 1, PI_I2S_OPT_RX_TX_SHIFT);
    __pi_i2s_slot_t *slot = is_rx ? i2s->rx_slots[slot_id] : i2s->tx_slots[slot_id];

    uint32_t base = ARCHI_UDMA_ADDR;
    uint8_t evt_id = conf->ts_evt_id;
    uint8_t soc_evt = slot->udma_channel;

    uint32_t cfg_evt_val = (udma_ctrl_cfg_event_get(base) & ~(0xFF<<evt_id*8))| (soc_evt << (evt_id*8));

    pi_soc_eu_pr_mask_set(soc_evt);
    udma_ctrl_cfg_event_set(base, cfg_evt_val);

    return 0;

}

int pi_i2s_ioctl(struct pi_device *device, uint32_t cmd, void *arg)
{
    __pi_i2s_t *i2s = (__pi_i2s_t *)device->data;
    int err = 0;

    //I2S_INF("Ioctl (cmd: %ld, arg: %p)\n", cmd, arg);

    int irq = pi_irq_disable();

    switch (cmd)
    {
        case PI_I2S_IOCTL_SYNC:
            __pi_i2s_sync(device, i2s, (int)arg);
            break;

        case PI_I2S_IOCTL_START:
            __pi_i2s_resume(device, i2s);
            break;

        case PI_I2S_IOCTL_STOP:
            __pi_i2s_suspend(i2s);
            break;

        default:
            pi_irq_restore(irq);
            return -1;
    }

    pi_irq_restore(irq);

    return err;
}

int pi_i2s_channel_read_async(struct pi_device *device, int channel, pi_task_t *task)
{
    __pi_i2s_t *i2s = (__pi_i2s_t *)device->data;
    __pi_i2s_slot_t *slot = i2s->rx_slots[channel];

    int irq = pi_irq_disable();

    if (slot->error)
    {
        task->data[0] = -1;
        pi_task_push_irq_safe(task);
        pi_irq_restore(irq);
        return -1;
    }

    // Prepare now the task results, the IRQ handler can still overwrite
    // them if they are different.
    task->data[0] = 0;

    task->data[2] = slot->block_size;

    if (slot->nb_ready_buffer)
    {
        task->data[1] = (int)__pi_i2s_ring_buffer_pop(slot);
        slot->nb_ready_buffer--;
        pi_task_push_irq_safe(task);
    }
    else
    {
        if (slot->waiting_first)
            slot->waiting_last->next = task;
        else
            slot->waiting_first = task;

        task->next = NULL;
        slot->waiting_last = task;
    }

    pi_irq_restore(irq);

    return 0;
}


int pi_i2s_channel_write_async(struct pi_device *dev, int channel,
    void *mem_block, size_t size, pi_task_t *task)
{
    __pi_i2s_t *i2s = (__pi_i2s_t *)dev->data;
    __pi_i2s_slot_t *slot = i2s->tx_slots[channel];

    int irq = pi_irq_disable();

    // Prepare now the task results, the IRQ handler can still overwrite
    // them if they are different.
    task->data[0] = 0;

    if (slot->pingpong_buffers[0])
    {
        int buffer_index = slot->current_buffer;
        mem_block = slot->pingpong_buffers[buffer_index];
        slot->current_buffer = buffer_index ^ 1;
    }

    task->data[3] = (uint32_t)mem_block;

    if (slot->tx_task0 == NULL || slot->tx_task1 == NULL)
    {
        uint32_t base = slot->channel_base;

        udma_core_lin_addrgen_cfg_sa_buf0_set(base, (int)mem_block);
        udma_core_lin_addrgen_cfg_ctrl_set(base, UDMA_CORE_LIN_ADDRGEN_CFG_CTRL_EN(1));

        if (slot->tx_task0 == NULL)
        {
            slot->tx_task0 = task;
        }
        else
        {
            slot->tx_task1 = task;
        }
    }
    else
    {
        if (slot->waiting_first)
            slot->waiting_last->next = task;
        else
            slot->waiting_first = task;

        task->next = NULL;
        slot->waiting_last = task;
    }

    pi_irq_restore(irq);

    return 0;
}


int pi_i2s_frame_write_async(struct pi_device *dev, uint32_t frame,
    void *mem_block, size_t size, pi_task_t *task)
{
    int channel = __FF1(frame);
    __pi_i2s_t *i2s = (__pi_i2s_t *)dev->data;
    __pi_i2s_slot_t *slot = i2s->tx_slots[channel];

    int irq = pi_irq_disable();

    // Prepare now the task results, the IRQ handler can still overwrite
    // them if they are different.
    task->data[0] = 0;

    if (!slot->mem_slab)
    {
        int buffer_index = slot->current_buffer;
        mem_block = slot->pingpong_buffers[buffer_index];
        slot->current_buffer = buffer_index ^ 1;
    }

    task->data[3] = (uint32_t)mem_block;

    if (slot->tx_task0 == NULL || slot->tx_task1 == NULL)
    {
        uint32_t frame = slot->frame;
        int block_size = slot->block_size;
        uint32_t buffer = (uint32_t)mem_block;

        while (frame)
        {
            int slot_id = __FF1(frame);

            frame = __BITCLR_R(frame, 1, slot_id);

            __pi_i2s_slot_t *slot_iter = slot->i2s->tx_slots[slot_id];
            uint32_t base = slot_iter->channel_base;

            udma_core_lin_addrgen_cfg_sa_buf0_set(base, buffer);

            udma_core_lin_addrgen_cfg_ctrl_set(base, UDMA_CORE_LIN_ADDRGEN_CFG_CTRL_EN(1));

            buffer += block_size;
        }

        if (slot->tx_task0 == NULL)
        {
            slot->tx_task0 = task;
        }
        else
        {
            slot->tx_task1 = task;
        }
    }
    else
    {
        if (slot->waiting_first)
            slot->waiting_last->next = task;
        else
            slot->waiting_first = task;

        task->next = NULL;
        slot->waiting_last = task;
    }

    pi_irq_restore(irq);

    return 0;
}


int pi_i2s_read_async(struct pi_device *device, pi_task_t *task)
{
    return pi_i2s_channel_read_async(device, 0, task);
}


int pi_i2s_read_status(pi_task_t *task, void **mem_block, size_t *size)
{
    if (mem_block)
        *mem_block = (void *)task->data[1];
    if (size)
        *size = task->data[2];

    return task->data[0];
}


int pi_i2s_write_status(pi_task_t *task)
{
    return task->data[0];
}


int pi_i2s_read(struct pi_device *dev, void **mem_block, size_t *size)
{
    pi_task_t task;
    pi_i2s_read_async(dev, pi_task_block(&task));
    pi_task_wait_on(&task);

    return pi_i2s_read_status(&task, mem_block, size);
}


int pi_i2s_channel_read(struct pi_device *dev, int channel, void **mem_block, size_t *size)
{
    pi_task_t task;
    pi_i2s_channel_read_async(dev, channel, pi_task_block(&task));
    pi_task_wait_on(&task);

    return pi_i2s_read_status(&task, mem_block, size);
}

int pi_i2s_channel_write(struct pi_device *dev, int channel, void *mem_block,
    size_t size)
{
    pi_task_t task;
    pi_i2s_channel_write_async(dev, channel, mem_block, size, pi_task_block(&task));
    pi_task_wait_on(&task);

    return pi_i2s_write_status(&task);
}

int pi_i2s_channel_write_async(struct pi_device *dev, int channel,
    void *mem_block, size_t size, pi_task_t *task);


int pi_i2s_write(struct pi_device *dev, void *mem_block, size_t size)
{
    return pi_i2s_channel_write(dev, 0, mem_block, size);
}

int pi_i2s_write_async(struct pi_device *dev,
    void *mem_block, size_t size, pi_task_t *task)
{
    return pi_i2s_channel_write_async(dev, 0, mem_block, size, task);
}

#if !defined(__FREERTOS__)
static void __attribute__((constructor)) ____pi_i2s_init()
{
    for (int i=0; i<ARCHI_UDMA_NB_I2S; i++)
    {
        __pi_i2s[i].open_count = 0;
    }
}
#endif  /* __FREERTOS__ */
