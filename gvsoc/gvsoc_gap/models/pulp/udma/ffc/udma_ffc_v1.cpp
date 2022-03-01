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
#include "udma_ffc_v1.hpp"

#include "archi/utils.h"
#include "udma_ffc_v1_model.hpp"

using namespace std::placeholders;

/* delay needed to replicate real performances */
/* this should be 1 because ffc is 1 data/cycle, but 2 is the closest value
 * to replicate performance */
#define FFC_DELAY_CYCLES (2)

Ffc_periph::Ffc_periph(udma *top, int id, int itf_id) : Udma_periph(top, id)
{
    std::string itf_name = "ffc" + std::to_string(itf_id);

    top->traces.new_trace(itf_name, &this->trace, vp::DEBUG);

    this->state = FFC_STATE_IDLE;

    /* Build the register map. It is auto-generated from the IP specs */
    this->regmap.build(top, &this->trace, itf_name);

    /* setup tx/rx channel register callbacks */
    this->regmap.rx_dest.register_callback(std::bind(&Ffc_periph::rx_dest_req, this, _1, _2, _3, _4));
    this->regmap.tx_dest.register_callback(std::bind(&Ffc_periph::tx_dest_req, this, _1, _2, _3, _4));

    /* setup udma channels */
    this->rx_channel = new Ffc_rx_channel(top, this, itf_name + "_rx");
    this->tx_channel = new Ffc_tx_channel(top, this, itf_name + "_tx");

    /* setup event handlers */
    this->event_convert = top->event_new(this, Ffc_periph::handle_event);

    /* Busy signal for VCD tracing */
    this->top->new_reg(itf_name + "/busy", &this->busy, 8);
}


void Ffc_periph::reset(bool active)
{
    Udma_periph::reset(active);

    this->rx_channel->reset(active);
    this->tx_channel->reset(active);

    // Since busy signal is displayed as a state, we need to release it when the FFC is not busy. */
    this->busy.release();
}


void Ffc_periph::rx_dest_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write)
{
    this->regmap.rx_dest.update(reg_offset, size, value, is_write);

    if (is_write)
    {
        this->top->channel_register(this->regmap.rx_dest.get(), this->rx_channel);
    }
}


void Ffc_periph::tx_dest_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write)
{
    this->regmap.tx_dest.update(reg_offset, size, value, is_write);

    if (is_write)
    {
        this->top->channel_register(this->regmap.tx_dest.get(), this->tx_channel);
    }

}


vp::io_req_status_e Ffc_periph::custom_req(vp::io_req *req, uint64_t offset)
{
    if (this->regmap.access(offset, req->get_size(), req->get_data(), req->get_is_write()))
    {
        return vp::IO_REQ_INVALID;
    }

    uint64_t reg_id = offset;
    switch(reg_id)
    {
        case UDMA_FFC_START_OFFSET:
            this->trace.msg(vp::trace::LEVEL_TRACE, "Received START access\n");
            /* start converting data */
            this->enqueue_event();
            this->busy.set(1);
            break;
        default:
            break;
    }

    return vp::IO_REQ_OK;
}

int Ffc_periph::compute_requested_size(void)
{
    int requested_size = 1;
    switch(this->regmap.mode.direction_get())
    {
        case 0: // floating point to fixed point
            switch(this->regmap.fl_format.get())
            {
                case FFC_FLOAT_BFP16: // fallthrough
                case FFC_FLOAT_FP16:
                    requested_size = 2;
                    break;
                case FFC_FLOAT_FP32: //fallthrough
                case FFC_FLOAT_FP32_BIS:
                    requested_size = 4;
                    break;
                default:
                    this->trace.msg(vp::trace::LEVEL_ERROR, "Invalid fl format: %d\n", this->regmap.fl_format.get());
                    break;
            }
            break;
        case 1: // fixed point to floating point
            switch(this->regmap.fp_format.get())
            {
                case FFC_FIXED_8:
                    requested_size = 1;
                    break;
                case FFC_FIXED_16:
                    requested_size = 2;
                    break;
                case FFC_FIXED_24: // fallthrough
                case FFC_FIXED_32:
                    requested_size = 4;
                    break;
                default:
                    this->trace.msg(vp::trace::LEVEL_ERROR, "Invalid fp format: %d\n", this->regmap.fp_format.get());
                    break;
            }
            break;
        default:
            this->trace.msg(vp::trace::LEVEL_ERROR, "Invalid direction %d\n", this->regmap.mode.direction_get());
            break;
    }
    return requested_size;
}

void Ffc_periph::handle_event(void* __this, vp::clock_event* event)
{
    Ffc_periph* _this = ((Ffc_periph*)__this);

    switch(_this->state)
    {
        case FFC_STATE_IDLE:

            _this->convert_num = _this->regmap.conv_num.get();
            /* pop a data from tx channel if possible */
            if (_this->convert_num > 0 && _this->regmap.start.get() == 1)
            {
                if (_this->tx_channel->is_ready())
                {
                    const int req_size = _this->compute_requested_size() * _this->convert_num;
                    _this->tx_channel->get_data(req_size);
                    _this->state = FFC_STATE_CONVERTING;
                }
                else
                {
                    /* could not get data, reenqueue event to retry */
                    _this->enqueue_event();

                    _this->state = FFC_STATE_IDLE;
                }
            }
            break;
        case FFC_STATE_CONVERTING:

            if (!_this->ffc_queue.empty())
            {
                /* iterate over queue to decrement cycle counter */
                for (Ffc_element& elt : _this->ffc_queue)
                {
                    if (elt.cycles > 0)
                    {
                        elt.cycles--;
                    }
                }

                /* push one data if possible */
                if (_this->rx_channel->is_ready())
                {
                    Ffc_element elt = _this->ffc_queue.front();
                    if (elt.cycles == 0)
                    {
                        _this->ffc_queue.pop_front();
                        _this->rx_channel->push_data((uint8_t*) &elt.data, elt.size);
                    }
                }
            }


            if (_this->convert_num == 0 && _this->ffc_queue.empty())
            {
                /* done with conversion */
                _this->state = FFC_STATE_IDLE;
                // Since busy signal is displayed as a state, we need to release it when the FFC is not busy. */
                _this->busy.release();
            }
            else if (!_this->ffc_queue.empty())
            {
                _this->enqueue_event();
            }

            break;
        default:
            break;
    }
}

void Ffc_periph::push_data(uint8_t* data, uint8_t size)
{
    uint32_t elt_data = *(reinterpret_cast<uint32_t*>(data));
    Ffc_element elt = {
        .size = size,
        .data = elt_data,
        .cycles = 6,
    };
    ffc_queue.push_back(elt);
}

Ffc_tx_channel::Ffc_tx_channel(udma *top, Ffc_periph *periph, string name)
    : Udma_tx_channel(top, name), periph(periph)
{
}

void Ffc_periph::convert_to_float(uint8_t* src,
        ffc_fixed_e fixed_type,
        ffc_float_e float_type,
        int precision,
        int scale)
{
    this->trace.msg(vp::trace::LEVEL_TRACE, "convert_to_float (fp: 0x%x, fl: 0x%x, precision: %d, scale %d)\n",
            fixed_type, float_type, precision, scale);
    switch(float_type)
    {
        case FFC_FLOAT_BFP16:
            {
                uint16_t dst = 0;
                switch(fixed_type)
                {
                    case FFC_FIXED_8:
                        dst = (uint16_t) FixedToFloatFormat(*((uint8_t*)src),
                                8,
                                precision,
                                scale,
                                1,
                                8,
                                7);
                        break;
                    case FFC_FIXED_16:
                        dst = (uint16_t) FixedToFloatFormat(*((uint16_t*)src),
                                16,
                                precision,
                                scale,
                                1,
                                8,
                                7);
                        break;
                    case FFC_FIXED_24:
                        dst = (uint16_t) FixedToFloatFormat(*((uint32_t*)src),
                                24,
                                precision,
                                scale,
                                1,
                                8,
                                7);
                        break;
                    case FFC_FIXED_32:
                        dst = (uint16_t) FixedToFloatFormat(*((uint32_t*)src),
                                32,
                                precision,
                                scale,
                                1,
                                8,
                                7);
                        break;
                    default:
                        this->trace.msg(vp::trace::LEVEL_ERROR, "Invalid fixed type 0x%x\n", fixed_type);
                        break;
                }
                this->push_data((uint8_t*) &dst, 2);
            }
            break;
        case FFC_FLOAT_FP16:
            {
                uint16_t dst = 0;
                switch(fixed_type)
                {
                    case FFC_FIXED_8:
                        dst = (uint16_t) FixedToFloatFormat(*((uint8_t*)src),
                                8,
                                precision,
                                scale,
                                1,
                                5,
                                10);
                        break;
                    case FFC_FIXED_16:
                        dst = (uint16_t) FixedToFloatFormat(*((uint16_t*)src),
                                16,
                                precision,
                                scale,
                                1,
                                5,
                                10);
                        break;
                    case FFC_FIXED_24:
                        dst = (uint16_t) FixedToFloatFormat(*((uint32_t*)src),
                                24,
                                precision,
                                scale,
                                1,
                                5,
                                10);
                        break;
                    case FFC_FIXED_32:
                        dst = (uint16_t) FixedToFloatFormat(*((uint32_t*)src),
                                32,
                                precision,
                                scale,
                                1,
                                5,
                                10);
                        break;
                    default:
                        this->trace.msg(vp::trace::LEVEL_ERROR, "Invalid fixed type 0x%x\n", fixed_type);
                        break;
                }
                this->push_data((uint8_t*) &dst, 2);
            }
            break;
        case FFC_FLOAT_FP32: //fallthrough
        case FFC_FLOAT_FP32_BIS:
            {
                uint32_t dst = 0;
                switch(fixed_type)
                {
                    case FFC_FIXED_8:
                        dst = FixedToFloatFormat(*((uint8_t*)src),
                                8,
                                precision,
                                scale,
                                1,
                                8,
                                23);
                        break;
                    case FFC_FIXED_16:
                        dst = FixedToFloatFormat(*((uint16_t*)src),
                                16,
                                precision,
                                scale,
                                1,
                                8,
                                23);
                        break;
                    case FFC_FIXED_24:
                        dst = FixedToFloatFormat(*((uint32_t*)src),
                                24,
                                precision,
                                scale,
                                1,
                                8,
                                23);
                        break;
                    case FFC_FIXED_32:
                        dst = FixedToFloatFormat(*((uint32_t*)src),
                                32,
                                precision,
                                scale,
                                1,
                                8,
                                23);
                        break;
                    default:
                        this->trace.msg(vp::trace::LEVEL_ERROR, "Invalid fixed type 0x%x\n", fixed_type);
                        break;
                }
                this->push_data((uint8_t*) &dst, 4);
            }
            break;
        default:
            this->trace.msg(vp::trace::LEVEL_ERROR, "Invalid float type 0x%x\n", float_type);
            break;
    }
}

void Ffc_periph::convert_to_fixed(uint8_t* src,
        ffc_fixed_e fixed_type,
        ffc_float_e float_type,
        int precision,
        int scale)
{
    this->trace.msg(vp::trace::LEVEL_TRACE, "convert_to_fixed (fp: 0x%x, fl: 0x%x, precision: %d, scale %d)\n",
            fixed_type, float_type, precision, scale);
    switch(fixed_type)
    {
        case FFC_FIXED_8:
            {
                uint8_t dst = 0;
                switch(float_type)
                {
                    case FFC_FLOAT_BFP16:
                        dst = FloatToFixedFormat(*((uint16_t*)src),
                                8,
                                precision,
                                scale,
                                1,
                                8,
                                7);
                        break;
                    case FFC_FLOAT_FP16:
                        {
                            dst = FloatToFixedFormat(*((uint16_t*)src),
                                    8,
                                    precision,
                                    scale,
                                    1,
                                    5,
                                    10);
                        }
                        break;
                    case FFC_FLOAT_FP32: //fallthrough
                    case FFC_FLOAT_FP32_BIS:
                        dst = FloatToFixedFormat(*((uint32_t*)src),
                                8,
                                precision,
                                scale,
                                1,
                                8,
                                23);
                        break;
                    default:
                        printf("Invalid float type\n");
                        break;
                }
                this->push_data(&dst, 1);
            }
            break;
        case FFC_FIXED_16:
            {
                uint16_t dst = 0;
                switch(float_type)
                {
                    case FFC_FLOAT_BFP16:
                        dst = FloatToFixedFormat(*((uint16_t*)src),
                                16,
                                precision,
                                scale,
                                1,
                                8,
                                7);
                        break;
                    case FFC_FLOAT_FP16:
                        dst = FloatToFixedFormat(*((uint16_t*)src),
                                16,
                                precision,
                                scale,
                                1,
                                5,
                                10);
                        break;
                    case FFC_FLOAT_FP32: //fallthrough
                    case FFC_FLOAT_FP32_BIS:
                        dst = FloatToFixedFormat(*((uint32_t*)src),
                                16,
                                precision,
                                scale,
                                1,
                                8,
                                23);
                        break;
                    default:
                        printf("Invalid float type\n");
                        break;
                }
                this->push_data((uint8_t*) &dst, 2);
            }
            break;
        case FFC_FIXED_24:
            {
                uint32_t dst = 0;
                switch(float_type)
                {
                    case FFC_FLOAT_BFP16:
                        dst = FloatToFixedFormat(*((uint16_t*)src),
                                24,
                                precision,
                                scale,
                                1,
                                8,
                                7);
                        break;
                    case FFC_FLOAT_FP16:
                        dst = FloatToFixedFormat(*((uint16_t*)src),
                                24,
                                precision,
                                scale,
                                1,
                                5,
                                10);
                        break;
                    case FFC_FLOAT_FP32: //fallthrough
                    case FFC_FLOAT_FP32_BIS:
                        dst = FloatToFixedFormat(*((uint32_t*)src),
                                24,
                                precision,
                                scale,
                                1,
                                8,
                                23);
                        break;
                    default:
                        printf("Invalid float type\n");
                        break;
                }
                /* sign extension */
                if (dst > 0x800000)
                {
                    dst = dst | 0xFF000000;
                }
                this->push_data((uint8_t*) &dst, 4);
            }
            break;
        case FFC_FIXED_32:
            {
                uint32_t dst = 0;
                switch(float_type)
                {
                    case FFC_FLOAT_BFP16:
                        dst = FloatToFixedFormat(*((uint16_t*)src),
                                32,
                                precision,
                                scale,
                                1,
                                8,
                                7);
                        break;
                    case FFC_FLOAT_FP16:
                        dst = FloatToFixedFormat(*((uint16_t*)src),
                                32,
                                precision,
                                scale,
                                1,
                                5,
                                10);
                        break;
                    case FFC_FLOAT_FP32: //fallthrough
                    case FFC_FLOAT_FP32_BIS:
                        dst = FloatToFixedFormat(*((uint32_t*)src),
                                32,
                                precision,
                                scale,
                                1,
                                8,
                                23);
                        break;
                    default:
                        printf("Invalid float type\n");
                        break;
                }

                this->push_data((uint8_t*) &dst, 4);
            }
            break;
        default:
            printf("Invalid fixed type\n");
            break;
    }
}

void Ffc_periph::enqueue_event(void)
{
    if (!(this->event_convert)->is_enqueued())
    {
        this->top->event_enqueue(this->event_convert, FFC_DELAY_CYCLES);
    }
}

void Ffc_periph::process_data(uint8_t* data, int size)
{
    int req_size = this-> compute_requested_size();
    /* retrieve incoming data and convert to correct format */
    for (int i = 0; i < (size / req_size); i++)
    {
        this->convert_num--;
        switch(this->regmap.mode.direction_get())
        {
            case 0: // float to fixed point
                this->convert_to_fixed(data + i * req_size,
                        (ffc_fixed_e) this->regmap.fp_format.get(),
                        (ffc_float_e) this->regmap.fl_format.get(),
                        this->regmap.fp_prec.get(),
                        this->regmap.fp_scale.get());
                break;
            case 1: // fixed point to floating point
                this->convert_to_float(data + i * req_size,
                        (ffc_fixed_e) this->regmap.fp_format.get(),
                        (ffc_float_e) this->regmap.fl_format.get(),
                        this->regmap.fp_prec.get(),
                        this->regmap.fp_scale.get());
                break;
            default:
                this->trace.msg(vp::trace::LEVEL_ERROR, "Invalid direction %d\n", this->regmap.mode.direction_get());
                break;
        }
    }

    /* enqueue event because we received a data */
    this->enqueue_event();
}

void Ffc_tx_channel::push_data(uint8_t *data, int size)
{
    this->periph->process_data(data, size);
}

Ffc_rx_channel::Ffc_rx_channel(udma *top, Ffc_periph *periph, string name) : Udma_rx_channel(top, name), periph(periph)
{
}
