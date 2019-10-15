#ifndef __UART_INTERNAL_H__
#define __UART_INTERNAL_H__

#include "stdlib.h"
#include "pmsis.h"
#include DEFAULT_MALLOC_INC
#include "pmsis/task.h"
#include "pmsis/drivers/uart.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define UART_TX_BUFFER_SIZE     16
#define UART_DEFAULT_PRE_ALLOC_EVT 5

#define NB_UART 1


#ifndef UART_DRIVER_DATA_IMPLEM_SPECIFC
    #define UART_DRIVER_DATA_IMPLEM_SPECIFC
#endif

struct pi_cl_uart_req_s
{
    pi_task_t done;
};

/* UART transfer state. */
enum uart_tansfer_states_e
{
    UART_TX_IDLE,          /* TX idle. */
    UART_TX_BUSY,          /* TX busy. */
    UART_RX_IDLE,          /* RX idle. */
    UART_RX_BUSY,          /* RX busy. */
    UART_RX_PARITY_ERROR   /* Rx parity error */
};

/*! @brief UART transfer structure. */
#define  uart_transfer_t udma_req_info_t

/*! @brief UART request structure. */
#define  uart_req_t udma_req_t

/*******************************************************************************
 * Variables
 ******************************************************************************/
#if (defined(UART))
#define UART_HANDLE_ARRAY_SIZE 1
#else /* UART */
#error No UART instance.
#endif /* UART */

/*******************************************************************************
 * Driver data
 *****************************************************************************/

struct uart_driver_data
{
    struct pi_task *uart_fifo_head[2]; /* 0 = RX; 1 = TX; */
    struct pi_task *uart_fifo_tail[2]; /* 0 = RX; 1 = TX; */
    uint32_t uart_open_nb;
    uint8_t uart_id;
    UART_DRIVER_DATA_IMPLEM_SPECIFC
};

struct uart_callback_args
{
    struct uart_driver_data *data;
    void *buffer;
    uint32_t size;
    pi_task_t *callback;
};

extern struct uart_driver_data *__global_uart_drv_data[NB_UART];

int __pi_uart_write(struct uart_driver_data *data, void *buffer, uint32_t size, pi_task_t *callback);

void __pi_uart_write_callback(void *arg);

int __pi_uart_read(struct uart_driver_data *data, void *buffer, uint32_t size, pi_task_t *callback);

void  __pi_uart_read_callback(void *arg);

void uart_handler(void *arg);

static inline int __uart_drv_fifo_not_empty(struct uart_driver_data *data)
{
    return (!!data->uart_fifo_head);
}

// Has to be synchronized with irq_disabled since irq handler might pop at the same time
static inline void __uart_drv_fifo_enqueue(struct uart_driver_data *data,
                                           pi_task_t *pi_task, udma_channel_e channel)
{
    int irq = __disable_irq();
    if (data->uart_fifo_tail[channel])
    {
        // tail insert
        data->uart_fifo_tail[channel]->next      = pi_task;
        data->uart_fifo_tail[channel]            = data->uart_fifo_tail[channel]->next;
        data->uart_fifo_tail[channel]->next      = NULL;
    }
    else
    {
        // Initialize the list
        data->uart_fifo_head[channel]          = pi_task;
        data->uart_fifo_head[channel]->next    = NULL;
        // set the base tail
        data->uart_fifo_tail[channel]          = data->uart_fifo_head[channel];
    }
    __restore_irq(irq);
}

static inline pi_task_t *__uart_drv_fifo_pop(struct uart_driver_data *data,
        udma_channel_e channel)
{
    int irq = __disable_irq();
    pi_task_t *ret_task = NULL;
    if (data->uart_fifo_head[channel])
    {
        ret_task = data->uart_fifo_head[channel];
        hal_compiler_barrier();
        data->uart_fifo_head[channel] = data->uart_fifo_head[channel]->next;
        if (data->uart_fifo_head[channel] == NULL)
        {
            data->uart_fifo_tail[channel] = NULL;
        }
    }
    __restore_irq(irq);
    return ret_task;
}

static inline pi_task_t *__uart_prepare_read_callback(struct uart_driver_data *data,
        void *buffer, uint32_t size, pi_task_t *callback)
{
    // TODO: use slab alloc?
    pi_task_t *read_callback = pi_default_malloc(sizeof(pi_task_t));
    struct uart_callback_args *callback_args = pi_default_malloc(sizeof(struct uart_callback_args));

    // prepare callback args
    callback_args->data     = data;
    callback_args->buffer   = buffer;
    callback_args->size     = size;
    callback_args->callback = callback;

    pi_task_callback_no_mutex(read_callback, __pi_uart_read_callback, callback_args);
    read_callback->destroy = 1;

    return read_callback;
}

static inline pi_task_t *__uart_prepare_write_callback(struct uart_driver_data *data,
        void *buffer, uint32_t size, pi_task_t *callback)
{
    // TODO: use slab alloc? FIXME: free at the end!!
    pi_task_t *write_callback = pi_default_malloc(sizeof(pi_task_t));
    struct uart_callback_args *callback_args = pi_default_malloc(sizeof(struct uart_callback_args));

    // prepare callback args
    callback_args->data     = data;
    callback_args->buffer   = buffer;
    callback_args->size     = size;
    callback_args->callback = callback;

    pi_task_callback_no_mutex(write_callback, __pi_uart_write_callback, callback_args);
    write_callback->destroy = 1;
    return write_callback;
}

static inline void pi_cl_uart_write_wait(pi_cl_uart_req_t *req)
{
    pi_task_wait_on_no_mutex(&req->done);
    hal_compiler_barrier();
}

static inline void pi_cl_uart_read_wait(pi_cl_uart_req_t *req)
{
    pi_task_wait_on_no_mutex(&req->done);
    hal_compiler_barrier();
}
#endif
