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
#include "spim_verif.hpp"


#define CMD_BUFFER_SIZE 256



typedef struct
{
    uint8_t frame_start;
    uint8_t cmd;
    uint32_t address;
    uint8_t size_minus_1;
    uint8_t crc;
} __attribute__((packed)) spi_boot_req_t;


Spim_verif::Spim_verif(Testbench *top, Spi *spi, vp::qspim_slave *itf, pi_testbench_req_spim_verif_setup_t *config)
  : vp::time_engine_client(NULL)
{
    this->slave_boot_jump = false;
    ::memcpy(&this->config, config, sizeof(pi_testbench_req_spim_verif_setup_t));
    int itf_id = config->itf;
    int cs = config->cs;
    int mem_size = (1<<config->mem_size_log2) + CMD_BUFFER_SIZE;
    this->itf = itf;
    this->mem_size = mem_size;
    verbose = true; //config->get("verbose")->get_bool();
    //print("Creating SPIM VERIF model (mem_size: 0x%x)", this->mem_size);
    data = new unsigned char[mem_size];
    wait_cs = false;
    this->current_cs = 1;
    this->tx_file = NULL;
    this->top = top;
    this->spi = spi;
    this->is_master = config->is_master;

    this->engine = (vp::time_engine*)this->top->get_service("time");

    std::string name = "spim_verif_itf" + std::to_string(itf_id) + "_cs" + std::to_string(cs);

    top->traces.new_trace(name, &trace, vp::DEBUG);
        
    this->init_pads = false;

    if (config->is_master)
    {
        this->enqueue_to_engine(1);
        this->init_pads = true;
    }

#if 0
  js::config *tx_file_config = config->get("tx_file");
  if (tx_file_config != NULL)
  {
    js::config *path_config = tx_file_config->get("path");
    js::config *qpi_config = tx_file_config->get("qpi");
    this->tx_dump_bits = 0;
    this->tx_file = fopen(path_config->get_str().c_str(), "wb");
    this->tx_dump_qpi = qpi_config != NULL && qpi_config->get_bool();
  }
#endif

    this->slave_state = SPIS_STATE_IDLE;
    this->slave_boot_state = SPIS_BOOT_STATE_IDLE;
    this->spi_load_config = NULL;
    this->is_enqueued = false;
    this->handle_wakeup = false;
    this->do_spi_load = false;
}

void Spim_verif::handle_read(uint64_t cmd, bool is_quad)
{
    this->is_quad = is_quad;
    int size = SPIM_VERIF_FIELD_GET(cmd, SPIM_VERIF_CMD_INFO_BIT, SPIM_VERIF_CMD_INFO_WIDTH);
    current_addr = SPIM_VERIF_FIELD_GET(cmd, SPIM_VERIF_CMD_ADDR_BIT, SPIM_VERIF_CMD_ADDR_WIDTH);
    command_addr = current_addr;

    this->trace.msg(vp::trace::LEVEL_INFO, "Handling read command (size: 0x%x, addr: 0x%x, is_quad: %d)\n", size, current_addr, is_quad);

    state = STATE_READ_CMD;
    current_size = size;
    nb_bits = 0;
    wait_cs = true;
    //dummy_cycles = 1;
}

void Spim_verif::handle_write(uint64_t cmd, bool is_quad)
{
    this->is_quad = is_quad;
    int size = SPIM_VERIF_FIELD_GET(cmd, SPIM_VERIF_CMD_INFO_BIT, SPIM_VERIF_CMD_INFO_WIDTH);
    current_write_addr = SPIM_VERIF_FIELD_GET(cmd, SPIM_VERIF_CMD_ADDR_BIT, SPIM_VERIF_CMD_ADDR_WIDTH);
    command_addr = current_write_addr;

    this->trace.msg(vp::trace::LEVEL_INFO, "Handling write command (size: 0x%x, addr: 0x%x, is_quad: %d)\n", size, current_write_addr, is_quad);

    state = STATE_WRITE_CMD;
    current_write_size = size;
    nb_write_bits = 0;
}

void Spim_verif::handle_full_duplex(uint64_t cmd)
{
    int size = SPIM_VERIF_FIELD_GET(cmd, SPIM_VERIF_CMD_INFO_BIT, SPIM_VERIF_CMD_INFO_WIDTH);
    current_addr = SPIM_VERIF_FIELD_GET(cmd, SPIM_VERIF_CMD_ADDR_BIT, SPIM_VERIF_CMD_ADDR_WIDTH);
    command_addr = current_addr;

    this->trace.msg(vp::trace::LEVEL_INFO, "Handling full duplex command (size: 0x%x, addr: 0x%x, is_quad: %d)\n", size, current_addr, is_quad);

    state = STATE_FULL_DUPLEX_CMD;
    current_write_addr = current_addr;
    current_size = size;
    current_write_size = size;
    nb_bits = 0;
    nb_write_bits = 0;
    wait_cs = true;
}

void Spim_verif::exec_read()
{
    if (dummy_cycles)
    {
        dummy_cycles--;
        return;
    }

    if (nb_bits == 0)
    {
        if (this->current_addr >= this->mem_size)
        {
            //this->fatal("Trying to read outside memory range (addr: 0x%x, mem size: 0x%x)\n", this->current_addr, this->mem_size);
        }
        else
        {
            byte = data[current_addr];
            this->trace.msg(vp::trace::LEVEL_DEBUG, "Read byte from memory (value: 0x%x, rem_size: 0x%x)\n", byte, current_size);
        }
        nb_bits = 8;
        current_addr++;
    }

    if (this->is_quad)
    {
        int data_0 = (byte >> 4) & 1;
        int data_1 = (byte >> 5) & 1;
        int data_2 = (byte >> 6) & 1;
        int data_3 = (byte >> 7) & 1;

        byte <<= 4;

        this->itf->sync(2, data_0, data_1, data_2, data_3, 0xf);

        nb_bits -= 4;
        current_size -= 4;
    }
    else
    {
        int bit = (byte >> 7) & 1;
        byte <<= 1;

        this->itf->sync(2, 2, bit, 2, 2, 0x2);

        nb_bits--;
        current_size--;
    }
    if (current_size == 0)
    {
        if (state == STATE_READ_CMD)
        {
            wait_cs = true;
            state = STATE_GET_CMD;
        }
    }
}


void Spim_verif::exec_write(int sdio0, int sdio1, int sdio2, int sdio3)
{
    if (this->is_quad)
    {
        pending_write = (pending_write << 4) | (sdio3 << 3) | (sdio2 << 2) | (sdio1 << 1) | (sdio0 << 0);;
        nb_write_bits +=4;
        current_write_size -= 4;
    }
    else
    {
        pending_write = (pending_write << 1) | sdio0;
        nb_write_bits++;
        current_write_size--;
    }
    if (nb_write_bits == 8)
    {
        if (this->current_write_addr >= this->mem_size)
        {
            //this->fatal("Trying to write outside memory range (addr: 0x%x, mem size: 0x%x)\n", this->current_write_addr, this->mem_size);
        }
        else
        {
            data[current_write_addr] = pending_write;

            this->trace.msg(vp::trace::LEVEL_DEBUG, "Wrote byte to memory (addr: 0x%x, value: 0x%x, rem_size: 0x%x)\n", current_write_addr, data[current_write_addr], current_write_size-1);
        }

        nb_write_bits = 0;
        current_write_addr++;
    }

    if (current_write_size == 0)
    {
        if (nb_write_bits != 0)
        {
            int shift = 8 - nb_write_bits;
            pending_write = (data[current_write_addr] & ~((1 << shift) - 1)) | (pending_write << shift);
            this->trace.msg(vp::trace::LEVEL_DEBUG, "Wrote byte to memory (value: 0x%x)\n", data[current_write_addr]);
        }
        wait_cs = true;
        state = STATE_GET_CMD;
    }
}

void Spim_verif::handle_command(uint64_t cmd)
{
   this->trace.msg(vp::trace::LEVEL_INFO, "Handling command %x\n", current_cmd);

    int cmd_id = SPIM_VERIF_FIELD_GET(cmd, SPIM_VERIF_CMD_BIT, SPIM_VERIF_CMD_WIDTH);

    switch (cmd_id)
    {
    case SPIM_VERIF_CMD_WRITE:
        handle_write(cmd, false);
        break;
    case SPIM_VERIF_CMD_READ:
        handle_read(cmd, false);
        break;
    case SPIM_VERIF_CMD_FULL_DUPLEX:
        handle_full_duplex(cmd);
        break;
    case SPIM_VERIF_CMD_WRITE_QUAD:
        handle_write(cmd, true);
        break;
    case SPIM_VERIF_CMD_READ_QUAD:
        handle_read(cmd, true);
        break;
    default: //print("WARNING: received unknown command (raw: 0x%llx, id: 0x%x)", cmd, cmd_id);
        break;
    }
}

void Spim_verif::cs_sync(int cs)
{
    if (this->is_master || this->current_cs == cs)
        return;

    this->current_cs = cs;

    this->trace.msg(vp::trace::LEVEL_DEBUG, "CS edge (cs: %d)\n", cs);
    if (cs == 1)
    {
        // Reset pending addresses to detect CS edge during command
        current_write_addr = command_addr;
        current_addr = command_addr;

        this->wait_cs = false;
        this->itf->sync(2, 2, 2, 2, 2, 0x1);
    }

    if (cs == 0)
    {
        if ((state == STATE_READ_CMD || state == STATE_FULL_DUPLEX_CMD) && this->config.phase == 0)
        {
            exec_read();
        }
    }
}

void Spim_verif::exec_dump_single(int sdio0)
{
    this->tx_dump_byte = (this->tx_dump_byte << 1) | sdio0;
    this->tx_dump_bits++;
    if (this->tx_dump_bits == 8)
    {
        this->tx_dump_bits = 0;
        fwrite((void *)&this->tx_dump_byte, 1, 1, this->tx_file);
    }
}

void Spim_verif::exec_dump_qpi(int sdio0, int sdio1, int sdio2, int sdio3)
{
    this->tx_dump_byte = (this->tx_dump_byte << 4) | (sdio3 << 3) | (sdio2 << 2) | (sdio1 << 1) | sdio0;
    this->tx_dump_bits += 4;
    if (this->tx_dump_bits == 8)
    {
        this->tx_dump_bits = 0;
        fwrite((void *)&this->tx_dump_byte, 1, 1, this->tx_file);
    }
}

void Spim_verif::handle_clk_high(int sdio0, int sdio1, int sdio2, int sdio3, int mask)
{
    if (wait_cs)
        return;

    if (this->tx_file != NULL)
    {
        if (this->tx_dump_qpi)
            exec_dump_qpi(sdio0, sdio1, sdio2, sdio3);
        else
            exec_dump_single(sdio0);
        // TODO properly destroy the model and close the logfile instead of flushing
        fflush(NULL);
    }

    if (state == STATE_GET_CMD)
    {
        current_cmd = (current_cmd << 1) | sdio0;
        this->trace.msg(vp::trace::LEVEL_DEBUG, "Received command bit (count: %d, pending: %x, bit: %d)\n", cmd_count, current_cmd, sdio0);
        cmd_count++;
        if (cmd_count == 64)
        {
            cmd_count = 0;
            handle_command(current_cmd);
        }
    }
    else if (state == STATE_WRITE_CMD || state == STATE_FULL_DUPLEX_CMD)
    {
        exec_write(sdio0, sdio1, sdio2, sdio3);
    }
}

void Spim_verif::handle_clk_low(int sdio0, int sdio1, int sdio2, int sdio3, int mask)
{
    if (wait_cs)
        return;

    if (state == STATE_READ_CMD || state == STATE_FULL_DUPLEX_CMD)
    {
        exec_read();
    }
}

void Spim_verif::sync(int sck, int sdio0, int sdio1, int sdio2, int sdio3, int mask)
{
    this->trace.msg(vp::trace::LEVEL_TRACE, "SCK edge (sck: %d, data_0: %d, data_1: %d, data_2: %d, data_3: %d, mask: 0x%x)\n", sck, sdio0, sdio1, sdio2, sdio3, mask);

    sck ^= this->config.polarity;

    if (this->is_master)
    {
        this->slave_pending_miso = sdio0;
    }
    else
    {
        if (prev_sck != sck)
        {
            int high = sck ^ this->config.phase;


            if (!high)
            {
                handle_clk_low(sdio0, sdio1, sdio2, sdio3, mask);
            }
            else
            {
                handle_clk_high(sdio0, sdio1, sdio2, sdio3, mask);
            }
        }
        prev_sck = sck;
    }
}


int64_t Spim_verif::exec()
{
    bool clock_toggle = false;

    if (this->init_pads)
    {
        this->init_pads = false;
        this->itf->sync(0, 2, 2, 2, 2, 0xf);
        this->spi->cs_itf.sync(1);
    }

    if (this->spi_load_config)
    {
        this->spi_load(this->spi_load_config);
        int64_t delay = this->spi_load_config->get_int("delay_ps");
        this->spi_load_config = NULL;
        return delay;
    }

    if (this->handle_wakeup)
    {
        this->handle_wakeup = false;
        this->slave_boot_state = SPIS_BOOT_STATE_SEND_WAKEUP_CMD;
        this->handle_boot_protocol_transfer();
    }

    if (this->slave_sck == 0)
    {
        switch (this->slave_state)
        {
            case SPIS_STATE_START_WAIT_CYCLES:
                this->trace.msg(vp::trace::LEVEL_TRACE, "SPI master start wait cycles (cycles: %d)\n", this->slave_wait_cycles);
                clock_toggle = true;
                this->slave_wait_cycles--;
                if (this->slave_wait_cycles == 0)
                {
                    this->slave_state = SPIS_STATE_CS_START_PRE_START_CYCLES;
                    this->slave_wait_cycles = 5;
                }
                break;

            case SPIS_STATE_CS_START_PRE_START_CYCLES:
                this->trace.msg(vp::trace::LEVEL_TRACE, "SPI master cs start wait cycles (cycles: %d)\n", this->slave_wait_cycles);
                this->slave_wait_cycles--;
                if (this->slave_wait_cycles == 0)
                {
                    this->slave_state = SPIS_STATE_CS_START;
                }
                break;

            case SPIS_STATE_CS_START:
                this->trace.msg(vp::trace::LEVEL_TRACE, "SPI master cs start\n");
                this->spi->cs_itf.sync(0);
                this->slave_state = SPIS_STATE_CS_START_POST_WAIT_CYCLES;
                this->slave_wait_cycles = 5;
                break;

            case SPIS_STATE_CS_START_POST_WAIT_CYCLES:
                this->trace.msg(vp::trace::LEVEL_TRACE, "SPI master cs start wait cycles (cycles: %d)\n", this->slave_wait_cycles);
                this->slave_wait_cycles--;
                if (this->slave_wait_cycles == 0)
                {
                    this->slave_state = SPIS_STATE_TRANSFER;
                }
                break;

            case SPIS_STATE_TRANSFER:
                clock_toggle = true;
                if (this->slave_pending_is_tx)
                {
                    if (this->slave_pending_tx_bits == 0)
                    {
                        this->slave_pending_tx_bits = 8;
                        this->slave_pending_tx_byte = this->data[this->slave_pending_tx_addr];
                        this->trace.msg(vp::trace::LEVEL_DEBUG, "Read from memory (address: 0x%lx, data: 0x%lx)\n", this->slave_pending_tx_addr, this->slave_pending_tx_byte);
                        this->slave_pending_tx_addr++;
                    }

                    this->slave_pending_mosi = (this->slave_pending_tx_byte >> 7) & 1;
                    this->slave_pending_tx_byte <<= 1;
                    this->slave_pending_tx_bits--;
                    if (this->slave_pending_tx_bits == 0)
                    {
                        this->slave_pending_tx_size--;
                    }
                    if (this->slave_pending_tx_size == 0 && !this->slave_pending_is_rx)
                    {
                        this->slave_state = SPIS_STATE_CS_END_WAIT_CYCLES;
                        this->slave_wait_cycles = 5;
                    }
                }
                break;

            case SPIS_STATE_CS_END_WAIT_CYCLES:
                this->trace.msg(vp::trace::LEVEL_TRACE, "SPI master cs end wait cycles (cycles: %d)\n", this->slave_wait_cycles);
                this->slave_wait_cycles--;
                this->slave_pending_mosi = 2;
                if (this->slave_wait_cycles == 0)
                {
                    this->slave_state = SPIS_STATE_CS_END;
                }
                break;

            case SPIS_STATE_CS_END:
                this->trace.msg(vp::trace::LEVEL_TRACE, "SPI master cs end\n");
                this->spi->cs_itf.sync(1);
                this->slave_state = SPIS_STATE_END_WAIT_CYCLES;
                this->slave_wait_cycles = 4; // GAP9_5 requires at least 4 dummy cycles
                break;

            case SPIS_STATE_END_WAIT_CYCLES:
                this->trace.msg(vp::trace::LEVEL_TRACE, "SPI master end wait cycles (cycles: %d)\n", this->slave_wait_cycles);
                clock_toggle = true;
                if (this->slave_wait_cycles == 0)
                {
                    this->slave_state = SPIS_STATE_IDLE;
                }
                else
                {
                    this->slave_wait_cycles--;
                }
                break;

        }
    }
    else
    {
        clock_toggle = true;

        switch (this->slave_state)
        {
            case SPIS_STATE_TRANSFER:
            if (this->slave_pending_is_rx)
            {
                this->slave_pending_rx_byte = (this->slave_pending_rx_byte << 1) | this->slave_pending_miso;
                this->trace.msg(vp::trace::LEVEL_TRACE, "SPI master sampled bit (bit: %d, value: 0x%x)\n", this->slave_pending_miso, this->slave_pending_rx_byte);
                this->slave_pending_rx_bits++;
                if (this->slave_pending_rx_bits == 8)
                {
                    this->trace.msg(vp::trace::LEVEL_DEBUG, "Writing to memory (address: 0x%lx, data: 0x%lx)\n", this->slave_pending_rx_addr, this->slave_pending_rx_byte & 0xff);
                    this->data[this->slave_pending_rx_addr] = this->slave_pending_rx_byte;
                    this->slave_pending_rx_size--;
                    this->slave_pending_rx_addr++;
                    this->slave_pending_rx_bits = 0;
                    if (this->slave_pending_rx_size == 0)
                    {
                        this->slave_state = SPIS_STATE_CS_END_WAIT_CYCLES;
                        this->slave_wait_cycles = 5;
                    }
                }
            }
            break;
        }
    }

    this->trace.msg(vp::trace::LEVEL_TRACE, "SPI master edge (sck: %d, mosi: %d, miso: %d)\n", this->slave_sck, this->slave_pending_mosi, this->slave_pending_miso);

    this->itf->sync(this->slave_sck, 2, this->slave_pending_mosi, 2, 2, 0xf);


    if (clock_toggle)
    {
        this->slave_sck ^= 1;
    }

    if (this->slave_state == SPIS_STATE_IDLE)
    {
        if (this->slave_boot_state != SPIS_BOOT_STATE_IDLE)
        {
            this->handle_boot_protocol_transfer();

            return this->slave_period;
        }
        else if (this->do_spi_load)
        {
            this->start_spi_load();
            return this->slave_period;
        }
        else
        {
            this->is_enqueued = false;
            return -1;
        }
    }
    else
    {
        return this->slave_period;
    }
}


void Spim_verif::transfer(pi_testbench_req_spim_verif_transfer_t *config)
{
    this->trace.msg(vp::trace::LEVEL_INFO, "Handling SPI transfer (frequency: %ld, address: 0x%lx, size: 0x%lx, is_master: %d, is_rx: %d, is_duplex: %d, is_boot_protocol: %d)\n",
        config->frequency, config->address, config->size, config->is_master, config->is_rx, config->is_duplex, config->is_boot_protocol
    );

    this->slave_period = 1000000000000ULL / config->frequency / 2;

    if (config->is_boot_protocol)
    {
        this->start_boot_protocol_transfer(config->address, config->size, config->is_rx);
    }
    else
    {
        this->enqueue_transfer(config->address, config->size, config->is_rx, config->is_duplex);
    }
}


void Spim_verif::spi_wakeup(pi_testbench_req_spim_verif_spi_wakeup_t *config)
{
    this->trace.msg(vp::trace::LEVEL_INFO, "Handling SPI wakeup (delay: %lld, mode: %d)\n",
        config->delay, config->mode
    );

    this->slave_period = 1000000000000ULL / config->frequency / 2;

    this->is_enqueued = 1;
    this->handle_wakeup = true;
    this->reload_spi = config->spi_reload;
    this->enqueue_to_engine(config->delay);
}


void Spim_verif::enqueue_boot_protocol_jump(int address)
{
    this->slave_boot_address = address;
    this->slave_boot_jump = true;

    this->slave_boot_state = SPIS_BOOT_STATE_SEND_SOF;

    this->handle_boot_protocol_transfer();
}



void Spim_verif::enqueue_boot_protocol_transfer(uint8_t *buffer, int address, int size, int is_rx)
{
    if (size > this->mem_size)
    {
        this->trace.fatal("SPI binary loading requiring too much memory\n");
        return;
    }

    memcpy(this->data, buffer, size);
    this->start_boot_protocol_transfer(address, size, is_rx);
}



void Spim_verif::start_boot_protocol_transfer(int address, int size, int is_rx)
{
    this->slave_boot_verif_address = 0;
    this->slave_boot_address = address;
    this->slave_boot_size = size;
    this->slave_boot_is_rx = is_rx;

    this->slave_boot_state = SPIS_BOOT_STATE_SEND_SOF;

    this->handle_boot_protocol_transfer();
}


void Spim_verif::handle_boot_protocol_transfer()
{
    switch (this->slave_boot_state)
    {
        case SPIS_BOOT_STATE_SEND_WAKEUP_CMD:
        {
            this->trace.msg(vp::trace::LEVEL_INFO, "Sending wakeup command (value: 0x3F)\n");

            this->data[this->mem_size] = 0x3F;
            this->enqueue_transfer(this->mem_size, 1, 0, 1);
            this->slave_boot_state = SPIS_BOOT_STATE_SEND_WAKEUP_STATUS;
            break;
        }


        case SPIS_BOOT_STATE_SEND_WAKEUP_STATUS:
        {
            this->trace.msg(vp::trace::LEVEL_INFO, "Sending wakeup status (value: 0x05)\n");

            this->data[this->mem_size] = 0x05;
            this->enqueue_transfer(this->mem_size, 2, 0, 1);
            this->slave_boot_state = SPIS_BOOT_STATE_SEND_WAKEUP_STATUS_RESPONSE;
            break;
        }


        case SPIS_BOOT_STATE_SEND_WAKEUP_STATUS_RESPONSE:
        {
            this->trace.msg(vp::trace::LEVEL_INFO, "Checking wakeup response (value: 0x%x)\n", this->data[this->mem_size + 1]);

            if (this->data[this->mem_size + 1] == 1)
            {
                this->slave_boot_state = SPIS_BOOT_STATE_SEND_WAKEUP_EXIT;
            }
            else
            {
                this->slave_boot_state = SPIS_BOOT_STATE_SEND_WAKEUP_STATUS;
            }
            break;
        }


        case SPIS_BOOT_STATE_SEND_WAKEUP_EXIT:
        {
            this->trace.msg(vp::trace::LEVEL_INFO, "Sending wakeup exit (value: 0x71)\n");

            this->data[this->mem_size] = 0x71;
            this->enqueue_transfer(this->mem_size, 1, 0, 0);
            if (this->reload_spi)
            {
                this->slave_boot_state = SPIS_BOOT_STATE_SEND_WAKEUP_DO_LOAD;
            }
            else
            {
                this->slave_boot_state = SPIS_BOOT_STATE_IDLE;
            }
            break;
        }


        case SPIS_BOOT_STATE_SEND_WAKEUP_DO_LOAD:
        {
            this->trace.msg(vp::trace::LEVEL_INFO, "Redoing SPI load\n");

            this->slave_boot_state = SPIS_BOOT_STATE_IDLE;
            this->do_spi_load = true;
            this->start_spi_load();
            break;
        }


        case SPIS_BOOT_STATE_SEND_SOF:
        {
            this->trace.msg(vp::trace::LEVEL_INFO, "Sending start of frame (value: 0x5A)\n");

            spi_boot_req_t *req = (spi_boot_req_t *)&this->data[this->mem_size];
            req->frame_start = 0x5a;
            req->cmd = this->slave_boot_jump ? 0x3 : this->slave_boot_is_rx ? 0x01 : 0x02;
            req->address = this->slave_boot_address;
            req->size_minus_1 = (this->slave_boot_size < CMD_BUFFER_SIZE ? this->slave_boot_size : CMD_BUFFER_SIZE) - 1;

            this->enqueue_transfer(this->mem_size, 1, 0, 1);
            this->slave_boot_state = SPIS_BOOT_STATE_CHECK_SOF;
            break;
        }

        case SPIS_BOOT_STATE_CHECK_SOF:
        {
            this->trace.msg(vp::trace::LEVEL_INFO, "Received start of frame (value: 0x%x)\n", this->data[mem_size]);

            if (this->data[mem_size] == 0xA5)
            {
                this->enqueue_transfer(this->mem_size+1, sizeof(spi_boot_req_t)-1, 0, 1);
                if (this->slave_boot_jump)
                {
                    this->slave_boot_jump = false;
                    this->slave_boot_state = SPIS_BOOT_STATE_IDLE;
                }
                else
                {
                    this->slave_boot_state = SPIS_BOOT_STATE_GET_ACK_STEP_0;
                }
            }
            else
            {
                this->slave_boot_state = SPIS_BOOT_STATE_SEND_SOF;
            }
            break;
        }

        case SPIS_BOOT_STATE_GET_ACK_STEP_0:
            this->trace.msg(vp::trace::LEVEL_INFO, "Read start of frame (value: 0x%x)\n", this->data[this->mem_size]);
            if (this->data[this->mem_size] == 0xa5)
            {
                this->slave_boot_state = SPIS_BOOT_STATE_GET_ACK_STEP_1;
            }
            else
            {
                this->slave_boot_state = SPIS_BOOT_STATE_SEND_SOF;
            }
            break;

        case SPIS_BOOT_STATE_GET_ACK_STEP_1:
            this->trace.msg(vp::trace::LEVEL_INFO, "Sending ack request (value: 0x00)\n");
            this->data[this->mem_size] = 0x00;
            this->enqueue_transfer(this->mem_size, 1, 0, 1);
            this->slave_boot_state = SPIS_BOOT_STATE_GET_ACK_STEP_2;
            break;

        case SPIS_BOOT_STATE_GET_ACK_STEP_2:
            this->trace.msg(vp::trace::LEVEL_INFO, "Read ack (value: 0x%x)\n", this->data[this->mem_size]);
            if (this->data[this->mem_size] == 0x79)
            {
                this->trace.msg(vp::trace::LEVEL_INFO, "Received ACK\n");
                if (this->slave_boot_is_rx)
                {
                    this->slave_boot_state = SPIS_BOOT_STATE_RECEIVE_CRC;
                }
                else
                {
                    this->slave_boot_state = SPIS_BOOT_STATE_SEND_DATA_CRC;
                }
            }
            else
            {
                this->slave_boot_state = SPIS_BOOT_STATE_GET_ACK_STEP_1;
            }
            break;

        case SPIS_BOOT_STATE_RECEIVE_CRC:
        {
            this->trace.msg(vp::trace::LEVEL_INFO, "Receiving CRC\n");
            this->enqueue_transfer(this->mem_size, 1, 1, 0);
            this->slave_boot_state = SPIS_BOOT_STATE_RECEIVE_DATA;
            break;
        }

        case SPIS_BOOT_STATE_RECEIVE_DATA:
        {
            int size = this->slave_boot_size < CMD_BUFFER_SIZE ? this->slave_boot_size : CMD_BUFFER_SIZE;
            this->trace.msg(vp::trace::LEVEL_INFO, "Receiving data (size: 0x%x)\n", size);
            this->enqueue_transfer(this->slave_boot_verif_address, size, 1, 0);
            this->slave_boot_state = SPIS_BOOT_STATE_RECEIVE_DATA_CRC;
            break;
        }

        case SPIS_BOOT_STATE_RECEIVE_DATA_CRC:
            this->trace.msg(vp::trace::LEVEL_INFO, "Finished receiving data\n");
            this->slave_boot_size -= CMD_BUFFER_SIZE;
            if (this->slave_boot_size > 0)
            {
                this->slave_boot_verif_address += CMD_BUFFER_SIZE;
                this->slave_boot_address += CMD_BUFFER_SIZE;
                this->slave_boot_state = SPIS_BOOT_STATE_SEND_SOF;
            }
            else
            {
                this->slave_boot_state = SPIS_BOOT_STATE_IDLE;
            }
            break;

        case SPIS_BOOT_STATE_SEND_DATA_DONE:
            this->trace.msg(vp::trace::LEVEL_INFO, "Finished sending data\n");
            this->slave_boot_size -= CMD_BUFFER_SIZE;
            if (this->slave_boot_size > 0)
            {
                this->slave_boot_verif_address += CMD_BUFFER_SIZE;
                this->slave_boot_address += CMD_BUFFER_SIZE;
                this->slave_boot_state = SPIS_BOOT_STATE_SEND_SOF;
            }
            else
            {
                this->slave_boot_state = SPIS_BOOT_STATE_IDLE;
            }
            break;

        case SPIS_BOOT_STATE_SEND_DATA_CRC:
            this->trace.msg(vp::trace::LEVEL_INFO, "Sending CRC\n");
            this->enqueue_transfer(this->mem_size, 1, 0, 0);
            this->slave_boot_state = SPIS_BOOT_STATE_SEND_DATA;
            break;

        case SPIS_BOOT_STATE_SEND_DATA:
            int size = this->slave_boot_size < CMD_BUFFER_SIZE ? this->slave_boot_size : CMD_BUFFER_SIZE;
            this->trace.msg(vp::trace::LEVEL_INFO, "Sending data (size: 0x%x)\n", size);
            this->enqueue_transfer(this->slave_boot_verif_address, size, 0, 0);
            this->slave_boot_state = SPIS_BOOT_STATE_SEND_DATA_DONE;
            break;





    }
}


void Spim_verif::enqueue_transfer(int address, int size, int is_rx, int is_duplex)
{
    this->slave_pending_tx_size = 0;
    this->slave_pending_rx_size = 0;
    this->slave_pending_mosi = 2;
    this->slave_pending_rx_addr = address;
    this->slave_pending_tx_addr = address;
    this->slave_wait_cycles = 4; // GAP9_5 requires at least 4 dummy cycles
    this->slave_pending_is_rx = is_rx || is_duplex;
    this->slave_pending_is_tx = !is_rx || is_duplex;

    if (this->slave_pending_is_rx)
    {
        this->slave_pending_rx_size = size;
    }

    if (this->slave_pending_is_tx)
    {
        this->slave_pending_tx_size = size;
    }

    this->slave_pending_rx_bits = 0;
    this->slave_pending_tx_bits = 0;

    this->slave_sck = 0;

    this->slave_state = SPIS_STATE_START_WAIT_CYCLES;

    if (!this->is_enqueued)
    {
        this->is_enqueued = true;
        this->enqueue_to_engine(this->slave_period);
    }
}


void Spim_verif::start_spi_load()
{
    if (this->do_spi_load)
    {
        if (this->current_section_load < this->sections.size())
        {
            Spi_loader_section *section = this->sections[this->current_section_load];

            if (section->access)
            {
                this->enqueue_boot_protocol_transfer(section->buffer, section->addr, section->size, 0);
            }
            else
            {
                this->enqueue_boot_protocol_jump(section->addr);
            }
            this->current_section_load++;
        }
        else
        {
            this->trace.msg(vp::trace::LEVEL_INFO, "Finished SPI loading\n");

            this->current_section_load = 0;
            this->do_spi_load = false;
            this->slave_boot_state = SPIS_BOOT_STATE_IDLE;
        }
    }
}


void Spim_verif::enqueue_spi_load(js::config *config)
{
    this->spi_load_config = config;

    this->slave_period = 1000000000000ULL / config->get_child_int("frequency") / 2;
}


void Spim_verif::spi_load(js::config *config)
{
    std::string path = config->get_child_str("stim_file");
    FILE *file = fopen(path.c_str(), "r");
    if (file == NULL)
    {
        this->get_trace()->fatal("Unable to open stim file (path: %s, error: %s)\n", path.c_str(), strerror(errno));
        return;
    }

    int buffer_size = 64;
    uint32_t *buffer;
    unsigned int pending_addr;
    unsigned int pending_start_addr = -1;
    int pending_index = 0;
    this->current_section_load = 0;

    while(1)
    {
        unsigned int addr, value;
        int err;
        if ((err = fscanf(file, "@%x %x\n", &addr, &value)) != 2)
        {
            if (err == EOF) break;
            this->get_trace()->fatal("Incorrect stimuli file (path: %s)\n", path.c_str());
            return;
        }
        
        if (pending_index && (addr != pending_addr + 4 || pending_index == buffer_size))
        {
            this->sections.push_back(new Spi_loader_section(true, (uint8_t *)buffer, pending_start_addr, pending_index*4));
            pending_index = 0;
            pending_start_addr = addr;
        }
        else if (pending_index == 0)
        {
            pending_start_addr = addr;
        }

        if (pending_index == 0)
        {
            buffer = new uint32_t[buffer_size];
        }

        buffer[pending_index] = value;

        pending_index++;
        pending_addr = addr;
    }

    if (pending_index)
    {
        this->sections.push_back(new Spi_loader_section(true, (uint8_t *)buffer, pending_start_addr, pending_index*4));
    }

    this->sections.push_back(new Spi_loader_section(false, NULL, config->get_child_int("entry"), 0));

    this->trace.msg(vp::trace::LEVEL_INFO, "Starting SPI loading\n");
    this->do_spi_load = true;
    this->start_spi_load();
}