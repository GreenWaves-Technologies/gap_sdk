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
#include "udma_aes_v1.hpp"

#include "archi/utils.h"

using namespace std::placeholders;

#define AES_PROCESSING_DELAY_CYCLES_128 (380)
#define AES_PROCESSING_DELAY_CYCLES_256 (560)
#define AES_SENDING_DELAY_CYCLES        ( 1)


Aes_periph::Aes_periph(udma *top, int id, int itf_id) : Udma_periph(top, id)
{
    std::string itf_name = "aes" + std::to_string(itf_id);

    top->traces.new_trace(itf_name, &this->trace, vp::DEBUG);

    /* Build the register map. It is auto-generated from the IP specs */
    this->regmap.build(top, &this->trace, itf_name);

    /* setup register callbacks */
    this->regmap.dest.register_callback(std::bind(&Aes_periph::dest_req, this, _1, _2, _3, _4));
    this->regmap.setup.register_callback(std::bind(&Aes_periph::setup_req, this, _1, _2, _3, _4));

    /* setup udma channels */
    this->rx_channel = new Aes_rx_channel(top, this, itf_name + "_rx");
    this->tx_channel = new Aes_tx_channel(top, this, itf_name + "_tx");

    /* setup event handlers */
    this->processing_event = top->event_new(this, Aes_periph::processing_handler);
    this->sending_event = top->event_new(this, Aes_periph::sending_handler);
    this->key_init_event = top->event_new(this, Aes_periph::key_init_handler);
}


void Aes_periph::reset(bool active)
{
    Udma_periph::reset(active);

    this->rx_channel->reset(active);
    this->tx_channel->reset(active);
}


void Aes_periph::dest_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write)
{
    this->regmap.dest.update(reg_offset, size, value, is_write);

    this->trace.msg(vp::trace::LEVEL_TRACE, "Set DEST channels\n");

    /* register channels */
    if (is_write)
    {
        this->top->channel_register(this->regmap.dest.rx_dest_get(), this->rx_channel);
        this->top->channel_register(this->regmap.dest.tx_dest_get(), this->tx_channel);

        if (this->tx_channel->requested_size < 16)
        {
            this->tx_channel->get_data(AES_BLOCK_SIZE_BYTES);
        }
    }
}

void Aes_periph::setup_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write)
{
    this->regmap.setup.update(reg_offset, size, value, is_write);
    this->trace.msg(vp::trace::LEVEL_TRACE, "%s:%d - func: %s\n",  __FILE__, __LINE__, __func__);

    if (is_write)
    {
        /* special case for key_init (operation should takes some time) */
        if (UDMA_AES_SETUP_KEY_INIT_MASK & value[0])
        {
            this->regmap.setup.key_init_set(0);
            if (!(this->key_init_event)->is_enqueued())
            {
                /* vary number of cycles depending on key_len */
                /* roughly the same number than processing */
                if (this->regmap.setup.key_type_get() == 0)
                {
                    /* 128 bits */
                    this->top->get_periph_clock()->enqueue(this->key_init_event, AES_PROCESSING_DELAY_CYCLES_128);
                }
                else
                {
                    /* 256 bits */
                    this->top->get_periph_clock()->enqueue(this->key_init_event, AES_PROCESSING_DELAY_CYCLES_256);
                }
            }
        }


        /* special case for block_rst */

        if (UDMA_AES_SETUP_BLOCK_RST_MASK & value[0])
        {
            uint8_t iv[AES_BLOCK_SIZE_BYTES] = { 0 };
            ((uint32_t*) iv)[0] = this->regmap.iv0_0.get();
            ((uint32_t*) iv)[1] = this->regmap.iv0_1.get();
            ((uint32_t*) iv)[2] = this->regmap.iv0_2.get();
            ((uint32_t*) iv)[3] = this->regmap.iv0_3.get();
            AES_ctx_set_iv(&this->aes_ctx, iv);
        }
    }
}

vp::io_req_status_e Aes_periph::custom_req(vp::io_req *req, uint64_t offset)
{
    this->trace.msg(vp::trace::LEVEL_TRACE, "aes_register_req\n");
    if (this->regmap.access(offset, req->get_size(), req->get_data(), req->get_is_write()))
    {
        return vp::IO_REQ_INVALID;
    }

    return vp::IO_REQ_OK;
}

void Aes_periph::process_data(uint8_t* data, int size)
{
    this->trace.msg(vp::trace::LEVEL_TRACE, "%s:%d - func: %s\n",  __FILE__, __LINE__, __func__);
    /* fill incoming data */
    for (int i = 0; i < size; i++)
    {
        this->incoming_data.push(data[i]);
    }

    /* launch encryption/decryption */
    if (this->incoming_data.size() >= AES_BLOCK_SIZE_BYTES)
    {
        /* can begin to process a block */
        for (int i = 0; i < AES_BLOCK_SIZE_BYTES; i++)
        {
            current_block[i] = incoming_data.front();
            incoming_data.pop();
        }

        /* send processing event */
        if (!(this->processing_event)->is_enqueued())
        {
            /* vary number of cycles depending on key_len */
            if (this->regmap.setup.key_type_get() == 0)
            {
                /* 128 bits */
                this->top->get_periph_clock()->enqueue(this->processing_event, AES_PROCESSING_DELAY_CYCLES_128);
            }
            else
            {
                /* 256 bits */
                this->top->get_periph_clock()->enqueue(this->processing_event, AES_PROCESSING_DELAY_CYCLES_256);
            }
        }
    }
}

void Aes_periph::processing_handler(void* __this, vp::clock_event* event)
{
    Aes_periph* _this = ((Aes_periph*)__this);
    _this->trace.msg(vp::trace::LEVEL_TRACE, "%s:%d - func: %s\n",  __FILE__, __LINE__, __func__);

    /* encryption/decryption */

    if (_this->regmap.setup.enc_dec_get() == 1) /* encrypt */
    {
        if (_this->regmap.setup.ecb_cbc_get() == 0) /* ecb */
        {
            AES_ECB_encrypt(&_this->aes_ctx, _this->current_block);
        }
        else /* cbc */
        {
            AES_CBC_encrypt_buffer(&_this->aes_ctx, _this->current_block, AES_BLOCK_SIZE_BYTES);
        }
    }
    else /* decrypt */
    {
        if (_this->regmap.setup.ecb_cbc_get() == 0) /* ecb */
        {
            AES_ECB_decrypt(&_this->aes_ctx, _this->current_block);
        }
        else /* cbc */
        {
            AES_CBC_decrypt_buffer(&_this->aes_ctx, _this->current_block, AES_BLOCK_SIZE_BYTES);
        }
    }

    /* fill outgoing data and previous block*/
    for (int i = 0; i < AES_BLOCK_SIZE_BYTES; i++)
    {
        _this->outgoing_data.push(_this->current_block[i]);
        _this->previous_block[i] = _this->current_block[i];
    }

    /* send sending event */
    if (!(_this->sending_event)->is_enqueued())
    {
        _this->top->get_periph_clock()->enqueue(_this->sending_event, AES_SENDING_DELAY_CYCLES);
    }
}

void Aes_periph::sending_handler(void* __this, vp::clock_event* event)
{
    Aes_periph* _this = ((Aes_periph*)__this);
    _this->trace.msg(vp::trace::LEVEL_TRACE, "%s:%d - func: %s\n",  __FILE__, __LINE__, __func__);

    /* push outgoing data to rx_channel if possible */
    while(!_this->outgoing_data.empty() && _this->rx_channel->is_ready())
    {
        uint8_t data = _this->outgoing_data.front();
        _this->outgoing_data.pop();
        _this->rx_channel->push_data(&data, 1);
    }

    if (!_this->outgoing_data.empty())
    {
        /* not all data could be sent, reenqueue send event */
        if (!(_this->sending_event)->is_enqueued())
        {
            _this->top->get_periph_clock()->enqueue(_this->sending_event, AES_SENDING_DELAY_CYCLES);
        }
    }
    else
    {
        /* request new data to process */
        _this->tx_channel->get_data(AES_BLOCK_SIZE_BYTES);
    }
}

void Aes_periph::key_init_handler(void* __this, vp::clock_event* event)
{
    Aes_periph* _this = ((Aes_periph*)__this);
    _this->trace.msg(vp::trace::LEVEL_TRACE, "%s:%d - func: %s\n",  __FILE__, __LINE__, __func__);

    /* notify that key_init has been done */
    _this->regmap.setup.key_init_set(1);

    /* init key */
    uint8_t key[AES_BLOCK_SIZE_BYTES * 2] = {0};

    ((uint32_t*)key)[0] = _this->regmap.key0_0.get();
    ((uint32_t*)key)[1] = _this->regmap.key0_1.get();
    ((uint32_t*)key)[2] = _this->regmap.key0_2.get();
    ((uint32_t*)key)[3] = _this->regmap.key0_3.get();
    ((uint32_t*)key)[4] = _this->regmap.key0_4.get();
    ((uint32_t*)key)[5] = _this->regmap.key0_5.get();
    ((uint32_t*)key)[6] = _this->regmap.key0_6.get();
    ((uint32_t*)key)[7] = _this->regmap.key0_7.get();

    aes_keylen_e keylen = (_this->regmap.setup.key_type_get() == 0) ? AES_KEY_128 : AES_KEY_256;

    AES_init_ctx(&_this->aes_ctx, keylen, key);
}

Aes_tx_channel::Aes_tx_channel(udma *top, Aes_periph *periph, string name)
    : Udma_tx_channel(top, name), periph(periph)
{
}

void Aes_tx_channel::push_data(uint8_t *data, int size)
{
    this->periph->process_data(data, size);
}


Aes_rx_channel::Aes_rx_channel(udma *top, Aes_periph *periph, string name) : Udma_rx_channel(top, name), periph(periph)
{
}
