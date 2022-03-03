/*
 * Copyright (C) 2020 GreenWaves Technologies
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/* 
 * Authors: Germain Haugou, GreenWaves Technologies (germain.haugou@greenwaves-technologies.com)
 */


#include <vp/vp.hpp>
#include <vp/itf/i2c.hpp>



typedef enum
{
  I2C_STATE_WAIT_START,
  I2C_STATE_WAIT_ADDRESS,
  I2C_STATE_GET_DATA,
  I2C_STATE_SAMPLE_DATA,
  I2C_STATE_ACK,
  I2C_STATE_READ_ACK
} I2c_state_e;


class Ak4332 : public vp::component
{
public:
    Ak4332(js::config *config);

    int build();

protected:
    static void i2c_sync(void *__this, int scl, int sda);
    void i2c_start(unsigned int address, bool is_read);
    void i2c_handle_byte(uint8_t byte);
    void i2c_stop();
    void i2c_get_data();
    void i2c_send_byte(uint8_t byte);

    void handle_reg_write(uint8_t address, uint8_t value);
    uint8_t handle_reg_read(uint8_t address);

    void start();

    vp::trace trace;
    vp::i2c_master i2c_itf;

    unsigned int device_address;

    bool i2c_being_addressed;
    unsigned int i2c_address;
    uint8_t i2c_pending_data;
    bool i2c_is_read;
    I2c_state_e i2c_state;
    int i2c_pending_bits;
    int i2c_prev_sda;
    int i2c_prev_scl;
    unsigned int i2c_pending_send_byte;
    uint8_t reg_address;
    bool waiting_reg_address;
    uint8_t power_1;
    uint8_t power_2;
    uint8_t power_3;
    uint8_t power_4;
    uint8_t output_mode;
    uint8_t clock_mode;
    uint8_t digital_filter;
    uint8_t dac_mono_mixing;
    uint8_t pdm_control;
    uint8_t dac_volume_control;
    uint8_t hp_volume_control;
    uint8_t pll_clk_selection;
    uint8_t pll_ref_clk_div_1;
    uint8_t pll_ref_clk_div_2;
    uint8_t pll_fb_clk_div_1;
    uint8_t pll_fb_clk_div_2;
    uint8_t dac_clk_source;
    uint8_t dac_clk_divider;
    uint8_t audio_format;
    uint8_t pdm_err;
    uint8_t dac_adjustment_1;
    uint8_t dac_adjustment_2;
};


Ak4332::Ak4332(js::config *config)
    : vp::component(config)
{
}


void Ak4332::start()
{
    this->i2c_itf.sync(1, 1);
}


void Ak4332::i2c_sync(void *__this, int scl, int sda)
{
    Ak4332 *_this = (Ak4332 *)__this;

    _this->trace.msg(vp::trace::LEVEL_TRACE, "I2C sync (scl: %d, sda: %d)\n", scl, sda);

    int sdo = 1;

    if (scl == 1 && _this->i2c_prev_sda != sda)
    {
        if (_this->i2c_prev_sda == 1)
        {
            _this->trace.msg(vp::trace::LEVEL_TRACE, "Detected start\n");

            _this->i2c_state = I2C_STATE_WAIT_ADDRESS;
            _this->i2c_address = 0;
            _this->i2c_pending_bits = 8;
        }
        else
        {
            _this->i2c_state = I2C_STATE_WAIT_START;
            _this->i2c_stop();
        }
        goto end;
    }

    if (!_this->i2c_prev_scl && scl)
    {
        switch (_this->i2c_state)
        {
            case I2C_STATE_WAIT_START:
            {
                sdo = 1;
                break;
            }

            case I2C_STATE_WAIT_ADDRESS:
            {
                if (_this->i2c_pending_bits > 1)
                {
                    _this->i2c_address = (_this->i2c_address << 1) | sda;
                    _this->trace.msg(vp::trace::LEVEL_TRACE, "Received address bit (bit: %d, address: 0x%x, pending_bits: %d)\n", sda, _this->i2c_address, _this->i2c_pending_bits);
                }
                else
                {
                    _this->i2c_is_read = sda;
                }
                _this->i2c_pending_bits--;
                if (_this->i2c_pending_bits == 0)
                {
                    _this->i2c_start(_this->i2c_address, _this->i2c_is_read);
                    _this->i2c_state = I2C_STATE_ACK;
                    _this->i2c_pending_bits = 8;
                }
                break;
            }

            case I2C_STATE_SAMPLE_DATA:
            {
                _this->i2c_pending_data = (_this->i2c_pending_data << 1) | sda;
                _this->trace.msg(vp::trace::LEVEL_TRACE, "Sampling data (bit: %d, pending_value: 0x%x, pending_bits: %d)\n", sda, _this->i2c_pending_data, _this->i2c_pending_bits);
                _this->i2c_pending_bits--;
                if (_this->i2c_pending_bits == 0)
                {
                    _this->i2c_pending_bits = 8;
                    _this->i2c_handle_byte(_this->i2c_pending_data);
                    _this->i2c_state = I2C_STATE_ACK;
                }
                break;
            }

            case I2C_STATE_ACK: {
                _this->trace.msg(vp::trace::LEVEL_TRACE, "Ack (being_addressed: %d)\n", _this->i2c_being_addressed);
                if (_this->i2c_being_addressed)
                {
                    if (_this->i2c_is_read)
                    {
                        _this->i2c_state = I2C_STATE_GET_DATA;
                        _this->i2c_pending_bits = 8;
                        _this->i2c_get_data();
                    }
                    else
                    {
                        _this->i2c_state = I2C_STATE_SAMPLE_DATA;
                    }
                }
                else
                {
                    _this->i2c_state = I2C_STATE_WAIT_START;
                }

                break;
            }

            case I2C_STATE_READ_ACK: {
                _this->i2c_state = I2C_STATE_WAIT_START;
                break;
            }
        }
    }

    if (_this->i2c_prev_scl && !scl)
    {
        switch (_this->i2c_state)
        {
            case I2C_STATE_ACK:
            {
                _this->trace.msg(vp::trace::LEVEL_TRACE, "Ack (being_addressed: %d)\n", _this->i2c_being_addressed);
                sdo = !_this->i2c_being_addressed;
                break;
            }

            case I2C_STATE_READ_ACK:
            {
                _this->trace.msg(vp::trace::LEVEL_TRACE, "Read ack\n");
                sdo = 0;
                break;
            }

            case I2C_STATE_GET_DATA:
            {
                sdo = (_this->i2c_pending_send_byte >> 7) & 1;
                _this->trace.msg(vp::trace::LEVEL_TRACE, "Sending bit (bit: %d, pending_value: 0x%x, pending_bits: %d)\n", sdo, _this->i2c_pending_send_byte, _this->i2c_pending_bits);
                _this->i2c_pending_send_byte <<= 1;
                _this->i2c_pending_bits--;
                if (_this->i2c_pending_bits == 0)
                {
                    _this->i2c_state = I2C_STATE_READ_ACK;
                }
                break;
            }
        }
    }

end:
    if (_this->i2c_prev_scl && !scl)
    {
        _this->trace.msg(vp::trace::LEVEL_TRACE, "Sync sda (value: %d)\n", sdo);
        _this->i2c_itf.sync(1, sdo);
    }
    _this->i2c_prev_sda = sda;
    _this->i2c_prev_scl = scl;
}

void Ak4332::i2c_start(unsigned int address, bool is_read)
{
    this->trace.msg(vp::trace::LEVEL_TRACE, "Received header (address: 0x%x, is_read: %d)\n", address, is_read);

    this->i2c_being_addressed = address == this->device_address;
    if (is_read)
    {
        this->i2c_send_byte(this->handle_reg_read(this->reg_address));
    }
}

void Ak4332::i2c_handle_byte(uint8_t byte)
{
    this->trace.msg(vp::trace::LEVEL_TRACE, "Handle byte (value: 0x%x)\n", byte);

    if (this->waiting_reg_address)
    {
        this->reg_address = byte;
        this->waiting_reg_address = false;
    }
    else
    {
        this->handle_reg_write(this->reg_address, byte);
        this->waiting_reg_address = true;
    }
}

void Ak4332::i2c_stop()
{
    this->trace.msg(vp::trace::LEVEL_TRACE, "Received stop bit\n");

}

void Ak4332::i2c_get_data()
{
    this->trace.msg(vp::trace::LEVEL_TRACE, "Getting data\n");
}

void Ak4332::i2c_send_byte(uint8_t byte)
{
  this->i2c_pending_send_byte = byte;
}


void Ak4332::handle_reg_write(uint8_t address, uint8_t value)
{
    this->trace.msg(vp::trace::LEVEL_TRACE, "Register write (address: 0x%x, value: 0x%x)\n", address, value);

    switch (address)
    {
        case 0x00:
        {
            this->trace.msg(vp::trace::LEVEL_INFO, "Writing register (name: %s, value: 0x%x)\n", "Power Management 1", value);
            this->power_1 = value;
            break;
        }
        case 0x01:
        {
            this->trace.msg(vp::trace::LEVEL_INFO, "Writing register (name: %s, value: 0x%x)\n", "Power Management 2", value);
            this->power_2 = value;
            break;
        }
        case 0x02:
        {
            this->trace.msg(vp::trace::LEVEL_INFO, "Writing register (name: %s, value: 0x%x)\n", "Power Management 3", value);
            this->power_3 = value;
            break;
        }
        case 0x03:
        {
            this->trace.msg(vp::trace::LEVEL_INFO, "Writing register (name: %s, value: 0x%x)\n", "Power Management 4", value);
            this->power_4 = value;
            break;
        }
        case 0x04:
        {
            this->trace.msg(vp::trace::LEVEL_INFO, "Writing register (name: %s, value: 0x%x)\n", "Output Mode Setting", value);
            this->output_mode = value;
            break;
        }
        case 0x05:
        {
            this->trace.msg(vp::trace::LEVEL_INFO, "Writing register (name: %s, value: 0x%x)\n", "Clock Mode Selection", value);
            this->clock_mode = value;
            break;
        }
        case 0x06:
        {
            this->trace.msg(vp::trace::LEVEL_INFO, "Writing register (name: %s, value: 0x%x)\n", "Digital Filter Selection", value);
            this->digital_filter = value;
            break;
        }
        case 0x07:
        {
            this->trace.msg(vp::trace::LEVEL_INFO, "Writing register (name: %s, value: 0x%x)\n", "DAC Mono Mixing", value);
            this->dac_mono_mixing = value;
            break;
        }
        case 0x08:
        {
            this->trace.msg(vp::trace::LEVEL_INFO, "Writing register (name: %s, value: 0x%x)\n", "PDM I/F Control", value);
            this->pdm_control = value;
            break;
        }
        case 0x0B:
        {
            this->trace.msg(vp::trace::LEVEL_INFO, "Writing register (name: %s, value: 0x%x)\n", "DAC Output Volume", value);
            this->dac_volume_control = value;
            break;
        }
        case 0x0D:
        {
            this->trace.msg(vp::trace::LEVEL_INFO, "Writing register (name: %s, value: 0x%x)\n", "HP Volume Control", value);
            this->hp_volume_control = value;
            break;
        }
        case 0x0E:
        {
            this->trace.msg(vp::trace::LEVEL_INFO, "Writing register (name: %s, value: 0x%x)\n", "PLL CLK Source Selection", value);
            this->pll_clk_selection = value;
            break;
        }
        case 0x0F:
        {
            this->trace.msg(vp::trace::LEVEL_INFO, "Writing register (name: %s, value: 0x%x)\n", "PLL Ref CLK Divider 1", value);
            this->pll_ref_clk_div_1 = value;
            break;
        }
        case 0x10:
        {
            this->trace.msg(vp::trace::LEVEL_INFO, "Writing register (name: %s, value: 0x%x)\n", "PLL Ref CLK Divider 2", value);
            this->pll_ref_clk_div_2 = value;
            break;
        }
        case 0x11:
        {
            this->trace.msg(vp::trace::LEVEL_INFO, "Writing register (name: %s, value: 0x%x)\n", "PLL FB CLK Divider 1", value);
            this->pll_fb_clk_div_1 = value;
            break;
        }
        case 0x12:
        {
            this->trace.msg(vp::trace::LEVEL_INFO, "Writing register (name: %s, value: 0x%x)\n", "PLL FB CLK Divider 2", value);
            this->pll_fb_clk_div_2 = value;
            break;
        }
        case 0x13:
        {
            this->trace.msg(vp::trace::LEVEL_INFO, "Writing register (name: %s, value: 0x%x)\n", "DAC CLK Source", value);
            this->dac_clk_source = value;
            break;
        }
        case 0x14:
        {
            this->trace.msg(vp::trace::LEVEL_INFO, "Writing register (name: %s, value: 0x%x)\n", "DAC CLK Divider", value);
            this->dac_clk_divider = value;
            break;
        }
        case 0x15:
        {
            this->trace.msg(vp::trace::LEVEL_INFO, "Writing register (name: %s, value: 0x%x)\n", "Audio I/F Format", value);
            this->audio_format = value;
            break;
        }
        case 0x17:
        {
            this->trace.msg(vp::trace::LEVEL_INFO, "Writing register (name: %s, value: 0x%x)\n", "PDMERR", value);
            this->pdm_err = value;
            break;
        }
        case 0x26:
        {
            this->trace.msg(vp::trace::LEVEL_INFO, "Writing register (name: %s, value: 0x%x)\n", "DAC Adjustment 1", value);
            this->dac_adjustment_1 = value;
            break;
        }
        case 0x27:
        {
            this->trace.msg(vp::trace::LEVEL_INFO, "Writing register (name: %s, value: 0x%x)\n", "DAC Adjustment 2", value);
            this->dac_adjustment_2 = value;
            break;
        }
        default:
            this->trace.force_warning("Writing invalid register (address: 0x%x)\n", address);
            break;
    }

}


uint8_t Ak4332::handle_reg_read(uint8_t address)
{
    this->trace.msg(vp::trace::LEVEL_DEBUG, "Register read (address: 0x%x)\n", address);

    uint8_t value = 0xFF;

    switch (address)
    {
        case 0x00:
        {
            value = this->power_1;
            this->trace.msg(vp::trace::LEVEL_INFO, "Reading register (name: %s, value: 0x%x)\n", "Power Management 1", value);
            break;
        }
        case 0x01:
        {
            value = this->power_2;
            this->trace.msg(vp::trace::LEVEL_INFO, "Reading register (name: %s, value: 0x%x)\n", "Power Management 2", value);
            break;
        }
        case 0x02:
        {
            value = this->power_3;
            this->trace.msg(vp::trace::LEVEL_INFO, "Reading register (name: %s, value: 0x%x)\n", "Power Management 3", value);
            break;
        }
        case 0x03:
        {
            value = this->power_4;
            this->trace.msg(vp::trace::LEVEL_INFO, "Reading register (name: %s, value: 0x%x)\n", "Power Management 4", value);
            break;
        }
        case 0x04:
        {
            value = this->output_mode;
            this->trace.msg(vp::trace::LEVEL_INFO, "Reading register (name: %s, value: 0x%x)\n", "Output Mode Setting", value);
            break;
        }
        case 0x05:
        {
            value = this->clock_mode;
            this->trace.msg(vp::trace::LEVEL_INFO, "Reading register (name: %s, value: 0x%x)\n", "Clock Mode Selection", value);
            break;
        }
        case 0x06:
        {
            value = this->digital_filter;
            this->trace.msg(vp::trace::LEVEL_INFO, "Reading register (name: %s, value: 0x%x)\n", "Digital Filter Selection", value);
            break;
        }
        case 0x07:
        {
            value = this->dac_mono_mixing;
            this->trace.msg(vp::trace::LEVEL_INFO, "Reading register (name: %s, value: 0x%x)\n", "DAC Mono Mixing", value);
            break;
        }
        case 0x08:
        {
            value = this->pdm_control;
            this->trace.msg(vp::trace::LEVEL_INFO, "Reading register (name: %s, value: 0x%x)\n", "PDM I/F Control", value);
            break;
        }
        case 0x0B:
        {
            value = this->dac_volume_control;
            this->trace.msg(vp::trace::LEVEL_INFO, "Reading register (name: %s, value: 0x%x)\n", "DAC Output Volume", value);
            break;
        }
        case 0x0D:
        {
            value = this->hp_volume_control;
            this->trace.msg(vp::trace::LEVEL_INFO, "Reading register (name: %s, value: 0x%x)\n", "HP Volume Control", value);
            break;
        }
        case 0x0E:
        {
            value = this->pll_clk_selection;
            this->trace.msg(vp::trace::LEVEL_INFO, "Reading register (name: %s, value: 0x%x)\n", "PLL CLK Source Selection", value);
            break;
        }
        case 0x0F:
        {
            value = this->pll_ref_clk_div_1;
            this->trace.msg(vp::trace::LEVEL_INFO, "Reading register (name: %s, value: 0x%x)\n", "PLL Ref CLK Divider 1", value);
            break;
        }
        case 0x10:
        {
            value = this->pll_ref_clk_div_2;
            this->trace.msg(vp::trace::LEVEL_INFO, "Reading register (name: %s, value: 0x%x)\n", "PLL Ref CLK Divider 2", value);
            break;
        }
        case 0x11:
        {
            value = this->pll_fb_clk_div_1;
            this->trace.msg(vp::trace::LEVEL_INFO, "Reading register (name: %s, value: 0x%x)\n", "PLL FB CLK Divider 1", value);
            break;
        }
        case 0x12:
        {
            value = this->pll_fb_clk_div_2;
            this->trace.msg(vp::trace::LEVEL_INFO, "Reading register (name: %s, value: 0x%x)\n", "PLL FB CLK Divider 2", value);
            break;
        }
        case 0x13:
        {
            value = this->dac_clk_source;
            this->trace.msg(vp::trace::LEVEL_INFO, "Reading register (name: %s, value: 0x%x)\n", "DAC CLK Source", value);
            break;
        }
        case 0x14:
        {
            value = this->dac_clk_divider;
            this->trace.msg(vp::trace::LEVEL_INFO, "Reading register (name: %s, value: 0x%x)\n", "DAC CLK Divider", value);
            break;
        }
        case 0x15:
        {
            value = this->audio_format;
            this->trace.msg(vp::trace::LEVEL_INFO, "Reading register (name: %s, value: 0x%x)\n", "Audio I/F Format", value);
            break;
        }
        case 0x17:
        {
            value = this->pdm_err;
            this->trace.msg(vp::trace::LEVEL_INFO, "Reading register (name: %s, value: 0x%x)\n", "PDMERR", value);
            break;
        }
        case 0x26:
        {
            value = this->dac_adjustment_1;
            this->trace.msg(vp::trace::LEVEL_INFO, "Reading register (name: %s, value: 0x%x)\n", "DAC Adjustment 1", value);
            break;
        }
        case 0x27:
        {
            value = this->dac_adjustment_2;
            this->trace.msg(vp::trace::LEVEL_INFO, "Reading register (name: %s, value: 0x%x)\n", "DAC Adjustment 2", value);
            break;
        }
        default:
            this->trace.force_warning("Reading invalid register (address: 0x%x)\n", address);
            break;
    }

    return value;
}


int Ak4332::build()
{
    traces.new_trace("trace", &trace, vp::DEBUG);

    this->i2c_itf.set_sync_meth(&Ak4332::i2c_sync);
    this->new_master_port("i2c", &this->i2c_itf);

    this->i2c_state = I2C_STATE_WAIT_START;
    this->i2c_prev_sda = 1;
    this->i2c_prev_scl = 1;
    this->i2c_being_addressed = false;
    this->device_address = 0x10;
    this->waiting_reg_address = true;

    this->power_1              = 0x00;
    this->power_2              = 0x00;
    this->power_3              = 0x00;
    this->power_4              = 0x00;
    this->output_mode          = 0x00;
    this->clock_mode           = 0x00;
    this->digital_filter       = 0x00;
    this->dac_mono_mixing      = 0x00;
    this->pdm_control          = 0x00;
    this->dac_volume_control   = 0x19;
    this->hp_volume_control    = 0x65;
    this->pll_clk_selection    = 0x00;
    this->pll_ref_clk_div_1    = 0x00;
    this->pll_ref_clk_div_2    = 0x00;
    this->pll_fb_clk_div_1     = 0x00;
    this->pll_fb_clk_div_2     = 0x00;
    this->dac_clk_source       = 0x00;
    this->dac_clk_divider      = 0x00;
    this->audio_format         = 0x00;
    this->pdm_err              = 0x00;
    this->dac_adjustment_1     = 0x6C;
    this->dac_adjustment_2     = 0x40;

    return 0;
}


extern "C" vp::component *vp_constructor(js::config *config)
{
    return new Ak4332(config);
}
