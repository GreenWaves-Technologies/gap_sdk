/*
 * Copyright (C) 2020  GreenWaves Technologies, SAS
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "archi/utils.h"
#include "vp/itf/i2c.hpp"
#include "udma_i2c.hpp"


#include <cassert>

#define I2C_PERIPH_FPRINTF(...) (this->trace.msg(vp::trace::LEVEL_TRACE, __VA_ARGS__))
//#define I2C_PERIPH_FPRINTF(...) fprintf(stderr, __VA_ARGS__)
//#define I2C_PERIPH_FPRINTF(...)

using namespace std::placeholders;

I2c_periph::I2c_periph(udma *top, int id, int itf_id) :
    Udma_periph(top, id),
    i2c_helper(top,
            &this->i2c_itf,
            std::bind(&I2c_periph::i2c_enqueue_event,
                this,
                _1,
                _2),
            std::bind(&I2c_periph::i2c_cancel_event,
                this,
                _1)
            ),
    is_waiting_i2c_start(false),
    is_waiting_i2c_data(false),
    is_waiting_i2c_ack(false),
    is_waiting_i2c_stop(false),
    is_waiting_i2c_stop_to_start(false),
    delayed_start_event(top, this, &I2c_periph::st_delayed_start_event_handler),
    lead_rx_channel(top, this, "i2c" + std::to_string(itf_id) + "_lead_rx"),
    lead_rx_fifo(top, &this->lead_rx_channel, 1),
    lead_tx_channel(top, this, "i2c" + std::to_string(itf_id) + "_lead_tx"),
    lead_tx_fifo(top, &this->lead_tx_channel, 4, 1),
    cmd_channel(top, this, "i2c" + std::to_string(itf_id) + "_cmd"),
    cmd_fifo(top, &this->cmd_channel, 4, 4)
{
    std::string itf_name = "i2c" + std::to_string(itf_id);

    top->traces.new_trace(itf_name, &trace, vp::DEBUG);

    this->is_tx_write = false;
    this->tx_write_offset = 0;

    this->is_rx_write = false;
    this->rx_write_offset = 0;

    this->repeat_downcounter = 0;

    /* create udma channels */
    this->foll_rx_channel = new I2c_foll_rx_channel(top, this, itf_name + "_foll_rx");
    this->foll_tx_channel = new I2c_foll_tx_channel(top, this, itf_name + "_foll_tx");

    this->i2c_itf.set_sync_meth(&I2c_periph::i2c_sync);
    top->new_master_port(this, itf_name, &this->i2c_itf);

    /* build the register map */
    this->regmap.build(top, &this->trace, itf_name);

    /* set register callbacks */
    this->regmap.udma_cmd_dest_reg_idx.register_callback(
            std::bind(&I2c_periph::cmd_req, this, _1, _2, _3, _4));
    this->regmap.lead_udma_rx_dest_reg_idx.register_callback(
            std::bind(&I2c_periph::lead_rx_req, this, _1, _2, _3, _4));
    this->regmap.lead_udma_tx_dest_reg_idx.register_callback(
            std::bind(&I2c_periph::lead_tx_req, this, _1, _2, _3, _4));
    this->regmap.status_reg_idx.register_callback(
            std::bind(&I2c_periph::status_req, this, _1, _2, _3, _4));

    /* set ucode decoder callback */
    this->ucode_decoder.register_callback(
            std::bind(&I2c_periph::ucode_handler, this, _1));

    /* set helper callback */
    this->i2c_helper.register_callback(std::bind(&I2c_periph::i2c_helper_callback,
                this, _1, _2, _3));

    /* get SOC event numbers */
    {
        this->leader_event_number = -1;
        js::config *config = this->top->get_js_config()->get("i2c/leader_events");
        if (NULL != config)
        {
            this->leader_event_number = config->get_elem(itf_id)->get_int();
        }
        assert(this->leader_event_number >= 0);
    }
    {
        this->follower_event_number = -1;
        js::config *config = this->top->get_js_config()->get("i2c/follower_events");
        if (NULL != config)
        {
            this->follower_event_number = config->get_elem(itf_id)->get_int();
        }
        assert(this->follower_event_number >= 0);
    }
}

void I2c_periph::st_delayed_start_event_handler(void* __this, vp::clock_event* event)
{
    assert(NULL != __this);
    assert(NULL != event);
    I2c_periph* _this = (I2c_periph*) __this;
    _this->i2c_start();
}

void I2c_periph::i2c_enqueue_event(vp::clock_event* event, uint64_t time_ps)
{
    auto clk = this->top->get_periph_clock();
    assert(clk != NULL);
    //TODO convert time_ps to cycle
    clk->event_enqueue(event, time_ps);
}

void I2c_periph::i2c_cancel_event(vp::clock_event* event)
{
    auto clk = this->top->get_periph_clock();
    assert(clk != NULL);
    clk->event_cancel(event);
}

void I2c_periph::reset(bool active)
{
    Udma_periph::reset(active);

    this->foll_rx_channel->reset(active);
    this->foll_tx_channel->reset(active);

    if (active)
    {
    }
}

void I2c_periph::lead_tx_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write)
{
    assert(value != NULL);

    this->regmap.lead_udma_tx_dest_reg_idx.update(reg_offset, size, value, is_write);

    if (is_write)
    {
        this->trace.msg(vp::trace::LEVEL_INFO, "Setting LEAD_TX channel (id: %d)\n",
                this->regmap.lead_udma_tx_dest_reg_idx.get());
        I2C_PERIPH_FPRINTF("[I2C] setting register: LEAD_TX channel #%d\n",
                this->regmap.lead_udma_tx_dest_reg_idx.get());
        this->top->channel_register(this->regmap.lead_udma_tx_dest_reg_idx.get(),
                &this->lead_tx_channel);
        this->lead_tx_fifo.start();
    }
}

void I2c_periph::lead_rx_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write)
{
    assert(value != NULL);

    this->regmap.lead_udma_rx_dest_reg_idx.update(reg_offset, size, value, is_write);

    if (is_write)
    {
        this->trace.msg(vp::trace::LEVEL_INFO, "Setting LEAD_RX channel (id: %d)\n",
                this->regmap.lead_udma_rx_dest_reg_idx.get());
        I2C_PERIPH_FPRINTF("[I2C] setting register: LEAD_RX channel #%d\n",
                this->regmap.lead_udma_rx_dest_reg_idx.get());
        this->top->channel_register(this->regmap.lead_udma_rx_dest_reg_idx.get(),
                &this->lead_rx_channel);
    }
}

void I2c_periph::cmd_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write)
{
    assert(value != NULL);

    this->regmap.udma_cmd_dest_reg_idx.update(reg_offset, size, value, is_write);

    if (is_write)
    {
        this->trace.msg(vp::trace::LEVEL_INFO, "Setting CMD channel (id: %d)\n",
                this->regmap.udma_cmd_dest_reg_idx.get());
        I2C_PERIPH_FPRINTF("[I2C] setting register: CMD channel #%d\n",
                this->regmap.udma_cmd_dest_reg_idx.get());
        this->top->channel_register(this->regmap.udma_cmd_dest_reg_idx.get(),
                &this->cmd_channel);

        this->cmd_fifo.start();
    }
}

void I2c_periph::status_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write)
{
    assert(value != NULL);
    assert(size >= 4);

    if (!is_write)
    {
        this->regmap.status_reg_idx.read(reg_offset, size, value);
    }
    //this->regmap.status_reg_idx.update(reg_offset, size, value, is_write);
    uint32_t w_value = *(uint32_t*) value;

    if (is_write)
    {
        /* clear events if 1 is written */
        //if (UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_SOF_RCV_EVENT_I_IDX_GET(w_value))
        //{
        //}
        //UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_SOF_SND_EVENT_I_IDX
        //UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_EOF_RCV_EVENT_I_IDX
        //UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_EOF_SND_EVENT_I_IDX
        //UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_ERROR_ARLO_EVENT_I_IDX
        //UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_ERROR_FRAMING_EVENT_I_IDX
        //UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_UNLOCK_EVENT_O_IDX
        //UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_PURGE_EVENT_O_IDX
        if (UDMA_I2C_STATUS_REG_IDX_STATUS_LEAD_ERROR_NACK_EVENT_I_IDX_GET(w_value))
        {
            this->regmap.status_reg_idx.status_lead_error_nack_event_i_idx_set(0);
        }

        if (UDMA_I2C_STATUS_REG_IDX_STATUS_LEAD_ERROR_ARLO_EVENT_I_IDX_GET(w_value))
        {
            this->regmap.status_reg_idx.status_lead_error_arlo_event_i_idx_set(0);
        }

        if (UDMA_I2C_STATUS_REG_IDX_STATUS_LEAD_ERROR_FRAMING_EVENT_I_IDX_GET(w_value))
        {
            this->regmap.status_reg_idx.status_lead_error_framing_event_i_idx_set(0);
        }

        if (UDMA_I2C_STATUS_REG_IDX_STATUS_LEAD_COMMAND_EVENT_I_IDX_GET(w_value))
        {
            this->regmap.status_reg_idx.status_lead_command_event_i_idx_set(0);
        }

        if (UDMA_I2C_STATUS_REG_IDX_STATUS_LEAD_UNLOCK_EVENT_O_IDX_GET(w_value))
        {
            //TODO unlock
            this->regmap.status_reg_idx.status_lead_unlock_event_o_idx_set(0);
        }

        if (UDMA_I2C_STATUS_REG_IDX_STATUS_LEAD_PURGE_EVENT_O_IDX_GET(w_value))
        {
            this->lead_purge();
            this->regmap.status_reg_idx.status_lead_purge_event_o_idx_set(0);
        }

        if (UDMA_I2C_STATUS_REG_IDX_STATUS_I2C_SOFT_RESET_EVENT_O_IDX_GET(w_value))
        {
            //TODO soft reset
            this->regmap.status_reg_idx.status_i2c_soft_reset_event_o_idx_set(0);
            assert(false);
        }
        if (UDMA_I2C_STATUS_REG_IDX_STATUS_I2C_PRESCALER_SET_DIV10_EVENT_O_IDX_GET(w_value))
        {
            //TODO prescaler
            this->regmap.status_reg_idx.status_i2c_prescaler_set_div10_event_o_idx_set(0);
        }
    }
}

vp::io_req_status_e I2c_periph::custom_req(vp::io_req *req, uint64_t offset)
{
    assert(req != NULL);

    if (this->regmap.access(offset, req->get_size(), req->get_data(), req->get_is_write()))
    {
        return vp::IO_REQ_INVALID;
    }

    return vp::IO_REQ_OK;
}

void I2c_periph::i2c_sync(void *__this, int scl, int sda)
{
    assert(NULL != __this);
    I2c_periph* _this = (I2c_periph*) __this;
    _this->i2c_helper.update_pins(scl, sda);
}

void I2c_periph::ucode_handler(ucode_data_t data)
{
    //I2C_PERIPH_FPRINTF("[I2C] ucode_handler: data.id=%d\n", data.id);
    switch(data.id)
    {
        case CMD_MISC_NOP:
            I2C_PERIPH_FPRINTF("[I2C] handler for id:CMD_MISC_NOP not implemented\n");
            break;
        case CMD_MISC_WAIT:
            I2C_PERIPH_FPRINTF("[I2C] handler for id:CMD_MISC_WAIT not implemented\n");
            break;
        case CMD_MISC_REPEAT:
            this->repeat_downcounter = data.value.repeat.counter;
            break;
        case CMD_MISC_WAIT_I2C_PERIOD_END:
            I2C_PERIPH_FPRINTF("[I2C] handler for id:CMD_MISC_WAIT_I2C_PERIOD_END not implemented\n");
            //TODO
            //this->is_waiting_i2c_operation = true;
            break;
        case CMD_TIMING:
            this->i2c_helper.set_timings(100, 100);
            // TODO ucode settings to timings
            // data.value.timings.div_prescaler,
            // data.value.timings.delay_low,
            // data.value.timings.delay_high,
            break;
        case CMD_FOLL_ADDR:
            I2C_PERIPH_FPRINTF("[I2C] handler for id:CMD_FOLL_ADDR not implemented\n");
            break;
        case CMD_LEAD_START:
            this->i2c_start();
            break;
        case CMD_LEAD_SEND:
            {
                if (this->repeat_downcounter == 0)
                {
                    this->repeat_downcounter = 1;
                }
                this->is_waiting_i2c_ack = true;
                uint8_t byte = 0x0;
                if (!this->lead_tx_fifo.empty())
                {
                    byte = this->lead_tx_fifo.get_data();
                }
                this->i2c_helper.send_data(byte);
                break;
            }
        case CMD_LEAD_SEND_IMM:
            this->is_waiting_i2c_ack = true;

            /* ignore repeat downcounter */
            this->repeat_downcounter = 1;

            this->i2c_helper.send_data(data.value.lead_send_imm.byte);
            break;
        case CMD_LEAD_SEND_IMM_ADDR:
            {
                int addr = data.value.lead_send_imm_addr.address >> 1;
                bool is_write = !(data.value.lead_send_imm_addr.address & 1);

                this->is_waiting_i2c_ack = true;

                /* ignore repeat downcounter */
                this->repeat_downcounter = 1;

                this->trace.msg(vp::trace::LEVEL_TRACE, "sending address=0x%x, write=%s\n",
                        addr,
                        is_write ? "true" : "false");
                this->i2c_helper.send_address(addr,
                        is_write,
                        data.value.lead_send_imm_addr.is_10bits);
            }
            break;
        case CMD_LEAD_RECV:
            if (this->repeat_downcounter == 0)
            {
                this->repeat_downcounter = 1;
            }
            this->is_waiting_i2c_data = true;
            break;
        case CMD_LEAD_RECV_LAST:
            // TODO
            if (this->repeat_downcounter == 0)
            {
                this->repeat_downcounter = 1;
            }
            this->is_waiting_i2c_data = true;
            break;
        case CMD_LEAD_STOP:
            this->is_waiting_i2c_stop = true;
            this->i2c_helper.send_stop();
            break;
        case CMD_EVENT_RECV:
            //TODO
            I2C_PERIPH_FPRINTF("[I2C] handler for id:CMD_EVENT_RECV not implemented\n");
            break;
        case CMD_EVENT_SEND:
            this->regmap.status_reg_idx.status_lead_command_event_i_idx_set(1);
            this->top->trigger_event(this->leader_event_number);
            break;
        case CMD_UDMA_TX_CHAN_CFG:
            this->is_tx_write = true;
            this->tx_write_offset = data.value.tx_chan_cfg.register_address;
            break;
        case CMD_UDMA_RX_CHAN_CFG:
            this->is_rx_write = true;
            this->rx_write_offset = data.value.rx_chan_cfg.register_address;
            break;
        default:
            I2C_PERIPH_FPRINTF("[I2C] handler for id:%d not implemented\n", data.id);
            break;
    }

    if (!this->is_waiting_i2c_operation())
    {
        this->process_command();
    }
}

bool I2c_periph::is_waiting_i2c_operation(void)
{
    return (this->is_waiting_i2c_start
            || this->is_waiting_i2c_data
            || this->is_waiting_i2c_ack
            || this->is_waiting_i2c_stop
            || this->is_waiting_i2c_stop_to_start
            || this->delayed_start_event.is_enqueued());
}

bool I2c_periph::is_in_error_mode(void)
{
    bool is_in_error = false;
    if (0 != this->regmap.status_reg_idx.status_lead_error_arlo_event_i_idx_get()
     || 0 != this->regmap.status_reg_idx.status_lead_error_nack_event_i_idx_get()
     || 0 != this->regmap.status_reg_idx.status_lead_error_framing_event_i_idx_get()
     || 0 != this->regmap.status_reg_idx.status_foll_error_arlo_event_i_idx_get()
     || 0 != this->regmap.status_reg_idx.status_foll_error_framing_event_i_idx_get()
     )
    {
        is_in_error = true;
    }
    return is_in_error;
}

void I2c_periph::lead_purge(void)
{
    this->trace.msg(vp::trace::LEVEL_TRACE, "purging lead data\n");
    /* empty fifos */
    this->cmd_fifo.reset();
    this->lead_tx_fifo.reset();
    this->lead_rx_fifo.reset();

    /* clear internal status */
    this->is_waiting_i2c_start = false;
    this->is_waiting_i2c_data = false;
    this->is_waiting_i2c_ack = false;
    this->is_waiting_i2c_stop = false;
    this->is_waiting_i2c_stop_to_start = false;

    /* restart fifos */
    this->cmd_fifo.start();
    this->lead_tx_fifo.start();
}

void I2c_periph::i2c_helper_callback(i2c_operation_e id, i2c_status_e status, int value)
{
    I2C_PERIPH_FPRINTF("CALLBACK id=%d, status=%d, value=%d !\n",
            id, status, value);

    switch(id)
    {
        case I2C_OP_START:
            assert(status == I2C_STATUS_OK);
            I2C_PERIPH_FPRINTF("START!\n");
            this->is_waiting_i2c_start = false;
            break;
        case I2C_OP_DATA:
            I2C_PERIPH_FPRINTF("DATA!\n");
            I2C_PERIPH_FPRINTF("(repeat=%d)\n", this->repeat_downcounter);

            //TODO choose if send ack or not
            if (status == I2C_STATUS_OK && this->is_waiting_i2c_data)
            {
                repeat_downcounter--;
                assert(repeat_downcounter >= 0);
                if (repeat_downcounter == 0)
                {
                    this->is_waiting_i2c_data = false;
                }
                uint8_t byte = value & 0xFF;
                this->lead_rx_fifo.push(&byte, 1);

                this->i2c_helper.send_ack(true);
            }
            else if (this->is_waiting_i2c_ack)
            {
                if (status == I2C_STATUS_ERROR_FRAMING)
                {
                    assert(false);
                }
                else if (status == I2C_STATUS_ERROR_ARBITRATION)
                {
                    this->trace.msg(vp::trace::LEVEL_TRACE, "Got an arbitration error\n");
                    // release pins
                    this->i2c_helper.release_pins();
                    // trigger error arbitration
                    this->regmap.status_reg_idx.status_lead_error_arlo_event_i_idx_set(1);
                    this->top->trigger_event(this->leader_event_number);
                }
            }

            break;
        case I2C_OP_ACK:
            //TODO
            I2C_PERIPH_FPRINTF("(repeat=%d)\n", this->repeat_downcounter);

            assert(status == I2C_STATUS_OK || status == I2C_STATUS_KO);
            if (this->is_waiting_i2c_ack)
            {
                if (status == I2C_STATUS_KO)
                {
                    // received a nack, go into error mode
                    // TODO lock_channels

                    I2C_PERIPH_FPRINTF("=> NACK ERROR!\n");

                    this->regmap.status_reg_idx.status_lead_error_nack_event_i_idx_set(1);
                    this->top->trigger_event(this->leader_event_number);

                    if (true) //this->autostop_enabled)
                    {
                        this->i2c_helper.send_stop();
                    }
                }
                else
                {
                    this->repeat_downcounter--;
                    if (this->repeat_downcounter != 0)
                    {
                        /* continue to send data */
                        uint8_t byte = 0x0;
                        if (!this->lead_tx_fifo.empty())
                        {
                            byte = this->lead_tx_fifo.get_data();
                        }
                        this->i2c_helper.send_data(byte);
                    }
                    else
                    {
                        this->is_waiting_i2c_ack = false;
                    }
                }
            }

            I2C_PERIPH_FPRINTF("ACK!\n");
            break;
        case I2C_OP_STOP:
            assert(status == I2C_STATUS_OK);
            this->is_waiting_i2c_stop = false;
            if (this->is_waiting_i2c_stop_to_start)
            {
                // enqueue delayed start
                this->is_waiting_i2c_stop_to_start = false;
                if (!this->delayed_start_event.is_enqueued())
                {
                    this->top->get_periph_clock()->event_enqueue(&this->delayed_start_event, 100);
                }
            }
            I2C_PERIPH_FPRINTF("STOP!\n");
            break;
        default:
            break;
    }

    I2C_PERIPH_FPRINTF("CALLBACK is_waiting_i2c_operation=%s\n", this->is_waiting_i2c_operation() ? "true" : "false");
    if (!this->is_waiting_i2c_operation())
    {
        this->process_command();
    }
}

void I2c_periph::i2c_start(void)
{
    if (!this->i2c_helper.is_busy())
    {
        I2C_PERIPH_FPRINTF("Sending start directly\n");
        this->is_waiting_i2c_start = true;
        this->i2c_helper.send_start();
    }
    else
    {
        I2C_PERIPH_FPRINTF("Could not send start, wait end of current i2c transfer\n");
        this->is_waiting_i2c_stop_to_start = true;
    }
}

void I2c_periph::push_cmd(uint8_t* data, int size)
{
    assert(data != NULL);
    assert(size > 0);

    this->cmd_fifo.push_data(data, size);
    this->process_command();
}


void I2c_periph::process_command(void)
{
    I2C_PERIPH_FPRINTF("process_command\n");
    I2C_PERIPH_FPRINTF("-> is_error=%s, is_waiting_i2c_operation=%s, cmd_fifo.empty=%s\n",
            this->is_in_error_mode() ? "true" : "false",
            this->is_waiting_i2c_operation() ? "true" : "false",
            this->cmd_fifo.empty() ? "true" : "false");

    if (!this->is_in_error_mode()
            && !this->is_waiting_i2c_operation()
            && !this->cmd_fifo.empty())
    {
        /* pop ucode from fifo */
        uint32_t ucode = 0;
        for (int i = 0; i < 4; i++)
        {
            uint8_t byte = this->cmd_fifo.get_data();
            I2C_PERIPH_FPRINTF("UCODE_b: %x\n", byte);
            ucode = byte << (8*i) | ucode ;
        }

        /* process ucode */
        if (this->is_rx_write)
        {
            I2C_PERIPH_FPRINTF("writing 0x%08x to rx register (offset: 0x%02x)\n",
                    ucode,
                    this->rx_write_offset);
            this->lead_rx_channel.access(this->rx_write_offset * 4,
                    4, (uint8_t*) &ucode, true);
            this->is_rx_write = false;
        }
        else if (this->is_tx_write)
        {
            I2C_PERIPH_FPRINTF("writing 0x%08x to tx register (offset: 0x%02x)\n",
                    ucode,
                    this->tx_write_offset);
            this->lead_tx_channel.access(this->tx_write_offset * 4,
                    4, (uint8_t*) &ucode, true);
            this->is_tx_write = false;
        }
        else
        {
            /* call ucode_decode */
            this->ucode_decoder.decode(ucode);
        }
    }
}

I2c_lead_tx_channel::I2c_lead_tx_channel(udma *top, I2c_periph *periph, string name)
    : Udma_tx_channel(top, name), periph(periph)
{
}


void I2c_lead_tx_channel::push_data(uint8_t *data, int size)
{
    assert(data != NULL);
    assert(size > 0);

    this->periph->lead_tx_fifo.push_data(data, size);
}


I2c_lead_rx_channel::I2c_lead_rx_channel(udma *top, I2c_periph *periph, string name)
    : Udma_rx_channel(top, name), periph(periph)
{
}

void I2c_lead_rx_channel::wait_active_done(void)
{
}

I2c_cmd_channel::I2c_cmd_channel(udma *top, I2c_periph *periph, string name)
    : Udma_tx_channel(top, name), periph(periph)
{
}


void I2c_cmd_channel::push_data(uint8_t *data, int size)
{
    assert(data != NULL);
    assert(size > 0);

    this->periph->push_cmd(data, size);
}

I2c_foll_tx_channel::I2c_foll_tx_channel(udma *top, I2c_periph *periph, string name)
    : Udma_tx_channel(top, name), periph(periph)
{
}


void I2c_foll_tx_channel::push_data(uint8_t *data, int size)
{
    assert(data != NULL);
    assert(size > 0);

    I2C_PERIPH_FPRINTF("[I2C][TX] pushing data (size: %d)\n", size);
}


I2c_foll_rx_channel::I2c_foll_rx_channel(udma *top, I2c_periph *periph, string name)
    : Udma_rx_channel(top, name), periph(periph)
{
}

void I2c_foll_rx_channel::wait_active_done(void)
{
}
