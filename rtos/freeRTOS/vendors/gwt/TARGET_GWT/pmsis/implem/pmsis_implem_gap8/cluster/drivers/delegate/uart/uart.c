#include "pmsis.h"
#include "pmsis/drivers/uart.h"
#include "pmsis/implem/drivers/udma/uart/uart_internal.h"
#include "pmsis/implem/hal/udma/udma_uart.h"
#include "pmsis/task.h"
#include "pmsis/rtos/malloc/l2_malloc.h"
#include "pmsis/cluster/cluster_sync/cl_to_fc_delegate.h"

void __pi_cl_delegate_uart_write(struct uart_itf_data_s *data, void *buffer,
        uint32_t size, pi_task_t *block_callback);
void __pi_cl_delegate_uart_write_req_callback(void *arg);

// executed by event kernel, on fc
void __pi_cl_delegate_uart_write_req_callback(void *arg)
{
    struct uart_callback_args *cb_args = (struct uart_callback_args *)arg;

    __pi_uart_write(cb_args->data, cb_args->buffer, cb_args->size,
            cb_args->callback);
}

void __pi_cl_delegate_uart_write(struct uart_itf_data_s *data, void *buffer,
        uint32_t size, pi_task_t *block_callback)
{
    struct uart_callback_args args;

    args.data = data;
    args.buffer = buffer;
    args.size = size;
    args.callback = block_callback;
    // ---------------------
    // prepare event and push

    pi_task_t task;

    pi_task_callback_no_mutex(&task, __pi_cl_delegate_uart_write_req_callback,&args);

    pi_cl_send_task_to_fc(&task);
}

int pi_cl_uart_write(pi_device_t *device, void *buffer, uint32_t size, pi_cl_uart_req_t *req)
{
    pi_task_block_no_mutex(&req->done);
    __pi_cl_delegate_uart_write(device->data, buffer, size, &(req->done));

    hal_compiler_barrier();
    return 0;
}

int pi_cl_uart_write_byte(pi_device_t *device, uint8_t *byte, pi_cl_uart_req_t *req)
{
    pi_task_block_no_mutex(&req->done);
    __pi_cl_delegate_uart_write(device->data, byte, 1, &req->done);

    hal_compiler_barrier();
    return 0;
}

int pi_cl_uart_read(pi_device_t *device, void *addr, uint32_t size, pi_cl_uart_req_t *req);

int pi_cl_uart_read_byte(pi_device_t *device, uint8_t *byte, pi_cl_uart_req_t *req);
