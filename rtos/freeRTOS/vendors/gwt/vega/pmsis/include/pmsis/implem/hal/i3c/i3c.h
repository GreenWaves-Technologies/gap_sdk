/*
 * Copyright (c) 2021, GreenWaves Technologies, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of GreenWaves Technologies, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#pragma once


/* I3C defines. */
/* DEVICE_ADDR_TABLE_REG_XX. */
#define I3C_IS_I3C       ( 0 )  /* Used as i3c. */
#define I3C_IS_I2C       ( 1 )  /* Used as i2c. */
/**
 * * Regular command:
 * CMD_TR_REQ_REG_1[31:16] = Size of data in bytes.
 * CMD_TR_REQ_REG_2[31] = TOC, next Transfer On Completion: 0=RPT_START | 1=STOP
 * CMD_TR_REQ_REG_2[30] = ROC, Response status On Completion: 0=not required | 1=required
 * CMD_TR_REQ_REG_2[29] = R/W command: 0=WRITE | 1=READ
 * CMD_TR_REQ_REG_2[28:26] = Mode and speed of the transfer using I3C or I2C bus.
 *                           Values for I3C mode :
 *                           * 3’h0 – Standard speed mode(12.5MHZ)
 *                           * 3’h1 – 8MHz speed mode
 *                           * 3’h2 – 6 MHz speed mode
 *                           * 3’h3 – 4 MHz speed mode
 *                           * 3’h4 – 2 MHz speed mode
 *                           * 3’h5 – Reserved
 *                           * 3’h6 : HDR-DDR mode (25MHz speed)
 *                           * 3’h7 – Reserved
 *                           Values for I2C Mode:
 *                           * 3’h0 – I2C FM
 *                           * 3’h1 – I2C FM+
 *                           * 3’h2-3’h7 : Reserved
 * CMD_TR_REQ_REG_2[21] = Broadcast message
 * CMD_TR_REQ_REG_2[20:16] = Slave address
 * CMD_TR_REQ_REG_2[6:3] = Tag ID
 * CMD_TR_REQ_REG_2[2:0] = CMD TYPE(reg, imm, addr assign,...)
 *
 * * Immediate command:
 * CMD_TR_REQ_REG_1[31:0] = Data.
 * CMD_TR_REQ_REG_2[31] = TOC, next Transfer On Completion: 0=RPT_START | 1=STOP
 * CMD_TR_REQ_REG_2[30] = ROC, Response status On Completion: 0=not required | 1=required
 * CMD_TR_REQ_REG_2[29] = 0=WRITE | 1=READ
 * CMD_TR_REQ_REG_2[28:26] = Mode and speed of the transfer using I3C or I2C bus.
 *                           Values for I3C mode :
 *                           * 3’h0 – Standard speed mode(12.5MHZ)
 *                           * 3’h1 – 8MHz speed mode
 *                           * 3’h2 – 6 MHz speed mode
 *                           * 3’h3 – 4 MHz speed mode
 *                           * 3’h4 – 2 MHz speed mode
 *                           * 3’h5 – Reserved
 *                           * 3’h6 : HDR-DDR mode (25MHz speed)
 *                           * 3’h7 – Reserved
 *                           Values for I2C Mode:
 *                           * 3’h0 – I2C FM
 *                           * 3’h1 – I2C FM+
 *                           * 3’h2-3’h7 : Reserved
 * CMD_TR_REQ_REG_2[25:23] = Number of valid bytes(in CMD_TR_REQ_REG_1) :
 *                           * 3’h0 : No payload
 *                           * 3’h1 : 3’h4 : N bytes payload
 *                           * 3’h5 : 3’h7 : Reserved
 * CMD_TR_REQ_REG_2[21] = Broadcast message
 * CMD_TR_REQ_REG_2[20:16] = Slave address
 * CMD_TR_REQ_REG_2[6:3] = Tag ID
 * CMD_TR_REQ_REG_2[2:0] = CMD TYPE(reg, imm, addr assign,...)
 */
#define I3C_CMD_REG      ( 0 )  /* Regular i3c command. */
#define I3C_CMD_IMM      ( 1 )  /* Immediate i3c command. */
/**
 * DEVICE_ADDR_REG[31] = Device type: 0=i3c | 1=i2c
 * DEVICE_ADDR_REG[23] = Parity bit for the dynamic address calculated by software
 * DEVICE_ADDR_REG[22:16] = Address of the slave
 * DEVICE_ADDR_REG[15] = Timestamp enable
 * DEVICE_ADDR_REG[13] = In-band IRQ enable
 * DEVICE_ADDR_REG[12] = IBI payload
 * DEVICE_ADDR_REG[6:0] = Static address
 */
/* IRQ STATUS. */
#define I3C_RESP_DONE             ( 0 ) /* Response completion is done. */
#define I3C_DATA_TX_FIFO_FULL     ( 1 ) /* Data TX fifo is full. */
#define I3C_DATA_TX_FIFO_EMPTY    ( 2 ) /* Data TX fifo is empty and write is done. */
#define I3C_CMD_REQ_FIFO_FULL     ( 3 ) /* Command transfer request fifo is full. */
#define I3C_DATA_RX_FIFO_FULL     ( 4 ) /* Data RX fifo is full. */
#define I3C_RESP_FIFO_FULL        ( 5 ) /* Response completion fifo is full. */
#define I3C_IBI_TRANSFER_DONE     ( 6 ) /* IBI transfer is done. */
#define I3C_IBI_DATA_RX_FIFO_FULL ( 7 ) /* IBI payload is full. */
#define I3C_RESET_COMPLETION      ( 8 ) /* SW reset is done. */



/* MST_CNTL_EN */
static inline void hal_i3c_master_control_enable_set(uint8_t device_id, uint8_t ena)
{
    uint32_t base = (uint32_t) i3c(device_id);
    i3c_mst_cntl_en_reg_set(base, ena);
}


/* DEVICE ADDR. */
static inline void i3c_device_addr_table_reg_set(uint8_t device_id,
                                                 uint8_t slave_target,
                                                 uint32_t device_value)
{
    uint32_t base = (uint32_t) i3c(device_id);
    uint32_t offset = (uint32_t) I3C_DEVICE_ADDR_TABLE_REG0_OFFSET;
    offset += (slave_target << 2);
    GAP_WRITE(base, offset, device_value);
}

static inline void hal_i3c_device_addr_table_reg_set(uint8_t device_id,
                                                     uint8_t slave_target,
                                                     uint8_t device_type,
                                                     uint8_t dynamic_addr,
                                                     uint8_t static_addr)
{
    uint32_t base = (uint32_t) i3c(device_id);
    uint32_t device_value = ( (device_type << 31) |
                              (dynamic_addr << 16) |
                              (static_addr << 0) );
    i3c_device_addr_table_reg_set(device_id, slave_target, device_value);
}


/* IRQ status. */
static inline uint32_t hal_i3c_irq_status_get(uint8_t device_id)
{
    uint32_t base = (uint32_t) i3c(device_id);
    return i3c_irq_status_reg_get(base);
}


/* Command transfer. */
static inline void hal_i3c_cmd_transfer_regular_set(uint8_t device_id, uint16_t size,
                                                    uint8_t stop, uint8_t roc_req,
                                                    uint8_t read, uint8_t slave_target,
                                                    uint8_t tag_id)
{
    uint32_t base = (uint32_t) i3c(device_id);
    uint32_t cmd_tr_req_reg_1 = 0, cmd_tr_req_reg_2 = 0;
    cmd_tr_req_reg_1 = ( (size << 16) );
    cmd_tr_req_reg_2 = ( (stop << 31) |
                         (roc_req << 30) |
                         (read << 29) |
                         (slave_target << 16) |
                         (tag_id << 3) |
                         (I3C_CMD_REG << 0) );
    i3c_cmd_tr_req_reg_1_set(base, cmd_tr_req_reg_1);
    i3c_cmd_tr_req_reg_2_set(base, cmd_tr_req_reg_2);
}

static inline void hal_i3c_cmd_transfer_immediate_set(uint8_t device_id, uint32_t data,
                                                      uint8_t size, uint8_t stop,
                                                      uint8_t roc_req, uint8_t read,
                                                      uint8_t slave_target)
{
    uint32_t base = (uint32_t) i3c(device_id);
    uint32_t cmd_tr_req_reg_1 = 0, cmd_tr_req_reg_2 = 0;
    cmd_tr_req_reg_1 = data;
    cmd_tr_req_reg_2 = ( (stop << 31) |
                         (roc_req << 30) |
                         (read << 29) |
                         (size << 23) |
                         (slave_target << 16) |
                         (I3C_CMD_IMM << 0) );
    i3c_cmd_tr_req_reg_1_set(base, cmd_tr_req_reg_1);
    i3c_cmd_tr_req_reg_2_set(base, cmd_tr_req_reg_2);
}


/* Command response. */
static inline uint32_t hal_i3c_resp_get(uint8_t device_id)
{
    uint32_t base = (uint32_t) i3c(device_id);
    return i3c_resp_reg_get(base);
}


/* DATA_RX_FIFO. */
static inline uint32_t hal_i3c_data_rx_fifo_get(uint8_t device_id)
{
    uint32_t base = (uint32_t) i3c(device_id);
    return i3c_data_rx_fifo_reg_get(base);
}


/* DATA_TX_FIFO. */
static inline void hal_i3c_data_tx_fifo_set(uint8_t device_id, uint32_t value)
{
    uint32_t base = (uint32_t) i3c(device_id);
    i3c_data_tx_fifo_reg_set(base, value);
}
