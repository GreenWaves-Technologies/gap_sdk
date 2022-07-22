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

#ifndef __PULP_UDMA_UDMA_MRAM_V3_IMPL_HPP__
#define __PULP_UDMA_UDMA_MRAM_V3_IMPL_HPP__

#include <vp/vp.hpp>
#include "../udma_impl.hpp"
#include "../dc_fifo.hpp"
#include <vp/itf/wire.hpp>
#include <stdio.h>
#include <string.h>
#include <vector>
#include "pulp/mram/mram.hpp"
#include <udma_mram/udma_mram_regs.h>
#include <udma_mram/udma_mram_regfields.h>
#include <udma_mram/udma_mram_gvsoc.h>
#include "../udma_mem_refill.hpp"


#define MRAM_LINE_SIZE 16


/*
 * States of the UDMA MRAM peripheral
 */
typedef enum
{
    MRAM_STATE_IDLE,                      // No activity
    MRAM_STATE_HANDLE_OPERATION,          // The current operation should be handled
    MRAM_STATE_SEND_ADDR,                 // Periph should send addr to udma core
    MRAM_STATE_SEND_SIZE,                 // Periph should send size to udma core
    MRAM_STATE_SEND_CFG,                  // Periph should send config to udma core
    MRAM_STATE_HANDLE_TRIM,               // Periph should send trim config
    MRAM_STATE_HANDLE_TRIM_ONGOING,       // Trim config being sent
    MRAM_STATE_HANDLE_PROGRAM,            // Periph should send data to be written to mram
    MRAM_STATE_HANDLE_PROGRAM_ONGOING,    // Data being sent to be written to mram
    MRAM_STATE_WAIT_SYNC_BACK,            // Periph waiting synchronization from mram
    MRAM_STATE_HANDLE_READ,               // Periph should read data from mram
    MRAM_STATE_PENDING_OP                 // An operation is on-going
} Mram_state_e;



class Mram_periph;

/*
 * Class used to model the UDMA MRAM periph rx channel
 */
class Mram_rx_channel : public Udma_rx_channel
{
public:
    Mram_rx_channel(udma *top, Mram_periph *periph, string name);

private:
    Mram_periph *periph;
};


/*
 * Class used to model the UDMA MRAM periph tx channel
 */
class Mram_tx_channel : public Udma_tx_channel
{
    friend class Mram_periph;

public:
    Mram_tx_channel(udma *top, Mram_periph *periph, string name);
    void push_data(uint8_t *data, int size);

protected:

private:
    Mram_periph *periph;
};


/*
 * Class used to model the UDMA MRAM periph
 *
 */
class Mram_periph : public Udma_periph
{
    friend class Mram_tx_channel;
    friend class Mram_rx_channel;

public:
    Mram_periph(udma *top, int id, int itf_id);
    vp::io_req_status_e custom_req(vp::io_req *req, uint64_t offset);
    void reset(bool active);
    void start();

protected:
    Mram_tx_channel *tx_channel;
    Mram_rx_channel *rx_channel;

private:
    void clk_div_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write);
    void trans_cfg_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write);
    void tx_dest_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write);
    void rx_dest_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write);
    static void handle_pending_operation(void *__this, vp::clock_event *event);
    static void handle_read_end(void *__this, vp::clock_event *event);
    static void handle_read_2d(void *__this, vp::clock_event *event);
    static void handle_rcv_data(void *__this, vp::clock_event *event);
    static void handle_dc_fifo_unstall(void *__this, vp::clock_event *event);
    void check_state();
    void push_data(uint8_t *data, int size);
    static void sync_req(void *__this, mram_req_t *req);
    static void sync_data(void *__this, mram_data_t *req);
    void handle_program();
    void handle_trim();
    void enqueue_read_req();
    static void handle_read_req(void *__this, vp::clock_event *event);
    int64_t get_mram_cycle_count(int64_t cycles);
    static void refill_req(void *__this, udma_refill_req_t *req);

    vp::wire_master<mram_req_t *> out_req;    // Output interface to mram for requests
    vp::wire_master<mram_data_t *> out_data;  // Output interface to mram for data
    vp::wire_master<mram_conf_t *> out_conf;  // Output interface to mram for configuration

    vp_regmap_udma_mram regmap;     // Register map
    vp::clock_event *event;         // Event for managing the FSM
    int pending_transfer_size;      // Remaining size of the whole transfer
    int req_size;                   // Size of the last request sent to mram
    Mram_state_e state;             // State of the mram periph
    Mram_state_e next_state;        // Next state of the mram periph that should be set once
                                    // the periph has setup the udma core
    int write_pending_size;         // Remaining size of the last program request sent to mram
    int read_pending_size;          // Remaining size of the last request sent to mram
    int read_pending_length;        // Remaining size of the current 2d line being read
    uint32_t pending_addr;          // Current mram address of the pending transfer
    uint32_t pending_trans_addr;    // Current L2 address of the pending transfer
    uint32_t pending_trans_size;    // L2 size of the current line

    int erase_event;    // Soc event number of erase operation
    int tx_event;       // Soc event number of tx operation
    int trim_event;     // Soc event number of trim config operation
    int rx_event;       // Soc event number of rx operation

    uint8_t line_data[MRAM_LINE_SIZE];   // Current data line read from mram, to be sent to L2
    uint8_t *read_pending_data;          // Pointer to the current data to be sent inside the data line

    Dc_fifo *read_fifo;   // DC fifo for reading from mram.

    vp::trace     trace;     // Periph trace
    std::string itf_name;    // Name of the mram interface

    uint8_t *stalled_data;   // Pending mram data to be sent when DC fifo is unstalled
    int stalled_size;        // Pending mram data size to be sent when DC fifo is unstalled
    bool stalled;            // Set to true when mram data could not be sent because the DC fifo was full

    vp::clock_event *read_end_event;   // Event enqueued when the end of read event should be triggered
    vp::clock_event *read_2d_event;    // Event used to enqueue a new line in 2d mode
    vp::clock_event *rcv_event;        // Event enqueued by DC fifo when data is available in the FIFO
    vp::clock_event *read_event;       // Event enqueued to start a read request

    vp::wire_slave<udma_refill_req_t *> refill_itf;
    udma_refill_req_t *pending_refill_req;
    bool is_refill_req;
};

#endif
