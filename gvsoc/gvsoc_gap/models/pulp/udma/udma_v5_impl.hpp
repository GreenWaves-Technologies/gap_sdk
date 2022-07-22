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

#ifndef __PULP_UDMA_UDMA_V2_IMPL_HPP__
#define __PULP_UDMA_UDMA_V2_IMPL_HPP__

#include <vp/vp.hpp>
#include <vp/itf/io.hpp>
#include <vp/itf/qspim.hpp>
#include <vp/itf/uart.hpp>
#include <vp/itf/cpi.hpp>
#include <vp/itf/wire.hpp>
#include <vp/itf/hyper.hpp>
#include <vp/itf/i2c.hpp>
#include <vp/itf/clk.hpp>
#include <vp/itf/i2s.hpp>
#include <stdio.h>
#include <string.h>
#include <vector>
#include "archi/udma/udma_v4.h"
#include "udma_ctrl/udma_ctrl_regs.h"
#include "udma_ctrl/udma_ctrl_regfields.h"
#include "udma_ctrl/udma_ctrl_gvsoc.h"
#include "udma_core_lin/udma_core_lin_regs.h"
#include "udma_core_lin/udma_core_lin_regfields.h"
#include "udma_core_lin/udma_core_lin_gvsoc.h"
#include "udma_core_2d/udma_core_2d_regs.h"
#include "udma_core_2d/udma_core_2d_regfields.h"
#include "udma_core_2d/udma_core_2d_gvsoc.h"
#include <queue>
#include <deque>

class udma;
class Udma_channel;
class Udma_rx_channels;
class Udma_tx_channel;
class Io_Periph;


class Udma_channel;

class Sfu_periph;


class Udma_addrgen
{
public:
    Udma_addrgen(udma *top, int id, int event_id) : top(top), id(id), event_id(event_id), channel(NULL) {}
    virtual vp::io_req_status_e access(uint64_t offset, int size, uint8_t *value, bool is_write) = 0;
    virtual bool get_next_transfer(uint32_t *addr, int *size) = 0;
    virtual void reset(bool active) = 0;
    void dec_size(int size) { this->remaining_size -= size; }
    int get_remaining_size() { return this->remaining_size; }
    bool is_active() { return this->active_transfer; }
    void register_channel(Udma_channel *channel);
    void set_active_transfer(bool active);

    int id;
    udma *top;
    vp::trace trace;
    Udma_channel *channel;
    int event_id;
    bool active_transfer;
    int remaining_size;
};



class Udma_addrgen_linear : public Udma_addrgen
{
public:
    Udma_addrgen_linear(udma *top, int id, int event_id);
    vp::io_req_status_e access(uint64_t offset, int size, uint8_t *value, bool is_write);
    bool get_next_transfer(uint32_t *addr, int *size);
    void reset(bool active);

private:
    void cfg_ctrl_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write);
    void check_pending_transfer();

    vp_regmap_udma_core_lin_addrgen regmap;

    uint32_t pending_addr;
    uint32_t pending_size;

    uint32_t current_addr;
    uint32_t current_size;

    int nb_pending_transfers;
};



class Udma_addrgen_2d : public Udma_addrgen
{
public:
    Udma_addrgen_2d(udma *top, int id, int event_id);
    vp::io_req_status_e access(uint64_t offset, int size, uint8_t *value, bool is_write);
    bool get_next_transfer(uint32_t *addr, int *size);
    void reset(bool active);

private:
    void cfg_ctrl_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write);
    void check_pending_transfer();

    vp_regmap_udma_core_2d regmap;

    uint32_t pending_addr;
    uint32_t pending_size;
    uint32_t pending_stride;
    uint32_t pending_length;

    uint32_t current_addr;
    uint32_t current_size;
    uint32_t current_stride;
    uint32_t current_length;
    uint32_t remaining_length;

    int nb_pending_transfers;
};



template <class T>
class Udma_queue
{
public:
    Udma_queue(int size) : size(size) { init(); }
    void init()
    {
        first = NULL;
        last = NULL;
        nb_cmd = 0;
    }
    T *pop();
    inline void push(T *cmd);
    void remove(T *cmd);
    void push_from_latency(T *cmd);
    bool is_full() { return nb_cmd >= size; }
    bool is_empty() { return nb_cmd == 0; }
    T *get_first() { return first; }
    int get_nb_pushed() { return this->nb_cmd; }
    int get_nb_free() { return this->size - this->nb_cmd; }

private:
    T *first; // First command of the queue, commands are popped from here
    T *last;  // Last command of the queue, commands are pushed here
    int nb_cmd;
    int size;
};


template <class T>
class Udma_fifo
{
public:
    Udma_fifo(vp::component *top, std::string name, int size);
    T pop();
    inline void push(T cmd);
    bool is_full() { return this->queue.size() >= this->size; }
    bool is_empty() { return this->queue.empty(); }
    T get_first() { return this->queue.front(); }
    T get_size() { return this->queue.size(); }
    void reset() { this->size = 0; while (!this->queue.empty()) {this->queue.pop(); } }

private:
    std::queue<T> queue;
    int size;
    vp::reg_32 nb_elems;
    vp::reg_1 reg_is_full;
};


class Udma_request
{
public:
    void set_next(Udma_request *next) { this->next = next; }
    Udma_request *get_next() { return next; }
    Udma_request *next;

    uint32_t data;
    uint32_t addr;
    int size;
    int offset;
    Udma_addrgen *addrgen;
    int addrgen_id;
    Udma_tx_channel *tx_channel;
};



class Udma_channel
{
public:
    Udma_channel(udma *top, std::string name);
    virtual vp::io_req_status_e req(vp::io_req *req, uint64_t offset);
    virtual vp::io_req_status_e access(uint64_t offset, int size, uint8_t *value, bool is_write);
    virtual void reset(bool active);
    void set_addrgen(Udma_addrgen *addrgen);
    void unset_addrgen();
    Udma_addrgen *get_addrgen() { return this->addrgen; }
    virtual bool is_ready() { return false; }
    virtual void check_state() {}
    virtual bool is_active() { return this->active; }
    virtual void set_active(bool active) { this->active = active; }
    virtual bool is_rx() = 0;

    bool is_stream;
    int stream_id;

protected:
    vp::trace trace;
    udma *top;
    Udma_addrgen *addrgen;
    bool active;

private:
    void cfg_ctrl_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write);

    string name;
};



// RX channel can be instantiated to transfer data from device to L2
class Udma_rx_channel : public Udma_channel
{
public:
    Udma_rx_channel(udma *top, string name) : Udma_channel(top, name), waiting_active(false) {}

    void reset(bool active);

    bool is_rx() { return true; }

    // Can be called to push data to be sent to L2. Channel must be ready when calling it.
    void push_data(uint8_t *data, int size, int addrgen_id=-1);

    // Can be called to wait until the channel gets ready
    void wait_active();

    // Can be called to cancel the request to wait fot the channel to get ready
    void wait_active_cancel();

    // Called when the channel gets ready after waiting for it
    virtual void wait_active_done() {}

    // Tell if the channel is ready for sending L2 data (push_data can be called)
    bool is_ready();

    bool is_active();

    void set_active(bool active);

    void notify_wait_active_done();

private:
    bool waiting_active;
};



// TX channel can be instantiated to transfer data from L2 to device
class Udma_tx_channel : public Udma_channel
{
public:
    Udma_tx_channel(udma *top, string name)
        : Udma_channel(top, name),
        enqueued(false)
    {}

    void reset(bool active);

    bool is_rx() { return false; }

    // Is called when data has been read from L2 and is ready to be pushed to device.
    // Can be overloaded by the device to forward the data
    virtual void push_data(uint8_t *data, int size) {}

    // Can be called to ask the channel to read data from L2. The channel must be ready when calling it
    void get_data(int size, int channel = -1);

    // Tell if the channel is ready to ready data from L2 (get_data can be called)
    bool is_ready();

    void check_state();

    void req_data();

    void set_active(bool active);

    std::deque<int> requested_size_queue;
    int requested_size;
    bool enqueued;
};



// Internal class managing RX channels
class Udma_rx_channels
{
public:
    Udma_rx_channels(udma *top, int fifo_size);
    bool is_ready();
    void push_data(uint8_t *data, int size, Udma_addrgen *addrgen, int addrgen_id);
    void check_state();
    static void handle_pending(void *__this, vp::clock_event *event);
    static void handle_waiting(void *__this, vp::clock_event *event);
    static void stream_in_ready_sync(void *__this, bool ready, int id);
    std::vector<Udma_rx_channel *> stream_in_channels;
    void add_waiting_channel(Udma_rx_channel *channel) { this->waiting_channels.push(channel); }

private:
    void check_waiting_channels();

    udma *top;

    Udma_queue<vp::io_req> *l2_free_reqs;    // L2 free requests to write to L2
    Udma_queue<vp::io_req> *l2_waiting_reqs; // L2 requests waiting for completion before being pushed back to l2_free_reqs

    Udma_queue<Udma_request> *fifo_free;  // Fifo free entries where incoming data is stored until it is pushed to L2
    Udma_queue<Udma_request> *fifo_ready; // Fifo ready entries containing incoming data and which will be pushed to L2 using l2 requests

    vp::clock_event *send_reqs_event;    // Event used for sending fifo entries to L2
    vp::clock_event *waiting_reqs_event; // Event used for processing l2 requests waiting for completion

    std::vector<vp::wire_master<uint32_t> *> stream_in_data_itf;
    std::vector<vp::wire_slave<bool> *> stream_in_ready_itf;

    std::queue<Udma_rx_channel *> waiting_channels;
};



class Udma_tx_channels
{
public:
    Udma_tx_channels(udma *top, int fifo_size);
    bool is_ready();
    void check_state();
    static void handle_pending(void *__this, vp::clock_event *event);
    static void handle_waiting(void *__this, vp::clock_event *event);
    void push_ready_channel(Udma_tx_channel *channel);
    void pop_ready_channel(Udma_tx_channel *channel);
    static void stream_out_data_sync(void *__this, uint32_t data, int id);
    uint32_t stream_out_data_pop(int id);
    std::vector<vp::wire_master<bool> *> stream_out_ready_itf;
    std::vector<Udma_tx_channel *> stream_out_channels;
    std::vector<uint32_t> stream_out_data;
    std::vector<bool> stream_out_data_ready;
    void reset(bool active);
    void stream_out_ready_set(int id, bool ready);

private:
    udma *top;

    Udma_queue<vp::io_req> *l2_free_reqs;
    Udma_queue<vp::io_req> *l2_waiting_reqs;

    std::queue<Udma_tx_channel *> pending_channels;

    vp::clock_event *send_reqs_event;
    vp::clock_event *waiting_reqs_event;

    std::vector<vp::wire_slave<uint32_t> *> stream_out_data_itf;
};



class Io_rx_channel : public Udma_rx_channel
{
    friend class Io_Periph;

public:
    Io_rx_channel(udma *top, Io_Periph *io_periph, int id, string name);

private:
    Io_Periph *io_periph;
};



class Io_tx_channel : public Udma_tx_channel
{
    friend class Io_Periph;

public:
    Io_tx_channel(udma *top, Io_Periph *io_periph, int id, string name);

protected:
    void handle_ready_reqs();

private:
    Io_Periph *io_periph;
};



class Udma_periph
{
public:
    Udma_periph(udma *top, int id);
    vp::io_req_status_e req(vp::io_req *req, uint64_t offset);
    virtual void reset(bool active);
    virtual void start() {}
    void clock_gate(bool is_on);

    int id;
    udma *top;

protected:
    Udma_channel *channel0 = NULL;
    Udma_channel *channel1 = NULL;
    Udma_channel *channel2 = NULL;

private:
    virtual vp::io_req_status_e custom_req(vp::io_req *req, uint64_t offset);
    bool is_on;
    bool reset_active;
};



class Io_Periph : public Udma_periph
{
    friend class Io_tx_channel;
    friend class Io_rx_channel;

public:
    Io_Periph(udma *top, int id, string itf_name);
    void handle_ready_reqs();
    void set_addr(unsigned int addr)
    {
        this->addr = addr;
        this->current_addr = addr;
    }
    void set_eot_event(int event) { this->eot_event = event; }

private:
    static void data_grant(void *_this, vp::io_req *req);
    static void data_response(void *_this, vp::io_req *req);
    static void handle_pending_word(void *__this, vp::clock_event *event);
    void check_state();

    vp::io_master io_itf;
    vp::clock_event *pending_access_event;
    vp::io_req *pending_req;
    vp::io_req io_req;
    unsigned int addr;
    unsigned int current_addr;
    int eot_event = -1;
};



/*
 * UDMA
 */

template <class T>
inline void Udma_queue<T>::push(T *cmd)
{
    if (first)
        last->set_next(cmd);
    else
        first = cmd;
    cmd->set_next(NULL);
    last = cmd;
    nb_cmd++;
}



template <class T>
T *Udma_queue<T>::pop()
{
    if (!first)
        return NULL;
    T *cmd = first;
    first = cmd->get_next();
    nb_cmd--;

    return cmd;
}



template <class T>
void Udma_queue<T>::push_from_latency(T *cmd)
{
    T *current = first, *prev = NULL;
    while (current && cmd->get_latency() > current->get_latency())
    {
        prev = current;
        current = current->get_next();
    }

    if (current == NULL)
        last = cmd;

    if (prev)
        prev->set_next(cmd);
    else
        first = cmd;
    cmd->set_next(current);
    nb_cmd++;
}

template <class T>
void Udma_queue<T>::remove(T *cmd)
{
    T *current = first, *prev = NULL;
    while (current != cmd)
    {
        prev = current;
        current = current->get_next();
    }

    if (prev)
        prev->set_next(cmd->get_next());
    else
        first = cmd;
    nb_cmd--;
}


template <class T>
Udma_fifo<T>::Udma_fifo(vp::component *top, std::string name, int size)
: size(size)
{
    top->new_reg(name + "/nb_elems", &this->nb_elems, 0);
    top->new_reg(name + "/is_full", &this->reg_is_full, 0);
}

template <class T>
inline void Udma_fifo<T>::push(T cmd)
{
    this->queue.push(cmd);
    this->nb_elems.set(this->nb_elems.get() + 1);
    this->reg_is_full.set(this->is_full());
}



template <class T>
T Udma_fifo<T>::pop()
{
    T result = this->queue.front();
    this->queue.pop();

    this->nb_elems.set(this->nb_elems.get() - 1);
    this->reg_is_full.set(this->is_full());

    return result;
}



class udma : public vp::component
{
    friend class Udma_periph;
    friend class Udma_rx_channel;
    friend class Udma_rx_channels;
    friend class Udma_tx_channels;

public:
    udma(js::config *config);

    int build();
    void start();
    void reset(bool active);

    void trigger_event(int event);

    vp::trace *get_trace() { return &this->trace; }
    vp::clock_engine *get_sfu_clock() { return this->sfu_clock; }
    vp::clock_engine *get_periph_clock() { return this->periph_clock; }
    vp::clock_engine *get_periph_clock_dual_edges() { return this->periph_clock_dual_edges; }
    vp::clock_engine *get_fast_clock() { return this->fast_clock; }

    void channel_register(int id, Udma_channel *channel);
    void channel_unregister(int id, Udma_channel *channel);

    void busy_set(int inc);

    vp::io_master l2_itf;

    Udma_rx_channels *rx_channels;
    Udma_tx_channels *tx_channels;

protected:
    vp::trace trace;
    int nb_udma_stream_in;
    int nb_udma_stream_out;

private:
    vp_regmap_udma_ctrl ctrl_regmap;

    void cfg_cg_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write);
    void cfg_cg_set_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write);
    void cfg_cg_clr_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write);
    void cfg_rstn_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write);
    void cfg_rstn_set_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write);
    void cfg_rstn_clr_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write);
    vp::io_req_status_e conf_req(vp::io_req *req, uint64_t offset);
    vp::io_req_status_e periph_req(vp::io_req *req, uint64_t offset);
    vp::io_req_status_e channel_req(vp::io_req *req, uint64_t offset);
    static vp::io_req_status_e req(void *__this, vp::io_req *req);
    static void l2_grant(void *__this, vp::io_req *req);
    static void sfu_reset_sync(void *__this, bool value);
    static void l2_response(void *__this, vp::io_req *req);
    static void clk_reg(component *_this, component *clock);
    static void sfu_clk_reg(component *_this, component *clock);
    static void dual_edges_clk_reg(component *_this, component *clock);
    static void fast_clk_reg(component *_this, component *clock);

    vp::io_slave in;
    vp::clk_slave periph_clock_itf;
    vp::wire_slave<bool> sfu_reset_itf;
    vp::clk_slave sfu_clock_itf;
    vp::clock_engine *periph_clock;
    vp::clock_engine *sfu_clock;
    vp::clk_slave periph_clock_dual_edges_itf;
    vp::clock_engine *periph_clock_dual_edges;
    vp::clk_slave fast_clock_itf;
    vp::clock_engine *fast_clock;

    vp::reg_1 busy;
    int busy_count;

    int nb_periphs;
    int l2_read_fifo_size;
    int l2_write_fifo_size;
    int nb_channels;
    std::vector<Udma_periph *> periphs;
    Sfu_periph *sfu_periph;
    uint32_t clock_gating;
    uint32_t ctrl_reset;

    vp::wire_master<int> event_itf;

    std::vector<Udma_channel *> channels;

    int nb_addrgen_linear;
    int nb_addrgen_2d;

    std::vector<Udma_addrgen_linear *> addrgen_linear;
    std::vector<Udma_addrgen_2d *> addrgen_2d;
};

#endif
