#ifndef __SPI_INTERNAL_H__
#define __SPI_INTERNAL_H__

#include "pmsis.h"
#include DEFAULT_MALLOC_INC
#include "stdlib.h"
#include "pmsis/implem/hal/hal.h"

#define SPIM_CS_DATA_GET_DRV_DATA(cs_data) (cs_data->drv_data)

// one for first byte, one for last byte
#define SPIM_TMP_RD_SIZE 2
#define SPIM_TMP_WR_SIZE 2

#define RD_TMP_BUF_F 0
#define WR_TMP_BUF_F 1
#define RD_TMP_BUF_L 2
#define WR_TMP_BUF_L 3

#define SPIM_TMP_BUF_SIZE (SPIM_TMP_RD_SIZE + SPIM_TMP_WR_SIZE)

struct spim_drv_fifo {
    pi_task_t *fifo_head;
    pi_task_t *fifo_tail;
};

/* Structure holding infos for each chip selects (itf, cs, polarity etc...) */
struct spim_cs_data {
    struct spim_cs_data *next;
    struct spim_driver_data *drv_data;
    uint32_t cfg;
    uint32_t udma_cmd[8];
    uint32_t tmp_buffer;
    // ucode template for tx and rx
    uint32_t ucode_tx[9];
    uint32_t ucode_rx[9];
    // addr and size to patch addr
    void *ucode_tx_addr;
    size_t ucode_tx_addr_size;
    void *ucode_rx_addr;
    size_t ucode_rx_addr_size;
    uint32_t ucode_tx_pos;
    uint32_t ucode_rx_pos;
    size_t chunk_size_rx;
    size_t chunk_size_tx;
    uint32_t max_baudrate;
    uint32_t polarity;
    uint32_t phase;
    uint8_t cs;
    uint8_t wordsize;
    uint8_t big_endian;
};

/* Structure holding info for each interfaces
 * most notably the fifo of enqueued transfers and meta to know whether
 * interface is free or not */
struct spim_driver_data {
    struct spim_drv_fifo *drv_fifo;
    struct spim_cs_data *cs_list;
    // pre and post transfer for alignment
    pi_task_t *end_of_transfer;
    pi_task_t post_transfer_task;
    pi_task_t repeat_transfer;
    pi_task_t transfer_2d;
    uint32_t nb_open;
    // misalignment stuff --> make sure tmp_buf is aligned
    uint32_t tmp_buf[SPIM_TMP_BUF_SIZE];
    uint32_t first_byte_addr;
    uint32_t first_byte_pos;
    uint32_t first_byte_size;
    uint32_t last_byte_addr;
    uint32_t first_byte_mask;
    uint32_t last_byte_mask;
    uint32_t first_byte_shift;
    uint32_t last_byte_shift;

    pi_freq_cb_t spi_freq_cb;                     /* Callback associated to frequency changes. */

    uint8_t device_id;
};

struct spim_transfer {
    pi_spi_flags_e flags;
    void *data;
    uint32_t len;
    uint32_t cfg_cmd;
    uint32_t byte_align;
    uint32_t is_send;
    uint32_t type;
    uint32_t device_addr;
};

void __spim_execute_callback(void *arg);
void spim_eot_handler(void *arg);
void spim_tx_handler(void *arg);
void spim_rx_handler(void *arg);
int __pi_spi_open(struct spim_cs_data **cs_data, struct pi_spi_conf *conf);
int __pi_spi_close(struct spim_cs_data *cs_data);

static int32_t __pi_spim_drv_fifo_enqueue(struct spim_cs_data *data,
                                           struct spim_transfer *transfer,
                                           pi_task_t *end_task);
static inline pi_task_t *__pi_spim_drv_fifo_pop(struct spim_driver_data *data);
static inline void __pi_spim_exec_transfer(pi_task_t *task);

void __pi_spi_send_ucode_set_addr_info(struct spim_cs_data *cs_data, void *addr,
        size_t size);
uint8_t *__pi_spi_send_ucode_set(struct spim_cs_data *cs_data, uint8_t *ucode,
        size_t size);
void __pi_spi_receive_ucode_set_addr_info(struct spim_cs_data *cs_data,
        void *addr, size_t size);
uint8_t *__pi_spi_receive_ucode_set(struct spim_cs_data *cs_data, uint8_t *ucode,
        size_t size);

void __pi_spi_send_async(struct spim_cs_data *cs_data, void *data, size_t len,
        pi_spi_flags_e flags, pi_task_t *task);
void __pi_spi_receive_async(struct spim_cs_data *cs_data, void *data, size_t len,
        pi_spi_flags_e flags, pi_task_t *task);
void __pi_spi_receive_async_with_ucode(struct spim_cs_data *cs_data, void *data, size_t len,
        pi_spi_flags_e flags, int ucode_size,
        void *ucode, pi_task_t *task);
void __pi_spi_send_async_with_ucode(struct spim_cs_data *cs_data, void *data, size_t len,
        pi_spi_flags_e flags, int ucode_size,
        void *ucode, pi_task_t *task);
void __pi_spi_xfer_async(struct spim_cs_data *cs_data, void *tx_data,
        void *rx_data, size_t len, pi_spi_flags_e flags, pi_task_t *task);

void __pi_spi_copy_async(struct spim_cs_data *cs_data, uint32_t addr, 
        void *data, size_t len, pi_spi_flags_e flags, pi_task_t *task);
void __pi_spi_copy_async_2d(struct spim_cs_data *cs_data, uint32_t addr,
        void *data, size_t len, size_t stride, size_t length,
        pi_spi_flags_e flags, pi_task_t *task);

static inline uint32_t __pi_spi_get_config(struct spim_cs_data *cs_data)
{
    return cs_data->cfg;
}

static inline int32_t __pi_spim_drv_fifo_enqueue(struct spim_cs_data *cs_data,
                                           struct spim_transfer *transfer,
                                           pi_task_t *end_task)
{
    uint32_t irq = __disable_irq();
    struct spim_driver_data *drv_data = cs_data->drv_data;
    /* Callback args. */
    end_task->data[0] = (uintptr_t)cs_data;
    end_task->data[1] = (uintptr_t)transfer->data;
    end_task->data[2] = (uintptr_t)transfer->len;
    end_task->data[3] = (uintptr_t)transfer->flags;
    end_task->data[4] = (uintptr_t)end_task;
    end_task->data[5] = (uintptr_t)transfer->is_send;
    end_task->next = NULL;
    /* Enqueue transfer in drv fifo. */
    if (drv_data->drv_fifo->fifo_head == NULL)
    {
        /* Empty fifo. */
        drv_data->drv_fifo->fifo_head = end_task;
        drv_data->drv_fifo->fifo_tail = drv_data->drv_fifo->fifo_head;
    }
    else
    {
        /* Enqueue to tail. */
        drv_data->drv_fifo->fifo_tail->next = end_task;
        drv_data->drv_fifo->fifo_tail       = drv_data->drv_fifo->fifo_tail->next;
    }
    __restore_irq(irq);
    return 0;
}


static inline void __pi_spim_drv_fifo_enqueue_task(struct spim_cs_data *cs_data,
                                           pi_task_t *end_task)
{
    uint32_t irq = __disable_irq();
    struct spim_driver_data *drv_data = cs_data->drv_data;
    /* Enqueue transfer in drv fifo. */
    if (drv_data->drv_fifo->fifo_head == NULL)
    {
        /* Empty fifo. */
        drv_data->drv_fifo->fifo_head = end_task;
        drv_data->drv_fifo->fifo_tail = drv_data->drv_fifo->fifo_head;
    }
    else
    {
        /* Enqueue to tail. */
        drv_data->drv_fifo->fifo_tail->next = end_task;
        drv_data->drv_fifo->fifo_tail       = drv_data->drv_fifo->fifo_tail->next;
    }
    __restore_irq(irq);
}

static inline pi_task_t *__pi_spim_drv_fifo_pop(struct spim_driver_data *data)
{

    pi_task_t *task_return = NULL;
    uint32_t irq = __disable_irq();
    if (data->drv_fifo->fifo_head != NULL)
    {
        task_return = data->drv_fifo->fifo_head;
        data->drv_fifo->fifo_head = data->drv_fifo->fifo_head->next;
        if (data->drv_fifo->fifo_head == NULL)
        {
            data->drv_fifo->fifo_tail = NULL;
        }
    }
    __restore_irq(irq);
    return task_return;
}

static inline struct spim_cs_data *__pi_spim_get_cs_data(struct spim_driver_data *drv_data,
        int cs)
{
    struct spim_cs_data *cs_cur = drv_data->cs_list;
    while(cs_cur != NULL && cs_cur->cs != cs)
    {
        cs_cur = cs_cur->next;
    }
    return cs_cur;
}

static inline void __pi_spim_cs_data_del(struct spim_driver_data *drv_data,
        int cs)
{
    struct spim_cs_data *cs_cur  = drv_data->cs_list;
    struct spim_cs_data *cs_prev = cs_cur;
    while(cs_cur != NULL && cs_cur->cs != cs)
    {
        cs_prev = cs_cur;
        cs_cur = cs_cur->next;
    }
    if(cs_cur)
    {
        cs_prev->next = cs_cur->next;
        cs_cur->next = NULL;
    }
}
static inline void __pi_spim_cs_data_add(struct spim_driver_data *drv_data,
        struct spim_cs_data *cs_data)
{
    // head insert, most recently allocated should be most recently used
    cs_data->drv_data = drv_data;
    cs_data->next = drv_data->cs_list;
    drv_data->cs_list = cs_data->next;
}

void __pi_spi_freq_cb(void *args);

#endif
