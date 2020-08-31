#include "pmsis.h"
#include "pmsis/implem/drivers/udma/spi/spi_internal.h"
#include "pmsis/rtos/event_kernel/event_kernel.h"

//#define DEBUG
#ifndef DEBUG
#define PRINTF( ... ) ((void)0)
#else
#define PRINTF printf
#endif

#define SPIM_GAP_ADDR_ID       1
#define PREALLOC_TASK_VALID_ID 2
#define SPIM_LEN_ID            2
#define TR_TYPE_ID             5
#define SPIM_DEV_ADDR_ID       6
#define SPIM_STRIDE_ID         7
#define SPIM_LENGTH_ID         8

static inline void __spi_cpy_read_aligned(struct spim_cs_data *cs_data,
        uint32_t addr, void *data, size_t len, pi_spi_flags_e flags,
        pi_task_t *task);
static inline void __spi_cpy_send_aligned(struct spim_cs_data *cs_data,
        uint32_t addr, void *data, size_t len, pi_spi_flags_e flags,
        pi_task_t *task);
static inline void __spi_cpy_read_misaligned(struct spim_cs_data *cs_data,
        uint32_t addr, void *data, size_t len, pi_spi_flags_e flags,
        pi_task_t *task);
static inline void __spi_cpy_send_misaligned(struct spim_cs_data *cs_data,
        uint32_t addr, void *data, size_t len, pi_spi_flags_e flags,
        pi_task_t *task);


#define SPI_TRANSFER_TYPE_COPY_2D 3
#define SPI_TRANSFER_TYPE_COPY    2
#define SPI_TRANSFER_TYPE_SEND    1
#define SPI_TRANSFER_TYPE_READ    0

struct spim_driver_data *__g_spim_drv_data[UDMA_NB_SPIM] = {0};

static uint32_t __pi_spi_clk_div_get(uint32_t spi_freq)
{
    uint32_t periph_freq = pi_freq_get(PI_FREQ_DOMAIN_PERIPH);
    if (spi_freq < periph_freq)
    {
        uint32_t clk_div = 0;
        clk_div = (periph_freq + spi_freq - 1) / spi_freq;
        /* The SPIM always divide by 2 once we activate the divider, thus 
         * increase by 1 in case it is even to not go above the max frequency.*/
        if (clk_div & 1)
        {
            clk_div += 1;
        }
        clk_div = clk_div >> 1;
        return clk_div;
    }
    return 0;
}

void pi_spi_conf_init(struct pi_spi_conf *conf)
{
    conf->wordsize = PI_SPI_WORDSIZE_8;
    conf->big_endian = 0;
    conf->max_baudrate = 10000000;
    conf->cs = -1;
    conf->itf = 0;
    conf->polarity = 0;
    conf->phase = 0;
    conf->max_rcv_chunk_size = -1;
    conf->max_snd_chunk_size = -1;
}

// TODO: prepare pseudo exec for delegate
void __pi_spim_execute_callback(void *arg)
{
    return;
}

static inline void __pi_spim_exec_next_transfer(pi_task_t *task)
{
    if(NULL == task)
    {
        return;
    }
    else if(task->data[TR_TYPE_ID] == SPI_TRANSFER_TYPE_COPY_2D)
    {
        __pi_spi_copy_async_2d((struct spim_cs_data *)task->data[0],
                (uint32_t)task->data[6], (void*)task->data[1], task->data[2],
                task->data[7], task->data[8],
                task->data[3], (pi_task_t *)task->data[4]);
    }
    else if(task->data[TR_TYPE_ID] == SPI_TRANSFER_TYPE_COPY)
    {
        // TODO/FIXME: check back proto and what really need to give here
        // add a data slot to tasks
        __pi_spi_copy_async((struct spim_cs_data *)task->data[0],
                (uint32_t)task->data[6], (void*)task->data[1], task->data[2],
                task->data[3], (pi_task_t *)task->data[4]);
    }
    else if(task->data[TR_TYPE_ID] == SPI_TRANSFER_TYPE_SEND)
    {// if is send
        // cs data | data buffer | len | flags | end of transfer task
        __pi_spi_send_async((struct spim_cs_data *)task->data[0],
                (void*)task->data[1], task->data[2],
                task->data[3], (pi_task_t *)task->data[4]);
    }
    else if(task->data[TR_TYPE_ID] == SPI_TRANSFER_TYPE_READ)
    {
        // cs data | data buffer | len | flags | end of transfer task
        __pi_spi_receive_async((struct spim_cs_data *)task->data[0],
                (void*)task->data[1], task->data[2],
                task->data[3], (pi_task_t *)task->data[4]);
    }
    else
    { // task->data[5] contains rx data addr
        // cs data | tx buffer | rx buffer| len | flags | end of transfer task
        __pi_spi_xfer_async((struct spim_cs_data *)task->data[0],
                (void*)task->data[5], (void*)task->data[1], task->data[2],
                task->data[3], (pi_task_t *)task->data[4]);
    }
}

static inline void  __pi_spim_exec_post_transfer(pi_task_t *task)
{
    if(task->data[3] & PI_SPI_COPY_EXT2LOC)
    {
        __spi_cpy_read_aligned((struct spim_cs_data *)task->data[0],
                task->data[6], (void*)task->data[1], task->data[2], task->data[3],
                task);
    }
    else
    {
        __spi_cpy_send_aligned((struct spim_cs_data *)task->data[0],
                task->data[6], (void*)task->data[1], task->data[2], task->data[3],
                task);
    }
}

extern struct pmsis_event_kernel_wrap *default_sched;

static inline void __spim_handle_end_of_transfer(pi_task_t *task)
{
    if(task!=NULL)
    {
        if (task->id == PI_TASK_NONE_ID)
        {
            PRINTF("%s:%d release task %x\n",__func__,__LINE__,task);
            pi_task_release(task);
        }
        else
        {
            PRINTF("%s:%d push task %x in sched%x\n",__func__,
                    __LINE__,
                    task,
                    default_sched);
            pmsis_event_push(default_sched,task);
            PRINTF("%s:%d\n",__func__,__LINE__);
        }
    }
    else
    {
        PRINTF("%s:%d null task %x\n",__func__,__LINE__,task);
    }
}



static inline void __spim_handle_first_byte_copy(
        struct spim_driver_data *drv_data)
{
    volatile uint32_t *misaligned_addr = (uint32_t*)drv_data->first_byte_addr;
    PRINTF("misaligned_addr = %x\n",misaligned_addr);
    uint32_t tmp_val = drv_data->tmp_buf[RD_TMP_BUF_F];
    uint32_t l2_data = *misaligned_addr;
    PRINTF("l2_data before = %x\n",l2_data);

    uint32_t pos = drv_data->first_byte_pos;
    uint32_t size = drv_data->first_byte_size;

    uint32_t f_size = 4-pos;
    if(f_size > size)
    {
        f_size = size;
    }

    PRINTF("pos: %x size: %x fsize %x\n", pos, size, f_size);
    // handle first byte here, if not across two bytes, this will be enough
    uint32_t f_mask = f_size < 4UL ? ((1<<(f_size*8UL)) -1UL) : 0xFFFFFFFFUL;
    PRINTF("f_mask = %x\n",f_mask);
    uint32_t tmp_f_byte = tmp_val & f_mask;
    l2_data &= ~(f_mask << (pos*8));
    l2_data |= tmp_f_byte << (pos*8);

    PRINTF("tmp_val = %x tmp_val addr = %x\n",tmp_val, &drv_data->tmp_buf[RD_TMP_BUF_F]);
    PRINTF("l2_data after = %x\n",l2_data);
    *misaligned_addr = l2_data;
    PRINTF("should write: %x wrote :%x\n",tmp_val&((1<<(f_size*8)) -1) ,l2_data &( ((1<<(f_size*8)) -1)<< (pos*8)) );
    // if we're in between two bytes (and size was <= sizeof(uintptr_t))
    if((pos + size) > 4)
    {
        misaligned_addr++;
        l2_data = *misaligned_addr;
        uint32_t post_size = size - f_size;
        uint32_t post_mask = (1 << (post_size*8)) - 1;
        PRINTF("post size= %x post_mask=%x\n", post_size, post_mask);
        uint32_t tmp_l_byte = tmp_val >> (f_size*8);
        tmp_l_byte &= post_mask;
        PRINTF("tmp_l_byte=%x\n",tmp_l_byte);
        
        PRINTF(" post l2_data before = %x\n",l2_data);
        l2_data &= ~post_mask;
        l2_data |= tmp_l_byte;
        PRINTF(" post l2_data after = %x\n",l2_data);
        *misaligned_addr = l2_data;
    }
}

static inline void __spim_handle_last_byte_copy(
        struct spim_driver_data *drv_data)
{
    uint32_t *misaligned_addr = (uint32_t*)drv_data->last_byte_addr;
    uint32_t tmp_val = drv_data->tmp_buf[RD_TMP_BUF_L];
    uint32_t mask = drv_data->last_byte_mask;
    PRINTF("%s: last_byte_mask=%x\n",__func__,drv_data->last_byte_mask);
    tmp_val = (tmp_val & mask);
    *misaligned_addr &= ~(mask);
    *misaligned_addr |= tmp_val;
}

        
static inline void __spim_exec_2d_transfer(pi_task_t *task)
{
    if(task->data[SPIM_LEN_ID] >= (task->data[SPIM_LENGTH_ID]*8))
    {
        __pi_spi_copy_async((struct spim_cs_data *)task->data[0],
            task->data[6], (void*)task->data[1], (task->data[SPIM_LENGTH_ID]*8),
            task->data[3], task);
        task->data[SPIM_LEN_ID] -= (task->data[SPIM_LENGTH_ID]*8);
        PRINTF("%s:%d\n",__func__,__LINE__);
    }
    else
    {
        __pi_spi_copy_async((struct spim_cs_data *)task->data[0],
            task->data[6], (void*)task->data[1], task->data[SPIM_LEN_ID],
            task->data[3], task);
        PRINTF("%s:%d %x\n",__func__,__LINE__,task->data[SPIM_LEN_ID]);
        task->data[SPIM_LEN_ID] = 0;
    }
    task->data[SPIM_DEV_ADDR_ID] += task->data[SPIM_STRIDE_ID];
    task->data[SPIM_GAP_ADDR_ID] += task->data[SPIM_LENGTH_ID];
    return;
}

void spim_eot_handler(void *arg)
{
    uint32_t event = (uint32_t) arg;
    // EOT is simply 22 + id in GAP8
    uint32_t periph_id = (event - SOC_EVENT_UDMA_SPIM_EOT(0));

    struct spim_driver_data *drv_data = __g_spim_drv_data[periph_id];
    if(drv_data->first_byte_addr)
    {// correct first misalignment -- copy to first byte
        PRINTF("copy first byte\n");
        __spim_handle_first_byte_copy(drv_data);
        // clr/ack the copy
        drv_data->first_byte_addr = 0;
    }
    if(drv_data->repeat_transfer.data[PREALLOC_TASK_VALID_ID])
    {// execute repeat or core transfer
        pi_task_t *task = &drv_data->repeat_transfer;
        //PRINTF("enqueue repeat transfer\n");
        __pi_spim_exec_post_transfer(task);
        return;
    }
    if(drv_data->post_transfer_task.data[PREALLOC_TASK_VALID_ID])
    { // enqueue the last part of a last word misaligned transfer
        pi_task_t *task = &drv_data->post_transfer_task;
        //PRINTF("enqueue post transfer\n");
        __pi_spim_exec_post_transfer(task);
        return;
    }
    if(drv_data->last_byte_mask)
    {// correct first misalignment -- copy to first byte
        //PRINTF("copy last byte\n");
        __spim_handle_last_byte_copy(drv_data);
        // clr/ack the copy
        drv_data->last_byte_mask = 0;
        drv_data->last_byte_addr = 0;
    }
#if 1
    if(drv_data->transfer_2d.data[PREALLOC_TASK_VALID_ID])
    {// compute and enqueue next part of transfer
        //PRINTF("enqueue 2d transfer\n");
        drv_data->post_transfer_task.data[PREALLOC_TASK_VALID_ID] = 0;
        drv_data->repeat_transfer.data[PREALLOC_TASK_VALID_ID] = 0;
        pi_task_t *task = &drv_data->transfer_2d;
        __spim_exec_2d_transfer(task);
        return;
    }
#endif

    __spim_handle_end_of_transfer(drv_data->end_of_transfer);
    drv_data->end_of_transfer = NULL; // either it was null, or needs to be now
    PRINTF("pop next transfer\n");
    __pi_spim_exec_next_transfer(__pi_spim_drv_fifo_pop(drv_data));
}

void spim_tx_handler(void *arg)
{ // if we're here, it's cs keep related
    uint32_t event = (uint32_t) arg;
    uint32_t periph_id = (event >> UDMA_CHANNEL_NB_EVENTS_LOG2) - UDMA_SPIM_ID(0);
    hal_soc_eu_clear_fc_mask(SOC_EVENT_UDMA_SPIM_TX(periph_id));
    arg = (void*) ( SOC_EVENT_UDMA_SPIM_EOT(0) + periph_id );
    PRINTF("%s:%d periph_id %x arg:%p\n",__func__,__LINE__, periph_id, arg);
    spim_eot_handler(arg);
    PRINTF("%s:%d\n",__func__,__LINE__);
}

void spim_rx_handler(void *arg)
{ // if we're here, it's cs keep related
    uint32_t event = (uint32_t) arg;
    uint32_t periph_id = (event >> UDMA_CHANNEL_NB_EVENTS_LOG2) - UDMA_SPIM_ID(0);
    hal_soc_eu_clear_fc_mask(SOC_EVENT_UDMA_SPIM_RX(periph_id));
    arg = (void*) ( SOC_EVENT_UDMA_SPIM_EOT(0) + periph_id );
    PRINTF("%s:%d periph_id %x arg:%p\n",__func__,__LINE__, periph_id, arg);
    spim_eot_handler(arg);
    PRINTF("%s:%d\n",__func__,__LINE__);
}

int __pi_spi_open(struct spim_cs_data **cs_data, struct pi_spi_conf *conf)
{
    // disable clock gating for said device
    udma_ctrl_cg_disable(UDMA_SPIM_ID(conf->itf));
    hal_soc_eu_set_fc_mask(SOC_EVENT_UDMA_SPIM_EOT(conf->itf));
    pi_fc_event_handler_set(SOC_EVENT_UDMA_SPIM_EOT(conf->itf), spim_eot_handler);
    pi_fc_event_handler_set(SOC_EVENT_UDMA_SPIM_TX(conf->itf), spim_tx_handler);
    pi_fc_event_handler_set(SOC_EVENT_UDMA_SPIM_RX(conf->itf), spim_rx_handler);

    // Take care of driver data
    struct spim_driver_data *drv_data;
    if(__g_spim_drv_data[conf->itf])
    {
        drv_data = __g_spim_drv_data[conf->itf];
    }
    else
    {
        __g_spim_drv_data[conf->itf]=pi_default_malloc(sizeof(struct spim_driver_data));
        memset(__g_spim_drv_data[conf->itf],0,sizeof(struct spim_driver_data));
        drv_data = __g_spim_drv_data[conf->itf];
        drv_data->drv_fifo = pi_default_malloc(sizeof(struct spim_drv_fifo));
        memset(drv_data->drv_fifo, 0, sizeof(struct spim_drv_fifo));
        if(!drv_data->drv_fifo)
        {
            return -1;
        }
        drv_data->device_id = conf->itf;

        /* Attach freq callback. */
        pi_freq_callback_init(&(drv_data->spi_freq_cb), __pi_spi_freq_cb, drv_data);
        pi_freq_callback_add(&(drv_data->spi_freq_cb));
    }
    drv_data->nb_open++;

    // Take care of cs data
    *cs_data = __pi_spim_get_cs_data(drv_data, conf->cs);
    if(!*cs_data)
    {
        uint32_t clk_div = __pi_spi_clk_div_get(conf->max_baudrate);
        // alloc a cs data, need to be in udma reachable ram
        struct spim_cs_data *_cs_data = pi_data_malloc(sizeof(struct spim_cs_data));
        if (_cs_data == NULL)
        {
            PRINTF("[%s] _cs_data alloc failed\n", __func__);
            return -2;
        }
        if (clk_div > 0xFF)
        {
            PRINTF("[%s] clk_div, %x, does not fit into 8 bits. SoC frequency too high.\n",
                       __func__, clk_div);
            return -3;
        }

        memset(_cs_data, 0, sizeof(struct spim_cs_data));
        _cs_data->max_baudrate  = conf->max_baudrate;
        _cs_data->polarity      = conf->polarity;
        _cs_data->phase         = conf->phase;
        _cs_data->big_endian    = conf->big_endian;
        _cs_data->wordsize      = conf->wordsize;
        _cs_data->cs            = conf->cs;
        _cs_data->cfg           = SPI_CMD_CFG(clk_div, _cs_data->phase, _cs_data->polarity);
        
        if (conf->max_snd_chunk_size != -1)
        {
            _cs_data->chunk_size_tx = (conf->max_snd_chunk_size / 
                (2*1000000000 / (system_core_clock_get()/(clk_div*2))) - 48)
                & ~0x3;
        }
        else
        {
            _cs_data->chunk_size_tx = 8192;
        }
        if (conf->max_rcv_chunk_size != -1)
        {
            _cs_data->chunk_size_rx = (conf->max_rcv_chunk_size / 
                (2*1000000000 / (system_core_clock_get()/(clk_div*2))) - 48)
                & ~0x3;
        }
        else
        {
            _cs_data->chunk_size_rx = 8192;
        }
        
        
        *cs_data                = _cs_data;
        __pi_spim_cs_data_add(drv_data,_cs_data);
    }
    PRINTF("%s:%d\n",__func__,__LINE__);
    return 0;
}

int __pi_spi_close(struct spim_cs_data *cs_data)
{
    struct spim_driver_data *drv_data = cs_data->drv_data;
    __pi_spim_cs_data_del(drv_data,cs_data->cs);
    drv_data->nb_open--;
    if(drv_data->nb_open == 0)
    {
        /* Remove freq callback. */
        pi_freq_callback_remove(&(drv_data->spi_freq_cb));

        // disable clock gating for said device
        udma_ctrl_cg_enable(UDMA_SPIM_ID(drv_data->device_id));
        hal_soc_eu_clear_fc_mask(SOC_EVENT_UDMA_SPIM_EOT(drv_data->device_id));
        pi_fc_event_handler_clear(SOC_EVENT_UDMA_SPIM_EOT(drv_data->device_id));
        __g_spim_drv_data[drv_data->device_id] = NULL;
        pi_default_free(drv_data->drv_fifo,sizeof(drv_data->drv_fifo));
        pi_default_free(drv_data,sizeof(drv_data));
        return 0;
    }
    pi_data_free(cs_data, sizeof(cs_data));
    return drv_data->nb_open;
}

void __pi_spi_freq_cb(void *args)
{
    uint32_t irq = __disable_irq();
    struct spim_driver_data *drv_data = (struct spim_driver_data *) args;
    struct spim_cs_data *cs_data = NULL;
    uint32_t clk_div = 0;
    /* Wait until current transfer is done. */
    while ((udma_channel_busy_get(&(((spi_t *) SPIM(drv_data->device_id))->udma), RX_CHANNEL)) ||
           (udma_channel_busy_get(&(((spi_t *) SPIM(drv_data->device_id))->udma), TX_CHANNEL)));
    /* Update all clock div. */
    cs_data = drv_data->cs_list;
    while (cs_data != NULL)
    {
        clk_div = __pi_spi_clk_div_get(cs_data->max_baudrate);
        cs_data->cfg = SPI_CMD_CFG(clk_div, cs_data->phase, cs_data->polarity);
        cs_data = cs_data->next;
    }
    __restore_irq(irq);
}

void __pi_spi_receive_async(struct spim_cs_data *cs_data, void *data, size_t len,
        pi_spi_flags_e flags, pi_task_t *task)
{
    struct spim_driver_data *drv_data = SPIM_CS_DATA_GET_DRV_DATA(cs_data);
    int qspi    = ((flags >> 2) & 0x3) == ((PI_SPI_LINES_QUAD>>2)&0x3);
    int cs_mode = (flags >> 0) & 0x3;

    int device_id = drv_data->device_id;
    uint32_t cfg = cs_data->cfg;
    PRINTF("%s:%d: core clock:%d, baudrate:%d, div=%d, udma_cmd cfg =%lx, qpi=%lx\n",
            __func__,__LINE__,system_core_clock_get(),cs_data->max_baudrate,
            system_core_clock_get() / cs_data->max_baudrate,cfg, qspi);
    uint32_t byte_align = (cs_data->wordsize == PI_SPI_WORDSIZE_32)
        && cs_data->big_endian;
    int size = (len + 7) >> 3;

    int cmd_id = 0;

    int irq = __disable_irq();
    if(!drv_data->end_of_transfer)
    {
        cs_data->udma_cmd[cmd_id++] = cfg;
        cs_data->udma_cmd[cmd_id++] = SPI_CMD_SOT(cs_data->cs);
        cs_data->udma_cmd[cmd_id++] = SPI_CMD_RX_DATA(len, qspi, byte_align);
        drv_data->end_of_transfer = task;
        if(cs_mode == PI_SPI_CS_AUTO)
        {
            cs_data->udma_cmd[cmd_id++] = SPI_CMD_EOT(1);
        }
        else
        {
            hal_soc_eu_set_fc_mask(SOC_EVENT_UDMA_SPIM_RX(device_id));
        }

        spim_enqueue_channel(SPIM(device_id), (uint32_t)data, size,
                UDMA_CORE_RX_CFG_EN(1) | (2<<1), RX_CHANNEL);
        spim_enqueue_channel(SPIM(device_id), (uint32_t)cs_data->udma_cmd,
                cmd_id*(sizeof(uint32_t)), UDMA_CORE_TX_CFG_EN(1), TX_CHANNEL);
    }
    else
    {
        struct spim_transfer transfer;
        transfer.data = data;
        transfer.flags = flags;
        transfer.len = len;
        transfer.cfg_cmd = cfg;
        transfer.byte_align = byte_align;
        transfer.is_send = 0;
        __pi_spim_drv_fifo_enqueue(cs_data, &transfer, task);
    }
    restore_irq(irq);
}

void __pi_spi_receive_async_with_ucode(struct spim_cs_data *cs_data, void *data, size_t len,
        pi_spi_flags_e flags, int ucode_size,
        void *ucode, pi_task_t *task)
{
    struct spim_driver_data *drv_data = SPIM_CS_DATA_GET_DRV_DATA(cs_data);
    int cs_mode = (flags >> 0) & 0x3;

    int device_id = drv_data->device_id;
    int size = (len + 7) >> 3;

    int irq = __disable_irq();
    if(!drv_data->end_of_transfer)
    {
        if(cs_mode != PI_SPI_CS_AUTO)
        {
            hal_soc_eu_set_fc_mask(SOC_EVENT_UDMA_SPIM_RX(device_id));
        }
        drv_data->end_of_transfer = task;
        if(((0xFFF00000 & (uint32_t)ucode)!= 0x1c000000))
        {
            memcpy(&(cs_data->udma_cmd[0]), ucode, ucode_size);
            spim_enqueue_channel(SPIM(device_id), (uint32_t)data, size,
                    UDMA_CORE_RX_CFG_EN(1) | (2<<1), RX_CHANNEL);
            spim_enqueue_channel(SPIM(device_id), (uint32_t)cs_data->udma_cmd,
                    ucode_size, UDMA_CORE_TX_CFG_EN(1), TX_CHANNEL);
        }
        else
        {
            spim_enqueue_channel(SPIM(device_id), (uint32_t)data, size,
                    UDMA_CORE_RX_CFG_EN(1) | (2<<1), RX_CHANNEL);
            spim_enqueue_channel(SPIM(device_id), (uint32_t)ucode,
                    ucode_size, UDMA_CORE_TX_CFG_EN(1), TX_CHANNEL);
        }
    }
    else
    {
#if 0
        struct spim_transfer transfer;
        transfer.data = data;
        transfer.flags = flags;
        transfer.len = len;
        transfer.cfg_cmd = cfg;
        transfer.byte_align = byte_align;
        transfer.is_send = 0;
        __pi_spim_drv_fifo_enqueue(cs_data, &transfer, task);
#endif
    }
    restore_irq(irq);
}

void __pi_spi_send_async_with_ucode(struct spim_cs_data *cs_data, void *data, size_t len,
        pi_spi_flags_e flags, int ucode_size,
        void *ucode, pi_task_t *task)
{
    struct spim_driver_data *drv_data = SPIM_CS_DATA_GET_DRV_DATA(cs_data);
    int cs_mode = (flags >> 0) & 0x3;

    int device_id = drv_data->device_id;
    int size = (len + 7) >> 3;

    int irq = __disable_irq();
    if(!drv_data->end_of_transfer)
    {
        if(cs_mode != PI_SPI_CS_AUTO)
        {
            hal_soc_eu_set_fc_mask(SOC_EVENT_UDMA_SPIM_TX(device_id));
        }
        drv_data->end_of_transfer = task;

        spim_enqueue_channel(SPIM(device_id), (uint32_t)ucode,
                ucode_size, UDMA_CORE_TX_CFG_EN(1), TX_CHANNEL);
        pi_time_wait_us(1000);
        spim_enqueue_channel(SPIM(device_id), (uint32_t)data,
                size, UDMA_CORE_TX_CFG_EN(1), TX_CHANNEL);
        if(cs_mode == PI_SPI_CS_AUTO)
        {
            // wait until channel is free
            while((hal_read32((void*)&(SPIM(device_id)->udma.tx_cfg))
                        & (1<<5))>>5)
            {
                PRINTF("%s:%d\n",__func__,__LINE__);
            }

            // enqueue EOT
            cs_data->udma_cmd[0] = SPI_CMD_EOT(1);
            spim_enqueue_channel(SPIM(device_id),
                    (uint32_t)&cs_data->udma_cmd[0], 1*(sizeof(uint32_t)),
                    UDMA_CORE_TX_CFG_EN(1), TX_CHANNEL);
        }
    }
    else
    {
#if 0
        struct spim_transfer transfer;
        transfer.data = data;
        transfer.flags = flags;
        transfer.len = len;
        transfer.cfg_cmd = cfg;
        transfer.byte_align = byte_align;
        transfer.is_send = 0;
        __pi_spim_drv_fifo_enqueue(cs_data, &transfer, task);
#endif
    }
    restore_irq(irq);
}

void __pi_spi_send_async(struct spim_cs_data *cs_data, void *data, size_t len,
        pi_spi_flags_e flags, pi_task_t *task)
{
    struct spim_driver_data *drv_data = SPIM_CS_DATA_GET_DRV_DATA(cs_data);
    int qspi = ((flags >> 2) & 0x3) == ((PI_SPI_LINES_QUAD>>2)&0x3);
    int cs_mode = (flags >> 0) & 0x3;

    int device_id = drv_data->device_id;
    uint32_t cfg = cs_data->cfg;
    PRINTF("%s:%d: core clock:%d, baudrate:%d, div=%d, udma_cmd cfg =%lx, qpi=%lx\n",
            __func__,__LINE__,system_core_clock_get(),cs_data->max_baudrate,
            system_core_clock_get() / cs_data->max_baudrate,cfg, qspi);
    uint32_t byte_align = (cs_data->wordsize == PI_SPI_WORDSIZE_32)
        && cs_data->big_endian;
    int size = (len + 7) >> 3;

    PRINTF("%s:%d: udma_cmd=%p\n",__func__,__LINE__, &(cs_data->udma_cmd[0]));
    int irq = disable_irq();
    if(!drv_data->end_of_transfer)
    {// enqueue the transfer
        cs_data->udma_cmd[0] = cfg;
        cs_data->udma_cmd[1] = SPI_CMD_SOT(cs_data->cs);
        cs_data->udma_cmd[2] = SPI_CMD_TX_DATA(len, qspi, byte_align);
        PRINTF("%s:%d\n",__func__,__LINE__);
        if(cs_mode == PI_SPI_CS_AUTO)
        {
            drv_data->end_of_transfer = task;

            // enqueue header first
            spim_enqueue_channel(SPIM(device_id), (uint32_t)cs_data->udma_cmd,
                    3*(sizeof(uint32_t)),
                    UDMA_CORE_TX_CFG_EN(1), TX_CHANNEL);
            // enqueue user data
            spim_enqueue_channel(SPIM(device_id), (uint32_t)data, size,
                    UDMA_CORE_TX_CFG_EN(1),
                    TX_CHANNEL);
            #if defined(DEBUG)
            PRINTF("%s:%d: udma_cmd: %x %x %x\n", __func__, __LINE__,
                       cs_data->udma_cmd[0], cs_data->udma_cmd[1], cs_data->udma_cmd[2]);
            PRINTF("%s:%d: data: %p size: %d\n", __func__, __LINE__, data, size);
            #endif  /* DEBUG */
            // wait until channel is free
            while((hal_read32((void*)&(SPIM(device_id)->udma.tx_cfg))
                        & (1<<5))>>5)
            {
                PRINTF("%s:%d\n",__func__,__LINE__);
            }

            // enqueue EOT
            cs_data->udma_cmd[0] = SPI_CMD_EOT(1);
            PRINTF("%s:%d: %lx\n",__func__,__LINE__,cs_data->udma_cmd[0]);
            spim_enqueue_channel(SPIM(device_id),
                    (uint32_t)&cs_data->udma_cmd[0], 1*(sizeof(uint32_t)),
                    UDMA_CORE_TX_CFG_EN(1), TX_CHANNEL);
        }
        else
        {// cs keep, has to use tx event
            drv_data->end_of_transfer = task;
            spim_enqueue_channel(SPIM(device_id),
                    (uint32_t)cs_data->udma_cmd, 3*(sizeof(uint32_t)),
                    UDMA_CORE_TX_CFG_EN(1), TX_CHANNEL);
            // wait for it to clear
            while((hal_read32((void*)&(SPIM(device_id)->udma.tx_cfg))
                        & (1<<5))>>5);
            // enqueue user data & enable tx event (EOT can't be used with cs keep)
            hal_soc_eu_set_fc_mask(SOC_EVENT_UDMA_SPIM_TX(device_id));
            spim_enqueue_channel(SPIM(device_id), (uint32_t)data,
                    size, UDMA_CORE_TX_CFG_EN(1),
                    TX_CHANNEL);
        }
    }
    else
    {
        struct spim_transfer transfer;
        transfer.data = data;
        transfer.flags = flags;
        transfer.len = len;
        transfer.cfg_cmd = cfg;
        transfer.byte_align = byte_align;
        transfer.is_send = 1;
        __pi_spim_drv_fifo_enqueue(cs_data, &transfer, task);
    }
    __restore_irq(irq);
}

void __pi_spi_xfer_async(struct spim_cs_data *cs_data, void *tx_data,
        void *rx_data, size_t len, pi_spi_flags_e flags, pi_task_t *task)
{
    struct spim_driver_data *drv_data = SPIM_CS_DATA_GET_DRV_DATA(cs_data);
    int cs_mode = (flags >> 0) & 0x3;

    int device_id = drv_data->device_id;
    uint32_t cfg = cs_data->cfg;
    PRINTF("%s:%d: core clock:%d, baudrate:%d, div=%d, udma_cmd cfg =%lx\n",
            __func__,__LINE__,system_core_clock_get(),cs_data->max_baudrate,
            system_core_clock_get() / cs_data->max_baudrate,cfg);
    uint32_t byte_align = (cs_data->wordsize == PI_SPI_WORDSIZE_32)
        && cs_data->big_endian;
    int size = (len + 7) >> 3;

    int irq = __disable_irq();
    if(!drv_data->end_of_transfer)
    {
        cs_data->udma_cmd[0] = cfg;
        cs_data->udma_cmd[1] = SPI_CMD_SOT(cs_data->cs);
        cs_data->udma_cmd[2] = SPI_CMD_FULL_DUPL(len, byte_align);
        drv_data->end_of_transfer = task;
        if(cs_mode == PI_SPI_CS_AUTO)
        {
            spim_enqueue_channel(SPIM(device_id), (uint32_t)cs_data->udma_cmd,
                    3*(sizeof(uint32_t)), UDMA_CORE_TX_CFG_EN(1),
                    TX_CHANNEL);
            spim_enqueue_channel(SPIM(device_id), (uint32_t)rx_data, size,
                    UDMA_CORE_RX_CFG_EN(1) | (2<<1), RX_CHANNEL);
            spim_enqueue_channel(SPIM(device_id), (uint32_t)tx_data,
                    size, UDMA_CORE_TX_CFG_EN(1),
                    TX_CHANNEL);
            // wait until TX channel is free
            while((hal_read32((void*)&(SPIM(device_id)->udma.tx_cfg))
                        & (1<<5))>>5)
            {
                PRINTF("%s:%d\n",__func__,__LINE__);
            }
            // send EOT
            cs_data->udma_cmd[3] = SPI_CMD_EOT(1);
            spim_enqueue_channel(SPIM(device_id),
                    (uint32_t)&cs_data->udma_cmd[3], sizeof(uint32_t),
                    UDMA_CORE_TX_CFG_EN(1), TX_CHANNEL);
        }
        else
        {
            spim_enqueue_channel(SPIM(device_id), (uint32_t)cs_data->udma_cmd,
                    3*(sizeof(uint32_t)), UDMA_CORE_TX_CFG_EN(1),
                    TX_CHANNEL);
            // wait until TX channel is free
            while((hal_read32((void*)&(SPIM(device_id)->udma.tx_cfg))
                        & (1<<5))>>5)
            {
                PRINTF("%s:%d\n",__func__,__LINE__);
            }
            // activate rx event
            hal_soc_eu_set_fc_mask(SOC_EVENT_UDMA_SPIM_RX(device_id));
            // NOTE: both transfers have the same size
            // does not matter which one we wait
            spim_enqueue_channel(SPIM(device_id), (uint32_t)rx_data, size,
                    UDMA_CORE_RX_CFG_EN(1) | (2<<1), RX_CHANNEL);
            spim_enqueue_channel(SPIM(device_id), (uint32_t)tx_data,
                    size, UDMA_CORE_TX_CFG_EN(1),
                    TX_CHANNEL);
        }

    }
    else
    {
        struct spim_transfer transfer;
        transfer.data = rx_data;
        transfer.flags = flags;
        transfer.len = len;
        transfer.cfg_cmd = cfg;
        transfer.byte_align = byte_align;
        transfer.is_send = (uint32_t) tx_data; // sending a pointer means xfer
        __pi_spim_drv_fifo_enqueue(cs_data, &transfer, task);
    }
    restore_irq(irq);
}

void __pi_spi_copy_async(struct spim_cs_data *cs_data, uint32_t addr, 
        void *data, size_t len, pi_spi_flags_e flags, pi_task_t *task)
{
    // check if it's a read
    int is_rx = flags & PI_SPI_COPY_EXT2LOC;
    int size = (len + 7) >> 3;
    // check sides on both alignment
    int misaligned = (((uint32_t)data & 0x3)!=0)
        || ((uint32_t)(((uint32_t)data+size)&0x3)!=0);

    if(misaligned)
    {
        PRINTF("misaligned transfer size: %x addr=%x\n",size,data);
        if(is_rx)
        {
            PRINTF("is_rx\n");
            __spi_cpy_read_misaligned(cs_data,addr,data,len,flags,task);
        }
        else
        {
            PRINTF("is_tx\n");
            __spi_cpy_send_misaligned(cs_data,addr,data,len,flags,task);
        }
    }
    else
    {
        PRINTF("aligned transfer size: %x\n",size);
        if(is_rx)
        {
            PRINTF("is_rx\n");
            __spi_cpy_read_aligned(cs_data,addr,data,len,flags,task);
        }
        else
        {
            PRINTF("is_tx\n");
            __spi_cpy_send_aligned(cs_data,addr,data,len,flags,task);
        }
    }
}

void __pi_spi_copy_async_2d(struct spim_cs_data *cs_data, uint32_t addr, 
        void *data, size_t len, size_t stride, size_t length,
        pi_spi_flags_e flags, pi_task_t *task)
{
    uint32_t size = (len + 7) >> 3;
    PRINTF("2d transfer size=%x, length=%x\n",size,length);
    int irq = disable_irq();
    struct spim_driver_data *drv_data = SPIM_CS_DATA_GET_DRV_DATA(cs_data);
    // check sides on both alignment
    if(!drv_data->end_of_transfer)
    {
        drv_data->end_of_transfer = task;
        // prepare 2d task
        pi_task_t *transfer_2d = &drv_data->transfer_2d;
        transfer_2d->data[0] = (uintptr_t)cs_data;
        if(size >= length)
        {
            transfer_2d->data[1] = (uintptr_t)((uint32_t)data)+length;
            transfer_2d->data[2] = (uintptr_t)((size-length)*8);
        }
        else
        {
            transfer_2d->data[1] = (uintptr_t)((uint32_t)data)+size;
            transfer_2d->data[2] = (uintptr_t)((size)*8);
        }
        transfer_2d->data[3] = (uintptr_t)flags;
        transfer_2d->data[4] = (uintptr_t)transfer_2d;
        transfer_2d->data[5] = (uintptr_t)SPI_TRANSFER_TYPE_COPY_2D;
        transfer_2d->data[6] = (uintptr_t)addr+stride;
        transfer_2d->data[SPIM_STRIDE_ID] = (uintptr_t)stride;
        transfer_2d->data[SPIM_LENGTH_ID] = (uintptr_t)length;

        // send first transfer
        __pi_spi_copy_async(cs_data,addr,data,length*8,flags,transfer_2d);
    }
    else
    {
        PRINTF("enqueue 2d transfer\n");
        task->data[0] = (uintptr_t)cs_data;
        task->data[SPIM_GAP_ADDR_ID] = (uintptr_t)data;
        task->data[2] = (uintptr_t)((size)*8);
        task->data[3] = (uintptr_t)flags;
        task->data[4] = (uintptr_t)task;
        task->data[TR_TYPE_ID]       = (uintptr_t)SPI_TRANSFER_TYPE_COPY_2D;
        task->data[SPIM_DEV_ADDR_ID] = (uintptr_t)addr;
        task->data[SPIM_STRIDE_ID]   = (uintptr_t)stride;
        task->data[SPIM_LENGTH_ID]   = (uintptr_t)length;
        task->next = NULL;
        __pi_spim_drv_fifo_enqueue_task(cs_data, task);
    }
    restore_irq(irq);
}

void __pi_spi_send_ucode_set_addr_info(struct spim_cs_data *cs_data, void *addr,
        size_t size)
{
    cs_data->ucode_tx_addr = addr;
    cs_data->ucode_tx_addr_size = size;
}

uint8_t *__pi_spi_send_ucode_set(struct spim_cs_data *cs_data, uint8_t *ucode,
        size_t size)
{
    memcpy(cs_data->ucode_tx+2, ucode, size);
    cs_data->ucode_tx[0] = cs_data->cfg;
    cs_data->ucode_tx[1] = SPI_CMD_SOT(cs_data->cs);
    // remember to skip first two instructions too
    cs_data->ucode_tx_pos = 2 + size/4 + ((size%4) ? 1:0);
    return (uint8_t*)&cs_data->ucode_tx[2];
}

void __pi_spi_receive_ucode_set_addr_info(struct spim_cs_data *cs_data,
        void *addr, size_t size)
{
    cs_data->ucode_rx_addr = addr;
    cs_data->ucode_rx_addr_size = size;
}

uint8_t *__pi_spi_receive_ucode_set(struct spim_cs_data *cs_data, uint8_t *ucode,
        size_t size)
{
    memcpy(cs_data->ucode_rx+2, ucode, size);
    cs_data->ucode_rx[0] = cs_data->cfg;
    cs_data->ucode_rx[1] = SPI_CMD_SOT(cs_data->cs);
    // remember to skip first two instructions too
    cs_data->ucode_rx_pos = 2 + size/4 + ((size%4) ? 1:0);
    return (uint8_t*)&cs_data->ucode_rx[2];
}

void __ucode_patch(struct spim_cs_data *cs_data, uint32_t addr, int is_rx)
{
    uint8_t *ucode_patch = is_rx? cs_data->ucode_rx_addr:cs_data->ucode_tx_addr;
    size_t size = is_rx? cs_data->ucode_rx_addr_size:cs_data->ucode_tx_addr_size;
    for(int i = size-1; i>=0; i--)
    {
        ucode_patch[i] = (addr >> (i*8)) & 0xFF;
    }
    PRINTF("ucode = %x\n",*(uint32_t*)ucode_patch);
}

/**
 * implem of cpy api, aligned version
 * Allows to access spi devices with a notion of address
 * Ucode for read (rx) has to be setup before use
 * API is orthogonal of other APIs
 */
static inline void __spi_cpy_read_aligned(struct spim_cs_data *cs_data,
        uint32_t addr, void *data, size_t len, pi_spi_flags_e flags,
        pi_task_t *task)
{
    struct spim_driver_data *drv_data = SPIM_CS_DATA_GET_DRV_DATA(cs_data);
    int quad    = ((flags >> 2) & 0x3) == ((PI_SPI_LINES_QUAD>>2) & 0x3);
    int cs_mode = (flags >> 0) & 0x3;
    size_t chunk_size = cs_data->chunk_size_rx;
    PRINTF("chunk_size_rx = %x\n",chunk_size);

    int device_id = drv_data->device_id;
    uint32_t cfg = cs_data->cfg;
    uint32_t byte_align = (cs_data->wordsize == PI_SPI_WORDSIZE_32)
        && cs_data->big_endian;
    uint32_t size = (len + 7) >> 3;
    int ucode_id = 0;

    int irq = disable_irq();
    __ucode_patch(cs_data, addr, 1);
    if(!drv_data->end_of_transfer || (&drv_data->repeat_transfer == task)
            || (&drv_data->post_transfer_task == task)
            || (task == &drv_data->transfer_2d))
    {// enqueue the transfer directly
        if(cs_mode != PI_SPI_CS_AUTO)
        {
            hal_soc_eu_set_fc_mask(SOC_EVENT_UDMA_SPIM_RX(device_id));
        }

        if(&drv_data->repeat_transfer == task)
        {// repeat transfer has been used, clear it
            drv_data->repeat_transfer.data[PREALLOC_TASK_VALID_ID] = 0;
        }
        else if(&drv_data->post_transfer_task == task)
        { // clear post transfer if we're here
            drv_data->post_transfer_task.data[PREALLOC_TASK_VALID_ID] = 0;
        }
        else
        {
            if(!drv_data->end_of_transfer)
            {
                drv_data->end_of_transfer = task;
            }
        }

        if(size < chunk_size)
        {
            drv_data->repeat_transfer.data[PREALLOC_TASK_VALID_ID] = 0;
        }
        else
        {
            // prepare next transfer if need be
            pi_task_t *repeat_task = &drv_data->repeat_transfer;
            repeat_task->data[0] = (uintptr_t)cs_data;
            repeat_task->data[1] = (uintptr_t)data + chunk_size;
            PRINTF("next transfer addr:%x data:%x size(B) :%x\n", repeat_task->data[1],
                    data, chunk_size);
            repeat_task->data[2] = (uintptr_t)len - (chunk_size*8);
            repeat_task->data[3] = (uintptr_t)flags;
            repeat_task->data[4] = (uintptr_t)repeat_task;
            repeat_task->data[5] = (uintptr_t)SPI_TRANSFER_TYPE_COPY;
            repeat_task->data[6] = (uintptr_t)addr+chunk_size;
            // truncate size
            size = chunk_size;
            len = chunk_size*8;
        }
        uint32_t* ucode = (uint32_t*)&cs_data->ucode_rx[cs_data->ucode_rx_pos];
        PRINTF("len = %x quad = %x byte_align = %x\n",len,quad,byte_align);
        ucode[ucode_id++] = SPI_CMD_RX_DATA(len,quad,byte_align);
        ucode[ucode_id++] = SPI_CMD_EOT(1);
        int ucode_size = cs_data->ucode_rx_pos + ucode_id;
        PRINTF("ucode = %x ucode_size = %x\n",ucode,ucode_size);
        PRINTF("enqueue transfer\n");
        PRINTF("size = %x\n",size);
        spim_enqueue_channel(SPIM(device_id),
                (uint32_t)data, size, UDMA_CORE_RX_CFG_EN(1) | (2<<1), RX_CHANNEL);
        spim_enqueue_channel(SPIM(device_id),
                (uint32_t)&cs_data->ucode_rx[0], ucode_size*(sizeof(uint32_t)),
                UDMA_CORE_TX_CFG_EN(1), TX_CHANNEL);
    }
    else
    {
        if(cs_mode != PI_SPI_CS_AUTO)
        {
            struct spim_transfer transfer;
            transfer.data = data;
            transfer.flags = flags;
            transfer.len = len;
            transfer.cfg_cmd = cfg;
            transfer.byte_align = byte_align;
            transfer.type = SPI_TRANSFER_TYPE_COPY;
            transfer.device_addr = addr;
            __pi_spim_drv_fifo_enqueue(cs_data, &transfer, task);
        }
    }
    restore_irq(irq);
}

/**
 * implem of cpy api, misaligned version
 * Allows to access spi devices with a notion of address
 * Ucode for read (rx) has to be setup before use
 * API is orthogonal of other APIs
 *
 * Misaglinment is handled by creating up to 3 fragments
 * set align_post_transfer if transfer is misaligned at the end
 */
static inline void __spi_cpy_read_misaligned(struct spim_cs_data *cs_data,
        uint32_t addr, void *data, size_t len, pi_spi_flags_e flags,
        pi_task_t *task)
{
    struct spim_driver_data *drv_data = SPIM_CS_DATA_GET_DRV_DATA(cs_data);
    int quad    = ((flags >> 2) & 0x3) == ((PI_SPI_LINES_QUAD>>2) & 0x3);

    int device_id = drv_data->device_id;
    uint32_t cfg = cs_data->cfg;
    uint32_t byte_align = (cs_data->wordsize == PI_SPI_WORDSIZE_32)
        && cs_data->big_endian;
    int size = (len + 7) >> 3;
    int ucode_id = 0;
    uint32_t l_size = 0;
    uint32_t f_size = 0;

    int irq = disable_irq();
    if(!drv_data->end_of_transfer || (task == &drv_data->transfer_2d))
    {
        if(!drv_data->end_of_transfer)
        {
            drv_data->end_of_transfer = task;
        }
        if(size < 5)
        {
            PRINTF("short misaligned size = %x\n",size);
            __ucode_patch(cs_data, addr, 1);
            drv_data->tmp_buf[RD_TMP_BUF_F] = 0;
            uint32_t* ucode = (uint32_t*)&cs_data->ucode_rx[cs_data->ucode_rx_pos];
            ucode[ucode_id++] = SPI_CMD_RX_DATA(size*8,quad,byte_align);
            ucode[ucode_id++] = SPI_CMD_EOT(1);
            int ucode_size = cs_data->ucode_rx_pos + ucode_id;
            spim_enqueue_channel(SPIM(device_id),
                    (uint32_t)&drv_data->tmp_buf[RD_TMP_BUF_F],
                    size,
                    UDMA_CORE_RX_CFG_EN(1) | (2<<1), RX_CHANNEL);
            spim_enqueue_channel(SPIM(device_id),
                    (uint32_t)&cs_data->ucode_rx[0], ucode_size*(sizeof(uint32_t)),
                    UDMA_CORE_TX_CFG_EN(1), TX_CHANNEL);
            // see at what byte we'll need to copy first transfer in our buffer
            // might be across two bytes, so give pos and size
            drv_data->first_byte_addr = (uint32_t)data & (~0x3);
            PRINTF("first_byte_addr = %x\n",drv_data->first_byte_addr);
            drv_data->first_byte_pos = (uint32_t)data & 0x3;
            drv_data->first_byte_size = size;
            restore_irq(irq);
            return;
        }
        l_size = (uintptr_t)((((uint32_t)data+size)&0x3));
        // prepare end of transfer alignment
        if(l_size)
        {
            PRINTF("%s:%d size unaligned = %x\n",__func__,__LINE__,size);
            l_size = (uintptr_t)((((uint32_t)data+size)&0x3));
            uint32_t l_mask = (1 << (l_size*8)) - 1;
            drv_data->last_byte_mask = l_mask;
            drv_data->last_byte_addr = (((uint32_t)data+size) - l_size);
            // prepare repeat transfer for the "core" aligned transfer
            pi_task_t *post_transfer_task = &drv_data->post_transfer_task;
            post_transfer_task->data[0] = (uintptr_t)cs_data;
            post_transfer_task->data[1] = 
                (uintptr_t)&drv_data->tmp_buf[RD_TMP_BUF_L];
            post_transfer_task->data[2] = l_size*8;
            post_transfer_task->data[3] = (uintptr_t)flags;
            post_transfer_task->data[4] = (uintptr_t)post_transfer_task;
            post_transfer_task->data[5] = (uintptr_t)SPI_TRANSFER_TYPE_COPY;
            post_transfer_task->data[6] = 
                (uintptr_t)(addr+size)- l_size;
            size -= l_size;
            PRINTF("size corrected:%x, l_size=%x\n",size,l_size);
        }
        // compute size of first bytes potential unalignment
        f_size = 4-((uint32_t)data & 0x3);
        size -= f_size;
        // prepare repeat transfer for the "core" aligned transfer
        pi_task_t *repeat_task = &drv_data->repeat_transfer;
        if(f_size)
        {
            repeat_task->data[0] = (uintptr_t)cs_data;
            repeat_task->data[1] = (uintptr_t)data+(4-((uint32_t)data&0x3));
            repeat_task->data[2] = (uintptr_t)size*8;
            repeat_task->data[3] = (uintptr_t)flags;
            repeat_task->data[4] = (uintptr_t)repeat_task;
            repeat_task->data[5] = (uintptr_t)SPI_TRANSFER_TYPE_COPY;
            repeat_task->data[6] = (uintptr_t)addr+(4-((uint32_t)data&0x3));
            PRINTF("f byte misalign\n");
            // truncate size -- will need to copy from tmp buffer to final buffer
            // in handler
            __ucode_patch(cs_data, addr, 1);
            uint32_t* ucode = (uint32_t*)&cs_data->ucode_rx[cs_data->ucode_rx_pos];
            ucode[ucode_id++] = SPI_CMD_RX_DATA(f_size*8,quad,byte_align);
            ucode[ucode_id++] = SPI_CMD_EOT(1);
            int ucode_size = cs_data->ucode_rx_pos + ucode_id;
            spim_enqueue_channel(SPIM(device_id),
                    (uint32_t)&drv_data->tmp_buf[RD_TMP_BUF_F],
                    f_size,
                    UDMA_CORE_RX_CFG_EN(1) | (2<<1), RX_CHANNEL);
            spim_enqueue_channel(SPIM(device_id),
                    (uint32_t)&cs_data->ucode_rx[0], ucode_size*(sizeof(uint32_t)),
                    UDMA_CORE_TX_CFG_EN(1), TX_CHANNEL);
            // see at what byte we'll need to copy first transfer in our buffer
            drv_data->first_byte_addr = (uint32_t)data & (~0x3);
            PRINTF("%s:%d first_byte_addr = %x\n",__func__,__LINE__,drv_data->first_byte_addr);
            drv_data->first_byte_pos = (uint32_t)data & 0x3;
            drv_data->first_byte_size = f_size;
        }
        else
        { // execute the transfer now -- size is amputated from last and first
          // bytes if misaligned
            repeat_task->data[PREALLOC_TASK_VALID_ID] = 0;
            __spi_cpy_read_aligned(cs_data, addr, data, size*8, flags,
                    repeat_task);
        }
    }
    else
    {
        struct spim_transfer transfer;
        transfer.data = data;
        transfer.flags = flags;
        transfer.len = len;
        transfer.cfg_cmd = cfg;
        transfer.byte_align = byte_align;
        transfer.type = SPI_TRANSFER_TYPE_COPY;
        transfer.device_addr = addr;
        __pi_spim_drv_fifo_enqueue(cs_data, &transfer, task);
    }
    restore_irq(irq);
}

/**
 * implem of cpy api, aligned version
 * Allows to access spi devices with a notion of address
 * Ucode for write (tx) has to be setup before use
 * API is orthogonal of other APIs
 *
 * Also handles the notion of chunk size (mostly for spiram) in the case
 * where command can be directly repeated to move forward in address space
 *
 */
static inline void __spi_cpy_send_aligned(struct spim_cs_data *cs_data,
        uint32_t addr, void *data, size_t len, pi_spi_flags_e flags,
        pi_task_t *task)
{
    struct spim_driver_data *drv_data = SPIM_CS_DATA_GET_DRV_DATA(cs_data);
    int quad    = ((flags >> 2) & 0x3) == ((PI_SPI_LINES_QUAD>>2) & 0x3);
    size_t chunk_size = cs_data->chunk_size_tx;
    PRINTF("chunk_size_tx = %x\n",chunk_size);

    int device_id = drv_data->device_id;
    uint32_t cfg = cs_data->cfg;
    uint32_t byte_align = (cs_data->wordsize == PI_SPI_WORDSIZE_32)
        && cs_data->big_endian;
    uint32_t size = (len + 7) >> 3;
    int ucode_id = 0;
    int irq = disable_irq();
    if(!drv_data->end_of_transfer || (&drv_data->repeat_transfer == task)
            || (&drv_data->post_transfer_task == task)
            || (task == &drv_data->transfer_2d))
    {// enqueue the transfer directly
        if(&drv_data->repeat_transfer == task)
        {// repeat transfer has been used, clear it
            drv_data->repeat_transfer.data[PREALLOC_TASK_VALID_ID] = 0;
        }
        else if(&drv_data->post_transfer_task == task)
        { // clear post transfer if we're here
            drv_data->post_transfer_task.data[PREALLOC_TASK_VALID_ID] = 0;
        }
        else
        {
            if(!drv_data->end_of_transfer)
            {
                //PRINTF("end of transfer <<== %x\n",task);
                drv_data->end_of_transfer = task;
            }
        }

        if(size < chunk_size)
        {
            drv_data->repeat_transfer.data[PREALLOC_TASK_VALID_ID] = 0;
        }
        else
        {
            // prepare next transfer if need be
            pi_task_t *repeat_task = &drv_data->repeat_transfer;
            repeat_task->data[0] = (uintptr_t)cs_data;
            repeat_task->data[1] = (uintptr_t)data + chunk_size;
            PRINTF("%s: next transfer addr:%x data:%x size(B) :%x\n", __func__,
                    repeat_task->data[1], data, chunk_size);
            repeat_task->data[2] = (uintptr_t)len - (chunk_size*8);
            repeat_task->data[3] = (uintptr_t)flags;
            repeat_task->data[4] = (uintptr_t)repeat_task;
            repeat_task->data[5] = (uintptr_t)SPI_TRANSFER_TYPE_COPY;
            repeat_task->data[6] = (uintptr_t)addr+chunk_size;
            // truncate size
            size = chunk_size;
            len = chunk_size*8;
        }
        __ucode_patch(cs_data, addr, 0);
        uint32_t* ucode = (uint32_t*)&cs_data->ucode_tx[cs_data->ucode_tx_pos];
        ucode[ucode_id++] = SPI_CMD_TX_DATA(len,quad,byte_align);
 
        int ucode_size = cs_data->ucode_tx_pos + ucode_id;
        PRINTF("data=0x%x, len= 0x%x, ucode_size=0x%x\n",data, len, ucode_size);
        spim_enqueue_channel(SPIM(device_id),
                (uint32_t)&cs_data->ucode_tx[0], ucode_size*(sizeof(uint32_t)),
                UDMA_CORE_TX_CFG_EN(1), TX_CHANNEL);
        spim_enqueue_channel(SPIM(device_id), (uint32_t)data,
                size, UDMA_CORE_TX_CFG_EN(1), TX_CHANNEL);

        while((hal_read32((void*)&(SPIM(device_id)->udma.tx_cfg))
                    & (1<<5))>>5)
        {
            PRINTF("%s:%d\n",__func__,__LINE__);
        }
        cs_data->udma_cmd[0] = SPI_CMD_EOT(1);
        PRINTF("%s:%d: %lx %lx\n",__func__,__LINE__,cs_data->udma_cmd[0],
                &cs_data->udma_cmd[0]);
        spim_enqueue_channel(SPIM(device_id),
                (uint32_t)&cs_data->udma_cmd[0], 1*(sizeof(uint32_t)),
                UDMA_CORE_TX_CFG_EN(1), TX_CHANNEL);
    }
    else
    {
        //PRINTF("enqueue a transfer in fifo, task=%x\n",task);
        struct spim_transfer transfer;
        transfer.data = data;
        transfer.flags = flags;
        transfer.len = len;
        transfer.cfg_cmd = cfg;
        transfer.byte_align = byte_align;
        transfer.type = SPI_TRANSFER_TYPE_COPY;
        transfer.device_addr = addr;
        __pi_spim_drv_fifo_enqueue(cs_data, &transfer, task);
    }
    restore_irq(irq);
}

/**
 * implem of cpy api, misaligned version
 * Allows to access spi devices with a notion of address
 * Ucode for write (tx) has to be setup before use
 * API is orthogonal of other APIs
 *
 * Also handles the notion of chunk size (mostly for spiram) in the case
 * where command can be directly repeated to move forward in address space
 *
 */
static inline void __spi_cpy_send_misaligned(struct spim_cs_data *cs_data,
        uint32_t addr, void *data, size_t len, pi_spi_flags_e flags,
        pi_task_t *task)
{ // TODO handle misalign tx here
    struct spim_driver_data *drv_data = SPIM_CS_DATA_GET_DRV_DATA(cs_data);
    int quad    = ((flags >> 2) & 0x3) == ((PI_SPI_LINES_QUAD>>2) & 0x3);

    int device_id = drv_data->device_id;
    uint32_t cfg = cs_data->cfg;
    uint32_t byte_align = (cs_data->wordsize == PI_SPI_WORDSIZE_32)
        && cs_data->big_endian;
    uint32_t size = (len + 7) >> 3;
    int ucode_id = 0;
    uint32_t l_size = 0;
    uint32_t f_size = 0;

    int irq = disable_irq();
    if(!drv_data->end_of_transfer || (task == &drv_data->transfer_2d))
    {
        if(!drv_data->end_of_transfer)
        {
            drv_data->end_of_transfer = task;
        }
        if(size < 5)
        {
            f_size = 4-((uint32_t)data & 0x3);
            if(f_size > size)
            {
                f_size=size;
            }
            uint32_t f_pos = (uint32_t)data & 0x3;
            
            // take care of pre alignment
            // copy data to aligned buffer before write
            drv_data->tmp_buf[WR_TMP_BUF_F] =
                *(uint32_t*)((uintptr_t)data & (~0x3));
            // shift away the lower bits (only the part where transfer begins 
            // interest us)
            drv_data->tmp_buf[WR_TMP_BUF_F] >>= (f_pos*8);
            drv_data->tmp_buf[WR_TMP_BUF_F] &= (1<<(f_size*8)) -1;
            PRINTF("f_size = %x, size=%x, data=%x\n",f_size,size,data);
            if(f_size < size)
            {// both post and pre alignement is needed --> add second part
                uint32_t tmp_2 = *(uint32_t*)((uintptr_t)data + f_size);
                uint32_t post_size = size - f_size;
                uint32_t post_mask = (post_size < 4UL) ? 
                    (1UL << (post_size*8UL))  - 1UL : 0xFFFFFFFFUL;
                tmp_2 &= post_mask;
                tmp_2 = tmp_2 << (f_size*8);
                drv_data->tmp_buf[WR_TMP_BUF_F] |= tmp_2;
                //PRINTF("new fsize=%x\n",f_size);
            }


            __ucode_patch(cs_data, addr, 0);
            uint32_t* ucode = (uint32_t*)&cs_data->ucode_tx[cs_data->ucode_tx_pos];
            ucode[ucode_id++] = SPI_CMD_TX_DATA(size*8,quad,byte_align);
 
            int ucode_size = cs_data->ucode_tx_pos + ucode_id;
            
            spim_enqueue_channel(SPIM(device_id),
                    (uint32_t)&cs_data->ucode_tx[0], ucode_size*(sizeof(uint32_t)),
                    UDMA_CORE_TX_CFG_EN(1), TX_CHANNEL);
           
            spim_enqueue_channel(SPIM(device_id),
                    (uint32_t)&drv_data->tmp_buf[WR_TMP_BUF_F], size,
                    UDMA_CORE_RX_CFG_EN(1), TX_CHANNEL);
            while((hal_read32((void*)&(SPIM(device_id)->udma.tx_cfg))
                        & (1<<5))>>5)
            {
                PRINTF("%s:%d\n",__func__,__LINE__);
            }
            // enqueue EOT
            cs_data->udma_cmd[0] = SPI_CMD_EOT(1);
            spim_enqueue_channel(SPIM(device_id),
                    (uint32_t)&cs_data->udma_cmd[0], 1*(sizeof(uint32_t)),
                    UDMA_CORE_TX_CFG_EN(1), TX_CHANNEL);
            restore_irq(irq);
            return;
        }
#if 1   // no need to post align write transfers
        l_size = (uintptr_t)((((uint32_t)data+size)&0x3));
        if(l_size)
        {
            PRINTF("l_size=%x\n",l_size);
            // prepare repeat transfer for the "core" aligned transfer
            pi_task_t *post_transfer_task = &drv_data->post_transfer_task;
            post_transfer_task->data[0] = (uintptr_t)cs_data;
            post_transfer_task->data[1] = 
                (uintptr_t)((uint32_t)data+size)-(l_size);
            post_transfer_task->data[2] = l_size*8;
            post_transfer_task->data[3] = (uintptr_t)flags;
            post_transfer_task->data[4] = (uintptr_t)post_transfer_task;
            post_transfer_task->data[5] = (uintptr_t)SPI_TRANSFER_TYPE_COPY;
            post_transfer_task->data[6] = 
                (uintptr_t)((uint32_t)addr+size)-l_size;
            size -= l_size;
        }
#endif
        f_size = 4-((uint32_t)data & 0x3);
        PRINTF("f_size = %x\n",f_size);
        size -= f_size;

        PRINTF("size aligned=%x\n",size);
        // prepare repeat transfer for the "core" aligned transfer
        pi_task_t *repeat_task = &drv_data->repeat_transfer;
        if(f_size)
        {
            if(size)
            {// it might be that we have a transfer < 8 bytes completely
                //unaligned
                repeat_task->data[0] = (uintptr_t)cs_data;
                repeat_task->data[1] = (uintptr_t)data+f_size;
                repeat_task->data[2] = (uintptr_t)size*8;
                repeat_task->data[3] = (uintptr_t)flags;
                repeat_task->data[4] = (uintptr_t)repeat_task;
                repeat_task->data[5] = (uintptr_t)SPI_TRANSFER_TYPE_COPY;
                repeat_task->data[6] = (uintptr_t)addr+f_size;
            }
            // truncate size -- will need to copy from tmp buffer to final buffer
            // in handler
            __ucode_patch(cs_data, addr, 0);
            uint32_t* ucode = (uint32_t*)&cs_data->ucode_tx[cs_data->ucode_tx_pos];
            ucode[ucode_id++] = SPI_CMD_TX_DATA(f_size*8,quad,byte_align);
 
            int ucode_size = cs_data->ucode_tx_pos + ucode_id;

            // copy data to aligned buffer before write
            drv_data->tmp_buf[WR_TMP_BUF_F] =
                *(uint32_t*)((uintptr_t)data & (~0x3));
            // shift away the lower bits (only the part where transfer begins 
            // interest us)
            drv_data->tmp_buf[WR_TMP_BUF_F] >>= ((4-f_size)*8);

            
            spim_enqueue_channel(SPIM(device_id),
                    (uint32_t)&cs_data->ucode_tx[0], ucode_size*(sizeof(uint32_t)),
                    UDMA_CORE_TX_CFG_EN(1), TX_CHANNEL);
           
            spim_enqueue_channel(SPIM(device_id),
                    (uint32_t)&drv_data->tmp_buf[WR_TMP_BUF_F], f_size,
                    UDMA_CORE_RX_CFG_EN(1), TX_CHANNEL);
            while((hal_read32((void*)&(SPIM(device_id)->udma.tx_cfg))
                        & (1<<5))>>5)
            {
                PRINTF("%s:%d\n",__func__,__LINE__);
            }
            // enqueue EOT
            cs_data->udma_cmd[0] = SPI_CMD_EOT(1);
            spim_enqueue_channel(SPIM(device_id),
                    (uint32_t)&cs_data->udma_cmd[0], 1*(sizeof(uint32_t)),
                    UDMA_CORE_TX_CFG_EN(1), TX_CHANNEL);
        }
        else
        {
            repeat_task->data[PREALLOC_TASK_VALID_ID]=0;
            // enqueue core part if there is no "first"
            __spi_cpy_send_aligned(cs_data, addr, data, size*8, flags,
                    repeat_task);
        }
    }
    else
    {
        PRINTF("%s enqueue a transfer in fifo\n",__func__);
        struct spim_transfer transfer;
        transfer.data = data;
        transfer.flags = flags;
        transfer.len = len;
        transfer.cfg_cmd = cfg;
        transfer.byte_align = byte_align;
        transfer.type = SPI_TRANSFER_TYPE_COPY;
        transfer.device_addr = addr;
        __pi_spim_drv_fifo_enqueue(cs_data, &transfer, task);
    }
    restore_irq(irq);
}
