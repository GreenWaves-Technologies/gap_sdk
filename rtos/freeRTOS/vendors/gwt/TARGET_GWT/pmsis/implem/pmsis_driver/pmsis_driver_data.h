#ifndef __PI_DRIVER_DATA_H__
#define __PI_DRIVER_DATA_H__

#include "pmsis_hal/pmsis_hal.h"

/* This file contains data structures used throughout  */

/* HYPERBUS */
/* TODO : This shall be changed. */
struct hyper_driver_task
{
    pi_task_t *task;
    struct hyper_driver_task *next;
};

struct hyper_driver_fifo
{
    struct hyper_driver_task *fifo_head;
    struct hyper_driver_task *fifo_tail;
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


#endif  /* __PI_DRIVER_DATA_H__ */
