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

class udma;
class Udma_channel;
class Udma_rx_channels;
class Udma_tx_channel;
class Io_Periph;


class Udma_channel;


class Udma_addrgen
{
public:
    Udma_addrgen(udma *top, int id, int event_id) : top(top), id(id), event_id(event_id) {}
    virtual vp::io_req_status_e access(uint64_t offset, int size, uint8_t *value, bool is_write) = 0;
    virtual void get_next_transfer(uint32_t *addr, int *size) = 0;
    virtual void reset(bool active) = 0;
    virtual bool is_active() = 0;
    void register_channel(Udma_channel *channel) { this->channel = channel; }

    int id;
    udma *top;
    vp::trace trace;
    Udma_channel *channel;
    int event_id;
};



class Udma_addrgen_linear : public Udma_addrgen
{
public:
    Udma_addrgen_linear(udma *top, int id, int event_id);
    vp::io_req_status_e access(uint64_t offset, int size, uint8_t *value, bool is_write);
    void get_next_transfer(uint32_t *addr, int *size);
    void reset(bool active);
    bool is_active() { return this->active_transfer; }

private:
    void cfg_ctrl_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write);
    void check_pending_transfer();

    vp_regmap_udma_core_lin_addrgen regmap;

    uint32_t pending_addr;
    uint32_t pending_size;

    uint32_t current_addr;
    uint32_t current_size;

    int nb_pending_transfers;
    bool active_transfer;
};



class Udma_addrgen_2d : public Udma_addrgen
{
public:
    Udma_addrgen_2d(udma *top, int id, int event_id);
    vp::io_req_status_e access(uint64_t offset, int size, uint8_t *value, bool is_write);
    void get_next_transfer(uint32_t *addr, int *size);
    void reset(bool active);
    bool is_active() { return this->active_transfer; }

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
    bool active_transfer;
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



class Udma_request
{
public:
    void set_next(Udma_request *next) { this->next = next; }
    Udma_request *get_next() { return next; }
    Udma_request *next;

    uint32_t data;
    int size;
    int offset;
    Udma_addrgen *addrgen;
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
    Udma_addrgen *get_addrgen() { return this->addrgen; }
    virtual bool is_ready() { return false; }
    virtual void check_state() {}
    bool is_active() { return this->addrgen ? this->addrgen->is_active() : false; }

protected:
    vp::trace trace;
    udma *top;
    Udma_addrgen *addrgen;

private:
    void cfg_ctrl_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write);

    string name;
};



// RX channel can be instantiated to transfer data from device to L2
class Udma_rx_channel : public Udma_channel
{
public:
    Udma_rx_channel(udma *top, string name) : Udma_channel(top, name) {}

    void reset(bool active);

    // Can be called to push data to be sent to L2. Channel must be ready when calling it.
    void push_data(uint8_t *data, int size);

    // Tell if the channel is ready for sending L2 data (push_data can be called)
    bool is_ready();
};



// TX channel can be instantiated to transfer data from L2 to device
class Udma_tx_channel : public Udma_channel
{
public:
    Udma_tx_channel(udma *top, string name) : Udma_channel(top, name) {}

    void reset(bool active);

    // Is called when data has been read from L2 and is ready to be pushed to device.
    // Can be overloaded by the device to forward the data
    virtual void push_data(uint8_t *data, int size) {}

    // Can be called to ask the channel to read data from L2. The channel must be ready when calling it
    void get_data(int size);

    // Tell if the channel is ready to ready data from L2 (get_data can be called)
    bool is_ready();

    void check_state();

    void req_data();


    int requested_size;
};



// Internal class managing RX channels
class Udma_rx_channels
{
public:
    Udma_rx_channels(udma *top, int fifo_size);
    bool is_ready();
    void push_data(uint8_t *data, int size, Udma_addrgen *addrgen);
    void check_state();
    static void handle_pending(void *__this, vp::clock_event *event);
    static void handle_waiting(void *__this, vp::clock_event *event);

private:
    udma *top;

    Udma_queue<vp::io_req> *l2_free_reqs;    // L2 free requests to write to L2
    Udma_queue<vp::io_req> *l2_waiting_reqs; // L2 requests waiting for completion before being pushed back to l2_free_reqs

    Udma_queue<Udma_request> *fifo_free;  // Fifo free entries where incoming data is stored until it is pushed to L2
    Udma_queue<Udma_request> *fifo_ready; // Fifo ready entries containing incoming data and which will be pushed to L2 using l2 requests

    vp::clock_event *send_reqs_event;    // Event used for sending fifo entries to L2
    vp::clock_event *waiting_reqs_event; // Event used for processing l2 requests waiting for completion
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

private:
    udma *top;

    Udma_queue<vp::io_req> *l2_free_reqs;
    Udma_queue<vp::io_req> *l2_waiting_reqs;

    std::queue<Udma_tx_channel *> pending_channels;

    vp::clock_event *send_reqs_event;
    vp::clock_event *waiting_reqs_event;
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
    vp::clock_engine *get_periph_clock() { return this->periph_clock; }

    void channel_register(int id, Udma_channel *channel);
    void channel_unregister(int id, Udma_channel *channel);

    vp::io_master l2_itf;

    Udma_rx_channels *rx_channels;
    Udma_tx_channels *tx_channels;

protected:
    vp::trace trace;

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
    static void l2_response(void *__this, vp::io_req *req);
    static void clk_reg(component *_this, component *clock);

    vp::io_slave in;
    vp::clk_slave periph_clock_itf;
    vp::clock_engine *periph_clock;

    int nb_periphs;
    int l2_read_fifo_size;
    int nb_channels;
    std::vector<Udma_periph *> periphs;
    uint32_t clock_gating;
    uint32_t ctrl_reset;

    vp::wire_master<int> event_itf;

    std::vector<Udma_channel *> channels;

    int nb_addrgen_linear;
    int nb_addrgen_2d;

    std::vector<Udma_addrgen_linear *> addrgen_linear;
    std::vector<Udma_addrgen_2d *> addrgen_2d;
};

#ifdef HAS_SPIM
#include "spim/udma_spim_v4.hpp"
#endif

#ifdef HAS_UART
#include "uart/udma_uart_v2.hpp"
#endif

//#ifdef HAS_I2S
#include "i2s/udma_i2s_v3.hpp"
//#endif

#ifdef HAS_HYPER
#include "hyper/udma_hyper_v3.hpp"
#endif

#ifdef HAS_CPI
#include "cpi/udma_cpi_v2.hpp"
#endif

#ifdef HAS_AES
#include "aes/udma_aes_v1.hpp"
#endif

#ifdef HAS_SFU
#include "sfu/udma_sfu_v1.hpp"
#endif

#ifdef HAS_EMPTY_SFU
#include "sfu/udma_sfu_v1_empty.hpp"
#endif

#ifdef HAS_FFC
#include "ffc/udma_ffc_v1.hpp"
#endif

#endif
