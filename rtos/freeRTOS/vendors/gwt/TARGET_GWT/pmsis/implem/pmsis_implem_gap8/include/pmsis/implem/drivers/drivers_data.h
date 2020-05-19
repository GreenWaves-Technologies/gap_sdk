#ifndef __PI_DRIVER_DATA_H__
#define __PI_DRIVER_DATA_H__

#include "pmsis/implem/hal/hal.h"

/* This file contains data structures used throughout drivers. */

/* HYPERBUS */
/* TODO : This shall be changed. */
/*
 * pi_task:
 * data[0] = hyper_addr
 * data[1] = l2_buf
 * data[2] = size
 * data[3] = stride
 * data[4] = length
 * data[5] = channel
 * data[6] = is_emulated ?
 * data[7] = repeat_size
 */

struct hyper_driver_fifo
{
    struct pi_task *fifo_head;
    struct pi_task *fifo_tail;
    uint32_t device_id;
    uint32_t pending_emu_hyper_addr;
    uint32_t pending_emu_buffer;
    uint32_t pending_emu_size;
    uint32_t pending_emu_size_2d;
    udma_channel_e pending_emu_channel;
    uint8_t pending_emu_do_memcpy;
};

struct hyper_transfer_s
{
    uint32_t hyper_addr;
    void *buffer;
    uint32_t size;
    uint32_t stride;
    uint32_t length;
    udma_channel_e channel;
    int8_t device_id;
};

struct hyper_cb_args_s
{
    struct pi_task *cb;
    struct hyper_transfer_s transfer;
};

static inline void __pi_irq_handle_end_of_task(pi_task_t *task)
{
    switch (task->id)
    {
    case PI_TASK_NONE_ID :
        pi_task_release(task);
        break;

    case PI_TASK_CALLBACK_ID :
        pi_task_push(task);
        break;

    default :
        return;
    }
}


#endif  /* __PI_DRIVER_DATA_H__ */
