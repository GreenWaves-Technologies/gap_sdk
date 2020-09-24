/*
 * Copyright (C) 2020 GreenWaves Technologies, SAS, ETH Zurich and
 *                    University of Bologna
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
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <iostream>
#ifdef USE_SNDFILE
#include <sndfile.hh>
#endif


class Stim_txt;

class Microphone : public vp::component
{
    friend class Stim_txt;

public:
    Microphone(js::config *config);

    int build();

protected:
    static void sync(void *__this, int sck, int ws, int sd);
    void start_sample();
    int pop_data();
    int get_data();

    vp::i2s_master i2s_itf;
    vp::i2s_master ws_out_itf;
    vp::i2s_slave ws_in_itf;
    vp::clock_master clock_cfg;

    int channel_ws;         // Word-Select of this microphone. It will send data whe the ws corresponds to this one
    int prev_ws;            // Value of the previous ws. Updated at each clock cycle to detect the beginning of the frame
    int ws_delay;           // Tells after how many cycles the first data is sampled when the ws of this channel becomes active
    int current_ws_delay;   // Remaining cycles before the first data will be sampled. It is initialized with ws_delay when the frame starts
    int current_value;      // Value of the current sample being sent
    int pending_bits;       // Number of bits of the current sample which remain to be sent
    int width;              // Number of bits of the samples
    int frequency;          // Sampling frequency
    int pdm;                // Generate samples in PDM mode
    bool stim_incr;         // True if the stimuli should be an incrising number
    int stim_incr_value;    // In case incr is active, give the increment value
    int current_stim;       // When using incrementing stim value, this gives the first value
    bool is_active;         // Set to true when the word-select of this microphone is detected. The microphone is sending samples when this is true;
    Stim_txt *stim;         // Pointer to the stim generator
    int ws_in;
    bool lower_ws_out;
    bool enabled;

    vp::trace trace;


    static void ws_in_sync(void *__this, int sck, int ws, int sd);

};


class Stim
{

public:
    virtual long long get_data(int64_t timestamp) = 0;

};


class Stim_txt : public Stim
{

public:
  Stim_txt(Microphone *top, std::string file, int width, int freq, bool raw=false, bool use_libsnd=false);
  long long get_data(int64_t timestamp);
  long long get_data_from_file();

private:
  Microphone *top;
  int width;
  FILE *stim_file;
  std::string file_path;
  int period;
  int64_t last_data_time;
  long long last_data;
  int64_t next_data_time;
  long long next_data;
  bool raw;
  bool use_libsnd;
#ifdef USE_SNDFILE
  SndfileHandle sndfile;
#endif
};


Stim_txt::Stim_txt(Microphone *top, std::string file, int width, int freq, bool raw, bool use_libsnd)
: top(top), width(width), file_path(file), raw(raw), use_libsnd(use_libsnd)
{
    if (use_libsnd)
    {

    #ifdef USE_SNDFILE

        unsigned int pcm_width = width == 16 ? SF_FORMAT_PCM_16 : SF_FORMAT_PCM_32;
        this->sndfile = SndfileHandle (file, SFM_READ, SF_FORMAT_WAV | pcm_width) ;
        freq = sndfile.samplerate ();

    #else

        this->top->get_trace()->fatal("Unable to open file (%s), libsndfile support is not active\n", file.c_str());
        return;

    #endif

    }
    else
    {
        stim_file = fopen(file.c_str(), "r");
        if (stim_file == NULL)
        {
            this->top->get_trace()->fatal("Failed to open stimuli file: %s: %s\n", file.c_str(), strerror(errno));
        }
    }

    if (freq)
        this->period = 1000000000000UL / freq;
    else
        this->period = 0;

    this->last_data_time = -1;
    this->next_data_time = -1;
}


static inline int get_signed_value(unsigned long long val, int bits)
{
    return ((int)val) << (64-bits) >> (64-bits);
}


long long Stim_txt::get_data_from_file()
{
    if (use_libsnd)
    {

    #ifdef USE_SNDFILE

        int32_t result;
        if (this->width <= 16)
        {
            int16_t sample;
            sndfile.read (&sample, 1);
            result = (int32_t)sample;
        }
        else if (this->width <= 32)
        {
            int32_t sample;
            sndfile.read (&sample, 1);
            result = (int32_t)sample;
        }

        return result;
    #else
        return 0;
    #endif

    }
    else if (raw)
    {
        unsigned long long data = 0;
        while(fread((void *)&data, 2, 1, this->stim_file) != 1)
        {
            fclose(this->stim_file);
            this->stim_file = fopen(this->file_path.c_str(), "r");    
        }

        long long result = get_signed_value(data, width);

        this->top->trace.msg(vp::trace::LEVEL_TRACE, "Got new sample (value: 0x%x)", result);
        return result;
    }
    else
    {
        char *line = NULL;
        size_t len = 0;
        while(::getline(&line, &len, this->stim_file) == -1)
        {
            fclose(this->stim_file);
            this->stim_file = fopen(this->file_path.c_str(), "r");    
        }
    
        unsigned long long data = strtol(line, NULL, 16);
        long long result = get_signed_value(data, width);

        this->top->trace.msg(vp::trace::LEVEL_TRACE, "Got new sample2 (value: 0x%x)", result);
    
        return result;
    }
}


long long Stim_txt::get_data(int64_t timestamp)
{
    if (this->period == 0)
        return this->get_data_from_file();

    if (this->last_data_time == -1)
    {
        this->last_data = this->get_data_from_file();
        this->last_data_time = timestamp;
    }

    if (this->next_data_time == -1)
    {
        this->next_data = this->get_data_from_file();
        this->next_data_time = this->last_data_time + this->period;
    }

    // Get samples from the file until the current timestamp fits the window
    while (timestamp >= this->next_data_time)
    {
        this->last_data_time = this->next_data_time;
        this->last_data = this->next_data;
        this->next_data_time = this->last_data_time + this->period;
        this->next_data = this->get_data_from_file();
    }

    // Now do the interpolation between the 2 known samples
    float coeff = (float)(timestamp - this->last_data_time) / (this->next_data_time - this->last_data_time);
    float value = (float)this->last_data + (float)(this->next_data - this->last_data) * coeff;


    this->top->trace.msg(vp::trace::LEVEL_TRACE, "Interpolated new sample (value: %d, timestamp: %ld, prev_timestamp: %ld, next_timestamp: %ld, prev_value: %d, next_value: %d)", value, timestamp, last_data_time, next_data_time, last_data, next_data);
    

    //printf("%f %f %d %d %ld %ld %ld\n", coeff, value, last_data, next_data, last_data_time, timestamp, next_data_time);

    return (int)value;
}


Microphone::Microphone(js::config *config)
    : vp::component(config)
{
}

void Microphone::ws_in_sync(void *__this, int sck, int ws, int sd)
{
    Microphone *_this = (Microphone *)__this;

    _this->trace.msg(vp::trace::LEVEL_TRACE, "Received input WS edge (ws: %d)\n", ws);

    _this->ws_in = ws;
}

int Microphone::build()
{
    traces.new_trace("trace", &trace, vp::DEBUG);

    this->new_master_port("i2s", &this->i2s_itf);
    this->new_master_port("ws_out", &this->ws_out_itf);
    this->new_slave_port("ws_in", &this->ws_in_itf);
    this->new_master_port("clock_cfg", &this->clock_cfg);

    this->ws_in_itf.set_sync_meth(&Microphone::ws_in_sync);

    this->i2s_itf.set_sync_meth(&Microphone::sync);

    this->channel_ws = this->get_js_config()->get_child_str("channel") != "left";
    this->prev_ws = 0;
    this->ws_delay = this->get_js_config()->get_int("ws-delay");
    this->width = this->get_js_config()->get_int("width");
    this->frequency = this->get_js_config()->get_int("frequency");
    this->enabled = this->get_js_config()->get_child_bool("enabled");
    this->pdm = this->get_js_config()->get_int("pdm");
    this->stim_incr = false;
    this->stim = NULL;
    this->is_active = false;
    std::string mode = this->get_js_config()->get_child_str("stim_mode");
    if (mode == "file")
    {
        std::string stim_file = this->get_js_config()->get_child_str("stim");

        if (stim_file != "") {

            char *ext = rindex((char *)stim_file.c_str(), '.');

            if (ext == NULL)
            {
                this->get_trace()->fatal("Unsupported file extension: %s\n", stim_file.c_str());
                return -1;
            }

            if (strcmp(ext, ".hex") == 0)
            {
                this->stim = new Stim_txt(this, stim_file, this->width, this->frequency);
            }
            else if (strcmp(ext, ".raw") == 0)
            {
                this->stim = new Stim_txt(this, stim_file, this->width, this->frequency, true);
            }
            else if (strcmp(ext, ".wav") == 0)
            {
                this->stim = new Stim_txt(this, stim_file, this->width, this->frequency, false, true);
            }
            else
            {
                this->get_trace()->fatal("Unsupported file extension: %s\n", stim_file.c_str());
            }
        }
    }
    else if (mode == "incr")
    {
        this->stim_incr = true;
        this->current_stim = this->get_js_config()->get_int("stim_incr_start");
        this->stim_incr_value = this->get_js_config()->get_int("stim_incr_value");
    }
    this->current_ws_delay = 0;
    this->pending_bits = -1;
    this->ws_in = 0;
    this->lower_ws_out = false;

    return 0;
}


void Microphone::start_sample()
{
    this->trace.msg(vp::trace::LEVEL_TRACE, "Starting sample\n");
    this->pending_bits = this->width;
    this->current_value = this->get_data();
}


int Microphone::get_data()
{
    if (this->stim)
    {
        return this->stim->get_data(this->get_time());
    }
    else
    {
        if (this->stim_incr)
        {
            this->trace.msg(vp::trace::LEVEL_TRACE, "Incrementing stim (value: 0x%x)\n", this->current_stim);
            //fprintf(stderr, "stim incr %x %d\n", this->current_stim, (short)this->current_stim);
            return this->current_stim += this->stim_incr_value;
        }
    }

    return 0;
}


int Microphone::pop_data()
{
    if (pdm)
    {
#if 0
        // PDM mode, only transmit one modulated bit per sample
        long long sample = this->getData();
        unsigned long long value = sample + (1 << (width - 1));

        unsigned long long maxVal = (1 << (width)) - 1;

        pdmError += value;

        if (pdmError >= maxVal)
        {
            pdmError -= maxVal;
            return 1;
        }
        else
        {
            return 0;
        }
        }
#endif
    }
    else
    {
        // Classic mode, sample one full value and stream one bit per cycle

        // Sample new data if all bits have been shifted
        if (this->pending_bits >= 0)
        {
            if (this->pending_bits > 0)
            {
                // Shift bits from MSB
                int bit = (this->current_value >> (this->pending_bits - 1)) & 1;
                this->pending_bits--;
                return bit;
            }

            this->pending_bits--;
            return -1;
        }
    
        return -2;
    }

    return 0;    
}


void Microphone::sync(void *__this, int sck, int ws, int sd)
{
    Microphone *_this = (Microphone *)__this;

    if (!_this->enabled)
        return;

    if (_this->ws_in_itf.is_bound())
        ws = _this->ws_in;

    _this->trace.msg(vp::trace::LEVEL_TRACE, "I2S edge (sck: %d, ws: %d, sdo: %d %d)\n", sck, ws, sd, _this->ws_in_itf.is_bound());

    if (sck)
    {
        if (_this->pending_bits == 1 && _this->ws_out_itf.is_bound())
        {
            _this->ws_out_itf.sync(0, 1, 0);
            _this->lower_ws_out = true;
        }
        else if ( _this->lower_ws_out)
        {
            _this->ws_out_itf.sync(0, 0, 0);
            _this->lower_ws_out = false;
        }
    }
    else
    {
        // The channel is the one of this microphone
        if (_this->prev_ws != ws && ws == _this->channel_ws)
        {
            if (!_this->is_active)
            {
                _this->trace.msg(vp::trace::LEVEL_TRACE, "Activating channel\n");
            }

            _this->is_active = true;

            // If the WS just changed, apply the delay before starting sending
            _this->current_ws_delay = _this->ws_delay + 1;
            if (_this->current_ws_delay == 0)
            {
                _this->is_active = true;
            }
        }

        // If there is a delay, decrease it
        if (_this->current_ws_delay > 0)
        {
            _this->current_ws_delay--;
            if (_this->current_ws_delay == 0)
            {
                // And reset the sample
                _this->start_sample();
            }
        }

        if (_this->is_active && _this->pending_bits > 0)
        {
            int data = _this->pop_data();
            if (data >= 0)
            {
                //fprintf(stderr, "Popped data %d\n", data);

                // If there is no more delay, set the sample now as it wil be sampled by the receiver in 1 cycle
                _this->trace.msg(vp::trace::LEVEL_TRACE, "Setting data (value: %d)\n", data);

                _this->i2s_itf.sync(sck, ws, data);
            }
            else if (data == -1)
            {
                _this->i2s_itf.sync(sck, ws, 2);
                _this->is_active = false;
                _this->ws_out_itf.sync(0, 0, 0);
            }
        }
        else if (_this->pending_bits == 0)
        {
            _this->pending_bits = -1;
            _this->trace.msg(vp::trace::LEVEL_TRACE, "Releasing output\n");
            _this->i2s_itf.sync(sck, ws, 2);
        }

        _this->prev_ws = ws;


    }
}


extern "C" vp::component *vp_constructor(js::config *config)
{
    return new Microphone(config);
}
