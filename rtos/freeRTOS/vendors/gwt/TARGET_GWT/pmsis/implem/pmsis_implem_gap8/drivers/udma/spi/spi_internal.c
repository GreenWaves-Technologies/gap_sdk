#include "pmsis.h"
#include "pmsis/implem/drivers/udma/spi/spi_internal.h"
#include "pmsis/rtos/event_kernel/event_kernel.h"

#ifndef DEBUG
#define DBG_PRINTF( ... ) ((void)0)
#else
#define DBG_PRINTF printf
#endif

struct spim_driver_data *__g_spim_drv_data[UDMA_NB_SPIM] = {0};

static uint32_t __pi_spi_clk_div_get(uint32_t spi_freq)
{
    uint32_t periph_freq = pi_freq_get(PI_FREQ_DOMAIN_PERIPH);
    if (spi_freq < periph_freq)
    {
        uint32_t clk_div = 0;
        clk_div = (periph_freq + spi_freq - 1) / spi_freq;
        if (clk_div & 1)
        {
            clk_div += 1;
        }
        /* The SPIM always divide by 2 once we activate the divider, thus increase by 1
           in case it is even to not go above the max frequency. */
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
}

// TODO: prepare pseudo exec for delegate
void __pi_spim_execute_callback(void *arg)
{
    return;
}

void __pi_spim_exec_next_transfer(pi_task_t *task)
{
    if(task->data[5] == 1)
    {// if is send
        // cs data | data buffer | len | flags | end of transfer task
        __pi_spi_send_async((struct spim_cs_data *)task->data[0],
                (void*)task->data[1], task->data[2],
                task->data[3], (pi_task_t *)task->data[4]);
    }
    else if(task->data[5] == 0)
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

extern struct pmsis_event_kernel_wrap *default_sched;
void spim_eot_handler(void *arg)
{
    uint32_t event = (uint32_t) arg;
    uint32_t channel = event & 0x1;
    // EOT is simply 22 + id in GAP8
    uint32_t periph_id = (event - SOC_EVENT_UDMA_SPIM_EOT(0));

    struct spim_driver_data *drv_data = __g_spim_drv_data[periph_id];

    if(drv_data->repeat_transfer)
    {
        DBG_PRINTF("%s:%d\n",__func__,__LINE__);
        // TODO: reenqueue the rest of the transfer
        DBG_PRINTF("Large transfers (>8k) are not implemented yet\n");
        return;
    }
    pi_task_t *task = drv_data->end_of_transfer;
    DBG_PRINTF("%s:%d\n",__func__,__LINE__);
    if(task!=NULL)
    {
        if (task->id == PI_TASK_NONE_ID)
        {
            DBG_PRINTF("%s:%d release task %p\n",__func__,__LINE__,task);
            pi_task_release(task);
        }
        else
        {
            DBG_PRINTF("%s:%d push task %p with id:%x in sched%p\n",__func__,
                    __LINE__,
                    task, task->id,
                    default_sched);
            DBG_PRINTF("%s:%d periph id:%x\n",__func__,__LINE__,periph_id);
            pmsis_event_push(default_sched,task);
            DBG_PRINTF("%s:%d\n",__func__,__LINE__);
        }
        drv_data->end_of_transfer = NULL;
    }
#ifdef DEBUG
    else
    {
        DBG_PRINTF("%s:%d null task %p\n",__func__,__LINE__,task);
    }
#endif
    task = __pi_spim_drv_fifo_pop(drv_data);
    if(task)
    {
        __pi_spim_exec_next_transfer(task);
    }
}

void spim_tx_handler(void *arg)
{ // if we're here, it's cs keep related
    uint32_t event = (uint32_t) arg;
    uint32_t channel = event & 0x1;
    uint32_t periph_id = (event >> UDMA_CHANNEL_NB_EVENTS_LOG2) - UDMA_SPIM_ID(0);
    hal_soc_eu_clear_fc_mask(SOC_EVENT_UDMA_SPIM_TX(periph_id));
    arg = (void*) ( SOC_EVENT_UDMA_SPIM_EOT(0) + periph_id );
    DBG_PRINTF("%s:%d periph_id %x arg:%p\n",__func__,__LINE__, periph_id, arg);
    spim_eot_handler(arg);
    DBG_PRINTF("%s:%d\n",__func__,__LINE__);
}

void spim_rx_handler(void *arg)
{ // if we're here, it's cs keep related
    uint32_t event = (uint32_t) arg;
    uint32_t channel = event & 0x1;
    uint32_t periph_id = (event >> UDMA_CHANNEL_NB_EVENTS_LOG2) - UDMA_SPIM_ID(0);
    hal_soc_eu_clear_fc_mask(SOC_EVENT_UDMA_SPIM_RX(periph_id));
    arg = (void*) ( SOC_EVENT_UDMA_SPIM_EOT(0) + periph_id );
    DBG_PRINTF("%s:%d periph_id %x arg:%p\n",__func__,__LINE__, periph_id, arg);
    spim_eot_handler(arg);
    DBG_PRINTF("%s:%d\n",__func__,__LINE__);
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
            DBG_PRINTF("[%s] _cs_data alloc failed\n", __func__);
            return -2;
        }
        if (clk_div > 0xFF)
        {
            DBG_PRINTF("[%s] clk_div, %x, does not fit into 8 bits. SoC frequency too high.\n",
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
        *cs_data                = _cs_data;
        __pi_spim_cs_data_add(drv_data,_cs_data);
    }
    DBG_PRINTF("%s:%d\n",__func__,__LINE__);
    return 0;
}

int __pi_spi_close(struct spim_cs_data *cs_data)
{
    struct spim_driver_data *drv_data = cs_data->drv_data;
    __pi_spim_cs_data_del(drv_data,cs_data->cs);
    drv_data->nb_open--;
    if(drv_data->nb_open == 0)
    {
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

void __pi_spi_receive_async(struct spim_cs_data *cs_data, void *data, size_t len,
        pi_spi_flags_e flags, pi_task_t *task)
{
    struct spim_driver_data *drv_data = SPIM_CS_DATA_GET_DRV_DATA(cs_data);
    int qspi    = ((flags >> 2) & 0x3) == ((PI_SPI_LINES_QUAD>>2)&0x3);
    int cs_mode = (flags >> 0) & 0x3;

    int device_id = drv_data->device_id;
    uint32_t cfg = cs_data->cfg;
    DBG_PRINTF("%s:%d: core clock:%d, baudrate:%d, div=%d, udma_cmd cfg =%lx, qpi=%lx\n",
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
        drv_data->repeat_transfer = NULL;
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
    int qspi    = ((flags >> 2) & 0x3) == ((PI_SPI_LINES_QUAD>>2) & 0x03);
    int cs_mode = (flags >> 0) & 0x3;

    int device_id = drv_data->device_id;
    uint32_t byte_align = (cs_data->wordsize == PI_SPI_WORDSIZE_32)
        && cs_data->big_endian;
    uint32_t cfg = cs_data->cfg;
    DBG_PRINTF("%s:%d: core clock:%d, baudrate:%d, div=%d,  byte_align =%lx, cfg= %lx, qspi=%lx\n",
            __func__,__LINE__,system_core_clock_get(),cs_data->max_baudrate,
            system_core_clock_get() / cs_data->max_baudrate,byte_align,cfg,qspi);
    int size = (len + 7) >> 3;

    int cmd_id = 0;

    int irq = __disable_irq();
    if(!drv_data->end_of_transfer)
    {
        if(cs_mode != PI_SPI_CS_AUTO)
        {
            hal_soc_eu_set_fc_mask(SOC_EVENT_UDMA_SPIM_RX(device_id));
        }
        drv_data->end_of_transfer = task;
        drv_data->repeat_transfer = NULL;
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
    int qspi    = ((flags >> 2) & 0x3) == ((PI_SPI_LINES_QUAD>>2) & 0x03);
    int cs_mode = (flags >> 0) & 0x3;

    int device_id = drv_data->device_id;
    uint32_t byte_align = (cs_data->wordsize == PI_SPI_WORDSIZE_32)
        && cs_data->big_endian;
    uint32_t cfg = cs_data->cfg;
    DBG_PRINTF("%s:%d: core clock:%d, baudrate:%d, div=%d,  byte_align =%lx, cfg= %lx, qspi=%lx\n",
            __func__,__LINE__,system_core_clock_get(),cs_data->max_baudrate,
            system_core_clock_get() / cs_data->max_baudrate,byte_align,cfg,qspi);
    int size = (len + 7) >> 3;

    int cmd_id = 0;

    int irq = __disable_irq();
    if(!drv_data->end_of_transfer)
    {
        if(cs_mode != PI_SPI_CS_AUTO)
        {
            hal_soc_eu_set_fc_mask(SOC_EVENT_UDMA_SPIM_TX(device_id));
        }
        drv_data->end_of_transfer = task;
        drv_data->repeat_transfer = NULL;

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
                DBG_PRINTF("%s:%d\n",__func__,__LINE__);
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
    DBG_PRINTF("%s:%d: core clock:%d, baudrate:%d, div=%d, udma_cmd cfg =%lx, qpi=%lx\n",
            __func__,__LINE__,system_core_clock_get(),cs_data->max_baudrate,
            system_core_clock_get() / cs_data->max_baudrate,cfg, qspi);
    uint32_t byte_align = (cs_data->wordsize == PI_SPI_WORDSIZE_32)
        && cs_data->big_endian;
    int size = (len + 7) >> 3;

    DBG_PRINTF("%s:%d: udma_cmd=%p\n",__func__,__LINE__, &(cs_data->udma_cmd[0]));
    int irq = disable_irq();
    if(!drv_data->end_of_transfer)
    {// enqueue the transfer
        cs_data->udma_cmd[0] = cfg;
        cs_data->udma_cmd[1] = SPI_CMD_SOT(cs_data->cs);
        cs_data->udma_cmd[2] = SPI_CMD_TX_DATA(len, qspi, byte_align);
        DBG_PRINTF("%s:%d\n",__func__,__LINE__);
        if(cs_mode == PI_SPI_CS_AUTO)
        {
            drv_data->end_of_transfer = task;
            drv_data->repeat_transfer = NULL;

            // enqueue header first
            spim_enqueue_channel(SPIM(device_id), (uint32_t)cs_data->udma_cmd,
                    3*(sizeof(uint32_t)),
                    UDMA_CORE_TX_CFG_EN(1), TX_CHANNEL);
            // enqueue user data
            spim_enqueue_channel(SPIM(device_id), (uint32_t)data, size,
                    UDMA_CORE_TX_CFG_EN(1),
                    TX_CHANNEL);
            #if defined(DEBUG)
            DBG_PRINTF("%s:%d: udma_cmd: %x %x %x\n", __func__, __LINE__,
                       cs_data->udma_cmd[0], cs_data->udma_cmd[1], cs_data->udma_cmd[2]);
            DBG_PRINTF("%s:%d: data: %p size: %d\n", __func__, __LINE__, data, size);
            #endif  /* DEBUG */
            // wait until channel is free
            while((hal_read32((void*)&(SPIM(device_id)->udma.tx_cfg))
                        & (1<<5))>>5)
            {
                DBG_PRINTF("%s:%d\n",__func__,__LINE__);
            }

            // enqueue EOT
            cs_data->udma_cmd[0] = SPI_CMD_EOT(1);
            DBG_PRINTF("%s:%d: %lx\n",__func__,__LINE__,cs_data->udma_cmd[0]);
            spim_enqueue_channel(SPIM(device_id),
                    (uint32_t)&cs_data->udma_cmd[0], 1*(sizeof(uint32_t)),
                    UDMA_CORE_TX_CFG_EN(1), TX_CHANNEL);
        }
        else
        {// cs keep, has to use tx event
            drv_data->end_of_transfer = task;
            drv_data->repeat_transfer = NULL;
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
    int qspi    = ((flags >> 2) & 0x3) == ((PI_SPI_LINES_QUAD>>2) & 0x3);
    int cs_mode = (flags >> 0) & 0x3;

    int device_id = drv_data->device_id;
    uint32_t cfg = cs_data->cfg;
    DBG_PRINTF("%s:%d: core clock:%d, baudrate:%d, div=%d, udma_cmd cfg =%lx\n",
            __func__,__LINE__,system_core_clock_get(),cs_data->max_baudrate,
            system_core_clock_get() / cs_data->max_baudrate,cfg);
    uint32_t byte_align = (cs_data->wordsize == PI_SPI_WORDSIZE_32)
        && cs_data->big_endian;
    int size = (len + 7) >> 3;

    int cmd_id = 0;

    int irq = __disable_irq();
    if(!drv_data->end_of_transfer)
    {
        cs_data->udma_cmd[0] = cfg;
        cs_data->udma_cmd[1] = SPI_CMD_SOT(cs_data->cs);
        cs_data->udma_cmd[2] = SPI_CMD_FULL_DUPL(len, byte_align);
        drv_data->end_of_transfer = task;
        drv_data->repeat_transfer = NULL;
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
                DBG_PRINTF("%s:%d\n",__func__,__LINE__);
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
                DBG_PRINTF("%s:%d\n",__func__,__LINE__);
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

