/*
 * Copyright (c) 2020, GreenWaves Technologies, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of GreenWaves Technologies, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "pmsis.h"
#include "udma_timestamp.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

#if !defined(__FREERTOS__)
#define UDMA_TIMESTAMP_ID(id)    ( ARCHI_UDMA_TS_ID((id)) )
#define UDMA_TIMESTAMP(id)       ( UDMA_TS_ADDR((id)) )
#define UDMA_NB_TIMESTAMP        ( ARCHI_UDMA_NB_TS )
#define TIMESTAMP_TRACE(...)     ( (void) 0 )
#define TIMESTAMP_TRACE_ERR(...) ( (void) 0 )
#endif  /* __FREERTOS__ */

/*******************************************************************************
 * Driver data
 ******************************************************************************/

static struct pi_udma_timestamp_cnt_t timestamp_cnt[UDMA_NB_TIMESTAMP];
static struct pi_udma_ts_evt_t ts_evt[UDMA_NB_TIMESTAMP_EVT];
static struct pi_udma_ts_input_t ts_input[UDMA_NB_TIMESTAMP_INPUT];
static uint8_t evt_mask = 0xF;
static uint8_t input_mask = 0xFF;

/*******************************************************************************
 * Function declaration
 ******************************************************************************/

/*******************************************************************************
 * Internal functions
 ******************************************************************************/

static void __pi_udma_timestamp_cnt_clr(uint32_t base)
{
    udma_timestamp_reg_cmd_cnt_clr_set(base, 1);
}

static void __pi_udma_timestamp_cnt_stop(uint32_t base)
{
    udma_timestamp_reg_cmd_cnt_stop_set(base, 1);
}

static void __pi_udma_timestamp_cnt_close(struct pi_udma_timestamp_cnt_t *ts)
{
    /* Stop the timestamp counter */
    __pi_udma_timestamp_cnt_stop(ts->base);

    ts->cnt_en = 0;

    if (ts->cnt_trig_gpio != 0xFF)
    {
        udma_timestamp_reg_setup_cnt_ext_sel_set(ts->base, 0);
        udma_timestamp_reg_setup_cnt_ext_type_set(ts->base, 0);
        udma_timestamp_reg_setup_cnt_ext_en_set(ts->base, 0);
    }

    udma_timestamp_reg_clk_cfg_clk_mux_set(ts->base,   0);
    udma_timestamp_reg_clk_cfg_gpio_sel_set(ts->base,  0);
    udma_timestamp_reg_clk_cfg_pwm_sel_set(ts->base,   0);
    udma_timestamp_reg_clk_cfg_prescaler_set(ts->base, 0);
    udma_timestamp_reg_clk_cfg_clk_mux_en_set(ts->base, 0);

    //TODO: clean the ts
    //memset(ts,0,sizeof(struct pi_udma_timestamp_cnt_t));
}

static void __pi_udma_evt_cfg_init(void)
{
    uint32_t base = ARCHI_UDMA_ADDR;
    // Init all the cfg event in udma ctrl to 0xFF
    udma_ctrl_cfg_event_cmp_evt0_set(base, 0xFF);
    udma_ctrl_cfg_event_cmp_evt1_set(base, 0xFF);
    udma_ctrl_cfg_event_cmp_evt2_set(base, 0xFF);
    udma_ctrl_cfg_event_cmp_evt3_set(base, 0xFF);
}


static int __pi_udma_timestamp_evt_alloc(uint32_t ts_base, pi_timestamp_event_t * evt)
{
    uint8_t src_id = 0;
    if(evt_mask)
    {
        src_id = __builtin_pulp_fl1((evt_mask));
        evt->ts_evt_id = src_id;
        evt_mask &= ~(1<<src_id);
    }
    else
    {
        TIMESTAMP_TRACE_ERR("All the events have been reserved\n");
        return -1;
    }
    return src_id;
}

static int __pi_udma_timestamp_evt_cfg(uint32_t ts_base, pi_timestamp_event_t * evt)
{

    ts_evt[evt->ts_evt_id].dest_id = evt->dest_id;

    switch (evt->ts_evt_id)
    {
        case 0:
            udma_timestamp_reg_event_dest_id_evt_0_set(ts_base, evt->dest_id);
            break;
        case 1:
            udma_timestamp_reg_event_dest_id_evt_1_set(ts_base, evt->dest_id);
            break;
        case 2:
            udma_timestamp_reg_event_dest_id_evt_2_set(ts_base, evt->dest_id);
            break;
        case 3:
            udma_timestamp_reg_event_dest_id_evt_3_set(ts_base, evt->dest_id);
            break;
        default:
            TIMESTAMP_TRACE_ERR("Unknown timestamp event numbe= %d\n", evt->ts_evt_id);
            break;
    }
    return 0;
}

static int __pi_udma_timestamp_input_set(uint32_t base, pi_timestamp_input_t * input)
{

    uint8_t src_id = 0;
    if(input_mask)
    {
        src_id = __builtin_pulp_fl1((input_mask));
        ts_input[src_id].ts_input_id = src_id;
        ts_input[src_id].dest_id = input->dest_id;
        ts_input[src_id].input_sel = input->input_sel;
        ts_input[src_id].input_type = input->input_type;
        input->ts_input_id = src_id;
        input_mask &= ~(1<<src_id);
    }
    else
    {
        TIMESTAMP_TRACE_ERR("All the events have been reserved\n");
        return -1;
    }


    if (input->input_sel >> 6)
    {
        TIMESTAMP_TRACE_ERR("GPIO ID bigger than 63\n");
        return -1;
    }
    else
    {
        switch (input->ts_input_id)
        {
            case 0:
                udma_timestamp_reg_setup_ch0_input_sel_set(base, input->input_sel);
                udma_timestamp_reg_setup_ch0_input_type_set(base, input->input_type);
                udma_timestamp_reg_setup_ch0_input_en_set(base, 1);
                udma_timestamp_reg_setup_ch0_dest_id_set(base, input->dest_id);
                break;
            case 1:
                udma_timestamp_reg_setup_ch1_input_sel_set(base, input->input_sel);
                udma_timestamp_reg_setup_ch1_input_type_set(base, input->input_type);
                udma_timestamp_reg_setup_ch1_input_en_set(base, 1);
                udma_timestamp_reg_setup_ch1_dest_id_set(base, input->dest_id);
                break;
            case 2:
                udma_timestamp_reg_setup_ch2_input_sel_set(base, input->input_sel);
                udma_timestamp_reg_setup_ch2_input_type_set(base, input->input_type);
                udma_timestamp_reg_setup_ch2_input_en_set(base, 1);
                udma_timestamp_reg_setup_ch2_dest_id_set(base, input->dest_id);
                break;
            case 3:
                udma_timestamp_reg_setup_ch3_input_sel_set(base, input->input_sel);
                udma_timestamp_reg_setup_ch3_input_type_set(base, input->input_type);
                udma_timestamp_reg_setup_ch3_input_en_set(base, 1);
                udma_timestamp_reg_setup_ch3_dest_id_set(base, input->dest_id);
                break;
            case 4:
                udma_timestamp_reg_setup_ch4_input_sel_set(base, input->input_sel);
                udma_timestamp_reg_setup_ch4_input_type_set(base, input->input_type);
                udma_timestamp_reg_setup_ch4_input_en_set(base, 1);
                udma_timestamp_reg_setup_ch4_dest_id_set(base, input->dest_id);
                break;
            case 5:
                udma_timestamp_reg_setup_ch5_input_sel_set(base, input->input_sel);
                udma_timestamp_reg_setup_ch5_input_type_set(base, input->input_type);
                udma_timestamp_reg_setup_ch5_input_en_set(base, 1);
                udma_timestamp_reg_setup_ch5_dest_id_set(base, input->dest_id);
                break;
            case 6:
                udma_timestamp_reg_setup_ch6_input_sel_set(base, input->input_sel);
                udma_timestamp_reg_setup_ch6_input_type_set(base, input->input_type);
                udma_timestamp_reg_setup_ch6_input_en_set(base, 1);
                udma_timestamp_reg_setup_ch6_dest_id_set(base, input->dest_id);
                break;
            case 7:
                udma_timestamp_reg_setup_ch7_input_sel_set(base, input->input_sel);
                udma_timestamp_reg_setup_ch7_input_type_set(base, input->input_type);
                udma_timestamp_reg_setup_ch7_input_en_set(base, 1);
                udma_timestamp_reg_setup_ch7_dest_id_set(base, input->dest_id);
                break;

            default:
                break;
        }

    }
    return 0;
}

/*******************************************************************************
 * API implementation
 ******************************************************************************/

void pi_timestamp_conf_init(struct pi_timestamp_conf *conf)
{
    conf->itf = 0;
    conf->cnt_trig_gpio = 0xFF;
    conf->cnt_trig_type = PI_TIMESTAMP_AUX_INPUT;
    conf->cnt_src = PI_TIMESTAMP_CNT_REF_CLK_QUICK;
    conf->cnt_src_id = 0xFF;
    conf->prescaler = 0;
}

void pi_udma_timestamp_open(struct pi_device * device)
{
    uint32_t irq = pi_irq_disable();
    struct pi_timestamp_conf *conf = (struct pi_timestamp_conf *) device -> config;
    struct pi_udma_timestamp_cnt_t *ts = &timestamp_cnt[conf->itf];

    device->data = (void *)ts;

    if (ts->cnt_en)
    {
        TIMESTAMP_TRACE("Timestamp counter already set, ignore this counter init");
    }
    else
    {
        ts->base = UDMA_TIMESTAMP(conf->itf);
        /* Disable UDMA CG and reset periph. */
        uint32_t periph_id = UDMA_TIMESTAMP_ID(conf->itf);
        udma_ctrl_cfg_rstn_set_set(ARCHI_UDMA_ADDR, (1 << periph_id));
        udma_ctrl_cfg_cg_set_set(ARCHI_UDMA_ADDR, (1 << periph_id));

        if (conf->cnt_trig_gpio != 0xFF)
        {
            udma_timestamp_reg_setup_cnt_ext_sel_set(ts->base, conf->cnt_trig_gpio);
            udma_timestamp_reg_setup_cnt_ext_type_set(ts->base, conf->cnt_trig_type);
            udma_timestamp_reg_setup_cnt_ext_en_set(ts->base, 1);
        }

        if (conf->cnt_src != PI_TIMESTAMP_CNT_SOC_CLK)
        {
            udma_timestamp_reg_clk_cfg_clk_mux_set(ts->base, conf->cnt_src);
            if (conf->cnt_src == PI_TIMESTAMP_CNT_GPIO)
            {
                udma_timestamp_reg_clk_cfg_gpio_sel_set(ts->base, conf->cnt_src_id);
            }
            else if(conf->cnt_src == PI_TIMESTAMP_CNT_PWM)
            {
                udma_timestamp_reg_clk_cfg_pwm_sel_set(ts->base, conf->cnt_src_id);
            }
            udma_timestamp_reg_clk_cfg_clk_mux_en_set(ts->base, 1);

        }

        if (conf->prescaler)
        {
            udma_timestamp_reg_clk_cfg_prescaler_set(ts->base, conf->prescaler);
        }

        ts->device_id = conf->itf;
        ts->cnt_trig_gpio = conf->cnt_trig_gpio;
        ts->cnt_trig_type = conf->cnt_trig_type;
        ts->cnt_src = conf->cnt_src;
        ts->cnt_src_id = conf->cnt_src_id;
        ts->prescaler = conf->prescaler;
        ts->cnt_en = 1;

        /* Set all the event to 0xFF */
        __pi_udma_evt_cfg_init();
    }
    pi_irq_restore(irq);
}

void pi_udma_timestamp_close(struct pi_device *device)
{
    uint32_t irq = pi_irq_disable();
    struct pi_udma_timestamp_cnt_t *ts = (struct pi_udma_timestamp_cnt_t *) device->data;

    __pi_udma_timestamp_cnt_close(ts);

    /* Set all the event to 0xFF */
    __pi_udma_evt_cfg_init();

    /* Enable UDMA CG and reset periph. */
    uint32_t periph_id = UDMA_TIMESTAMP_ID(ts->device_id);
    udma_ctrl_cfg_rstn_clr_set(ARCHI_UDMA_ADDR, (1 << periph_id));
    udma_ctrl_cfg_cg_clr_set(ARCHI_UDMA_ADDR, (1 << periph_id));

    /* Free all the udma timestamp allocated. */
    //TODO: clean all the event and input
    /*
    for (uint32_t tid = 0; tid < (uint32_t) UDMA_NB_TIMESTAMP_EVT; tid++)
    {
        if(ts_evt[tid].soc_evt)
            soc_eu_prEventMask_setEvent(ts_evt[tid].soc_evt);
    }
    */

    evt_mask = 0xF;
    input_mask = 0xFF;
    pi_irq_restore(irq);
}


int32_t pi_udma_timestamp_ioctl(struct pi_device *device, uint32_t cmd, void *arg)
{
    uint32_t irq = pi_irq_disable();
    struct pi_udma_timestamp_cnt_t *ts = (struct pi_udma_timestamp_cnt_t *) device->data;
    int32_t status = 0;
    int src_id = 0;

    switch (cmd)
    {
    case PI_UDMA_TIMESTAMP_IOCTL_CLR :
        __pi_udma_timestamp_cnt_clr(ts->base);
        break;

    case PI_UDMA_TIMESTAMP_IOCTL_STOP :
        __pi_udma_timestamp_cnt_stop(ts->base);
        break;

    case PI_UDMA_TIMESTAMP_IOCTL_EVT_ALLOC:
        __pi_udma_timestamp_evt_alloc(ts->base, arg);
        break;

    case PI_UDMA_TIMESTAMP_IOCTL_SET_EVT :
        status = __pi_udma_timestamp_evt_cfg(ts->base, arg);
        break;

    case PI_UDMA_TIMESTAMP_IOCTL_SET_INPUT :
        status = __pi_udma_timestamp_input_set(ts->base, arg);
        break;

        // TODO: complete these cases
    case PI_UDMA_TIMESTAMP_IOCTL_FREE_EVT :
        break;

    case PI_UDMA_TIMESTAMP_IOCTL_FREE_INPUT :
        break;

    default :
        TIMESTAMP_TRACE_ERR("Unknown timestamp command, cmd=%ld\n", cmd);
    }
    pi_irq_restore(irq);
    return status;
}

/** Not in PMSIS_API. */
#if 0
void pi_udma_timestamp_read_async(unsigned char src_type, unsigned char ts_id,
                                  void *buffer, int32_t size, pi_task_t *task)
{
    pos_udma_channel_t *channel = src_type? &ts_evt[ts_id].channel : &ts_input[ts_id].channel;

    uint32_t irq = pi_irq_disable();
    pos_udma_enqueue(channel, task, (int)buffer, size);
    pi_irq_restore(irq);
}
#endif  /* 0 */
