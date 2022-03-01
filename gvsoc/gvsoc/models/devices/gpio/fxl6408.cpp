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


class Fxl6408 : public vp::component
{
public:
    Fxl6408(js::config *config);

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

    uint8_t device_id;
    uint8_t io_dir;
    uint8_t output_state;
    uint8_t output_high_z;
    uint8_t input_default_state;
    uint8_t pull_enable;
    uint8_t pull_down_up;
    uint8_t input_status;
    uint8_t interrupt_mask;
    uint8_t interrupt_status;
};


Fxl6408::Fxl6408(js::config *config)
    : vp::component(config)
{
}


void Fxl6408::start()
{
    this->i2c_itf.sync(1, 1);
}


void Fxl6408::i2c_sync(void *__this, int scl, int sda)
{
    Fxl6408 *_this = (Fxl6408 *)__this;

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

void Fxl6408::i2c_start(unsigned int address, bool is_read)
{
    this->trace.msg(vp::trace::LEVEL_TRACE, "Received header (address: 0x%x, is_read: %d)\n", address, is_read);

    this->i2c_being_addressed = address == this->device_address;
    if (this->i2c_being_addressed && is_read)
    {
        this->i2c_send_byte(this->handle_reg_read(this->reg_address));
    }
}

void Fxl6408::i2c_handle_byte(uint8_t byte)
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

void Fxl6408::i2c_stop()
{
    this->trace.msg(vp::trace::LEVEL_TRACE, "Received stop bit\n");

}

void Fxl6408::i2c_get_data()
{
    this->trace.msg(vp::trace::LEVEL_TRACE, "Getting data\n");
}

void Fxl6408::i2c_send_byte(uint8_t byte)
{
  this->i2c_pending_send_byte = byte;
}


void Fxl6408::handle_reg_write(uint8_t address, uint8_t value)
{
    this->trace.msg(vp::trace::LEVEL_TRACE, "Register write (address: 0x%x, value: 0x%x)\n", address, value);

    switch (address)
    {
        case 0x01:
        {
            this->trace.msg(vp::trace::LEVEL_INFO, "Writing register (name: %s, value: 0x%x)\n", "Device ID & Ctrl", value);
            this->device_id = value;
            break;
        }
        case 0x03:
        {
            this->trace.msg(vp::trace::LEVEL_INFO, "Writing register (name: %s, value: 0x%x)\n", "IO Direction", value);
            this->io_dir = value;
            break;
        }
        case 0x05:
        {
            this->trace.msg(vp::trace::LEVEL_INFO, "Writing register (name: %s, value: 0x%x)\n", "Output State", value);
            this->output_state = value;
            break;
        }
        case 0x07:
        {
            this->trace.msg(vp::trace::LEVEL_INFO, "Writing register (name: %s, value: 0x%x)\n", "Output High-Z", value);
            this->output_high_z = value;
            break;
        }
        case 0x09:
        {
            this->trace.msg(vp::trace::LEVEL_INFO, "Writing register (name: %s, value: 0x%x)\n", "Input Default State", value);
            this->input_default_state = value;
            break;
        }
        case 0x0B:
        {
            this->trace.msg(vp::trace::LEVEL_INFO, "Writing register (name: %s, value: 0x%x)\n", "Pull Enable", value);
            this->pull_enable = value;
            break;
        }
        case 0x0D:
        {
            this->trace.msg(vp::trace::LEVEL_INFO, "Writing register (name: %s, value: 0x%x)\n", "Pull-Down/Pull-Up", value);
            this->pull_down_up = value;
            break;
        }
        case 0x0F:
        {
            this->trace.msg(vp::trace::LEVEL_INFO, "Writing register (name: %s, value: 0x%x)\n", "Input Status", value);
            this->input_status = value;
            break;
        }
        case 0x11:
        {
            this->trace.msg(vp::trace::LEVEL_INFO, "Writing register (name: %s, value: 0x%x)\n", "Interrupt Mask", value);
            this->interrupt_mask = value;
            break;
        }
        case 0x13:
        {
            this->trace.msg(vp::trace::LEVEL_INFO, "Writing register (name: %s, value: 0x%x)\n", "interrupt Status", value);
            this->interrupt_status = value;
            break;
        }
        default:
            this->trace.force_warning("Writing invalid register (address: 0x%x)\n", address);
            break;
    }

}


uint8_t Fxl6408::handle_reg_read(uint8_t address)
{
    this->trace.msg(vp::trace::LEVEL_DEBUG, "Register read (address: 0x%x)\n", address);

    uint8_t value = 0xFF;

    switch (address)
    {
        case 0x01:
        {
            value = this->device_id;
            this->trace.msg(vp::trace::LEVEL_INFO, "Reading register (name: %s, value: 0x%x)\n", "Device ID & Ctrl", value);
            break;
        }
        case 0x03:
        {
            value = this->io_dir;
            this->trace.msg(vp::trace::LEVEL_INFO, "Reading register (name: %s, value: 0x%x)\n", "IO Direction", value);
            break;
        }
        case 0x05:
        {
            value = this->output_state;
            this->trace.msg(vp::trace::LEVEL_INFO, "Reading register (name: %s, value: 0x%x)\n", "Output State", value);
            break;
        }
        case 0x07:
        {
            value = this->output_high_z;
            this->trace.msg(vp::trace::LEVEL_INFO, "Reading register (name: %s, value: 0x%x)\n", "Output High-Z", value);
            break;
        }
        case 0x09:
        {
            value = this->input_default_state;
            this->trace.msg(vp::trace::LEVEL_INFO, "Reading register (name: %s, value: 0x%x)\n", "Input Default State", value);
            break;
        }
        case 0x0B:
        {
            value = this->pull_enable;
            this->trace.msg(vp::trace::LEVEL_INFO, "Reading register (name: %s, value: 0x%x)\n", "Pull Enable", value);
            break;
        }
        case 0x0D:
        {
            value = this->pull_down_up;
            this->trace.msg(vp::trace::LEVEL_INFO, "Reading register (name: %s, value: 0x%x)\n", "Pull-Down/Pull-Up", value);
            break;
        }
        case 0x0F:
        {
            value = this->input_status;
            this->trace.msg(vp::trace::LEVEL_INFO, "Reading register (name: %s, value: 0x%x)\n", "Input Status", value);
            break;
        }
        case 0x11:
        {
            value = this->interrupt_mask;
            this->trace.msg(vp::trace::LEVEL_INFO, "Reading register (name: %s, value: 0x%x)\n", "Interrupt Mask", value);
            break;
        }
        case 0x13:
        {
            value = this->interrupt_status;
            this->trace.msg(vp::trace::LEVEL_INFO, "Reading register (name: %s, value: 0x%x)\n", "Interrupt Status", value);
            break;
        }
        default:
            this->trace.force_warning("Reading invalid register (address: 0x%x)\n", address);
            break;
    }

    return value;
}


int Fxl6408::build()
{
    traces.new_trace("trace", &trace, vp::DEBUG);

    this->i2c_itf.set_sync_meth(&Fxl6408::i2c_sync);
    this->new_master_port("i2c", &this->i2c_itf);

    this->i2c_state = I2C_STATE_WAIT_START;
    this->i2c_prev_sda = 1;
    this->i2c_prev_scl = 1;
    this->i2c_being_addressed = false;
    this->device_address = 0x43;
    this->waiting_reg_address = true;

    this->device_id           = 0xC2;
    this->io_dir              = 0x00;
    this->output_state        = 0x00;
    this->output_high_z       = 0xFF;
    this->input_default_state = 0x00;
    this->pull_enable         = 0xFF;
    this->pull_down_up        = 0x00;
    this->input_status        = 0xFF;
    this->interrupt_mask      = 0x00;
    this->interrupt_status    = 0xFF;

    return 0;
}


extern "C" vp::component *vp_constructor(js::config *config)
{
    return new Fxl6408(config);
}
