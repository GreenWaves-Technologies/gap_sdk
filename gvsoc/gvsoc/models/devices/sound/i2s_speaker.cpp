/*
 * Copyright (C) 2018 GreenWaves Technologies
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
#include <vp/itf/io.hpp>
#include <vp/itf/i2s.hpp>
#include <vp/itf/clock.hpp>

#ifdef USE_SNDFILE
#include <sndfile.hh>
#endif


class Speaker;


class Output
{

public:
    virtual void write_data(int64_t timestamp, long long int data) = 0;

};


class Outfile_hex : public Output
{

public:
    Outfile_hex(Speaker *top, std::string file, int width);
    void write_data(int64_t timestamp, long long int data);

private:
    Speaker *top;
    FILE *file;
};





class Speaker : public vp::component
{
    friend class Outfile_hex;

public:
    Speaker(js::config *config);

    int build();
    int push_data();

protected:
    static void sync(void *__this, int sck, int ws, int sd);
    void start_sample();
    void  push_data(int data);
    int get_data();
    void write_data();

    vp::i2s_master i2s_itf;
    vp::clock_master clock_cfg;

    int prev_ws;            // Value of the previous ws. Updated at each clock cycle to detect the beginning of the frame
    int ws_delay;           // Tells after how many cycles the first data is sampled when the ws of this channel becomes active
    int current_ws_delay;   // Remaining cycles before the first data will be sampled. It is initialized with ws_delay when the frame starts
    bool is_active;         // Set to true when the word-select of this microphone is detected. The microphone is sending samples when this is true;
    int pending_bits;       // Number of bits of the current sample which remain to be sent
    int width;              // Number of bits of the samples
    int current_value;      // Value of the current sample being sent
    Output *out = NULL;
    
    vp::trace trace;

};


Outfile_hex::Outfile_hex(Speaker *top, std::string file, int width)
: top(top)
{
    this->file = fopen(file.c_str(), "w");
}


void Outfile_hex::write_data(int64_t timestamp, long long int data)
{
    this->top->trace.msg(vp::trace::LEVEL_DEBUG, "Writing sample (value: 0x%x)\n", data);
    fprintf(this->file, "%llx\n", data);
}


Speaker::Speaker(js::config *config)
    : vp::component(config)
{
}


int Speaker::build()
{
    traces.new_trace("trace", &trace, vp::DEBUG);

    this->new_master_port("i2s", &this->i2s_itf);
    this->new_master_port("clock_cfg", &this->clock_cfg);

    this->i2s_itf.set_sync_meth(&Speaker::sync);

    this->is_active = false;
    this->prev_ws = 0;
    this->ws_delay = this->get_js_config()->get_int("ws-delay");
    this->current_ws_delay = 0;
    this->pending_bits = 0;
    this->width = this->get_js_config()->get_int("width");

    std::string mode = this->get_js_config()->get_child_str("out_mode");
    if (mode == "file")
    {
        std::string out_file = this->get_js_config()->get_child_str("outfile");

        printf("OUT FILE %s\n", out_file.c_str());

        if (out_file != "") {

            char *ext = rindex((char *)out_file.c_str(), '.');

            if (ext == NULL)
            {
                this->get_trace()->fatal("Unsupported file extension: %s\n", out_file.c_str());
                return -1;
            }

            if (strcmp(ext, ".hex") == 0)
            {
                this->out = new Outfile_hex(this, out_file, this->width);
            }
            else
            {
                this->get_trace()->fatal("Unsupported file extension: %s\n", out_file.c_str());
            }
        }
    }
    return 0;
}


void Speaker::start_sample()
{
    this->trace.msg(vp::trace::LEVEL_TRACE, "Starting sample\n");
    this->pending_bits = this->width;
    this->current_value = 0;
}


void Speaker::write_data()
{
    if (this->out)
    {
        this->out->write_data(this->get_time(), this->current_value);
    }
}


void Speaker::push_data(int data)
{
    if (this->pending_bits == 0)
        return;

    this->trace.msg(vp::trace::LEVEL_TRACE, "Writing data (value: %d)\n", data);
    // Shift bits from MSB
    this->current_value |= data << (this->pending_bits - 1);
    this->pending_bits--;

    // Sample new data if all bits have been shifted
    if (this->pending_bits == 0)
    {
        this->pending_bits = this->width;
        this->write_data();
    }
}


void Speaker::sync(void *__this, int sck, int ws, int sd)
{
    Speaker *_this = (Speaker *)__this;

    _this->trace.msg(vp::trace::LEVEL_TRACE, "I2S edge (sck: %d, ws: %d, sdo: %d)\n", sck, ws, sd);

    if (sck)
    {
        // The channel is the one of this microphone
        if (_this->prev_ws != ws && ws == 1)
        {
            if (!_this->is_active)
            {
                _this->trace.msg(vp::trace::LEVEL_TRACE, "Activating channel\n");
            }

            // If the WS just changed, apply the delay before starting sending
            _this->current_ws_delay = _this->ws_delay;
            if (_this->current_ws_delay == 0)
            {
                _this->is_active = true;
            }
        }

        if (_this->is_active)
        {
            _this->push_data(sd);
        }

        // If there is a delay, decrease it
        if (_this->current_ws_delay > 0)
        {
            _this->current_ws_delay--;
            if (_this->current_ws_delay == 0)
            {
                // And reset the sample
                _this->is_active = true;
                _this->start_sample();
            }
        }

        _this->prev_ws = ws;
    }
    else
    {

    }
}


extern "C" vp::component *vp_constructor(js::config *config)
{
    return new Speaker(config);
}
