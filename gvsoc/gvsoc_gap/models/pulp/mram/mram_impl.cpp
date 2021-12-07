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

/* 
 * Authors: Germain Haugou, GreenWaves Technologies (germain.haugou@greenwaves-technologies.com)
 */


#include <vp/vp.hpp>
#include <vp/itf/wire.hpp>
#include <stdio.h>
#include <string.h>
#include <pulp/mram/mram.hpp>


#define MRAM_WORD_SIZE 16
#define SECTOR_SIZE 8192


typedef enum
{
    MRAM_STATE_IDLE,
    MRAM_STATE_HANDLE_TRIM,
    MRAM_STATE_HANDLE_PROGRAM,
    MRAM_STATE_HANDLE_READ,
} Mram_state_e;


class mram : public vp::component
{

public:

    mram(js::config *config);

    int build();
    void start();
    void reset(bool active);

private:

    static void sync_req(void *__this, mram_req_t *req);
    static void sync_data(void *__this, mram_data_t *req);
    static void sync_conf(void *__this, mram_conf_t *req);
    static void handle_erase(void *__this, vp::clock_event *event);
    static void handle_read(void *__this, vp::clock_event *event);
    static void handle_trim(void *__this, vp::clock_event *event);
    static void handle_program(void *__this, vp::clock_event *event);

    void erase_chip();
    void erase_sect(uint32_t addr);
    void erase_word(uint32_t addr, uint32_t size);
    void req_read();
    int64_t get_cycle_count(int64_t cycles);

    vp::trace     trace;
    vp::wire_slave<mram_req_t *> in_req;
    vp::wire_slave<mram_data_t *> in_data;
    vp::wire_slave<mram_conf_t *> in_conf;

    uint64_t size = 0;

    uint8_t *mem_data;
    Mram_state_e state;
    int pending_size;
    uint32_t pending_addr;
    uint32_t div;
    bool write_first;

    vp::clock_event *erase_event;
    vp::clock_event *read_event;
    vp::clock_event *trim_event;
    vp::clock_event *program_event;
};

mram::mram(js::config *config)
: vp::component(config)
{

}

int64_t mram::get_cycle_count(int64_t cycles)
{
    if (this->div == 0)
    {
        return cycles;
    }
    else
    {
        return cycles * this->div;
    }
}

void mram::sync_conf(void *__this, mram_conf_t *req)
{
    mram *_this = (mram *)__this;
    _this->div = req->div;
    _this->trace.msg(vp::trace::LEVEL_INFO, "Setting divider (value: 0x%x)\n", _this->div);
}

void mram::sync_data(void *__this, mram_data_t *req)
{
    mram *_this = (mram *)__this;

    switch (_this->state)
    {
        case MRAM_STATE_HANDLE_TRIM:
            for (int i=0; i<req->size; i++)
            {
                _this->trace.msg(vp::trace::LEVEL_DEBUG, "Received trim data (value: 0x%x)\n", req->data[i]);
            }
            _this->event_enqueue(_this->trim_event, _this->get_cycle_count(125));
            break;

        case MRAM_STATE_HANDLE_PROGRAM:
        {
            for (int i=0; i<req->size; i++)
            {
                _this->trace.msg(vp::trace::LEVEL_DEBUG, "Received program data (value: 0x%x, addr: 0x%x)\n", req->data[i], _this->pending_addr+i);
            }
            memcpy(&_this->mem_data[_this->pending_addr], req->data, req->size);
            _this->pending_addr += req->size;
            int64_t cycles;
            if (_this->write_first)
            {
                _this->write_first = false;
                cycles = 917;
            }
            else
            {
                int64_t period = _this->get_period();
                if (_this->div != 0)
                {
                    period = period * _this->div;
                }
                cycles = (330000 + period - 1) / period + 13;
            }

            _this->event_enqueue(_this->program_event, _this->get_cycle_count(cycles));
            break;
        }

        case MRAM_STATE_HANDLE_READ:
            _this->event_enqueue(_this->read_event, _this->get_cycle_count(1));
            break;
    }
}


void mram::erase_chip()
{
    this->trace.msg(vp::trace::LEVEL_DEBUG, "Erasing chip\n");
    int64_t period = this->get_period();
    if (this->div != 0)
    {
        period = period * this->div;
    }
    int64_t cycles = (45050823000 + period - 1) / period;
    this->event_enqueue(this->erase_event, this->get_cycle_count(cycles));
}


void mram::erase_sect(uint32_t addr)
{
    this->trace.msg(vp::trace::LEVEL_DEBUG, "Erasing sect (addr: 0x%x)\n", addr);
    int64_t period = this->get_period();
    if (this->div != 0)
    {
        period = period * this->div;
    }
    int64_t time = 920 * period + 330000 * (SECTOR_SIZE/16);
    int64_t cycles = (time + period - 1) / period;
    this->event_enqueue(this->erase_event, this->get_cycle_count(cycles));
}


void mram::erase_word(uint32_t addr, uint32_t size)
{
    this->trace.msg(vp::trace::LEVEL_DEBUG, "Erasing word (addr: 0x%x, size: 0x%x)\n", addr, size * MRAM_WORD_SIZE);
    int64_t period = this->get_period();
    if (this->div != 0)
    {
        period = period * this->div;
    }
    int64_t time = 920 * period + (330000 + 8 * period) * (size + 1);
    int64_t cycles = (time + period - 1) / period;

    this->event_enqueue(this->erase_event, this->get_cycle_count(cycles));
}


void mram::handle_erase(void *__this, vp::clock_event *event)
{

    mram *_this = (mram *)__this;
    _this->in_req.sync(NULL);
}



void mram::handle_trim(void *__this, vp::clock_event *event)
{
    mram *_this = (mram *)__this;
    _this->in_data.sync(NULL);
}


void mram::handle_program(void *__this, vp::clock_event *event)
{
    mram *_this = (mram *)__this;
    _this->in_data.sync(NULL);
}


void mram::handle_read(void *__this, vp::clock_event *event)
{

    mram *_this = (mram *)__this;

    if (_this->pending_size > 0)
    {
        if (_this->pending_addr >= _this->size)
        {
            _this->trace.force_warning("Received out-of-bound request (addr: 0x%x, mram_size: 0x%x)\n", _this->pending_addr, _this->size);
        }
        else
        {
            int size = MRAM_WORD_SIZE;
            uint32_t misaligned_offset = _this->pending_addr & (MRAM_WORD_SIZE - 1);

            if (misaligned_offset > 0)
            {
                size = MRAM_WORD_SIZE - misaligned_offset;
            }

            if (size > _this->pending_size)
            {
                size = _this->pending_size;
            }

            mram_data_t mram_data = { .data=&_this->mem_data[_this->pending_addr], .size=size };
            _this->trace.msg(vp::trace::LEVEL_DEBUG, "Sending line (addr: 0x%x, size: 0x%x, remaining_size: 0x%x)\n", _this->pending_addr, size, _this->pending_size);
            _this->pending_size -= size;
            _this->pending_addr += size;
            _this->in_data.sync(&mram_data);
        }
    }
}

void mram::req_read()
{
    if (!(read_event->is_enqueued()))
    {
        this->event_enqueue(this->read_event, this->get_cycle_count(13));
    }
}


void mram::sync_req(void *__this, mram_req_t *req)
{
    mram *_this = (mram *)__this;
    std::string req_name = "UNDEF";

    switch (req->operation)
    {
        case MRAM_OP_TRIM_CFG: 
            req_name = "TRIM CFG";
            _this->state = MRAM_STATE_HANDLE_TRIM;
            break;

        case MRAM_OP_NORMAL_TX:
            req_name = "PROGRAM";
            _this->state = MRAM_STATE_HANDLE_PROGRAM;
            _this->pending_addr = req->addr;
            _this->write_first = true;
            break;

        case MRAM_OP_ERASE_CHIP:
            req_name = "ERASE CHIP";
            _this->erase_chip();
            break;

        case MRAM_OP_ERASE_SECT:
            req_name = "ERASE SECT";
            _this->erase_sect(req->addr);
            break;

        case MRAM_OP_ERASE_WORD:
            req_name = "ERASE WORD";
            _this->erase_word(req->addr, req->size);
            break;

        case MRAM_OP_READ_RX:
            req_name = "READ";
            _this->state = MRAM_STATE_HANDLE_READ;
            _this->pending_size = req->size;
            _this->pending_addr = req->addr;
            _this->req_read();
            break;

        case MRAM_OP_PWDN: req_name = "PWDN"; break;
        case MRAM_OP_REF_LINE_P: req_name = "REF LINE P"; break;
        case MRAM_OP_REF_LINE_AP: req_name = "REF LINE AP"; break;
    }

    _this->trace.msg(vp::trace::LEVEL_DEBUG, "Received request (op: %s)\n", req_name.c_str());
}


void mram::reset(bool active)
{
    if (active)
    {
        this->state = MRAM_STATE_IDLE;
    }
}

int mram::build()
{
    traces.new_trace("trace", &trace, vp::DEBUG);

    this->in_req.set_sync_meth(&mram::sync_req);
    new_slave_port("input_req", &this->in_req);

    this->in_data.set_sync_meth(&mram::sync_data);
    new_slave_port("input_data", &this->in_data);

    this->in_conf.set_sync_meth(&mram::sync_conf);
    new_slave_port("input_conf", &this->in_conf);

    this->erase_event = this->event_new(this, mram::handle_erase);
    this->read_event = this->event_new(this, mram::handle_read);
    this->trim_event = this->event_new(this, mram::handle_trim);
    this->program_event = this->event_new(this, mram::handle_program);
    this->div = 0;

    return 0;
}

void mram::start()
{
    this->size = get_config_int("size");

    trace.msg("Building MRAM (size: 0x%x)\n", this->size);

    this->mem_data = new uint8_t[this->size];

    // Initialize the mram with a special value to detect uninitialized
    // variables
    memset(mem_data, 0x57, size);
    // Preload the mram
    js::config *stim_file_conf = this->get_js_config()->get("stim_file");
    if (stim_file_conf == NULL)
    {
        stim_file_conf = this->get_js_config()->get("content/image");
    }

    if (stim_file_conf != NULL)
    {
        string path = stim_file_conf->get_str();
        trace.msg("Preloading mram with stimuli file (path: %s)\n", path.c_str());

        FILE *file = fopen(path.c_str(), "rb");
        if (file == NULL)
        {
            this->trace.fatal("Unable to open stim file: %s, %s\n", path.c_str(), strerror(errno));
            return;
        }
        if (fread(this->mem_data, 1, size, file) == 0)
        {
            this->trace.fatal("Failed to read stim file: %s, %s\n", path.c_str(), strerror(errno));
            return;
        }
    }
}

extern "C" vp::component *vp_constructor(js::config *config)
{
    return new mram(config);
}
