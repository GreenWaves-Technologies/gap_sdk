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
#include "i2s_verif.hpp"

class Slot
{
public:
    Slot(Testbench *top, I2s_verif *i2s, int itf, int id);
    void setup(pi_testbench_i2s_verif_slot_config_t *config);
    void start(pi_testbench_i2s_verif_slot_start_config_t *config);
    void stop();
    void start_frame();
    int get_data();
    void send_data(int sdo);
    void pdm_sync(int sd);
    int pdm_get();

private:
    Testbench *top;
    I2s_verif *i2s;
    int id;
    vp::trace trace;
    pi_testbench_i2s_verif_slot_config_t config_rx;
    pi_testbench_i2s_verif_slot_config_t config_tx;
    pi_testbench_i2s_verif_slot_start_config_t start_config_rx;
    pi_testbench_i2s_verif_slot_start_config_t start_config_tx;
    bool rx_started;
    int rx_pending_bits;
    int rx_current_value;
    uint32_t rx_pending_value;

    bool tx_started;
    int tx_pending_bits;
    uint32_t tx_pending_value;
    FILE *outfile;
    FILE *infile;
};


I2s_verif::~I2s_verif()
{
    this->trace.msg(vp::trace::LEVEL_INFO, "Closing I2S verif\n");
    this->engine->dequeue(this);
    this->itf->sync(2, 2, (2 << 2) | 2);
}


I2s_verif::I2s_verif(Testbench *top, vp::i2s_master *itf, int itf_id, pi_testbench_i2s_verif_config_t *config)
  : vp::time_engine_client(NULL)
{
    ::memcpy(&this->config, config, sizeof(pi_testbench_i2s_verif_config_t));

    this->itf = itf;
    this->prev_ws = 0;
    this->frame_active = false;
    this->ws_delay = 1;
    this->current_ws_delay = 0;
    this->is_pdm = config->is_pdm;
    this->is_full_duplex = config->is_full_duplex;
    this->prev_sck = 0;
    this->is_ext_clk = config->is_ext_clk;

    this->clk = 2;
    this->propagated_clk = 0;
    this->propagated_ws = 0;
    this->ws_count = 0;
    this->ws_value = 2;
    this->data = (2 << 2) | 2;

    if (this->is_pdm)
    {
        this->config.nb_slots = 4;
    }
    else
    {
        if (this->is_ext_clk)
        {
            this->clk_period = 1000000000000ULL / config->sampling_freq / config->nb_slots / config->word_size / 2;

            this->clk = 0;
        }
    }

    if (config->is_sai0_clk)
    {
        top->i2ss[0]->clk_propagate |= 1 << itf_id;
    }

    if (config->is_sai0_ws)
    {
        top->i2ss[0]->ws_propagate |= 1 << itf_id;
    }

    this->engine = (vp::time_engine*)top->get_service("time");

    top->traces.new_trace("i2s_verif_itf" + std::to_string(itf_id), &trace, vp::DEBUG);

    for (int i=0; i<this->config.nb_slots; i++)
    {
        this->slots.push_back(new Slot(top, this, itf_id, i));
    }

    if (this->config.is_ext_ws)
    {
        this->ws_value = 0;
    }

    if (this->config.is_ext_clk)
    {
        this->clk = 0;
    }

    this->itf->sync(this->clk, this->ws_value, this->data);

}


void I2s_verif::slot_setup(pi_testbench_i2s_verif_slot_config_t *config)
{
    if (config->slot >= this->config.nb_slots)
    {
        this->trace.fatal("Trying to configure invalid slot (slot: %d, nb_slot: %d)", config->slot, this->config.nb_slots);
        return;
    }

    this->slots[config->slot]->setup(config);

}


void I2s_verif::slot_start(pi_testbench_i2s_verif_slot_start_config_t *config)
{
    this->trace.msg(vp::trace::LEVEL_INFO, "Starting (slot: %d, nb_samples: %d, incr_start: 0x%x, incr_end: 0x%x, incr_value: 0x%x)\n",
        config->slot, config->rx_iter.nb_samples, config->rx_iter.incr_start, config->rx_iter.incr_end, config->rx_iter.incr_value);

    int slot = config->slot;

    if (slot >= this->config.nb_slots)
    {
        this->trace.fatal("Trying to configure invalid slot (slot: %d, nb_slot: %d)", slot, this->config.nb_slots);
        return;
    }

    this->slots[slot]->start(config);

}



void I2s_verif::slot_stop(pi_testbench_i2s_verif_slot_stop_config_t *config)
{
    this->trace.msg(vp::trace::LEVEL_INFO, "Stopping\n");

    int slot = config->slot;

    if (slot >= this->config.nb_slots)
    {
        this->trace.fatal("Trying to configure invalid slot (slot: %d, nb_slot: %d)", slot, this->config.nb_slots);
        return;
    }

    this->slots[slot]->stop();
}


void I2s_verif::sync_sck(int sck)
{
    this->propagated_clk = sck;
    this->sync(sck, this->ws, this->sdio);
}

void I2s_verif::sync_ws(int ws)
{
    this->propagated_ws = ws;
    this->sync(this->prev_sck, ws, this->sdio);
}



void I2s_verif::sync(int sck, int ws, int sdio)
{
    this->sdio = sdio;

    if (this->config.is_sai0_clk)
    {
        sck = this->propagated_clk;
    }

    if (this->config.is_sai0_ws)
    {
        ws = this->propagated_ws;
    }

    sck = sck ^ this->config.clk_polarity;
    ws = ws ^ this->config.ws_polarity;

    this->ws = ws;

    int sd = this->is_full_duplex ? sdio >> 2 : sdio & 0x3;


    if (sck != this->prev_sck)
    {
        this->trace.msg(vp::trace::LEVEL_TRACE, "I2S edge (sck: %d, ws: %d, sdo: %d)\n", sck, ws, sd);

        if (this->is_pdm)
        {
            if (!sck)
            {
                int val0 = this->slots[0]->pdm_get();
                int val1 = this->slots[2]->pdm_get();
                this->itf->sync(2, 2, val0 | (val1 << 2));
            }
            else
            {
                this->slots[0]->pdm_sync(sdio & 3);
                this->slots[2]->pdm_sync(sdio >> 2);

                int val0 = this->slots[1]->pdm_get();
                int val1 = this->slots[3]->pdm_get();
                this->itf->sync(2, 2, val0 | (val1 << 2));
            }
        }
        else
        {
            if (sck)
            {
                // The channel is the one of this microphone
                if (this->prev_ws != ws && ws == 1)
                {
                    this->trace.msg(vp::trace::LEVEL_DEBUG, "Detected frame start\n");

                    // If the WS just changed, apply the delay before starting sending
                    this->current_ws_delay = this->ws_delay;
                    if (this->current_ws_delay == 0)
                    {
                        this->frame_active = true;
                        this->pending_bits = this->config.word_size;
                        this->active_slot = 0;
                    }
                }

                if (this->frame_active)
                {
                    this->slots[this->active_slot]->send_data(sd);

                    this->pending_bits--;

                    if (this->pending_bits == 0)
                    {
                        this->pending_bits = this->config.word_size;
                        this->active_slot++;
                        if (this->active_slot == this->config.nb_slots)
                        {
                            this->frame_active = false;
                        }
                    }

                }

                // If there is a delay, decrease it
                if (this->current_ws_delay > 0)
                {
                    this->current_ws_delay--;
                    if (this->current_ws_delay == 0)
                    {
                        this->frame_active = true;
                        this->pending_bits = this->config.word_size;
                        this->active_slot = 0;
                    }
                }

                this->prev_ws = ws;
            }
            else if (!sck)
            {
                if (this->frame_active)
                {

                    if (this->pending_bits == this->config.word_size)
                    {
                        this->slots[this->active_slot]->start_frame();
                    }
                    this->data = this->slots[this->active_slot]->get_data() | (2 << 2);


                    this->trace.msg(vp::trace::LEVEL_TRACE, "I2S output data (sdi: 0x%x)\n", this->data & 3);
                    this->itf->sync(this->clk, this->ws_value, this->data);
                }
            }

            if (sck == 1)
            {
                if (this->config.is_ext_ws)
                {
                    this->ws_value = 0;
                    if (this->ws_count == 0)
                    {
                        this->ws_value = 1;
                        this->ws_count = this->config.word_size * this->config.nb_slots;
                    }
                    this->itf->sync(this->clk, this->ws_value, this->data);
                    this->ws_count--;
                }
            }

        }
    }

    this->prev_sck = sck;
}



void I2s_verif::start(pi_testbench_i2s_verif_start_config_t *config)
{
    if (this->is_ext_clk)
    {
        this->enqueue_to_engine(this->clk_period);
    }
}




Slot::Slot(Testbench *top, I2s_verif *i2s, int itf, int id) : top(top), i2s(i2s), id(id)
{
    top->traces.new_trace("i2s_verif_itf" + std::to_string(itf) + "_slot" + std::to_string(id), &trace, vp::DEBUG);

    this->config_rx.enabled = false;
    this->rx_started = false;
    this->tx_started = false;
    this->infile = NULL;
    this->outfile = NULL;
}


void Slot::setup(pi_testbench_i2s_verif_slot_config_t *config)
{

    if (config->word_size == 0)
    {
        config->word_size = this->i2s->config.word_size;
    }

    if (config->is_rx)
    {
        ::memcpy(&this->config_rx, config, sizeof(pi_testbench_i2s_verif_slot_config_t));
    }
    else
    {
        ::memcpy(&this->config_tx, config, sizeof(pi_testbench_i2s_verif_slot_config_t));
    }

    this->trace.msg(vp::trace::LEVEL_INFO, "Slot setup (is_rx: %d, enabled: %d, word_size: %d)\n",
        config->is_rx, config->enabled, config->word_size);
}


void Slot::start(pi_testbench_i2s_verif_slot_start_config_t *config)
{
    if (config->type == PI_TESTBENCH_I2S_VERIF_RX_ITER)
    {
        ::memcpy(&this->start_config_rx, config, sizeof(pi_testbench_i2s_verif_slot_start_config_t));

        this->start_config_rx.rx_iter.incr_end &= (1ULL << this->config_rx.word_size) - 1;
        this->start_config_rx.rx_iter.incr_start &= (1ULL << this->config_rx.word_size) - 1;

        if (this->start_config_rx.rx_iter.incr_value >= this->start_config_rx.rx_iter.incr_end)
            this->start_config_rx.rx_iter.incr_value = 0;

        this->rx_started = true;
        this->rx_current_value = this->start_config_rx.rx_iter.incr_start;
        this->rx_pending_bits = 0;
    }
    else if (config->type == PI_TESTBENCH_I2S_VERIF_TX_FILE_DUMPER)
    {
        ::memcpy(&this->start_config_tx, config, sizeof(pi_testbench_i2s_verif_slot_start_config_t));

        this->tx_started = true;

        char *filepath = (char *)config + sizeof(pi_testbench_i2s_verif_slot_start_config_t);
        this->outfile = fopen(filepath, "w");
        if (this->outfile == NULL)
        {
            this->trace.fatal("Unable to open file (file: %s, error: %s)\n", filepath, strerror(errno));
        }

        if (this->i2s->is_pdm)
        {
            this->i2s->pdm_lanes_is_out[this->id / 2] = false;
        }
    }
    else if (config->type == PI_TESTBENCH_I2S_VERIF_RX_FILE_READER)
    {
        ::memcpy(&this->start_config_tx, config, sizeof(pi_testbench_i2s_verif_slot_start_config_t));

        this->rx_started = true;

        char *filepath = (char *)config + sizeof(pi_testbench_i2s_verif_slot_start_config_t);
        this->infile = fopen(filepath, "r");
        if (this->infile == NULL)
        {
            this->trace.fatal("Unable to open file (file: %s, error: %s)\n", filepath, strerror(errno));
        }

        if (this->i2s->is_pdm)
        {
            this->i2s->pdm_lanes_is_out[this->id / 2] = true;
        }

        this->i2s->data = this->id < 2 ? this->i2s->data & 0xC : this->i2s->data & 0x3;
        this->i2s->itf->sync(2, 2, this->i2s->data);
    }
}


void Slot::stop()
{
    this->rx_started = false;
}


void Slot::start_frame()
{
    if (this->rx_started)
    {
        if (this->infile)
        {
            char line [64];

            if (fgets(line, 64, this->infile) == NULL)
            {
                fseek(this->infile, 0, SEEK_SET);
                if (fgets(line, 16, this->infile) == NULL)
                {
                    this->trace.fatal("Unable to get sample from file (error: %s)\n", strerror(errno));
                    return;
                }
            }

            this->rx_pending_bits = this->i2s->config.word_size;
            this->rx_pending_value = strtol(line, NULL, 0);
        }
        else
        {
            if (this->start_config_rx.rx_iter.nb_samples > 0 || this->start_config_rx.rx_iter.nb_samples == -1)
            {
                this->rx_pending_bits = this->i2s->config.word_size;
                this->rx_pending_value = this->rx_current_value;
            }
        }

        if (this->rx_pending_bits > 0)
        {
            this->trace.msg(vp::trace::LEVEL_DEBUG, "Starting RX sample (sample: 0x%x)\n", this->rx_pending_value);


            int msb_first = (this->config_rx.format >> 0) & 1;
            int left_align = (this->config_rx.format >> 1) & 1;
            int sign_extend = (this->config_rx.format >> 2) & 1;
            int dummy_cycles = this->i2s->config.word_size - this->config_rx.word_size;

            if (this->start_config_rx.rx_iter.incr_end - this->rx_current_value <= this->start_config_rx.rx_iter.incr_value)
            {
                this->rx_current_value = this->start_config_rx.rx_iter.incr_start;
            }
            else
            {
                this->rx_current_value += this->start_config_rx.rx_iter.incr_value;
            }
            if (this->start_config_rx.rx_iter.nb_samples > 0)
            {
                this->start_config_rx.rx_iter.nb_samples--;
            }

            bool changed = false;
            if (!msb_first)
            {
                changed = true;
                uint32_t value = 0;
                for (int i=0; i<this->config_rx.word_size; i++)
                {
                    value = (value << 1) | (this->rx_pending_value & 1);
                    this->rx_pending_value >>= 1;
                }
                this->rx_pending_value = value;
            }

            if (dummy_cycles)
            {
                changed = true;

                if (left_align)
                {
                    this->rx_pending_value = this->rx_pending_value << dummy_cycles;
                }
                else
                {
                    this->rx_pending_value = this->rx_pending_value & (1ULL << this->config_rx.word_size) - 1;
                }
            }

            if (changed)
            {
                this->trace.msg(vp::trace::LEVEL_DEBUG, "Adapted sample to format (sample: 0x%x)\n", this->rx_pending_value);
            }
        }
    }

    if (this->tx_started && (this->start_config_tx.tx_file_dumper.nb_samples > 0 || this->start_config_tx.tx_file_dumper.nb_samples == -1))
    {
        this->tx_pending_value = 0;
        this->tx_pending_bits = this->i2s->config.word_size;
        this->trace.msg(vp::trace::LEVEL_DEBUG, "Starting TX sample\n");

    }
}

void Slot::send_data(int sd)
{
    if (this->tx_started)
    {
        if (this->tx_pending_bits > 0)
        {
            this->tx_pending_bits--;
            this->tx_pending_value = (this->tx_pending_value << 1) | (sd == 1);

            if (this->tx_pending_bits == 0)
            {
                int msb_first = (this->config_tx.format >> 0) & 1;
                int left_align = (this->config_tx.format >> 1) & 1;
                int sign_extend = (this->config_tx.format >> 2) & 1;
                int dummy_cycles = this->i2s->config.word_size - this->config_tx.word_size;

                //fprintf(stderr, "Captured sample %x\n", this->tx_pending_value);

                if (dummy_cycles)
                {
                    if (msb_first)
                    {
                        if (left_align)
                        {
                            this->tx_pending_value >>= dummy_cycles;
                        }
                        else
                        {
                        }
                    }
                    else
                    {
                        if (left_align)
                        {
                            uint32_t value = 0;
                            for (int i=0; i<this->i2s->config.word_size; i++)
                            {
                                value = (value << 1) | (this->tx_pending_value & 1);
                                this->tx_pending_value >>= 1;
                            }
                            this->tx_pending_value = value;
                        }
                        else
                        {
                            uint32_t value = 0;
                            for (int i=0; i<this->config_tx.word_size; i++)
                            {
                                value = (value << 1) | (this->tx_pending_value & 1);
                                this->tx_pending_value >>= 1;
                            }
                            this->tx_pending_value = value;
                        }
                    }
                }
                else
                {
                    if (!msb_first)
                    {
                        uint32_t value = 0;
                        for (int i=0; i<this->config_tx.word_size; i++)
                        {
                            value = (value << 1) | (this->tx_pending_value & 1);
                            this->tx_pending_value >>= 1;
                        }
                        this->tx_pending_value = value;
                    }
                }

                //fprintf(stderr, "Reordered sample %x\n", this->tx_pending_value);

                this->trace.msg(vp::trace::LEVEL_DEBUG, "Writing sample (value: 0x%lx)\n", this->tx_pending_value);
                fprintf(this->outfile, "0x%x\n", this->tx_pending_value);
                this->tx_pending_bits = this->i2s->config.word_size;
            }
        }
    }
}


int Slot::get_data()
{
    if (this->rx_started)
    {
        int data;

        if (this->rx_pending_bits > 0)
        {
            data = (this->rx_pending_value >> (this->i2s->config.word_size - 1)) & 1;
            this->rx_pending_bits--;
            this->rx_pending_value <<= 1;
        }
        else
        {
            if (this->start_config_rx.rx_iter.nb_samples == 0)
            {
                this->rx_started = false;
            }
            data = 0;
        }

        this->trace.msg(vp::trace::LEVEL_TRACE, "Getting data (data: %d)\n", data);

        return data;
    }

    return 2;
}


void Slot::pdm_sync(int sd)
{
    if (this->outfile)
    {
        fprintf(this->outfile, "%d\n", sd);
    }
}


int Slot::pdm_get()
{
    if (this->infile)
    {
        char line [16];

        if (fgets(line, 16, this->infile) == NULL)
        {
            fseek(this->infile, 0, SEEK_SET);
            if (fgets(line, 16, this->infile) == NULL)
            {
                this->trace.fatal("Unable to get sample from file (error: %s)\n", strerror(errno));
                return 3;
            }
        }

        int data = atoi(line);

        return data;
    }
    else
    {
        if (this->i2s->pdm_lanes_is_out[this->id / 2])
            return 0;
        else
            return 2;
    }
}


int64_t I2s_verif::exec()
{
    this->clk ^= 1;
    this->itf->sync(this->clk, this->ws_value, this->data);

    return this->clk_period;
}